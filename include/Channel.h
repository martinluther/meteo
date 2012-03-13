/*
 * Channel.h -- abstract communication device to talk to the station
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Channel.h,v 1.8 2006/05/07 19:47:22 afm Exp $
 */
#ifndef _Channel_h
#define _Channel_h

#include <string>
#include <Timeval.h>

namespace meteo {

#define	DEFAULT_TIMEOUT	6	// 5 seconds seems to be too short for some
				// stations to work reliably

class Channel {
protected:
	int	f; // file descriptor for connection
	Timeval	t;
public:
	Channel(int delay = DEFAULT_TIMEOUT) : t(delay) { f = -1; }
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

