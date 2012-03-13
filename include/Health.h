/*
 * Health.h -- encode Weather Monitor II Station information
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
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
