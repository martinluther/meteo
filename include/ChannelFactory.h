/*
 * ChannelFactory.h -- create a channel
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: ChannelFactory.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _ChannelFactory_h
#define _ChannelFactory_h

#include <string>
#include <Channel.h>

namespace meteo {

class	ChannelFactory {
public:
	ChannelFactory(void) { }
	~ChannelFactory(void) { }
	Channel	*newChannel(const std::string& name); //channel to named station
};

} /* namespace meteo */

#endif /* _ChannelFactory_h */

