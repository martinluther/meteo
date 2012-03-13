/*
 * Channel.cc -- encapsulate the communication logic 
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 *
 * $Id: Channel.cc,v 1.13 2009/01/10 19:00:23 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <Channel.h>
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
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif /* HAVE_ALLOCA_H */
#include <Timeval.h>
#include <mdebug.h>

namespace meteo {

Channel::~Channel(void) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "closing channel %d", f);
	if ((f >= 0) && (f != fileno(stdin))) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "closing file %d", f);
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
	if (s.length() != (size_t)write(f, s.c_str(), s.length())) {
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

} /* namespace meteo */
