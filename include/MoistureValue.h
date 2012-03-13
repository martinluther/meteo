/*
 * MoistureValue.h -- soil moisture abstraction
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: MoistureValue.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _MoistureValue_h
#define _MoistureValue_h

#include <BasicValue.h>
#include <Timeval.h>
#include <string>

namespace meteo {

class	MoistureValue : public BasicValue {
public:
	MoistureValue(void);
	MoistureValue(double);
	MoistureValue(double, const std::string&);
	MoistureValue(const std::string&);
	virtual	~MoistureValue(void) { }
	virtual void	setUnit(const std::string& unit);
	virtual std::string	getClass(void) const { return "MoistureValue"; }
};

} // namespace meteo

#endif /* _MoistureValue_h */
