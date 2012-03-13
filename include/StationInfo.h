/*
 * StationInfo.h
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: StationInfo.h,v 1.4 2004/02/25 23:52:35 afm Exp $
 */
#ifndef _StationInfo_h
#define _StationInfo_h

#include <MeteoTypes.h>

namespace meteo {

class StationInfo {
	std::string	stationname;
	int		offset;
	bool		offsetknown;
private:
	std::string	getField(const std::string& fieldname) const;
public:
	StationInfo(const std::string& name) : stationname(name) {
		offsetknown = false;
	}
	~StationInfo(void) { }
	int		getId(void) const;
	int		getOffset(void);
	std::string	getTimezone(void) const;
	std::list<int>	getSensorIds(void) const;
	stringlist 	getSensornames(void) const;
	stringlist	getFieldnames(void) const;
};

} /* namespace meteo */

#endif /* _StationInfo_h */
