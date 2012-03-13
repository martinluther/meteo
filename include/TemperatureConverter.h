/*
 * TemperatureConverter.h -- convert temperatures
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TemperatureConverter.h,v 1.2 2004/02/25 23:52:35 afm Exp $
 */
#ifndef _TemperatureConverter_h
#define _TemperatureConverter_h

#include <string>
#include <BasicConverter.h>
#include <TemperatureValue.h>

namespace meteo {

class	TemperatureConverter : public BasicConverter {
public:
	TemperatureConverter(void) : BasicConverter("C") { }
	TemperatureConverter(const std::string& t) : BasicConverter(t) { }
	virtual ~TemperatureConverter(void) { }
	virtual double	operator()(const double value,
		const std::string& sourceunit) const;
};

} /* namespace meteo */

#endif /* _TemperatureConverter_h */
