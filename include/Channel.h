/*
 * Channel.h -- abstract communication device to talk to the station
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Channel_h
#define _Channel_h

#include <string>
#include <Timeval.h>
#include <Configuration.h>

namespace meteo {

class Channel {
protected:
	int	f;
	Timeval	t;
public:
	Channel(void) : t(5) { f = -1; }
	virtual	~Channel(void);

	// send operations
	virtual void	sendChar(unsigned char c);
	virtual void	sendString(const std::string& s);

	// receive operations
	void	drain(int delay);
	char	recvChar(void);
	std::string	recvString(int len);
};

class FileChannel : public Channel {
public:
	FileChannel(void);
	FileChannel(const std::string& filename);
	virtual	~FileChannel(void);
	virtual void	sendChar(unsigned char c);
	virtual void	sendString(const std::string& s);
};

class SerialChannel : public Channel {
public:
	SerialChannel(void) { }
	SerialChannel(const std::string& device, int baudrate);
	virtual	~SerialChannel(void);
};

class TcpChannel : public Channel {
public:
	TcpChannel(void) { }
	TcpChannel(const std::string& url);
	virtual	~TcpChannel(void);
};

class	ChannelFactory {
	Configuration	conf;
public:
	ChannelFactory(const Configuration& c) : conf(c) { }
	~ChannelFactory(void) { }
	Channel	*newChannel(const std::string& name); // channel to named station
};

} /* namespace meteo */

#endif /* _Channel_h */

