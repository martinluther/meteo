/*
 * watchdog.c --watchdog functionality for the meteo package
 *
 * (c) 2002 Dr. Andreas Mueller, Beartung und Entwicklung
 *
 * $Id$ 
 */
#include <watchdog.h>
#include <meteo.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

static char	**wdargv = NULL;

static void	wd_sigalrm(int signum) {
	/* check for the signal that caused the handler to be called	*/
	if (signum != SIGALRM) {
		fprintf(stderr, "%s:%d: signal handler for wrong signal "
			"called\n", __FILE__, __LINE__);
		return;
	}

	/* restart the process with the the arguments in wdargv		*/
	execvp(wdargv[0], wdargv);
	fprintf(stderr, "%s:%d: cannot execute: %s (%d)\n", __FILE__,
		__LINE__, strerror(errno), errno);
}

/*
 * setup the watchdog, but don't arm it yet
 */
int	wd_setup(int argc, char *argv[]) {
	char	**p;
	int	i;

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
			fprintf(stderr, "%s:%d: watchdog arg[%d] = '%s'\n",
				__FILE__, __LINE__, i, wdargv[i]);
	}
	wdargv[argc] = NULL;

	/* install the signal handler					*/
	signal(SIGALRM, wd_sigalrm);
	return 0;
}

/*
 * start the timer, but only if the watchdog has been set up
 */
int	wd_arm(int timeout) {
	if (debug)
		fprintf(stderr, "%s:%d: arming time with timeout %d\n",
			__FILE__, __LINE__, timeout);
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
	fprintf(stderr, "%s:%d: Watchdog event fired: %s\n",
		__FILE__, __LINE__, (reason) ? reason : "(none specified)");

	/* call the signal handler					*/
	wd_sigalrm(SIGALRM);
}
