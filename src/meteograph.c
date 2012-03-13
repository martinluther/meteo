/*
 * meteograph.c -- create meteo data graphs similar to MRTGs
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: meteograph.c,v 1.6 2002/11/24 19:48:02 afm Exp $
 */
#include <meteo.h>
#include <meteograph.h>
#include <xmlconf.h>
#include <graph.h>
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <dewpoint.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <database.h>
#include <dograph.h>
#include <printver.h>
#include <mdebug.h>

extern int	optind;
extern char	*optarg;

typedef struct namegraph_s {
	char	*name;
	int	graph;
} namegraph_t;

#define	NGRAPHS	14
static namegraph_t	ngs[NGRAPHS] = {
	{ "barometer",		DOGRAPH_BAROMETER			},
	{ "pressure",		DOGRAPH_BAROMETER			},
	{ "temperature",	DOGRAPH_TEMPERATURE			},
	{ "thermo",		DOGRAPH_TEMPERATURE			},
	{ "temperature_inside",	DOGRAPH_TEMPERATURE_INSIDE		},
	{ "thermo_inside",	DOGRAPH_TEMPERATURE_INSIDE		},
	{ "humidity",		DOGRAPH_HUMIDITY			},
	{ "hygro",		DOGRAPH_HUMIDITY			},
	{ "humidity_inside",	DOGRAPH_HUMIDITY_INSIDE			},
	{ "hygro_inside",	DOGRAPH_HUMIDITY_INSIDE			},
	{ "rain",		DOGRAPH_RAIN				},
	{ "precipitation",	DOGRAPH_RAIN				},
	{ "wind",		DOGRAPH_WIND				},
	{ "radiation",		DOGRAPH_RADIATION			}
};

/* declare to keep compiler happy					*/
void	usage(void);

void	usage(void) {
	printf(
"usage: meteograph [ -adtV ] [ -l logfile ] [ -c dir ] [ -G graph ] \n"
"[ -g graph ] [ -f config ] [ -e endtime ] [ -V ]\n"
"	-a		use averages table\n"
"	-d		increase debug level\n"
"	-t		include timestamp in generated file name\n"
"	-s stationname	name of the station for which to compute graphs\n"
"	-V		display version and exit\n"
"	-l logfile	write debug log to logfile\n"
"	-c dir		change directory to dir before writing any image files\n"
"	-g graph	include only graph (can be given multiple times)\n"
"	-G graph	exclude graph (can given multiple times)\n"
"			[ -g and -G can not be used simulatineouly ]\n"
"	-f config	use configuration file config\n"
"	-e endtime	specify end time of graph\n");
}

int	main(int argc, char *argv[]) {
	char		*conffilename = METEOCONFFILE;
	char		*station = NULL;
	int		c, interval, i, G = 0;
	dograph_t	dg;
	time_t		t;
	struct tm	*tp;
	char		timestamp[64];
	meteoconf_t	*mc;

	/* initialize defaults						*/
	time(&dg.end);
	dg.useaverages = 0;
	dg.requestedgraphs = 0;
	dg.withtimestamps = 0;
	dg.timestamp = NULL;
	
	/* parse command line						*/
	while (EOF != (c = getopt(argc, argv, "adc:e:f:g:l:G:ts:V?")))
		switch (c) {
		case '?':
			usage();
			exit(EXIT_SUCCESS);
			break;
		case 'l':
			if (mdebug_setup("meteograph", optarg) < 0) {
				fprintf(stderr, "%s: cannot init log\n",
					argv[0]);
				exit(EXIT_FAILURE);
			}
			break;
		case 'c':
			if (chdir(optarg) < 0) {
				mdebug(LOG_CRIT, MDEBUG_LOG, 0,
					"cannot chdir to %s", optarg);
				exit(EXIT_FAILURE);
			}
			break;
		case 'd':
			debug++;
			break;
		case 'e':
			/* convert a timestamp into a time_t		*/
			dg.timestamp = optarg;
			dg.end = localtime2time(optarg);
			if (debug)
				printf("%s:%d: timestamp %d\n", __FILE__,
					__LINE__, (int)dg.end);
			break;
		case 't':
			/* include time stamp in generated file		*/
			dg.withtimestamps = 1;
			break;
		case 'f':
			conffilename = optarg;
			break;
		case 'a':
			dg.useaverages = 1;
			if (debug)
				mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"use of average table requested");
			break;
		case 'g':
			if (debug)
				mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"graph for %s requested", optarg);
			for (i = 0; i < NGRAPHS; i++) 
				if (0 == strcasecmp(optarg, ngs[i].name))
					dg.requestedgraphs |= ngs[i].graph;
			break;
		case 'G':
			if (!G) { dg.requestedgraphs = 0xff; G = 1; }
			for (i = 0; i < NGRAPHS; i++) 
				if (0 == strcasecmp(optarg, ngs[i].name))
					dg.requestedgraphs ^= ngs[i].graph;
			break;
		case 's':
			station = optarg;
			break;
		case 'V':
			fprintver(stdout);
			exit(EXIT_SUCCESS);
			break;
		default:
			fprintf(stderr, "%s:%d: unknown option: %c\n",
				__FILE__, __LINE__, c);
			usage();
			exit(EXIT_FAILURE);
		}

	/* check for requested graphs					*/
	if (dg.requestedgraphs == 0) {
		if (debug)
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "no graphs selected, "
				"creating all");
		dg.requestedgraphs = 0xff;
	}
	if (debug)
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "graph mask: %02x",
			dg.requestedgraphs);

	/* station name is mandatory					*/
	if (NULL == station) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "must specify -s <station>");
		exit(EXIT_FAILURE);
	}

	/* it is an error not to specify a configuration file		*/
	if (NULL == conffilename) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "must specify -f <config>");
		exit(EXIT_FAILURE);
	}

	/* read the configuration file					*/
	if (NULL == (mc = xmlconf_new(conffilename, station))) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "configuration invalid");
		exit(EXIT_FAILURE);
	}

	/* open the database connection					*/
	dg.mysql = mc_opendb(mc, O_RDONLY);
	if (dg.mysql == NULL) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0,
			"could not connect to database");
		exit(EXIT_FAILURE);
	}

	/* get some other data from the configuration file		*/
	dg.prefix = mc->station;

	/* if the timestamp is not set, we take the current time for 	*/
	/* timestamp							*/
	if (NULL == dg.timestamp) {
		time(&t);
		tp = localtime(&t);
		strftime(timestamp, sizeof(timestamp), "%Y%m%d%H%M%S", tp);
		dg.timestamp = timestamp;
	}
	dg.mc = mc;

	for (; optind < argc; optind++) {
		switch(interval = atoi(argv[optind])) {
		case 300:	/* 5 minutes ==> 1 day of data		*/
			dg.suffix = "day";
			break;
		case 1800:	/* 30 minutes ==> 1 week of data	*/
			dg.suffix = "week";
			break;
		case 7200:	/* 2 hours ==> 1 month of data		*/
			dg.suffix = "month";
			break;
		case 86400:	/* 1 day ==> 1 year of data		*/
			dg.suffix = "year";
			break;
		default:
			mdebug(LOG_CRIT, MDEBUG_LOG, 0, "unknown interval %s",
				argv[optind]);
			exit(EXIT_FAILURE);
			break;
		}
		all_graphs(&dg, interval);
	}

	/* that's all							*/
	exit(EXIT_SUCCESS);
}
