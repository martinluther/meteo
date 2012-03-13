/*
 * daemon.c -- common functions that help in becoming a daemon
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: daemon.c,v 1.2 2002/01/27 21:01:42 afm Exp $
 */
#include <daemon.h>
#include <meteo.h>
#include <string.h>
#include <unistd.h>
#include <mdebug.h>
#include <sys/types.h>
#include <sys/stat.h>

int	daemonize(const char *pidfilenamepattern, const char *station) {
	char	*pidfilename;
	int	l;
	pid_t	pid;
	FILE	*pidfile;

	/* fork								*/
	pid = fork();
	if (pid < 0) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, MDEBUG_ERRNO, "cannot fork");
		return -1;
	}
	if (pid > 0) {
		return 0;
	}

	/* start a new session						*/
	setsid();

	/* switch to the / directory					*/
	chdir("/");

	/* set a good umask						*/
	umask(022);

	/* compute the pidfile name					*/
	l = strlen(pidfilenamepattern) + ((station) ? strlen(station) : 1) + 1;
	pidfilename = (char *)alloca(l);
	if (station)
		snprintf(pidfilename, l, pidfilenamepattern, station);
	else
		snprintf(pidfilename, l, "%s", pidfilenamepattern);

	/* write our pid to the file					*/
	if (NULL == (pidfile = fopen(pidfilename, "w"))) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, MDEBUG_ERRNO,
			"cannot open pid file");
		return -1;
	}
	fprintf(pidfile, "%d\n", pid = getpid());
	fclose(pidfile);

	/* return our pid to indicate success				*/
	return pid;
}
