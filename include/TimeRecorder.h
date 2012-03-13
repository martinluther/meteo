/*
 * TimeRecorder.h -- class to record times
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TimeRecorder.h,v 1.1 2004/02/26 14:00:19 afm Exp $
 */
#ifndef _TimeRecorder_h
#define _TimeRecorder_h

#include <LastRecorder.h>
#include <string>

namespace meteo {

class	TimeRecorder : public LastRecorder {
public:
	TimeRecorder(void);
	TimeRecorder(double);
	TimeRecorder(double, const std::string&);
	TimeRecorder(const std::string&);
	virtual	~TimeRecorder(void) { }
	virtual std::string	getValueClass(void) const {
		return "TimeValue";
	}
};

} // namespace meteo

#endif /* _TimeRecorder_h */
