/*
 * Energy.h -- encode Energy Station information
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Energy_h
#define _Energy_h

#include <OldDavisStation.h>
#include <string>

namespace meteo {

class	Energy : public OldDavisStation {
public:
	Energy(const std::string& n);
	virtual	~Energy(void);
};

} /* namespace meteo */

#endif /* _Energy_h */
