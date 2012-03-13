/*
 * TimeConverter.h -- convert time values
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TimeConverter.h,v 1.1 2004/02/26 14:00:19 afm Exp $
 */
#ifndef _TimeConverter_h
#define _TimeConverter_h

#include <string>
#include <BasicConverter.h>
#include <TimeValue.h>

namespace meteo {

class	TimeConverter : public BasicConverter {
public:
	TimeConverter(void) : BasicConverter("unix") { }
	TimeConverter(const std::string& t) : BasicConverter(t) { }
	virtual ~TimeConverter(void) { }
	virtual double	operator()(const double value, const std::string& from) const;
};

} /* namespace meteo */

#endif /* _TimeConverter_h */
