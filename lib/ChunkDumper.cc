/*
 * ChunkDumper.cc -- class to dump avg, sdata and header tables in chunks
 *                   with minimal contention
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <ChunkDumper.h>

#include <MeteoTypes.h>
#include <MeteoException.h>
#include <StationInfo.h>
#include <errno.h>
#include <strings.h>
#include <unistd.h>
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
	headerfile((prefix + "header").c_str()),
	chunksize(size) {
	// set the do flags
	doavg = true;
	dosdata = true;
	doheader = true;
	sql = true;

	// check files
	checkFile(sdatafile, "sdata");
	checkFile(headerfile, "header");
	checkFile(avgfile, "avg");

	// statistics counters
	avg = sdata = header = 0;
}

ChunkDumper::~ChunkDumper(void) {
	if (sql) {
		sdatafile << "-- " << sdata << " statements written"
			<< std::endl;
		headerfile << "-- " << header << " statements written"
			<< std::endl;
		avgfile << "-- " << avg << " statements written"
			<< std::endl;
	}
	sdatafile.close();
	headerfile.close();
	avgfile.close();
}

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
	char	query[1024];
	int	endtime;
	BasicQueryResult	bqr;

	// find th einitial timekey if the timekey is 0
	int	t = timekey;
	if (t == 0) {
		snprintf(query, sizeof(query),
			"select min(timekey) from sdata where sensorid = %d",
			sensorid);
		bqr = qp(query);
		if (1 != bqr.size()) {
			mdebug(LOG_ERR, MDEBUG_LOG, 0,
				"no initial time from query '%s'", query);
			throw MeteoException("no initial time from query",
				query);
		}
		t = atoi((*bqr.begin())[0].c_str());
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"start dumping sensor %d at %d", sensorid, t);
	}

	// set stage
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "getchunk %d/%d", t, chunksize);

	// perform a header query for this station and at most chunksize
	// records
	snprintf(query, sizeof(query), 
		"select timekey, group300, group1800, group7200, group86400 "
		"from header "
		"where timekey > %d order by timekey limit %d",
		t, chunksize);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "header query: %s\n", query);
	
	bqr = qp(query);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query complete");
	if (bqr.size() == 0) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "no more rows");
		return timekey;
	}

	// read the data and write it to the sdata file
	BasicQueryResult::iterator	i;
	if (doheader) {
		for (i = bqr.begin(); i != bqr.end(); i++) {
			endtime = atoi((*i)[0].c_str());
			headerfile << out(
				"insert into header(timekey, group300, "
				"group1800, group7200, group86400) ", *i)
				<< std::endl;;
			header++;
		}
	} else {
		// read the timekey from the last element, using the reverse
		// iterator
		endtime = atoi((*bqr.rbegin())[0].c_str());
	}

	// perform a sdata query for this station and the complete time
	// interval
	if (dosdata) {
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
			sdatafile << out("insert into sdata(timekey, "
				"sensorid, fieldid, value)", *i) << std::endl;
			sdata++;
		}
	}

	// perform an avg query for this station and the interval starting
	// at timekey and ending at the timekey of the last record of the
	// previous query
	if (doavg) {
		snprintf(query, sizeof(query),
			"select timekey, intval, sensorid, fieldid, value "
			"from avg "
			"where sensorid = %d "
			"  and timekey > %d and timekey <= %d",
			sensorid, t, endtime);
		bqr = qp(query);

		// retrieve result for avg
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "found %d rows in avg",
			bqr.size());
		for (i = bqr.begin(); i != bqr.end(); i++) {
			avgfile << out("insert into avg(timekey, "
				"intval, sensorid, fieldid, value) ", *i)
				<< std::endl;
			avg++;
		}
	}

	// return the new last records timekey
	return endtime;
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
