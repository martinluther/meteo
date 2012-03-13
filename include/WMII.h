/*
 * WMII.h -- encode Weather Monitor II Station information
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _WMII_h
#define _WMII_h

#include <OldDavisStation.h>
#include <string>

namespace meteo {

class	WMII : public OldDavisStation {
public:
	WMII(const std::string& n);
	virtual	~WMII(void);
};

} /* namespace meteo */

#endif /* _WMII_h */
