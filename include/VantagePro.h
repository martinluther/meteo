/*
 * VantagePro.h -- encode Vantage Pro Station information
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _VantagePro_h
#define _VantagePro_h

#include <DavisStation.h>
#include <string>

namespace meteo {

class	VantagePro : public DavisStation {
public:
	VantagePro(const std::string& n);
	virtual	~VantagePro(void);

	// loop control
	virtual void	startLoop(int p);

	// wake up station
	void	wakeup(int iterations = 10);

	// read eprom data
	std::string	eeprom(const int offset, const int bytes);

	// read a packet
	virtual std::string	readPacket(void);
};

} /* namespace meteo */

#endif /* _VantagePro_h */
