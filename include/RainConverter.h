/*
 * RainConverter.h -- convert rain values
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _RainConverter_h
#define _RainConverter_h

#include <string>
#include <Rain.h>
#include <BasicConverter.h>

namespace meteo {

class	RainConverter : public BasicConverter {
public:
	RainConverter(void) : BasicConverter("mm") { }
	RainConverter(const std::string& t) : BasicConverter(t) { }
	virtual ~RainConverter(void) { }
	virtual double	operator()(const double value, const std::string& from) const;
};

} /* namespace meteo */

#endif /* _RainConverter_h */
