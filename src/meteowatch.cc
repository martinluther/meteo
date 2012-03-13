/*
 * meteowatch.cc -- a watchdog program to restart meteopoll programs
 *                  that have stopped processing information
 *
 * (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: meteowatch.cc,v 1.4 2003/06/12 23:29:47 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <meteo.h>
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <daemon.h>
#include <mysql.h>
#include <signal.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <timestamp.h>
#include <printver.h>
#include <mdebug.h>
#include <xmlconf.h>
#include <database.h>

#define	UPDATECHECKINTERVAL	800
#define	CHECKINTERVAL		400

typedef struct watch_s {
	char	*station;
	char	*stationpidfile;
	time_t	lastcheck;
	time_t	lastupdate;
	pid_t	lastpid;
} watch_t;

/*
 * last_update	retrieve the timestamp of the last update for this station
 *              that happened within the last 10 minutes (this should be much
 *              faster than scanning the whole table)
 */
static time_t	last_update(watch_t *w, MYSQL *mysql) {
	char		query[1024];
	MYSQL_RES	*res;
	MYSQL_ROW	row;
	time_t		lu, t;

	/* compute a time key for the current time minus the check	*/
	/* interval							*/
	time(&t);
	t -= UPDATECHECKINTERVAL;

	/* compute the query to use to retrieve the last update 	*/	
	/* anything further than UPDATECHECKINTERVAL in the past is	*/
	/* considered to be at the beginning of all things Unix, at the	*/
	/* epoch							*/
	/* XXX what is returned for max(timekey) if result set  empty?	*/
	snprintf(query, sizeof(query),
		"select max(timekey) from stationdata where station = '%s' "
		"and timekey > %d", w->station, (int)t);
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"querying last update with query '%s'", query);

	/* run the query against the database				*/
	if (mysql_query(mysql, query)) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "could not query database: %s",
			mysql_error(mysql));
		return -1;
	}

	/* get the result set from the query				*/
	res = mysql_store_result(mysql);
	if (1 != mysql_num_rows(res)) {
		/* if nothing is returned, consider this as infinitely	*/
		/* far in the past					*/
		lu = 0;
		mysql_free_result(res);
		return lu;
	}
	row = mysql_fetch_row(res);

	/* convert the timestamp into a time_t				*/
	lu = atoi(row[0]);
	mysql_free_result(res);
	return lu;
}

/*
 * find_pid	read the pid file to get the pid registered therein
 */
static pid_t	find_pid(watch_t *w) {
	FILE	*f;
	pid_t	p = -1;

	/* open the pid file						*/
	if (NULL == (f = fopen(w->stationpidfile, "r"))) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"cannot open pid file named '%s' found", w->station);
		return -1;
	}

	/* read a short integer from the file				*/
	if (1 != fscanf(f, "%d", &p)) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "pid not found");
	} else {
		if (debug)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "pid = %d", p);
	}

	/* clean up and return pid					*/
	fclose(f);
	return p;
}

/*
 * check_station	perform the check of a single station, returns 0 if
 *			the station is OK, a negative number if the station
 *			has some problem
 */
static int	check_station(watch_t *w, MYSQL *mysql) {
	pid_t	p;
	time_t	now;

	/* make sure we don't check again too early			*/
	time(&now);
	if (now - w->lastcheck < CHECKINTERVAL)
		return -1;
	
	/* request the time of the last update from the database	*/
	w->lastupdate = last_update(w, mysql);

	/* see whether this is longer than the maximum silence interval	*/
	if ((now - w->lastupdate) < UPDATECHECKINTERVAL)
		return -1;

	/* get the pid							*/
	p = find_pid(w);

	/* if the pid hasn't changed, send the alarm signal		*/
	if (p == w->lastpid)
		goto dosig;

	/* if the pid has changed, remember it and give it a head start	*/
	w->lastpid = p;
	w->lastcheck = now + CHECKINTERVAL;
	return 0;

	/* do the common things	for a signal				*/
dosig:
	kill(p, SIGALRM);
	time(&w->lastupdate);
	w->lastpid = p;
	return -1;
}

