/*
 * meteowatch.c -- a watchdog program to restart meteoloop programs
 *                 that have stopped processing information
 *
 * (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: meteowatch.c,v 1.1 2002/01/09 23:36:07 afm Exp $
 */
#include <meteo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <daemon.h>
#include <mysql/mysql.h>
#include <signal.h>
#include <string.h>
#include <timestamp.h>

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
 */
static time_t	last_update(watch_t *w, MYSQL *mysql) {
	char		query[1024];
	MYSQL_RES	*res;
	MYSQL_ROW	row;
	time_t		lu;

	/* compute the query to use to retrieve the last update 	*/	
	snprintf(query, sizeof(query),
		"select max(timekey) from stationdata where station = '%s'",
		w->station);
	if (debug)
		fprintf(stderr, "%s:%d: querying last update with query '%s'\n",
			__FILE__, __LINE__, query);

	/* run the query against the database				*/
	if (mysql_query(mysql, query)) {
		fprintf(stderr, "%s:%d: could not query database\n",
			__FILE__, __LINE__);
		return -1;
	}

	/* get the result set from the query				*/
	res = mysql_store_result(mysql);
	if (1 != mysql_num_rows(res)) {
		fprintf(stderr, "%s:%d: not exactly 1 row returned\n",
			__FILE__, __LINE__);
		mysql_free_result(res);
		return -1;
	}
	row = mysql_fetch_row(res);

	/* convert the timestamp into a time_t				*/
	lu = string2time(row[0]);
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
		fprintf(stderr, "%s:%d: cannot open pid file named '%s' "
			"found\n", __FILE__, __LINE__, w->station);
		return -1;
	}

	/* read a short integer from the file				*/
	if (1 != fscanf(f, "%d", &p)) {
		fprintf(stderr, "%s:%d: pid not found\n", __FILE__, __LINE__);
	} else {
		if (debug)
			fprintf(stderr, "%s:%d: pid = %d\n",
				__FILE__, __LINE__, p);
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
			fprintf(stderr, "%s:%d: problem detected during "
				"check of stations %s\n", __FILE__, __LINE__,
				watchlist[i].station);
			rcrc--;
		}
	}
	return rcrc;
}

int	main(int argc, char *argv[]) {
	char	*conffilename = METEOCONFFILE;
	char	*pidfilename = "/var/run/meteowatch.pid";
	char	*stationpidpattern = "/var/run/meteoloop-%s.pid";
	int	foreground = 0, stations = 0, c, i;
	char	*station;
	MYSQL	*mysql;
	watch_t	*watchlist;

	/* parse command line						*/
	while (EOF != (c = getopt(argc, argv, "dp:P:f:")))
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
		case 'p':
			pidfilename = optarg;
			break;
		case 'P':
			stationpidpattern = optarg;
			break;
		}

	/* if we don't have a conf file at this point, it's a fatal err	*/
	if (conffilename) {
		fprintf(stderr, "%s:%d: must specify -f <config>\n", __FILE__,
			__LINE__);
		exit(EXIT_FAILURE);
	}

	/* remaining options should be station names, build a list	*/
	/* of stations							*/
	if (argc <= optind) {
		fprintf(stderr, "%s:%d: no stations specified, exiting.\n",
			__FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	stations = optind - argc;
	if (debug)
		fprintf(stderr, "%s:%d: %d stations specified\n", __FILE__,
			__LINE__, stations);
	watchlist = (watch_t *)malloc(stations * sizeof(watch_t));
	for (i = optind; i < argc; i++) {
		/* get station name					*/
		station = strdup(argv[optind]);
		watchlist[i].station = station;
		if (debug)
			fprintf(stderr, "%s:%d: setting up station %s\n",
				__FILE__, __LINE__, station);

		/* set the pid file name and pid			*/
		watchlist[i].stationpidfile = (char *)malloc(2
			+ strlen(pidfilename) + strlen(station));
		sprintf(watchlist[i].stationpidfile, pidfilename, station);

		/* set time stamps					*/
		watchlist[i].lastcheck = 0;
		watchlist[i].lastupdate = 0;
	}

	/* read the configuration file					*/
	if (NULL == (meteoconfig = mc_readconf(conffilename))) {
		fprintf(stderr, "%s:%d: configuration %s is invalid\n",
			__FILE__, __LINE__, conffilename);
		exit(EXIT_FAILURE);
	}

	/* become a daemon						*/
	if (!foreground) {
		switch (daemonize(pidfilename, NULL)) {
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
			/* inside child					*/
			break;
		}
	}

	/* start build connection to database				*/
	
	/* start infinite loop of watching all stations			*/
	while (1) {
		/* see whether any station has failed to produce an	*/
		/* update during the last 10 minutes			*/
		watch_all(watchlist, stations, mysql);
		
		/* sleep for 5 minutes					*/
		sleep(300);
	}
	exit(EXIT_FAILURE);
}
