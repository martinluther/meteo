/*
 * meteoloop.c 
 *
 * read data from the weather station and update the stationdata table
 *
 * (c) 2001 Dr. Andreas Mueller
 *
 * $Id: meteoloop.c,v 1.6 2002/01/30 10:38:24 afm Exp $
 */
#include <stdlib.h>
#include <stdio.h>
#include <mysql/mysql.h>
#include <meteo.h>
#include <meteodata.h>
#include <sys/types.h>
#include <errno.h>
#include <davis.h>
#include <string.h>
#include <sercom.h>
#include <tcpcom.h>
#include <dloop.h>
#include <dbupdate.h>
#include <unistd.h>
#include <database.h>
#include <msgque.h>
#include <daemon.h>
#include <watchdog.h>
#include <printver.h>
#include <mdebug.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

extern int	optind;
extern char	*optarg;


typedef struct loopdata_s {
	const char	*station;
	const char	*url;
	const char	*queuename;
	int		usequeue;
	int		n;
} loopdata_t;

int	minutes;
int	watchinterval = -1;
pid_t	slavepid;

/*
 * the following signal handler ensures that the slave is killed whenever
 * we receive the INT or TERM signal ourselves, 
 */
static void	termhandler(int sig) {
	switch (sig) {
	case SIGTERM:
	case SIGINT:
		if (debug)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"signal %d received, sending to child pid %d",
				sig, slavepid);
		kill(slavepid, sig);
		exit(EXIT_FAILURE);
		break;
	default:
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "termhandler called for "
			"wrong signal %d", sig);
		break;
	}
	return;
}

/*
 * childhandler -- this handler is only here so that we can catch the signal
 *                 SIGCHLD, as we are not allowed to ignore it (Posix.1 says)
 *                 but the default action will kill us.
 */
static void	childhandler(int sig) {
	return;
}

/*
 * the setsighandlers function enables or disables signal handling, so
 * that we are always protected from sig chld and handle the TERM and INT
 * signals only if there really is a client to be killed
 */
static void	setsighandlers(int yesno) {
	signal(SIGCHLD, childhandler);
	if (yesno) {
		signal(SIGINT, termhandler);
		signal(SIGTERM, termhandler);
	} else {
		signal(SIGINT, SIG_DFL);
		signal(SIGTERM, SIG_DFL);
	}
}

/*
 * the updateloop function is quite davis specific. It keeps reading meteo
 * data images for n cycles und continuously updates the data in the
 * meteodata_t record. Every time a minute boundary is traversed, the
 * current data is sent to the database using the dbupdate function.
 */
static int	updateloop(dest_t *ddp, loop_t *l, meteodata_t *md, int n,
	const char *station) {
	time_t	now;
	int	i, nn;

	/* if this is a vantage station, wake it up first		*/
	if (l->m->m->flags & COM_VANTAGE)
		vantage_wakeup(l->m->m);

	/* send loop command to station					*/
	put_string(l->m->m, "LOOP");
	if (l->m->m->flags & COM_VANTAGE) {
		put_char(l->m->m, '1');	/* 1 second interval		*/
		nn = n;
		if (n > 9) nn = 9;
		if (n < 1) nn = 1;
		put_char(l->m->m, '0' + nn);
	} else {
		put_unsigned(l->m->m, 0x10000 - n);
	}
	put_char(l->m->m, 0x0d);
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"command 'LOOP' command written");

	/* wait for acknowledgment					*/
	if (get_acknowledge(l->m->m) < 0) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "no ack received");
		if (watchinterval > 0)
			wd_fire("no ack from station");
		else
			exit(EXIT_FAILURE);
	}

	/* arm the watchdog timer, if set. We do this here so we can	*/
	/* detect a sequence of failed reads. We will reset the watch	*/
	/* on every successfull read					*/
	if (watchinterval > 0)
		wd_arm(watchinterval);

	/* read n reply packets						*/
	for (i = 0; i < n; i++) {

		/* read a new record from the station			*/
		if (debug)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"reading record %d through function %p",
				i, l->read);
		if (l->read(l, md) == 0) {
			if (watchinterval >= 0)
				wd_arm(watchinterval);
		} else {
			mdebug(LOG_ERR, MDEBUG_LOG, 0,
				"retrieving an image failed");
			return -1;
		}
			
		if (debug) {
			meteodata_display(stdout, md);
		}
		time(&now);
		if ((minutes != (now / 60)) && (md->samples > 0)) {
			if (dbupdate(ddp, md, station) < 0) {
				return n - i;
			}
			meteodata_start(md);
			minutes = now / 60;
		}
	}
	/* on an orderly return, disarm the watchdog timer		*/
	wd_disarm();
	return 0;
}

