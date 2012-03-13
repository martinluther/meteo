/*
 * TimeValue.h -- value representing a point in time
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TimeValue.h,v 1.1 2004/02/26 14:00:19 afm Exp $
 */
#ifndef _TimeValue_h
#define _TimeValue_h

#include <BasicValue.h>
#include <Timeval.h>
#include <string>

namespace meteo {

class	TimeValue : public BasicValue {
public:
	TimeValue(void);
	TimeValue(double);
	TimeValue(double, const std::string&);
	TimeValue(const std::string&);
	virtual	~TimeValue(void) { }
	virtual void	setUnit(const std::string& unit);
	virtual std::string	getClass(void) const { return "TimeValue"; }
};

} // namespace meteo

#endif /* _TimeValue_h */
