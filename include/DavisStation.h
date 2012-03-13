/*
 * DavisStation.h -- encode DavisStation information, i.e. behavior specific
 *                   to Davis stations.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: DavisStation.h,v 1.3 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _DavisStation_h
#define _DavisStation_h

#include <string>
#include <Station.h>

namespace meteo {

class	DavisStation : public Station {
	int	packetsize;
public:
	// construction
	DavisStation(const std::string& n, int ps)
		: Station(n), packetsize(ps) { }
	~DavisStation(void) { }

	// accessor for packetsize
	int	getPacketsize(void) const { return packetsize; }

	// starting a loop for a Davis station is slightly different
	// so we override the Station.startLoop method here
	virtual void	startLoop(int p);
	virtual std::string	readPacket(void);
};

#define ACK	6

} /* namespace meteo */

#endif /* _DavisStation_h */
