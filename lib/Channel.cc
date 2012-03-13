/*
 * Channel.cc -- encapsulate the communication logic 
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 */
#include <Channel.h>
#include <strings.h>
#include <errno.h>
#include <MeteoException.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <Timeval.h>
#include <mdebug.h>

namespace meteo {

#define	LOOPLIMIT	10

Channel::~Channel(void) {
	if ((f >= 0) && (f != fileno(stdin))) {
		close(f);
	}
}

void	Channel::sendChar(unsigned char c) {
	if (f == fileno(stdin)) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "cannot write to stdin");
		return;
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "writing char %02x '%c'", c, c);
	if (1 != write(f, &c, 1)) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "failed to write one char: %s",
			strerror(errno));
		throw MeteoException("cannot send character", strerror(errno));
	}
}
void	Channel::sendString(const std::string& s) {
	if (f == fileno(stdin)) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "cannot write to stdin");
		return;
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "writing %d char string '%s'",
		s.length(), s.c_str());
	if (s.length() != write(f, s.c_str(), s.length())) {
		throw MeteoException("cannot send complete string",
			strerror(errno));
	}
}

char	Channel::recvChar(void) {
	// select on the file descriptor
	struct timeval	delay = t.timeval();
	char		b[1];
	fd_set		readfds;
	FD_ZERO(&readfds);
	FD_SET(f, &readfds);
	switch (select(f + 1, &readfds, NULL, NULL, &delay)) {
	case 0:	// timeout
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "read timeout");
		throw MeteoException("timeout in read", "");
		break;
	case 1:	// at least one character is ready for reading
		if (debug > 1)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"reading char ready on port");
		if (read(f, b, 1) == 1) {
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "got char '%02x'",
				(unsigned char)b[0]);
			return b[0];
		}
	default:
		// some error occured
		throw MeteoException("cannot select for/read a character",
			strerror(errno));
		break;
	}
}

std::string	Channel::recvString(int len) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "request for %d bytes", len);
	// add timed read functionality
	char	*b;
	b = (char *)alloca(len);
	for (int i = 0; i < len; i++) {
		b[i] = recvChar();
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "received %d bytes", len);
	return std::string(b, len);
}

// new drain algorithm: keep reading until the station is quiet for at
// least delay seconds
void	Channel::drain(int delay) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "drain(%d) called", delay);
	// don't drain stdin
	if ((f < 0) || (f == fileno(stdin))) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "not draining stdin");
		return;
	}

	// select for at most the time specified by the caller
	Timeval	dv(delay);
	int	bytes;
	while (true) {
		struct timeval	d = dv.timeval();
		// select the file descriptor
		fd_set	readfds;
		FD_ZERO(&readfds);
		FD_SET(f, &readfds);
		switch (select(f + 1, &readfds, NULL, NULL, &d)) {
		case 0:	// timeout
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "drain complete");
			return;
			break;
		case 1: // data available
			char	buffer[1024];
			bytes = read(f, buffer, sizeof(buffer));
			if (bytes <= 0) {
				mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"no more data to drain");
				return;
			}
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%d bytes drained",
				bytes);
			break;
		default:
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "select failed: %s",
				strerror(errno));
			break;
		}
	}
}

FileChannel::FileChannel(void) {
	// use standard input
	f = fileno(stdin);
}
FileChannel::FileChannel(const std::string& filename) {
	if ((f = open(filename.c_str(), O_RDONLY)) < 0) {
		// throw an IO exception
		throw MeteoException("cannot open file", filename);
	}
}
FileChannel::~FileChannel(void) { }

void	FileChannel::sendChar(unsigned char c) { }
void	FileChannel::sendString(const std::string& s) { }

