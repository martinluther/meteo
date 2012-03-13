/*
 * meteoavg.c -- utility programm to compute averages and insert them into
 *               the database
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: meteoavg.c,v 1.5 2002/01/09 23:36:07 afm Exp $
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
				fprintf(stderr, "%s:%d: next event at "
					"%-24.24s, in %d seconds\n",
					__FILE__, __LINE__,
					ctime(&next), (int)(next - now));
		} while (0 != sleep(next - now));

		/* find the point in to for which to compute the avgs	*/
		next -= next % 300;

		/* compute all necessary averages			*/
		add_average(mysql, next, 300, station);
		if (0 == (next % 1800)) {
			if (debug)
				fprintf(stderr, "%s:%d: adding 1/2h avgs "
					"necessary\n", __FILE__, __LINE__);
			add_average(mysql, next, 1800, station);
		}
		if (0 == (next % 7200)) {
			if (debug)
				fprintf(stderr, "%s:%d: adding 2h avgs "
					"necessary\n", __FILE__, __LINE__);
			add_average(mysql, next, 7200, station);
		}
		if (0 == (next % 86400)) {
			if (debug)
				fprintf(stderr, "%s:%d: adding 1day avgs "
					"necessary\n", __FILE__, __LINE__);
			add_average(mysql, next, 86400, station);
		}
	}
}

int	main(int argc, char *argv[]) {
	char		*conffilename = METEOCONFFILE;
	const char	*station;
	int		c, naverages = -1, interval = 0, remainder,
			daemonmode = 0, all = 0, haveavg, foreground = 0;
	time_t		fromt, tot, t;
	MYSQL		*mysql;
	char		*pidfilename = "/var/run/meteoavg-%s.pid";

	/* parse the command line					*/
	while (EOF != (c = getopt(argc, argv, "adf:Fi:r:np:")))
		switch (c) {
		case 'd':
			debug++;
			break;
		case 'a':
			all = 1;
			if (debug)
				fprintf(stderr, "%s:%d: all averages forced\n",
					__FILE__, __LINE__);
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
				fprintf(stderr, "%s:%d: illegal interval: %d\n",
					__FILE__, __LINE__, interval);
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
		}

	/* there should be zero to two more arguments: timestamps for	*/
	/* the range for which we should compute averages		*/
	switch (argc - optind) {
	case 0:	/* no time stamps means that we should run as a daemon	*/
		/* and update the database automagically with the most	*/
		/* recent averages					*/
		daemonmode = 1;
		if (debug)
			fprintf(stderr, "%s:%d: running in daemon mode\n",
				__FILE__, __LINE__);
		break;
	case 1:
		if ((naverages < 0) || (interval == 0)) {
			fprintf(stderr, "%s:%d: must specify repeats (-r) if "
				"if only first interval end given\n", __FILE__,
				__LINE__);
			exit(EXIT_FAILURE);
		}
		if (debug)
			fprintf(stderr, "%s:%d: start timestamp: %s\n",
				__FILE__, __LINE__, argv[optind]);
		fromt = string2time(argv[optind]);
		tot = fromt + interval * naverages;
		break;
	case 2:
		if (debug)
			fprintf(stderr, "%s%d: interval %s - %s\n",
				__FILE__, __LINE__,
				argv[optind], argv[optind + 1]);
		fromt = string2time(argv[optind]);
		tot = string2time(argv[optind + 1]);
		break;
	default:
		fprintf(stderr, "%s:%d: wrong number of arguments\n", __FILE__,
			__LINE__);
		exit(EXIT_FAILURE);
	}

	/* it is an error not to specify a configuration file		*/
	if (conffilename == NULL) {
		fprintf(stderr, "%s:%d: must specify -f <config>\n", __FILE__,
			__LINE__);
		exit(EXIT_FAILURE);
	}

	/* read the configuration file					*/
	if (NULL == (meteoconfig = mc_readconf(conffilename))) {
		fprintf(stderr, "%s:%d: configuration invalid\n",
			__FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	/* consistency checks						*/
	/* 1. Station must be defined					*/
	if (NULL == (station = mc_get_string(meteoconfig, "database.prefix",
		NULL))) {
		fprintf(stderr, "%s:%d: station name not defined\n", __FILE__,
			__LINE__);
	}
	station = strdup(station);
	if (debug)
		fprintf(stderr, "%s:%d: station = %s\n", __FILE__, __LINE__,
			station);
	/* interval must be set						*/
	if ((interval <= 0) && (!daemonmode)) {
		fprintf(stderr, "%s:%d: interval (-i) not specified\n",
			__FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	mysql = mc_opendb(meteoconfig, O_RDWR);
	if (mysql == NULL) {
		fprintf(stderr, "%s:%d: cannot connect to database\n",
			__FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	/* connect to the database					*/
	if (daemonmode) {
		/* become a daemon					*/
		if (0 == foreground)
			switch (daemonize(pidfilename, station)) {
			case -1:
				fprintf(stderr, "%s:%d: cannot daemonize\n",
					__FILE__, __LINE__);
				exit(EXIT_FAILURE);
				break;
			case 0:
				/* parent code				*/
				exit(EXIT_FAILURE);
				break;
			default:
				if (debug)
					fprintf(stderr, "%s:%d: new pid = %d\n",
						__FILE__, __LINE__, getpid());
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
			fprintf(stderr, "%s:%d: start querying for averages "
				"between %d and %d\n", __FILE__, __LINE__,
				(int)fromt, (int)tot);
		for (t = fromt; t <= tot; t += interval) {
			if (debug)
				fprintf(stderr, "%s:%d: averages for time "
					"%d/%d, station '%s'\n",
					__FILE__, __LINE__,
					(int)t, interval, station);
			if (!all)
				haveavg = have_average(mysql, t, interval,
					station);
			else
				haveavg = 0;
			if (!haveavg) {
				if (add_average(mysql, t, interval,
					station) < 0) {
					fprintf(stderr, "%s:%d: update at time "
						"%d failed\n", __FILE__,
						__LINE__, (int)t);
					exit(EXIT_FAILURE);
				}
			}
		}
	}

	/* cleanup							*/
	mc_closedb(mysql);

	exit(EXIT_SUCCESS);
}