static void	slaveloop(loopdata_t *ld) {
	meteocom_t	*m;
	meteodata_t	*md;
	meteoaccess_t	*d;
	loop_t		*l;
	dest_t		*ddp;
	int		speed = 2400, r;
	struct timeval	tv;
	
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "starting slaveloop with pid %d",
		getpid());

	/* establish the connection to the weather station		*/
	if (0 == strncmp(ld->url, "file://", 7)) {
		speed = mc_get_int(meteoconfig, "station.speed", speed);
		m = sercom_new((char *)ld->url, speed);
	} else {
		/* setting up a TCP connection can be time consuming...	*/
		if (watchinterval > 0)
			wd_arm(watchinterval);
		m = tcpcom_new((char *)ld->url);
		wd_disarm();
	}
	if (m == NULL) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "cannot open device: %s (%d)",
			  strerror(errno), errno);
		exit(EXIT_FAILURE);
	}

	/* if there are any characters in some buffers, let them drain	*/
	tv.tv_sec = 5; tv.tv_usec = 0;
	com_drain(m, &tv);

	/* create a weather station structure from the connection 	*/
	d = davis_new(m);
	if (d == NULL) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "cannot create davis interface");
		exit(EXIT_FAILURE);
	}

	/* create a loop structure for this weather station		*/
	l = dloop_new(d);
	if (l == NULL) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "cannot create loop handler");
		exit(EXIT_FAILURE);
	}

	/* establish the database connection				*/
	ddp = dest_new();
	if (ld->usequeue) {
		ddp->type = DEST_MSGQUE;
		ddp->destdata.msgque = msgque_setup(ld->queuename);
		if (ddp->destdata.msgque < 0) {
			mdebug(LOG_CRIT, MDEBUG_LOG, 0, "cannot open msg queue");
			exit(EXIT_FAILURE);
		}
	} else {
		ddp->type = DEST_MYSQL;
		ddp->destdata.mysql = mc_opendb(meteoconfig, O_WRONLY);
		if (NULL == ddp->destdata.mysql) {
			mdebug(LOG_CRIT, MDEBUG_LOG, 0, "failed to connect to "
				"database");
			exit(EXIT_FAILURE);
		}
	}

	/* initialize accumulator data					*/
	md = meteodata_new();
	meteodata_start(md);

	/* initialize the minute counter				*/
	minutes = time(NULL) / 60;

	/* send loop command to station					*/
	while (1) {
		r = updateloop(ddp, l, md, ld->n, ld->station);
		if (debug)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"did %d rounds of updates", ld->n - r);
		if (r != 0) {
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"failed with %d remaining updates", r);
			if (watchinterval >= 0)
				wd_fire("updateloop function failed");
			else {
				mdebug(LOG_CRIT, MDEBUG_LOG, 0,
					"loop failed, exiting");
				exit(EXIT_FAILURE);
			}
		}
		wd_disarm();
	}

	/* close the connection to the database				*/
	dest_free(ddp);
}

/*
 * masterloop
 *
 * this function forks a child and returns only if the child has died
 */
