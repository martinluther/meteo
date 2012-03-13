/*
 * TcpChannel.h -- abstract communication device to talk to the station
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TcpChannel.h,v 1.2 2004/02/25 23:52:35 afm Exp $
 */
#ifndef _TcpChannel_h
#define _TcpChannel_h

#include <Channel.h>
#include <string>

namespace meteo {

class TcpChannel : public Channel {
public:
	TcpChannel(void) { }
	TcpChannel(const std::string& url);
	virtual	~TcpChannel(void);
};

} /* namespace meteo */

#endif /* _TcpChannel_h */

