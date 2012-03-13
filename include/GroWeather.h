/*
 * GroWeather.h -- encode GroWeather Station information
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
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