static void	masterloop(loopdata_t *l) {
	int	status;

	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "starting master loop");

	/* fork a child							*/
	slavepid = fork();
	if (slavepid < 0) {
		mdebug(LOG_CRIT, MDEBUG_LOG, MDEBUG_ERRNO,
			"cannot fork");
		exit(EXIT_FAILURE);
	}

	/* make sure the child is in a new process group. Otherwise	*/
	/* signals generated by the serial line over which the process	*/
	/* is communicating may reach the parent, and kill it, and this	*/
	/* is precisely what we would like to prevent with this master/	*/
	/* slave stuff							*/

	/* the parent waits for the child				*/
	if (slavepid > 0) {
		/* parent sets the pgid of the slave to slave's pid	*/
		setpgid(slavepid, slavepid);

		/* set signal handlers					*/
		setsighandlers(1);

		/* wait for the child					*/
		if (waitpid(slavepid, &status, 0) < 0) {
			/* ignore the ECHILD error, which happens if we	*/
			/* SIG_IGN the CHLD signal, and the child dies	*/
			/* also ignore the EINTR error			*/
			switch (errno) {
			case ECHILD:
			case EINTR:
				break;
			default:
				mdebug(LOG_CRIT, MDEBUG_LOG, MDEBUG_ERRNO,
					"fatal error during wait");
				exit(EXIT_FAILURE);
			}
		}

		/* as the child really should not have exited at all	*/
		/* log an error message					*/
		mdebug(LOG_ERR, MDEBUG_LOG, 0,
			"child %d exited with status %d", slavepid,
			WEXITSTATUS(status));
		if (WIFSIGNALED(status)) {
			mdebug(LOG_ERR, MDEBUG_LOG, 0,
				"exit caused by signal %d",
				WTERMSIG(status));
		}
		setsighandlers(0);

		/* the parent returns as soon as it has waited for the	*/
		/* child						*/
		return;
	}

	/* the child simply calls the slave part			*/
	if (slavepid == 0) {
		/* slave sets its pgid to its own pid			*/
		setpgid(getpid(), getpid());

		/* execute the slave loop				*/
		slaveloop(l);
		exit(EXIT_FAILURE);
	}
}

/*
 * the main function of the meteoloop program simply calls the updateloop
 * function again and again
 */
int	main(int argc, char *argv[]) {
	const char	*url = NULL,
			*station = "Altendorf",
			*queuename = NULL;
	char		*conffilename = METEOCONFFILE;
	loopdata_t	ld;
	int		c, foreground = 0, usemaster = 0;
	char		*pidfilename = "/var/run/meteoloop-%s.pid";

	/* do some initializations that cannot be done in the 		*/
	/* declarations							*/
	ld.usequeue = 0;
	ld.n = 1;

	/* parse command line arguments					*/
	while (EOF != (c = getopt(argc, argv, "dn:Ff:l:p:qw:Vm")))
		switch (c) {
		case 'l':
			if (mdebug_setup("meteoloop", optarg) < 0) {
				fprintf(stderr, "%s cannot init log\n",
					argv[0]);
				exit(EXIT_FAILURE);
			}
			break;
		case 'd':
			debug++;
			break;
		case 'f':
			conffilename = optarg;
			break;
		case 'F':
			foreground = 1;
			break;
		case 'n':
			ld.n = atoi(optarg);
			if ((ld.n <= 0) || (ld.n > 65535)) {
				mdebug(LOG_CRIT, MDEBUG_LOG, 0,
					"illegal number of repeats: %d", ld.n);
				exit(EXIT_FAILURE);
			}
			break;
		case 'q':
			ld.usequeue = 1;
			if (debug)
				mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"updates via msgque requested");
			break;
		case 'p':
			pidfilename = optarg;
			break;
		case 'w':
			watchinterval = atoi(optarg);
			break;
		case 'V':
			fprintver(stdout);
			exit(EXIT_SUCCESS);
			break;
		case 'm':
			usemaster = 1;
			break;
		}

	/* prepare the watchdog timer					*/
	if (watchinterval > 0) {
		if (debug)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"set up the watchdog timer");
		wd_setup(0, NULL);
	}

	/* it is an error not to specify a configuration file		*/
	if (NULL == conffilename) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "must specify -f <config>");
		exit(EXIT_FAILURE);
	}

	/* open the configuration file 					*/
	if (NULL == (meteoconfig = mc_readconf(conffilename))) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "configuration %s invalid", 
			 conffilename);
		exit(EXIT_FAILURE);
	}

	/* extract the information from the configuration data		*/
	ld.url = mc_get_string(meteoconfig, "station.url", url);
	ld.station = mc_get_string(meteoconfig, "database.prefix", station);
	ld.queuename = mc_get_string(meteoconfig, "database.msgqueue",
		queuename);

	/* become a daemon						*/
	if (!foreground) {
		switch (daemonize(pidfilename, station)) {
		case -1:
			mdebug(LOG_CRIT, MDEBUG_LOG, 0, "failed to daemonize");
			exit(EXIT_FAILURE);
			break;
		case 0:
			/* inside parent				*/
			exit(EXIT_SUCCESS);
			break;
		default:
			break;
		}
	}

	/* start the master/slave stuff (or simply the slave, if the	*/
	/* master is not required					*/
	if (usemaster) {
		while (1) {
			masterloop(&ld);
		}
	} else {
		slaveloop(&ld);
	}

	exit(EXIT_SUCCESS);
}
