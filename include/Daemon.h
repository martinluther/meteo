/*
 * Daemon.h -- class that encapsulates the daemonization station
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
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
