/*
 * OldDavisStation.h -- encode DavisStation information, i.e. behavior specific
 *                      to Old Davis stations.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _OldDavisStation_h
#define _OldDavisStation_h

#include <string>
#include <DavisStation.h>

namespace meteo {

class	OldDavisStation : public DavisStation {
protected:
	// reading station data (for calibration numbers)
	virtual std::string	readBytes(int bank, int offset, int count);
public:
	// construction
	OldDavisStation(const std::string& n, int ps);
	~OldDavisStation(void) { }

	// starting a loop for a Davis station is slightly different
	// so we override the Station.startLoop method here
	virtual void	startLoop(int p);
};

} /* namespace meteo */

#endif /* _OldDavisStation_h */
