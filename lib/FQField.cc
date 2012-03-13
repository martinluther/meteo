/*
 * FQField.cc -- implement field query oracle
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 */
#include <FQField.h>
#include <FQField_internals.h>
#include <QueryProcessor.h>
#include <MeteoException.h>
#include <map>
#include <SensorStationInfo.h>

namespace meteo {

// comparison operator for fieldid
bool	fieldid::operator<(const fieldid& other) const {
	if (stationid == other.stationid) {
		if (sensorid == other.sensorid) {
			return other.mfieldid < mfieldid;
		} else
			return other.sensorid < sensorid;
	} else
		return other.stationid < stationid;
}

// internals: this implements a cache for field information
FQField_internals	*FQField::fi	= NULL;

// FQField  class implementation, mainly delegation
FQField::FQField(void) {
	if (NULL == fi) {
		fi = new FQField_internals();
	}
}
FQField::~FQField(void) { }

// accessors defined for FQField
fieldid	FQField::getFieldid(const std::string& fieldname) {
	return fi->getFieldid(fieldname);
}

fieldid	FQField::getFieldid(int sensorid, int mfieldid) {
	return fi->getFieldid(sensorid, mfieldid);
}

fqfieldname	FQField::getFqfieldname(const fieldid& fid) {
	return fi->getFqfieldname(fid);
}

fqfieldname	FQField::getFqfieldname(const std::string& fieldname) {
	return fi->split(fieldname);
}

} /* namespace meteo */
