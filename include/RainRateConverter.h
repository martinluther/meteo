/*
 * RainRateConverter.h -- convert rain rates, for stations that know about
 *                        the rain rate
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _RainRateConverter_h
#define _RainRateConverter_h

#include <string>
#include <RainRate.h>
#include <BasicConverter.h>

namespace meteo {

class	RainRateConverter : public BasicConverter {
public:
	RainRateConverter(void) : BasicConverter("mm/h") { }
	RainRateConverter(const std::string& t) : BasicConverter(t) { }
	virtual ~RainRateConverter(void) { }
	virtual double	operator()(const double value, const std::string& from) const;
};

} /* namespace meteo */

#endif /* _RainRateConverter_h */
