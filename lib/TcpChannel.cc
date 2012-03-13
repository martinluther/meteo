/*
 * TcpChannel.cc -- encapsulate the communication logic 
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 *
 * $Id: TcpChannel.cc,v 1.7 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif /* HAVE_STDIO_H */
#include <TcpChannel.h>
#include <Configuration.h>
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
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif /* HAVE_SYS_SOCKET_H */
#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif /* HAVE_NETINET_IN_H */
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif /* HAVE_NETDB_H */
#include <Timeval.h>
#include <mdebug.h>

namespace meteo {

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
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "connect failed: %s",
			strerror(errno));
		// wait e few seconds before retrying, or you may
		// overwhelm the system
		sleep(5);
		throw MeteoException("cannot connect", strerror(errno));
	}
	f = s;

	// drain the channel for 5 seconds
	drain(5);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "TcpChannel created");
}

TcpChannel::~TcpChannel(void) {
	if (f >= 0) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"closing TCP connection, waiting for 3 seconds");
		shutdown(f, 2);
		// some terminal servers have problems with not shutting
		// down the connection quickly enough, so we give it a few
		// seconds
		sleep(3);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "TCP channel closed");
	}
}

} /* namespace meteo */
