//
// SunData.cc -- class to compute the map of booleans that indicate whether
//               the sun is above the horizon
//
// (c) 2004 Dr. Andreas Mueller, Beratung und Entwicklung
// $Id: SunData.cc,v 1.1 2004/02/27 22:09:04 afm Exp $
//
#include <SunData.h>
#include <SensorStationInfo.h>
#include <Field.h>
#include <QueryProcessor.h>

namespace meteo {

SunData::SunData(const std::string &station, int off) {
	// find the sensorid
	SensorStationInfo	si(station, "console");
	sensorid = si.getId();
	
	// store the offset
	offset = off;
}

std::vector<time_t>	SunData::operator()(int interval, time_t startkey,
	time_t endkey) {
	char	query[1024];

	// find the field id for sunrise and sunset
	int	sunriseid = Field().getId("sunrise");
	int	sunsetid = Field().getId("sunset");

	// formulate a query that will retrieve the data we are looking for
	if (interval == 60) {
		// not tested...
		snprintf(query, sizeof(query),
			"select a.timekey "
			"from sdata a, sdata b "
			"where a.timekey = b.timekey "
			"  and a.sensorid = %d and b.sensorid = %d "
			"  and a.fieldid = %d "		/* sunriseid */
			"  and b.fieldid = %d "		/* sunsetid */
			"  and a.timekey >= %d and a.timekey <= %d "
			"  and (a.timekey - %d) < b.value " /* now < sunset */
			"  and (b.timekey - %d) > a.value ", /* now > sunrise */
			sensorid, sensorid,
			sunriseid, sunsetid,
			startkey, endkey,
			offset, offset);
	} else {
		snprintf(query, sizeof(query),
			"select a.timekey "
			"from avg a, avg b "
			"where a.timekey = b.timekey "
			"  and a.intval = %d and b.intval = %d " /* intval */
			"  and a.sensorid = %d and b.sensorid = %d "
			"  and a.fieldid = %d "		/* sunriseid */
			"  and b.fieldid = %d "		/* sunsetid */
			"  and a.timekey >= %d and a.timekey <= %d "
			"  and (a.timekey - %d) < b.value " /* now < sunset */
			"  and (b.timekey - %d) > a.value ", /* now > sunrise */
			interval, interval,
			sensorid, sensorid,
			sunriseid, sunsetid,
			startkey, endkey,
			offset, offset);
	}

	// perform the query
	QueryProcessor	qp(false);
	BasicQueryResult	bqr = qp(query);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "found %d sun points", bqr.size());

	// go through the result set and add all time keys to the result
	// vector
	std::vector<time_t>	result;
	for (BasicQueryResult::iterator i = bqr.begin(); i != bqr.end(); i++) {
		result.push_back(atoi((*i)[0].c_str()));
	}
	
	// return the result
	return result;
}

} // namespace meteo
