/*
 * ChunkDumper.cc -- class to dump avg and sdata tables in chunks
 *                   with minimal contention
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: ChunkDumper.cc,v 1.5 2009/01/10 19:00:23 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#include <ChunkDumper.h>
#include <MeteoTypes.h>
#include <MeteoException.h>
#include <StationInfo.h>
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif /* HAVE_ERRNO_H */
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif /* HAVE_STRINGS_H */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#include <mdebug.h>

namespace meteo {

// open a single file, we need this already in the constructor
void	ChunkDumper::checkFile(const std::ofstream& file, const char *name) {
	if (!file) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "cannot open sdata file %s: "
			"%s (%d)", name, strerror(errno), errno);
		exit(EXIT_FAILURE);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "data file %s opened", name);
}

ChunkDumper::ChunkDumper(const std::string prefix, const int size)
	: qp(false),
	avgfile((prefix + "avg").c_str()),
	sdatafile((prefix + "sdata").c_str()),
	chunksize(size) {
	// set the do flags
	doavg = true;
	dosdata = true;
	sql = true;

	// check files
	checkFile(sdatafile, "sdata");
	checkFile(avgfile, "avg");

	// start and end time set to -1 (first record in database) and
	// present time
	starttime = -1;
	endtime = -1;

	// statistics counters
	avg = sdata = 0;
}

ChunkDumper::~ChunkDumper(void) {
	if (sql) {
		sdatafile << "-- " << sdata << " statements written"
			<< std::endl;
		avgfile << "-- " << avg << " statements written"
			<< std::endl;
	}
	sdatafile.close();
	avgfile.close();
}

// write a row to the output file
std::string	ChunkDumper::out(const std::string& leadin,
		std::vector<std::string>& row) const {
	std::string	result;
	if (sql) {
		// if sql is asserted, then write an insert statement
		result = leadin + " values (";
		for (unsigned int i = 0; i < row.size(); i++) {
			if (i > 0)
				 result.append(", ");
			result.append(row[i]);
		}
		result.append(");");
	} else {
		// if rawformat is asserted, write just raw data, suitable
		// for bulk loading (tab separated fields)
		for (unsigned int i = 0; i < row.size(); i++) {
			if (i > 0)
				result.append("\t");
			result.append(row[i]);
		}
	}
	return result;
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
		dump(sensorid);
	}
}

// dump the sdata table
int	ChunkDumper::dumpSdata(int start, int sensorid) {
	char	query[1024];

	int	end = start + chunksize;

	snprintf(query, sizeof(query), 
		"select timekey, sensorid, fieldid, value "
		"from sdata "
		"where sensorid = %d "
		"  and timekey >= %d and timekey < %d",
		sensorid, start, end);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "sdata query: %s", query);
	BasicQueryResult	bqr = qp(query);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query complete");

	for (BasicQueryResult::iterator i = bqr.begin(); i != bqr.end(); i++) {
		sdatafile << out("insert into sdata(timekey, "
			"sensorid, fieldid, value)", *i) << std::endl;
		sdata++;
	}

	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "next start time: %d", end);
	return end;
}

// dump the avg table
int	ChunkDumper::dumpAvg(const int start, int sensorid) {
	char	query[1024];

	int	end = start + chunksize;

	snprintf(query, sizeof(query), 
		"select timekey, intval, sensorid, fieldid, value "
		"from avg "
		"where sensorid = %d "
		"  and timekey >= %d and timekey < %d",
		sensorid, start, end);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "avg query: %s", query);
	BasicQueryResult	bqr = qp(query);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query complete");

	for (BasicQueryResult::iterator i = bqr.begin(); i != bqr.end(); i++) {
		avgfile << out("insert into avg(timekey, intval, "
			"sensorid, fieldid, value)", *i) << std::endl;
		avg++;
	}

	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "next start time: %d", end);
	return end;
}

// read a chunk of data from the database
void	ChunkDumper::dump(int sensorid) {
	char	query[1024];
	int	t, last, end = endtime;

	// dump the sdata table
	if (dosdata) {
		// find the good start value
		if (starttime < 0) {
			snprintf(query, sizeof(query),
				"select min(timekey), max(timekey) from sdata "
				"where sensorid = %d",
				sensorid);
			BasicQueryResult	bqr = qp(query);
			t = atoi((*bqr.begin())[0].c_str());
			last = atoi((*bqr.begin())[1].c_str());
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "data from %d to %d",
				t, last);
		} else {
			t = starttime;
		}
		// find the end time
		if (end < 0) {
			end = last;
		}
		if (end < 0) {
			end = time(NULL);
		}
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "performing dumps between "
			"%d and %d", t, end);
		while ((t > 0) && (t < end)) {
			t = dumpSdata(t, sensorid);
		}
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"sdata dump complete at end time %d", t);
	}

	// dump the avg table
	if (doavg) {
		// find the good start value
		if (starttime < 0) {
			snprintf(query, sizeof(query),
				"select min(timekey), max(timekey) from avg "
				"where sensorid = %d",
				sensorid);
			BasicQueryResult	bqr = qp(query);
			t = atoi((*bqr.begin())[0].c_str());
			last = atoi((*bqr.begin())[1].c_str());
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "data from %d to %d",
				t, last);
		} else {
			t = starttime;
		}
		// find the end time
		end = endtime;
		if (end < 0) {
			end = last;
		}
		if (end < 0) {
			end = time(NULL);
		}
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "performing dumps between "
			"%d and %d", t, end);
		while ((t > 0) && (t < end)) {
			t = dumpAvg(t, sensorid);
		}
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"avg dump complete at end time %d", t);
	}
}

void	ChunkDumper::dumpStations(const stringlist& stations) {
	// read data from the database
	for (stringlist::const_iterator i = stations.begin();
		i != stations.end(); i++) {
		// dump data for this station
		try {
			dumpStation(*i);
		} catch(MeteoException me) {
			fprintf(stderr, "exception: %s/%s\n",
				me.getReason().c_str(),
				me.getAddinfo().c_str());
		}
	}
}

} /* namespace meteo */
