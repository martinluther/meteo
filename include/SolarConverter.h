/*
 * SolarConverter.h -- convert solar radiation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _SolarConverter_h
#define _SolarConverter_h

#include <string>
#include <SolarValue.h>
#include <BasicConverter.h>

namespace meteo {

class	SolarConverter : public BasicConverter {
public:
	SolarConverter(void) : BasicConverter("W/m2") { }
	SolarConverter(const std::string& t) : BasicConverter(t) { }
	virtual ~SolarConverter(void) { }
	virtual double	operator()(const double value, const std::string& from) const;
};

} /* namespace meteo */

#endif /* _SolarConverter_h */
