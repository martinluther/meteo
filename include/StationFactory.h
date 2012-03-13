/*
 * StationFactory.h -- create a Station based on the configuration file and
 *                     the station name
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _StationFactory_h
#define _StationFactory_h

#include <Station.h>
#include <string>

namespace meteo {

class	StationFactory {
public:
	StationFactory() { }
	// the newStation returns a (connected) station
	Station	*newStation(const std::string& name) const;
};

} /* namespace meteo */

#endif /* _StationFactory_h */
