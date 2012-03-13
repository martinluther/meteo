/*
 * GroWeather.h -- encode GroWeather Station information
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: GroWeather.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _GroWeather_h
#define _GroWeather_h

#include <OldDavisStation.h>
#include <string>

namespace meteo {

class	GroWeather : public OldDavisStation {
public:
	GroWeather(const std::string& n);
	virtual	~GroWeather(void);
};

} /* namespace meteo */

#endif /* _GroWeather_h */
