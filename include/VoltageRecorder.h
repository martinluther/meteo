/*
 * VoltageRecorder.h -- encapsulate temperature
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: VoltageRecorder.h,v 1.2 2004/02/25 23:52:35 afm Exp $
 */
#ifndef _VoltageRecorder_h
#define _VoltageRecorder_h

#include <MinmaxRecorder.h>
#include <string>

namespace meteo {

class	VoltageRecorder : public BasicRecorder {
public:
	VoltageRecorder(void);
	VoltageRecorder(double);
	VoltageRecorder(double, const std::string&);
	VoltageRecorder(const std::string&);
	virtual ~VoltageRecorder(void) { }
	virtual std::string	getValueClass(void) const {
		return "VoltageValue";
	}
};

} // namespace meteo

#endif /* _VoltageRecorder_h */
