/*
 * PressureValue.h -- pressure abstraction
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: PressureValue.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _PressureValue_h
#define _PressureValue_h

#include <BasicValue.h>
#include <Timeval.h>
#include <string>

namespace meteo {

class	PressureValue : public BasicValue {
public:
	PressureValue(void);
	PressureValue(double);
	PressureValue(double, const std::string&);
	PressureValue(const std::string&);
	virtual	~PressureValue(void) { }
	virtual void	setUnit(const std::string& unit);
	virtual std::string	getClass(void) const { return "PressureValue"; }
};

} // namespace meteo

#endif /* _PressureValue_h */
