/*
 * RainRateRecorder.h -- encapsulate rain rate
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _RainRateRecorder_h
#define _RainRateRecorder_h

#include <MinmaxRecorder.h>
#include <string>

namespace meteo {

class	RainRateRecorder : public MinmaxRecorder {
public:
	RainRateRecorder(void);
	RainRateRecorder(double);
	RainRateRecorder(double, const std::string&);
	RainRateRecorder(const std::string&);
	virtual ~RainRateRecorder(void) { }
	virtual std::string	getValueClass(void) const {
		return "RainRate";
	}
};

} // namespace meteo

#endif /* _RainRateRecorder_h */
