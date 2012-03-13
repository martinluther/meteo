/*
 * meteoavg.cc -- utility programm to compute averages and insert them into
 *                the database
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: meteoavg.cc,v 1.2 2003/06/08 22:32:10 afm Exp $
 */
#include <meteo.h>
#include <database.h>
#include <average.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <timestamp.h>
#include <unistd.h>
#include <daemon.h>
#include <printver.h>
#include <mdebug.h>

extern int	optind;
extern char	*optarg;

/*
 * the daemon usually waits for the next time stamp that leaves 30 when
 * divided by 300, and computes adds all the averages that are required
 * for that timestamp
 */
static void	avg_daemon(MYSQL *mysql, const char *station) {
	time_t		now, next;

	while (1) {
		do {
			/* determine current time			*/
			time(&now);

			/* find out when will be the next point in time	*/
			/* to compute averages for			*/
			next = now - (now % 300) + 330;

			/* wait till this happens			*/
			if (debug)
				mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"next event at %-24.24s, in %d seconds",
					ctime(&next), (int)(next - now));
		} while (0 != sleep(next - now));

		/* find the point in to for which to compute the avgs	*/
		next -= next % 300;

		/* compute all necessary averages			*/
		add_average(mysql, next, 300, station);
		if (0 == (next % 1800)) {
			if (debug)
				mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"adding 1/2h avgs necessary");
			add_average(mysql, next, 1800, station);
		}
		if (0 == (next % 7200)) {
			if (debug)
				mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"adding 2h avgs necessary");
			add_average(mysql, next, 7200, station);
		}
		if (0 == (next % 86400)) {
			if (debug)
				mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"adding 1day avgs necessary");
			add_average(mysql, next, 86400, station);
		}
	}
}

static void	usage(void) {
printf(
"usage: meteoavg [ -FdnVh? ] [ -l logurl ] [ -f conffile ] [ -p pidfile ] \\\n"
"       [ -i interval ] [ -r repeats ] -s station [ starttime [ endtime ] ]\n"
"computes averages of meteo data between starttime and endtime or for\n"
"repeats intervals. With no arguments run as a daemon that computes\n"
"averages whenever all the data from the station can reasonably be expected\n"
"to have been inserted in the database.\n"
"  -l logurl       send log messages to this url\n"
"  -p pidfile      write the process pid to the pidfile\n"
"  -F              stay in foreground (for debugging)\n"
"  -s stationname  station name, mandatory\n"
"  -d              increase debug level\n"
"  -f conffile     read configuration from conffile\n"
"  -i interval     compute averages for this interval (300, 1800, 7200 or 86400\n"
"  -r              number of averages to compute\n"
"  -n              just tell what you are about to do, don't actually do it\n"
"  -V              print version and exit\n"
"  -h, -?          print this help and exit\n"
"starttime and endtime have format YYYYMMDDhhmmss\n"
);
}

