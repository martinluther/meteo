/*
 * Channel.h -- abstract communication device to talk to the station
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Channel_h
#define _Channel_h

#include <string>
#include <Timeval.h>

namespace meteo {

class Channel {
protected:
	int	f;
	Timeval	t;
public:
	Channel(int delay = 5) : t(delay) { f = -1; }
	virtual	~Channel(void);

	// send operations
	virtual void	sendChar(unsigned char c);
	virtual void	sendString(const std::string& s);

	// receive operations
	void	drain(int delay);
	char	recvChar(void);
	std::string	recvString(int len);
};

} /* namespace meteo */

#endif /* _Channel_h */

