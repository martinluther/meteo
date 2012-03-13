/*
 * SensorStationInfo.h -- access to sensor station information, used by the
 *                        station constructors
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _SensorStationInfo_h
#define _SensorStationInfo_h

#include <MeteoTypes.h>

namespace meteo {

class SSI_internals;

class SensorStationInfo {
	std::string	stationname;
	std::string	name;
	int		sensorid;
	static SSI_internals	*ssi;
public:
	// constructors/destructor
	SensorStationInfo(const std::string& statname, const std::string& name);
	SensorStationInfo(const int id);
	~SensorStationInfo(void);

	// access to fields
	const std::string&	getName(void) const { return name; }
	const std::string&	getStationname(void) const {
		return stationname;
	}
	int	getId(void) const { return sensorid; }

	// methods to retrieve fields
	stringlist	getFieldnames(void) const;
	stringlist	getQualifiedFieldnames(void) const;
};

} /* namespace meteo */

#endif /* _SensorStationInfo_h */
