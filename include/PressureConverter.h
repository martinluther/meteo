/*
 * PressureConverter.h -- convert pressure values
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: PressureConverter.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _PressureConverter_h
#define _PressureConverter_h

#include <string>
#include <BasicConverter.h>
#include <PressureValue.h>

namespace meteo {

class	PressureConverter : public BasicConverter {
public:
	PressureConverter(void) : BasicConverter("hPa") { }
	PressureConverter(const std::string& t) : BasicConverter(t) { }
	virtual ~PressureConverter(void) { }
	virtual double	operator()(const double value, const std::string& from) const;
};

} /* namespace meteo */

#endif /* _PressureConverter_h */
