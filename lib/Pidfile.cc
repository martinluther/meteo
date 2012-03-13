/*
 * Pidfile.cc -- C++ implementation of pid file
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <Pidfile.h>
#include <fstream>
#include <iostream>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#include <signal.h>
#include <errno.h>
#include <mdebug.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <MeteoException.h>

namespace meteo {

Pidfile::Pidfile(const std::string& filename) {
	// check whether this file already exists
	try {
		std::ifstream	oldpidfile(filename.c_str());
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "pid file %s already exists",
			filename.c_str());

		// if it does, read the pid from it
		int	pid;
		oldpidfile >> pid;
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "old pid file contains pid %d",
			pid);

		// check whether a process with this pid exists
		if (kill(pid, 0) < 0) {
			switch (errno) {
			case ESRCH:
				mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"process no longer there, fine");
				break;
			case EPERM:
				mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"process has different uid");
				throw MeteoException("permission", "");
			default:
				break;
			}
		}
	} catch (...) {
		throw MeteoException("there is a problem with the pidfile",
			filename);
	}

	// create a new pid file
	std::ofstream	pidfile(filename.c_str());
	pidfile << getpid() << std::endl;
}

} /* namespace meteo */
