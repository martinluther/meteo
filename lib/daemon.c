/*
 * daemon.c -- common functions that help in becoming a daemon
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: daemon.c,v 1.6 2003/06/12 23:29:46 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <daemon.h>
#include <meteo.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h> /* some systems have alloca defined in stdlib.h */
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <mdebug.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif /* HAVE_ALLOCA_H */

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