/*
 * watch_all	check all stations of the watchlist, and return 0 if no
 *		problems were found. Otherwise, return the negative of the
 *		number of problematic stations.
 */
static int	watch_all(watch_t *watchlist, int stations, MYSQL *mysql) {
	int	i;
	int	rc, rcrc = 0;
	for (i = 0; i < stations; i++) {
		rc = check_station(&watchlist[i], mysql);
		if (rc < 0) {
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"problem detected during check of stations %s",
				watchlist[i].station);
			rcrc--;
		}
	}
	return rcrc;
}

int	main(int argc, char *argv[]) {
	char	*conffilename = METEOCONFFILE;
	char	*pidfilename = "/var/run/meteowatch.pid";
	char	*stationpidpattern = "/var/run/meteopoll-%s.pid";
	int	foreground = 0, stations = 0, c, i;
	char	*station;
	MYSQL	*mysql;
	watch_t	*watchlist;
	meteoconf_t	*mc;

	/* parse command line						*/
	while (EOF != (c = getopt(argc, argv, "dp:l:P:f:V")))
		switch (c) {
		case 'l':
			if (mdebug_setup("meteowatch", optarg) < 0) {
				fprintf(stderr, "%s: cannot init log\n",
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
		case 'p':
			pidfilename = optarg;
			break;
		case 'P':
			stationpidpattern = optarg;
			break;
		case 'V':
			fprintver(stdout);
			exit(EXIT_SUCCESS);
			break;
		}

	/* if we don't have a conf file at this point, it's a fatal err	*/
	if (conffilename) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "must specify -f <config>");
		exit(EXIT_FAILURE);
	}

	/* remaining options should be station names, build a list	*/
	/* of stations							*/
	if (argc <= optind) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0,
			"no stations specified, exiting.");
		exit(EXIT_FAILURE);
	}
	stations = optind - argc;
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%d stations specified",
			stations);
	watchlist = (watch_t *)malloc(stations * sizeof(watch_t));
	for (i = optind; i < argc; i++) {
		/* get station name					*/
		station = strdup(argv[optind]);
		watchlist[i].station = station;
		if (debug)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"setting up station %s", station);

		/* set the pid file name and pid			*/
		watchlist[i].stationpidfile = (char *)malloc(2
			+ strlen(pidfilename) + strlen(station));
		sprintf(watchlist[i].stationpidfile, pidfilename, station);

		/* set time stamps					*/
		watchlist[i].lastcheck = 0;
		watchlist[i].lastupdate = 0;
	}

	/* read the configuration file					*/
	if (NULL == (mc = xmlconf_new(conffilename, ""))) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "configuration %s is invalid",
			conffilename);
		exit(EXIT_FAILURE);
	}

	/* become a daemon						*/
	if (!foreground) {
		switch (daemonize(pidfilename, NULL)) {
		case -1:
			mdebug(LOG_CRIT, MDEBUG_LOG, 0, "failed to daemonize");
			exit(EXIT_FAILURE);
			break;
		case 0:
			/* inside parent				*/
			exit(EXIT_SUCCESS);
			break;
		default:
			/* inside child					*/
			break;
		}
	}

	/* build connection to database					*/
	mysql = mc_opendb(mc, O_RDONLY);
	if (NULL == mysql) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "cannot connect to database");
		exit(EXIT_FAILURE);
	}
	
	/* start infinite loop of watching all stations			*/
	while (1) {
		/* see whether any station has failed to produce an	*/
		/* update during the last 10 minutes			*/
		watch_all(watchlist, stations, mysql);
		
		/* sleep for 5 minutes					*/
		sleep(300);
	}
	mc_closedb(mysql);

	exit(EXIT_FAILURE);
}
