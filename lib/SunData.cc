//
// SunData.cc -- class to compute the map of booleans that indicate whether
//               the sun is above the horizon
//
// (c) 2004 Dr. Andreas Mueller, Beratung und Entwicklung
// $Id: SunData.cc,v 1.3 2004/05/08 22:07:17 afm Exp $
//
#include <SunData.h>
#include <StationInfo.h>
#include <SensorStationInfo.h>
#include <Field.h>
#include <QueryProcessor.h>

namespace meteo {

void	SunData::setCompute(bool _compute) {
	if (sensorid >= 0) {
		compute = _compute;
	}
}

SunData::SunData(const std::string &station, int off) {
	sensorid = -1;
	compute = false;
	// find the station type
	StationInfo	si(station);
	if (si.getStationtype() == "VantagePro") {
		SensorStationInfo	ssi(station, "console");
		sensorid = ssi.getId();
	} else {
		compute = true;
	}
	
	// store the offset (this is not the offset in the station, but
	// the offset in the graph!)
	offset = off;

	// create a Sun object for this station
	double	longitude = si.getLongitude();
	double	latitude = si.getLatitude();
	thesun = Sun(longitude, latitude);
}

std::vector<time_t>	SunData::from_database(int interval, time_t startkey,
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

// use the sun class to compute the sunrise and sunset times
std::vector<time_t>	SunData::computed(int interval, time_t startkey,
	time_t endkey) {
	std::vector<time_t>	result;
	for (time_t tk = startkey; tk <= endkey; tk += interval) {
		time_t	t = tk - offset;
		if ((thesun.sunrise(t) <= t) && (thesun.sunset(t) >= t))
			result.push_back(tk);
	}
	return result;
}

// the operator() automatically selects either the database or computation
std::vector<time_t>	SunData::operator()(int interval, time_t startkey,
	time_t endkey) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "compute daylight timekeys between "
		"%d and %d, interval %d", startkey, endkey, interval);
	if (compute) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "compute sun position");
		return computed(interval, startkey, endkey);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "get sun position from database");
	return from_database(interval, startkey, endkey);
}

} // namespace meteo
