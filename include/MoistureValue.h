/*
 * MoistureValue.h -- soil moisture abstraction
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
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
