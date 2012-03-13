/*
 * HumidityConverter.h -- convert humidity values
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: HumidityConverter.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _HumidityConverter_h
#define _HumidityConverter_h

#include <string>
#include <BasicConverter.h>
#include <HumidityValue.h>

namespace meteo {

class	HumidityConverter : public BasicConverter {
public:
	HumidityConverter(void) : BasicConverter("%") { }
	HumidityConverter(const std::string& unit) : BasicConverter(unit) { }
	virtual ~HumidityConverter(void) { }
	virtual double	operator()(const double value, const std::string& from) const;
};

} /* namespace meteo */

#endif /* _HumidityConverter_h */
