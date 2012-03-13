/*
 * Pidfile.cc -- C++ implementation of pid file
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Pidfile.cc,v 1.7 2009/01/10 19:00:24 afm Exp $
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
#include <Pidfile.h>
#include <fstream>
#include <iostream>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif /* HAVE_SYS_TYPES_H */
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif /* HAVE_SYS_STAT_H */
#ifdef HAVE_SIGNAL_H
#include <signal.h>
#endif /* HAVE_SIGNAL_H */
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif /* HAVE_ERRNO_H */
#include <mdebug.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#include <MeteoException.h>

namespace meteo {

Pidfile::Pidfile(const std::string& filename) {
	int	pid = -1;

	// let's see whether the file exists, we do this using stat
	struct stat	sb;
	if (stat(filename.c_str(), &sb) < 0) {
		switch (errno) {
		case ENOENT:
			// ENOENT means that the file does not exist,
			// so we just forget about it
			goto forgetoldpid;
			break;
		case ENOTDIR:
		case ELOOP:
		case EACCES:
		case ENAMETOOLONG:
			// any of these errors indicates a serious
			// problem with the file name
			throw MeteoException(strerror(errno),
				filename.c_str());
			break;
		}
	}

	// so we know the file exists, but can we do something with it?
	{	// the { is only here to create a scope, so that the ifstream
		// will get destroyed when going out of scope
		std::ifstream	oldpidfile(filename.c_str());
		if (!oldpidfile) {
			// the file was there, but we could not open it, so
			// we are in serious trouble
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"cannot open pid file %s", filename.c_str());
			throw MeteoException("cannot open pidfile", filename);
		}

		// if it does, read the pid from it (note that we have to
		// clear the oldpidfile stream state first)
		oldpidfile.clear();
		oldpidfile >> pid;

		// check the state of the oldpidfile to make sure a number
		// was read
		if (oldpidfile.fail()) {
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"old pid file did not contain a pid");
			goto forgetoldpid;
		}
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "pid set to %d", pid);
	};

	// check whether a process with this pid exists, we can do this since
	// we have made sure previously that we could read a reasonable pid 
	// from the file
	if (kill(pid, 0) < 0) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"cannot kill proccess %d: %s", pid,
			strerror(errno));
		switch (errno) {
		case ESRCH:
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"process gone, proceed");
			break;
		case EPERM:
			// give up if we cannot kill the process in
			// question
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"process has different uid");
			throw MeteoException("not allowed to kill old process",
				"");
		default:
			break;
		}
	}

	// create a new pid file
forgetoldpid:
	std::ofstream	pidfile(filename.c_str());
	if (!pidfile) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "cannot write pid file %s",
			filename.c_str());
		throw MeteoException("cannot write pid", filename);
	}

	// write pid to the pidfile
	pidfile << getpid() << std::endl;
}

} /* namespace meteo */
