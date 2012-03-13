/*
 * VoltageConverter.h -- convert voltage values
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: VoltageConverter.h,v 1.2 2004/02/25 23:52:35 afm Exp $
 */
#ifndef _VoltageConverter_h
#define _VoltageConverter_h

#include <string>
#include <BasicConverter.h>
#include <VoltageValue.h>

namespace meteo {

class	VoltageConverter : public BasicConverter {
public:
	VoltageConverter(void) : BasicConverter("V") { }
	VoltageConverter(const std::string& unit) : BasicConverter(unit) { }
	virtual ~VoltageConverter(void) { }
	virtual double	operator()(const double value, const std::string& from) const;
};

} /* namespace meteo */

#endif /* _VoltageConverter_h */
