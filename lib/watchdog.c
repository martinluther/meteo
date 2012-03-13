/*
 * watchdog.c --watchdog functionality for the meteo package
 *
 * (c) 2002 Dr. Andreas Mueller, Beartung und Entwicklung
 *
 * $Id: watchdog.c,v 1.4 2002/01/30 10:38:24 afm Exp $ 
 */
#include <watchdog.h>
#include <meteo.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <mdebug.h>

static char	**wdargv = NULL;

static void	wd_sigalrm(int signum) {
	/* check for the signal that caused the handler to be called	*/
	if (signum != SIGALRM) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0,
			"signal handler for wrong signal called");
		return;
	}

	/* restart the process with the the arguments in wdargv		*/
	if (NULL != wdargv) {
		execvp(wdargv[0], wdargv);
		mdebug(LOG_CRIT, MDEBUG_LOG, MDEBUG_ERRNO, "cannot execute");
	} else {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "no command line, exiting");
	}
	exit(EXIT_FAILURE);
}

/*
 * setup the watchdog, but don't arm it yet
 */
int	wd_setup(int argc, char *argv[]) {
	char	**p;
	int	i;

	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "installing watchdog");

	/* install the signal handler					*/
	signal(SIGALRM, wd_sigalrm);

	/* argv must not be NULL					*/
	if ((argc == 0) || (argv == NULL))
		return -1;

	/* clean out a potential old argument vector			*/
	if (wdargv != NULL)
		for (p = wdargv; *p; p++)
			free(*p);

	/* remember argv and argc					*/
	wdargv = (char **)malloc((argc + 1) * sizeof(char *));
	for (i = 0; i < argc; i++) {
		wdargv[i] = strdup(argv[i]);
		if (debug)
			mdebug(LOG_CRIT, MDEBUG_LOG, 0,
				"watchdog arg[%d] = '%s'", i, wdargv[i]);
	}
	wdargv[argc] = NULL;

	return 0;
}

/*
 * start the timer, but only if the watchdog has been set up
 */
int	wd_arm(int timeout) {
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "arming time with timeout %d",
			timeout);
	if (timeout >= 0)
		return alarm(timeout);
	return timeout;
}

int	wd_disarm(void) {
	return wd_arm(0);
}

/*
 * fire the alarm
 */
void	wd_fire(const char *reason) {
	/* log a message						*/
	mdebug(LOG_CRIT, MDEBUG_LOG, 0, "Watchdog event fired: %s",
		(reason) ? reason : "(none specified)");

	/* call the signal handler					*/
	wd_sigalrm(SIGALRM);
}
