/*
 * UVValue.h -- base class for meteorological values retrieved from
 *                 a weather station, allows for updates and knows about
 *                 about maximum and minimum
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: UVValue.h,v 1.2 2004/02/25 23:52:35 afm Exp $
 */
#ifndef _UVValue_h
#define _UVValue_h

#include <BasicValue.h>
#include <Timeval.h>
#include <string>

namespace meteo {

class	UVValue : public BasicValue {
public:
	UVValue(void);
	UVValue(double);
	UVValue(double, const std::string&);
	UVValue(const std::string&);
	virtual	~UVValue(void) { }
	virtual void	setUnit(const std::string& unit);
	virtual std::string	getClass(void) const { return "UVValue"; }
};

} // namespace meteo

#endif /* _UVValue_h */
