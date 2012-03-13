/*
 * SerialChannel.h -- abstract communication device to talk to the station
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: SerialChannel.h,v 1.2 2004/02/25 23:52:35 afm Exp $
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

