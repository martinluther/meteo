/*
 * SerialChannel.cc -- encapsulate the communication logic 
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 *
 * $Id: SerialChannel.cc,v 1.3 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <SerialChannel.h>
#include <Configuration.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif /* HAVE_STDIO_H */
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif /* HAVE_STRINGS_H */
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif /* HAVE_ERRNO_H */
#include <MeteoException.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif /* HAVE_FCNTL_H */
#ifdef HAVE_TERMIOS_H
#include <termios.h>
#endif /* HAVE_TERMIOS_H */
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif /* HAVE_SYS_SOCKET_H */
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif /* HAVE_NETINET_IN_H */
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif /* HAVE_NETDB_H */
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif /* HAVE_ALLOCA_H */
#include <Timeval.h>
#include <mdebug.h>

namespace meteo {

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

} /* namespace meteo */
