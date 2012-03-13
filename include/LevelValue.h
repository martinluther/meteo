/*
 * LevelValue.h -- base class for meteorological values retrieved from
 *                 a weather station, allows for updates and knows about
 *                 about maximum and minimum
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _LevelValue_h
#define _LevelValue_h

#include <BasicValue.h>
#include <Timeval.h>
#include <string>

namespace meteo {

class	LevelValue : public BasicValue {
public:
	LevelValue(void);
	LevelValue(double);
	LevelValue(double, const std::string&);
	LevelValue(const std::string&);
	virtual	~LevelValue(void) { }
	virtual void	setUnit(const std::string& unit);
	virtual std::string	getClass(void) const { return "LevelValue"; }
};

} // namespace meteo

#endif /* _LevelValue_h */
