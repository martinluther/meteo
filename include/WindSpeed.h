/*
 * WindSpeed.h -- encapsulate windspeed
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _WindSpeed_h
#define _WindSpeed_h

#include <BasicValue.h>
#include <Timeval.h>
#include <string>

namespace meteo {

class	WindSpeed : public BasicValue {
public:
	WindSpeed(void);
	WindSpeed(double T);
	WindSpeed(double T, const std::string& u);
	WindSpeed(const std::string& u);
	virtual ~WindSpeed(void) { }
	virtual void	setUnit(const std::string& u);
	virtual std::string	getClass(void) const {
		return "WindSpeed";
	}
};

} // namespace meteo

#endif /* _WindSpeed_h */
