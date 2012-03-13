/*
 * StationInfo.cc -- retreive information about a station
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 *
 * $Id: StationInfo.cc,v 1.6 2004/02/25 23:48:05 afm Exp $
 */
#include <StationInfo.h>
#include <Configuration.h>
#include <QueryProcessor.h>
#include <MeteoException.h>
#include <SensorStationInfo.h>

namespace meteo {

std::string	StationInfo::getField(const std::string& fieldname) const {
	// create a queryprocessor object
	QueryProcessor	qp(false);

	// build the query for the fieldname
	std::string	query = "select " + fieldname
		+ " from station where name = '" + stationname + "'";
	BasicQueryResult	bqr = qp(query);
	if (bqr.size() != 1) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "incorrect result for query %s",
			query.c_str());
		throw MeteoException("incorrect result for query", query);
	}

	// return the field
	return (*bqr.begin())[0];
}

int	StationInfo::getId(void) const {
	return atoi(getField("id").c_str());
}
int	StationInfo::getOffset(void) {
	// in case we have done the query before, return the already known
	// offset value
	if (offsetknown) {
		return offset;
	}

	// get the offset as a string
	std::string	offsetstring = getField("offset");

	// remember the offset value
	offset = atoi(offsetstring.c_str());
	offsetknown = true;

	// return the field
	return offset;
}
std::string	StationInfo::getTimezone(void) const {
	std::string	f = getField("timezone");
	if (f == "NULL")
		throw MeteoException("Timezone not found", "");
	return f;
}

// retrieve the nonqualified names of the sensors
stringlist	StationInfo::getSensornames(void) const {
	Configuration	conf;
	std::string	fieldnamexpath = "/meteo/station[@name='"
				+ stationname + "']/sensors/sensor/@name";
	return conf.getStringList(fieldnamexpath);
}

intlist	StationInfo::getSensorIds(void) const {
	QueryProcessor	qp(false);
	BasicQueryResult	bqr = qp(std::string(
		"select b.id from station a, sensor b ") +
		"where a.name = '" + stationname + "' and a.id = b.stationid");
	BasicQueryResult::iterator	i;
	intlist	result;
	for (i = bqr.begin(); i != bqr.end(); i++) {
		result.push_back(atoi((*i)[0].c_str()));
	}
	return result;
}

stringlist	StationInfo::getFieldnames(void) const {
	// retrieve the sensor names
	stringlist	result, sensors = getSensornames();

	// for each sensor, retrieve the fully qualified field names
	stringlist::const_iterator	i;
	for (i = sensors.begin(); i != sensors.end(); i++) {
		SensorStationInfo	si(stationname, *i);
		stringlist	fi = si.getQualifiedFieldnames();
		result.splice(result.end(), fi);
	}
	return result;
}

} /* namespace meteo */
