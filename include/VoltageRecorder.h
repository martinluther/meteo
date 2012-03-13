/*
 * VoltageRecorder.h -- encapsulate temperature
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _VoltageRecorder_h
#define _VoltageRecorder_h

#include <MinmaxRecorder.h>
#include <string>

namespace meteo {

class	VoltageRecorder : public BasicRecorder {
public:
	VoltageRecorder(void);
	VoltageRecorder(double);
	VoltageRecorder(double, const std::string&);
	VoltageRecorder(const std::string&);
	virtual ~VoltageRecorder(void) { }
	virtual std::string	getValueClass(void) const {
		return "VoltageValue";
	}
};

} // namespace meteo

#endif /* _VoltageRecorder_h */
