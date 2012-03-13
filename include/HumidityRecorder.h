/*
 * HumidityRecorder.h -- humidity abstraction
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _HumidityRecorder_h
#define _HumidityRecorder_h

#include <MinmaxRecorder.h>
#include <string>

namespace meteo {

class	HumidityRecorder : public MinmaxRecorder {
public:
	HumidityRecorder(void);
	HumidityRecorder(double);
	HumidityRecorder(double, const std::string&);
	HumidityRecorder(const std::string&);
	virtual	~HumidityRecorder(void) { }
	virtual std::string	getValueClass(void) const {
		return "HumidityValue";
	}
};

} // namespace meteo

#endif /* _HumidityRecorder_h */
