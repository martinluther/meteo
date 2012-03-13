/*
 * daemon.c -- common functions that help in becoming a daemon
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: daemon.c,v 1.2 2002/01/09 23:36:06 afm Exp $
 */
#include <daemon.h>
#include <meteo.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int	daemonize(const char *pidfilenamepattern, const char *station) {
	char	*pidfilename;
	int	l;
	pid_t	pid;
	FILE	*pidfile;

	/* fork								*/
	pid = fork();
	if (pid < 0) {
		fprintf(stderr, "%s:%d: cannot fork: %s (%d)\n", __FILE__,
			__LINE__, strerror(errno), errno);
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
		fprintf(stderr, "%s:%d: cannot open pid file. %s (%d)\n",
			__FILE__, __LINE__, strerror(errno), errno);
		return -1;
	}
	fprintf(pidfile, "%d\n", pid = getpid());
	fclose(pidfile);

	/* return our pid to indicate success				*/
	return pid;
}
