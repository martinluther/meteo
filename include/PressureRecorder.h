/*
 * PressureRecorder.h -- pressure abstraction
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: PressureRecorder.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _PressureRecorder_h
#define _PressureRecorder_h

#include <MinmaxRecorder.h>
#include <string>

namespace meteo {

class	PressureRecorder : public MinmaxRecorder {
public:
	PressureRecorder(void);
	PressureRecorder(double);
	PressureRecorder(double, const std::string&);
	PressureRecorder(const std::string&);
	virtual	~PressureRecorder(void) { }
	virtual std::string	getValueClass(void) const {
		return "PressureValue";
	}
};

} // namespace meteo

#endif /* _PressureRecorder_h */
