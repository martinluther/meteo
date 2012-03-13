/*
 * HumidityValue.h -- humidity abstraction
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _HumidityValue_h
#define _HumidityValue_h

#include <HumidityValue.h>
#include <TemperatureValue.h>
#include <Timeval.h>
#include <string>

namespace meteo {
class	HumidityValue : public BasicValue {
protected:
	void	update(const BasicValue *other);
public:
	HumidityValue(void);
	HumidityValue(double);
	HumidityValue(double, const std::string&);
	HumidityValue(const std::string&);
	virtual	~HumidityValue(void) { }
	virtual void	setUnit(const std::string&);
	virtual std::string	getClass(void) const { return "HumidityValue"; }
	TemperatureValue	Dewpoint(const TemperatureValue&);
};

} // namespace meteo

#endif /* _HumidityValue_h */