SerialChannel::SerialChannel(const std::string& device, int baudrate) {
	struct termios	term;
	unsigned int	speedconst;

	// open the device
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "opening device %s", device.c_str());
	f = open(device.c_str(), O_RDWR|O_NOCTTY);
	if (f < 0) {
		throw MeteoException("cannot open device", device.c_str());
	}

	// this device must be e terminal
	if (!isatty(f)) {
		close(f);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "device %s ist no a tty",
			device.c_str());
		MeteoException("device ist not a tty", device);
	}

	// get the term settings
	tcgetattr(f, &term);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "read terminal data for %s",
		device.c_str());
	term.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	term.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	term.c_cflag &= ~(CSIZE | PARENB);
	term.c_cflag |= CS8 | CREAD | CLOCAL;
	term.c_oflag &= ~(OPOST);
	term.c_cc[VMIN] = 1;
	term.c_cc[VTIME] = 0;

	// set the speed according to the baudrate argument
	switch (baudrate) {
	case 50:	speedconst = B50;	break;
	case 75:	speedconst = B75;	break;
	case 110:	speedconst = B110;	break;
	case 134:	speedconst = B134;	break;
	case 150:	speedconst = B150;	break;
	case 200:	speedconst = B200;	break;
	case 300:	speedconst = B300;	break;
	case 600:	speedconst = B600;	break;
	case 1200:	speedconst = B1200;	break;
	case 2400:	speedconst = B2400;	break;
	case 4800:	speedconst = B4800;	break;
	case 9600:	speedconst = B9600;	break;
	case 19200:	speedconst = B19200;	break;
	case 38400:	speedconst = B38400;	break;
	default:
		throw MeteoException("illegal baudrate", "");
		break;
	}
	cfsetispeed(&term, speedconst);
	cfsetospeed(&term, speedconst);
	if (tcsetattr(f, TCSANOW, &term) < 0) {
		int	e = errno;
		close(f);
		throw MeteoException("failed to set term attributes",
			strerror(e));
	}

	// check whether baud rate has actually been set
	if (cfgetispeed(&term) != speedconst) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "ispeed not correctly set");
		close(f);
		throw MeteoException("ispeed not set correctly", "");
	}
	if (cfgetospeed(&term) != speedconst) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "ospeed not correctly set");
		close(f);
		throw MeteoException("ispeed not set correctly", "");
	}

	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "speed set to %d", baudrate);

	// drain the channel for 5 seconds
	drain(5);
}

SerialChannel::~SerialChannel(void) { }

TcpChannel::TcpChannel(const std::string& url) {
	// create a socket
	int	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		throw MeteoException("cannot create a socket", strerror(errno));
	}

	// initialize the socket address
	struct sockaddr_in	sin;
	sin.sin_family = AF_INET;

	// extract address information from the URL
	std::string	hostname = url.substr(6);
	std::string::size_type	ci = hostname.find(":");
	if (ci == std::string::npos) {
		throw MeteoException("colon in TCP url mandatory", url);
	}
	std::string	port = hostname.substr(ci + 1);
	hostname = hostname.substr(0, ci);

	// resolve the hostname
	struct hostent	*hep = gethostbyname(hostname.c_str());
	if (NULL == hep) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "host %s not found",
			hostname.c_str());
		MeteoException("host not found", hostname);
	}
	memcpy(&sin.sin_addr.s_addr, hep->h_addr, sizeof(in_addr_t));

	// convert port number from string to short
	unsigned short	ps = atoi(port.c_str());
	sin.sin_port = htons(ps);

	// connect to the server
	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		close(s);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "sleep for 5 seconds");
		sleep(5);
		throw MeteoException("cannot connect", strerror(errno));
	}
	f = s;

	// drain the channel for 5 seconds
	drain(5);
}

TcpChannel::~TcpChannel(void) { }

Channel	*ChannelFactory::newChannel(const std::string& name) {
	std::string	url = conf.getString("/meteo/station[@name='"
		+ name + "']/url", "undefined");

	// find out what type of a device the url describes
	Channel	*result = NULL;
	if (url.substr(0, 4) == "tcp:") {
		// TCP case
		result	= new TcpChannel(url);
	} else if (url.substr(0, 5) == "file:") {
		// try to guess the baud rate from the type
		std::string	type = conf.getString("/meteo/station[@name='"
			+ name + "']/type", "");
		int	baudrate;
		if (type == "WMII") {
			baudrate = 2400;
		} else if (type == "VantagePro") {
			baudrate = 19200;
		} else {
			baudrate = 9600;
		}
		baudrate = conf.getInt("/meteo/station[@name='"
			+ name + "']/speed", baudrate);

		// get the device name
		std::string	device = url.substr(7);
		result = new SerialChannel(device, baudrate);
	} else if (url.substr(0, 6) == "image:") {
		std::string	file = url.substr(8);
		result = new FileChannel(file);
	} else {
		throw MeteoException("unknown URL type", url);
	}
	return result;
}

} /* namespace meteo */