int	main(int argc, char *argv[]) {
	char		*conffilename = METEOCONFFILE;
	char		*station = NULL;
	int		c, naverages = -1, interval = 0, remainder,
			daemonmode = 0, all = 0, haveavg, foreground = 0;
	time_t		fromt, tot, t;
	MYSQL		*mysql;
	char		*pidfilename = "/var/run/meteoavg-%s.pid";
	meteoconf_t	*mc;

	/* parse the command line					*/
	while (EOF != (c = getopt(argc, argv, "adf:Fi:l:r:np:Vs:h?")))
		switch (c) {
		case 'l':
			if (mdebug_setup("meteoavg", optarg) < 0) {
				fprintf(stderr, "%s: cannot init log\n",
					argv[0]);
				exit(EXIT_FAILURE);
			}
			break;
		case 's':
			station = optarg;
			break;
		case 'd':
			debug++;
			break;
		case 'a':
			all = 1;
			if (debug)
				mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"all averages forced");
			break;
		case 'f':
			conffilename = optarg;
			break;
		case 'F':
			foreground = 1;
			break;
		case 'i':
			interval = atoi(optarg);
			switch (interval) {
			case 300:
			case 1800:
			case 7200:
			case 86400:
				break;
			default:
				mdebug(LOG_CRIT, MDEBUG_LOG, 0,
					"illegal interval: %d", interval);
				exit(EXIT_FAILURE);
			}
			break;
		case 'p':
			pidfilename = optarg;
			break;
		case 'r':
			naverages = atoi(optarg);
			break;
		case 'n':
			average_fake = 1;
			break;
		case 'V':
			fprintver(stdout);
			exit(EXIT_SUCCESS);
			break;
		case 'h':
		case '?':
			usage();
			exit(EXIT_SUCCESS);
		}

	/* station name is required					*/
	if (NULL == station) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "no station specified, use -s");
		exit(EXIT_FAILURE);
	}

	/* there should be zero to two more arguments: timestamps for	*/
	/* the range for which we should compute averages		*/
	switch (argc - optind) {
	case 0:	/* no time stamps means that we should run as a daemon	*/
		/* and update the database automagically with the most	*/
		/* recent averages					*/
		daemonmode = 1;
		if (debug)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"running in daemon mode");
		break;
	case 1:
		if ((naverages < 0) || (interval == 0)) {
			mdebug(LOG_CRIT, MDEBUG_LOG, 0,
				"must specify repeats (-r) if if only first "
				"interval end given");
			exit(EXIT_FAILURE);
		}
		if (debug)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "start timestamp: %s",
				argv[optind]);
		fromt = localtime2time(argv[optind]);
		tot = fromt + interval * naverages;
		break;
	case 2:
		if (debug)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "interval %s - %s",
				argv[optind], argv[optind + 1]);
		fromt = localtime2time(argv[optind]);
		tot = localtime2time(argv[optind + 1]);
		break;
	default:
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "wrong number of arguments");
		exit(EXIT_FAILURE);
	}

	/* it is an error not to specify a configuration file		*/
	if (conffilename == NULL) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "must specify -f <config>");
		exit(EXIT_FAILURE);
	}

	/* read the configuration file					*/
	if (NULL == (mc = xmlconf_new(conffilename, station))) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "configuration invalid");
		exit(EXIT_FAILURE);
	}

	/* consistency checks						*/
	/* interval must be set						*/
	if ((interval <= 0) && (!daemonmode)) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "interval (-i) not specified");
		exit(EXIT_FAILURE);
	}

	mysql = mc_opendb(mc, O_RDWR);
	if (mysql == NULL) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "cannot connect to database");
		exit(EXIT_FAILURE);
	}

	/* connect to the database					*/
	if (daemonmode) {
		/* become a daemon					*/
		if (0 == foreground)
			switch (daemonize(pidfilename, station)) {
			case -1:
				mdebug(LOG_CRIT, MDEBUG_LOG, 0,
					"cannot daemonize");
				exit(EXIT_FAILURE);
				break;
			case 0:
				/* parent code				*/
				exit(EXIT_FAILURE);
				break;
			default:
				if (debug)
					mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
						"new pid = %d", getpid());
				break;
			}

		/* start daemon mode for averages			*/
		avg_daemon(mysql, station);
	} else {
		/* compute first timestamp				*/
		remainder = fromt % interval;
		if (remainder)
			fromt = fromt - (fromt % interval) + interval;

		/* compute updates in the range according to the 	*/
		/* command line parameters				*/
		if (debug)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"start querying for averages between %d and %d",
				(int)fromt, (int)tot);
		for (t = fromt; t <= tot; t += interval) {
			if (debug)
				mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"averages for time %d/%d, station '%s'",
					(int)t, interval, station);
			if (!all)
				haveavg = have_average(mysql, t, interval,
					station);
			else
				haveavg = 0;
			if (!haveavg) {
				if (add_average(mysql, t, interval,
					station) < 0) {
					mdebug(LOG_CRIT, MDEBUG_LOG, 0,
						"update at time %d failed",
						(int)t);
					exit(EXIT_FAILURE);
				}
			}
		}
	}

	/* cleanup							*/
	mc_closedb(mysql);

	exit(EXIT_SUCCESS);
}