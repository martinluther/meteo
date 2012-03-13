/*
 * Daemon.cc -- common functions that help in becoming a daemon
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Daemon.cc,v 1.1 2003/10/18 07:33:04 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h> /* some systems have alloca defined in stdlib.h */
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif /* HAVE_ALLOCA_H */
#include <mdebug.h>
#include <MeteoException.h>
#include <Daemon.h>
#include <fstream>
#include <Pidfile.h>

namespace meteo {


Daemon::Daemon(const std::string& pidfileprefix, const std::string& station,
	bool nofork) {
	pid_t	pid;

	// decide wether to fork or not
	if (nofork) {
		// simulating a child return
		pid = 0;
	} else {
		// do a real fork
		pid = fork();
		if (pid < 0) {
			mdebug(LOG_DEBUG, MDEBUG_LOG, MDEBUG_ERRNO,
				"cannot fork");
			throw MeteoException("cannot fork", strerror(errno));
		}
	}

	// the positive pid returned is the child pid, so this is actually
	// the parent
	if (pid > 0) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "returning as parent, "
			"and exiting");
		exit(EXIT_SUCCESS);
	}
	// if we get to this point, we are a child, and the pid must be 0
	//assert(pid == 0);

	// start a new session
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "startin new child session");
	setsid();

	// switch to the / directory
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "chdir to /");
	chdir("/");

	// set a good umask
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "switch umask to 022");
	umask(022);

	// compute the pidfile name
	if (station.empty())
		pidfilename = pidfileprefix;
	else
		pidfilename = pidfileprefix + station + "pid";
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "pid file is %s", pidfilename.c_str());

	// write our pid to the file
	Pidfile	pf(pidfilename);
}

// the destructor is supposed to remove the pid file, but only if it contains
// the pid of the current process
Daemon::~Daemon(void) {
	// first check whether the pid file exists
	std::ifstream	pidfile(pidfilename.c_str());
	if (!pidfile) {
		mdebug(LOG_INFO, MDEBUG_LOG, 0, "cannot open pid file %s",
			pidfilename.c_str());
		return;
	}

	// read the pid from the pidfile
	int	pid;
	pidfile >> pid;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "found pid = %d", pid);

	// compare to our own pid
	if (pid == getpid()) {
		if (unlink(pidfilename.c_str()) < 0) {
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"cannot unlink pid file %s: %s",
				pidfilename.c_str(), strerror(errno));
		}
	}
}

} /* namespace meteo */
