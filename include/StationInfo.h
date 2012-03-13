/*
 * StationInfo.h
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _StationInfo_h
#define _StationInfo_h

#include <MeteoTypes.h>

namespace meteo {

class StationInfo {
	std::string	stationname;
private:
	std::string	getField(const std::string& fieldname) const;
public:
	StationInfo(const std::string& name) : stationname(name) { }
	~StationInfo(void) { }
	int		getId(void) const;
	int		getOffset(void) const;
	std::string	getTimezone(void) const;
	std::list<int>	getSensorIds(void) const;
	stringlist 	getSensornames(void) const;
	stringlist	getFieldnames(void) const;
};

} /* namespace meteo */

#endif /* _StationInfo_h */
