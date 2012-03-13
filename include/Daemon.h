/*
 * Daemon.h -- class that encapsulates the daemonization station
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Daemon.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _Daemon_h
#define _Daemon_h

#include <string>

namespace meteo {

class Daemon {
	std::string	pidfilename;
public:
	Daemon(const std::string& pidfileprefix, const std::string& station,
		bool foreground = false);
	~Daemon(void);
};

} /* namespace meteo */

#endif /* _Daemon_h */
