/*
 * TemperatureRecorder.h -- encapsulate temperature
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _TemperatureRecorder_h
#define _TemperatureRecorder_h

#include <MinmaxRecorder.h>
#include <string>

namespace meteo {

class	TemperatureRecorder : public MinmaxRecorder {
public:
	TemperatureRecorder(void);
	TemperatureRecorder(double);
	TemperatureRecorder(double, const std::string&);
	TemperatureRecorder(const std::string&);
	virtual ~TemperatureRecorder(void) { }
	virtual std::string	getValueClass(void) const {
		return "TemperatureValue";
	}
};

} // namespace meteo

#endif /* _TemperatureRecorder_h */
