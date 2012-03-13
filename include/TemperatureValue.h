/*
 * TemperatureValue.h -- encapsulate temperature
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TemperatureValue.h,v 1.2 2004/02/25 23:52:35 afm Exp $
 */
#ifndef _TemperatureValue_h
#define _TemperatureValue_h

#include <BasicValue.h>
#include <Timeval.h>
#include <string>

namespace meteo {

class	TemperatureValue : public BasicValue {
public:
	TemperatureValue(void);
	TemperatureValue(double T);
	TemperatureValue(double T, const std::string& u);
	TemperatureValue(const std::string& u);
	virtual ~TemperatureValue(void) { }
	virtual void	setUnit(const std::string& u);
	virtual std::string	getClass(void) const {
		return "TemperatureValue";
	}
};

} // namespace meteo

#endif /* _TemperatureValue_h */
