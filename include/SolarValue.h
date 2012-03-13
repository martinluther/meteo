/*
 * SolarValue.h -- solar radiation abstraction
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: SolarValue.h,v 1.2 2004/02/25 23:52:35 afm Exp $
 */
#ifndef _SolarValue_h
#define _SolarValue_h

#include <BasicValue.h>
#include <Timeval.h>
#include <string>

namespace meteo {

class	SolarValue : public BasicValue {
public:
	SolarValue(void);
	SolarValue(double);
	SolarValue(double, const std::string&);
	SolarValue(const std::string&);
	virtual	~SolarValue(void) { }
	virtual void	setUnit(const std::string& unit);
	virtual std::string	getClass(void) const { return "SolarValue"; }
};

} // namespace meteo

#endif /* _SolarValue_h */
