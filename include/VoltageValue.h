/*
 * VoltageValue.h -- encapsulate voltage
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: VoltageValue.h,v 1.2 2004/02/25 23:52:35 afm Exp $
 */
#ifndef _VoltageValue_h
#define _VoltageValue_h

#include <BasicValue.h>
#include <Timeval.h>
#include <string>

namespace meteo {

class	VoltageValue : public BasicValue {
public:
	VoltageValue(void);
	VoltageValue(double T);
	VoltageValue(double T, const std::string& u);
	VoltageValue(const std::string& u);
	virtual ~VoltageValue(void) { }
	virtual void	setUnit(const std::string& u);
	virtual std::string	getClass(void) const {
		return "VoltageValue";
	}
};

} // namespace meteo

#endif /* _VoltageValue_h */
