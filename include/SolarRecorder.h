/*
 * SolarRecorder.h -- solar radiation abstraction
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: SolarRecorder.h,v 1.2 2004/02/25 23:52:35 afm Exp $
 */
#ifndef _SolarRecorder_h
#define _SolarRecorder_h

#include <MinmaxRecorder.h>
#include <string>

namespace meteo {

class	SolarRecorder : public MinmaxRecorder {
public:
	SolarRecorder(void);
	SolarRecorder(double);
	SolarRecorder(double, const std::string&);
	SolarRecorder(const std::string&);
	virtual	~SolarRecorder(void) { }
	virtual const bool	hasMax(void) const { return false; }
	virtual const bool	hasMin(void) const { return false; }
	virtual std::string	getValueClass(void) const {
		return "SolarValue";
	}
};

} // namespace meteo

#endif /* _SolarRecorder_h */
