/*
 * meteoloop.c 
 *
 * read data from the weather station and update the stationdata table
 *
 * (c) 2001 Dr. Andreas Mueller
 *
 * $Id: meteoloop.c,v 1.11 2002/01/06 23:02:07 afm Exp $
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

extern int	optind;
extern char	*optarg;

int	minutes;
int	watchinterval;

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
		fprintf(stderr, "%s:%d: command 'LOOP' command written\n",
			__FILE__, __LINE__);

	/* wait for acknowledgment					*/
	if (l->m->m->flags & COM_VANTAGE) {
		if (get_acknowledge(l->m->m) < 0) {
			fprintf(stderr, "%s:%d: no ack received\n",
				__FILE__, __LINE__);
			if (watchinterval > 0)
				wd_fire("no ack from station");
			else
				exit(EXIT_FAILURE);
		}
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
			fprintf(stderr, "%s:%d: reading record %d through "
				"function %p\n", __FILE__, __LINE__, i,
				l->read);
		if (l->read(l, md) == 0) {
			if (watchinterval > 0)
				wd_arm(watchinterval);
		} else {
			fprintf(stderr, "%s:%d: retrieving an image failed\n",
				__FILE__, __LINE__);
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

/*
 * the main function of the meteoloop program simply calls the updateloop
 * function again and again
 */
int	main(int argc, char *argv[]) {
	meteocom_t	*m;
	meteodata_t	*md;
	meteoaccess_t	*d;
	const char	*url = NULL,
			*station = "Altendorf",
			*queuename = NULL;
	char		*conffilename = NULL;
	int		r, c, n = 1, speed = 2400, usequeue = 0,
			foreground = 0;
	loop_t		*l;
	dest_t		*ddp;
	char		*pidfilename = "/var/run/meteoloop-%s.pid";

	/* parse command line arguments					*/
	while (EOF != (c = getopt(argc, argv, "dn:Ff:p:qw:")))
		switch (c) {
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
			n = atoi(optarg);
			if ((n <= 0) || (n > 65535)) {
				fprintf(stderr, "%s:%d: illegal number of "
					"repeats: %d\n", __FILE__, __LINE__, n);
				exit(EXIT_FAILURE);
			}
			break;
		case 'q':
			usequeue = 1;
			if (debug)
				fprintf(stderr, "%s:%d: updates via msgque "
					"requested\n", __FILE__, __LINE__);
			break;
		case 'p':
			pidfilename = optarg;
			break;
		case 'w':
			watchinterval = atoi(optarg);
			if (watchinterval < 0)
				watchinterval = 0;
			break;
		}

	/* prepare the watchdog timer					*/
	if (watchinterval > 0) {
		if (debug)
			fprintf(stderr, "%s:%d: set up the watchdog timer\n",
				__FILE__, __LINE__);
		wd_setup(argc, argv);
	}

	/* it is an error not to specify a configuration file		*/
	if (NULL == conffilename) {
		fprintf(stderr, "%s:%d: must specify -f <config>\n", __FILE__,
			__LINE__);
		exit(EXIT_FAILURE);
	}

	/* open the configuration file 					*/
	if (NULL == (meteoconfig = mc_readconf(conffilename))) {
		fprintf(stderr, "%s:%d: configuration %s invalid\n", __FILE__,
			__LINE__, conffilename);
		exit(EXIT_FAILURE);
	}

	/* extract the information from the configuration data		*/
	url = mc_get_string(meteoconfig, "station.url", url);
	station = mc_get_string(meteoconfig, "database.prefix", station);
	queuename = mc_get_string(meteoconfig, "database.msgqueue", queuename);

	/* become a daemon						*/
	if (!foreground) {
		switch (daemonize(pidfilename, station)) {
		case -1:
			fprintf(stderr, "%s:%d: failed to daemonize\n",
				__FILE__, __LINE__);
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

	/* establish the connection to the weather station		*/
	if (0 == strncmp(url, "file://", 7)) {
		speed = mc_get_int(meteoconfig, "station.speed", speed);
		m = sercom_new((char *)url, speed);
	} else {
		m = tcpcom_new((char *)url);
	}
	if (m == NULL) {
		fprintf(stderr, "%s:%d: cannot open device: %s (%d)\n",
			__FILE__, __LINE__, strerror(errno), errno);
		exit(EXIT_FAILURE);
	}

	/* create a weather station structure from the connection 	*/
	d = davis_new(m);
	if (d == NULL) {
		fprintf(stderr, "%s:%d: cannot create davis interface\n",
			__FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	/* create a loop structure for this weather station		*/
	l = dloop_new(d);
	if (l == NULL) {
		fprintf(stderr, "%s:%d: cannot create loop handlier\n",
			__FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	/* establish the database connection				*/
	ddp = dest_new();
	if (usequeue) {
		ddp->type = DEST_MSGQUE;
		ddp->destdata.msgque = msgque_setup(queuename);
		if (ddp->destdata.msgque < 0) {
			fprintf(stderr, "%s:%d: cannot open msg queue\n",
				__FILE__, __LINE__);
			exit(EXIT_FAILURE);
		}
	} else {
		ddp->type = DEST_MYSQL;
		ddp->destdata.mysql = mc_opendb(meteoconfig, O_WRONLY);
		if (NULL == ddp->destdata.mysql) {
			fprintf(stderr, "%s:%d: failed to connect to "
				"database\n", __FILE__, __LINE__);
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
		r = updateloop(ddp, l, md, n, station);
		if (debug)
			fprintf(stderr, "%s:%d: did %d rounds of updates\n",
				__FILE__, __LINE__, n - r);
		if (r != 0) {
			fprintf(stderr, "%s:%d: failed with %d remaining "
				"updates\n", __FILE__, __LINE__, r);
			if (watchinterval > 0)
				wd_fire("updateloop function failed");
			else
				exit(EXIT_FAILURE);
		}
		wd_disarm();
	}

	/* close the connection to the database				*/
	dest_free(ddp);

	exit(EXIT_SUCCESS);
}
