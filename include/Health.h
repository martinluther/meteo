/*
 * Health.h -- encode Weather Monitor II Station information
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Health.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _Health_h
#define _Health_h

#include <OldDavisStation.h>
#include <string>

namespace meteo {

class	Health : public OldDavisStation {
public:
	Health(const std::string& n);
	virtual	~Health(void);
};

} /* namespace meteo */

#endif /* _Health_h */
