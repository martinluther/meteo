/*
 * SerialChannel.h -- abstract communication device to talk to the station
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _SerialChannel_h
#define _SerialChannel_h

#include <Channel.h>
#include <string>

namespace meteo {

class SerialChannel : public Channel {
public:
	SerialChannel(void) { }
	SerialChannel(const std::string& device, int baudrate);
	virtual	~SerialChannel(void);
};

} /* namespace meteo */

#endif /* _SerialChannel_h */

