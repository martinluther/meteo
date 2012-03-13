/*
 * UVConverter.h -- convert UV value
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: UVConverter.h,v 1.2 2004/02/25 23:52:35 afm Exp $
 */
#ifndef _UVConverter_h
#define _UVConverter_h

#include <string>
#include <UVValue.h>
#include <BasicConverter.h>

namespace meteo {

class	UVConverter : public BasicConverter {
public:
	UVConverter(void) : BasicConverter("index") { }
	UVConverter(const std::string& t) : BasicConverter(t) { }
	virtual ~UVConverter(void) { }
	virtual double	operator()(const double value, const std::string& from) const;
};

} /* namespace meteo */

#endif /* _UVConverter_h */
