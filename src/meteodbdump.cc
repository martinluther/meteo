/*
 * meteodbdump.cc -- dump the meteo database for a given set of stations
 *                   to suitable load files, and provide load stations
 *                   that will load the files again
 *
 *                   This program is written so that it can be compiled and
 *                   run independently of the meteo library. I use it to
 *                   synchronize the database on my notebook with timon's.
 *                   It isn't documented in a manual page either, because it
 *                   is hardly useful anybody but me.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <stdio.h>
#include <stdlib.h>
#include <MeteoTypes.h>
#include <QueryProcessor.h>
#include <Configuration.h>
#include <MeteoException.h>
#include <StationInfo.h>
#include <string>
#include <vector>
#include <errno.h>
#include <strings.h>
#include <unistd.h>
#include <mdebug.h>
#include <fstream>

namespace meteo {

class	ChunkDumper {
	QueryProcessor	qp;
	std::ofstream	avgfile;
	std::ofstream	sdatafile;
	std::ofstream	headerfile;
	int	avg, sdata, header;
	int	chunksize;
	void	checkFile(const std::ofstream& file, const char *name);
public:
	ChunkDumper(const std::string prefix, int size);
	~ChunkDumper(void);

	std::string	insertQuery(const std::string& insertpart,
		std::vector<std::string>& row) const;
	int	dump(const int timekey, const int sensorid);
	void	dumpStation(const std::string& stationname);
};

// open a single file, we need this already in the constructor
void	ChunkDumper::checkFile(const std::ofstream& file, const char *name) {
	if (!file) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "cannot open sdata file %s: "
			"%s (%d)", name, strerror(errno), errno);
		exit(EXIT_FAILURE);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "sdata file %s opened", name);
}

ChunkDumper::ChunkDumper(const std::string prefix, const int size)
	: qp(false),
	avgfile((prefix + "avg").c_str()),
	sdatafile((prefix + "sdata").c_str()),
	headerfile((prefix + "header").c_str()),
	chunksize(size) {
	// check files
	checkFile(sdatafile, "sdata");
	checkFile(headerfile, "header");
	checkFile(avgfile, "avg");

	// statistics counters
	avg = sdata = header = 0;
}

ChunkDumper::~ChunkDumper(void) {
	sdatafile << "-- " << sdata << " statements written" << std::endl;
	sdatafile.close();
	headerfile << "-- " << header << " statements written" << std::endl;
	headerfile.close();
	avgfile << "-- " << avg << " statements written" << std::endl;
	avgfile.close();
}

std::string	ChunkDumper::insertQuery(const std::string& insertpart,
		std::vector<std::string>& row) const {
	std::string	result(insertpart + " values (");
	for (unsigned int i = 0; i < row.size(); i++) {
		if (i > 0)
			 result.append(", ");
		result.append(row[i]);
	}
	return result.append(");");
}

void	ChunkDumper::dumpStation(const std::string& stationname) {
	// retrieve the station id for this station
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "working on station %s",
		stationname.c_str());
	StationInfo	si(stationname);
	intlist	sensorids = si.getSensorIds();
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "station %s has %d sensorstations",
		stationname.c_str(), sensorids.size());

	// retrieve the data
	intlist::iterator	i;
	for (i = sensorids.begin(); i != sensorids.end(); i++) {
		int	sensorid = *i;
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "working on sensorid %d",
			sensorid);
		int	timekey = 0;
		do {
			int	oldtimekey = timekey;
			timekey = dump(timekey, sensorid);
			if (timekey == oldtimekey)
				break;
		} while (timekey > 0);
	}
}

// read a chunk of data from the database
int	ChunkDumper::dump(const int timekey, int sensorid) {
	int	endtime;
	BasicQueryResult	bqr;

	// set stage
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "getchunk %d/%d", timekey, chunksize);

	// perform a header query for this station and at most chunksize
	// records
	char	query[1024];
	snprintf(query, sizeof(query), 
		"select timekey, group300, group1800, group7200, group86400 "
		"from header "
		"where timekey > %d order by timekey limit %d",
		timekey, chunksize);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "header query: %s\n", query);
	
	bqr = qp(query);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query complete");

	// read the data and write it to the sdata file
	BasicQueryResult::iterator	i;
	for (i = bqr.begin(); i != bqr.end(); i++) {
		endtime = atoi((*i)[0].c_str());
		headerfile << insertQuery(
			"insert into header(timekey, group300, group1800, "
			"	group7200, group86400) ", *i) << std::endl;;
		header++;
	}

	// perform a sdata query for this station and the complete time
	// interval
	snprintf(query, sizeof(query), 
		"select timekey, sensorid, fieldid, value "
		"from sdata "
		"where sensorid = %d "
		"  and timekey > %d and timekey <= %d",
		sensorid, timekey, endtime);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "sdata query: %s", query);
	bqr = qp(query);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query complete");

	for (i = bqr.begin(); i != bqr.end(); i++) {
		sdatafile << insertQuery(
			"insert into sdata(timekey, sensorid, fieldid, value)",
			*i) << std::endl;
		sdata++;
	}

	// perform an avg query for this station and the interval starting
	// at timekey and ending at the timekey of the last record of the
	// previous query
	snprintf(query, sizeof(query),
		"select timekey, intval, sensorid, fieldid, value "
		"from avg "
		"where sensorid = %d "
		"  and timekey > %d and timekey <= %d",
		sensorid, timekey, endtime);
	bqr = qp(query);

	// retrieve result for avg
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "found %d rows in avg", bqr.size());
	for (i = bqr.begin(); i != bqr.end(); i++) {
		avgfile << insertQuery(
			"insert into avg(timekey, intval, sensorid, fieldid, "
			"	value) ", *i) << std::endl;
		avg++;
	}

	// return the new last records timekey
	return endtime;
}

static void	dumpStations(const stringlist& stations,
	const std::string& basename, const int chunksize) {
	// open database connection
	ChunkDumper	cd(basename, chunksize);

	// read data from the database
	for (stringlist::const_iterator i = stations.begin();
		i != stations.end(); i++) {
		// dump data for this station
		try {
			cd.dumpStation(*i);
		} catch(MeteoException me) {
			fprintf(stderr, "exception: %s/%s\n",
				me.getReason().c_str(),
				me.getAddinfo().c_str());
		}
	}
}

} /* namespace meteo */

int	main(int argc, char *argv[]) {
	meteo::stringlist	stations;
	std::string	basename("./");
	std::string	logurl("file:///-");
	std::string	conffile(METEOCONFFILE);
	int	chunksize = 349; 	// one day worth of data
	int	c;

	// parse the command line
	while (EOF != (c = getopt(argc, argv, "db:l:s:f:c:")))
		switch (c) {
		case 'f':
			conffile = std::string(optarg);
			break;
		case 'l':
			logurl = std::string(optarg);
			break;
		case 'd':
			debug++;
			break;
		case 's':
			// shorten the station name to 8 characters
			stations.push_back(std::string(optarg).substr(0, 8));
			break;
		case 'b':
			basename = std::string(optarg);
			break;
		case 'c':
			chunksize = atoi(optarg);
			break;
		}

	// open configuration
	meteo::Configuration	conf(conffile);

	// do the work in a function, so that destructors are properly
	// called when it completes
	meteo::dumpStations(stations, basename, chunksize);

	// that's it
	exit(EXIT_SUCCESS);
}
