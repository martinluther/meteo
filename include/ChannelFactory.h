/*
 * ChannelFactory.h -- create a channel
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
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

