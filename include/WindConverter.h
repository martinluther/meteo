/*
 * WindConverter.h -- convert wind values
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: WindConverter.h,v 1.2 2004/02/25 23:52:35 afm Exp $
 */
#ifndef _WindConverter_h
#define _WindConverter_h

#include <string>
#include <BasicConverter.h>
#include <Wind.h>

namespace meteo {

class	WindConverter : public BasicConverter {
public:
	WindConverter(void) : BasicConverter("m/s") { }
	WindConverter(const std::string& t) : BasicConverter(t) { }
	virtual ~WindConverter(void) { }
	virtual double	operator()(const double value, const std::string& from) const;
};

} /* namespace meteo */

#endif /* _WindConverter_h */
