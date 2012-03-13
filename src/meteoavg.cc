/*
 * meteoavg.cc -- utility programm to compute averages and insert them into
 *                the database
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: meteoavg.cc,v 1.11 2004/02/24 23:33:21 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#include <errno.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <Averager.h>
#include <Configuration.h>
#include <StationInfo.h>
#include <Timestamp.h>
#include <Daemon.h>
#include <printver.h>
#include <string>
#include <mdebug.h>
#include <MeteoException.h>

extern int	optind;
extern char	*optarg;

// the daemon usually waits for the next time stamp that leaves 30 when
// divided by 300, and computes adds all the averages that are required
// for that timestamp
static void	avg_daemon(const std::string& station) {
	meteo::Averager	avg(station);
	int	offset = avg.getOffset();
	time_t	now, next;

	while (1) {
		// we go through the following loop repeatedly because it
		// can happen that the sleep call is interrupted by a signal
		// in which case we restart it
		do {
			// determine current time
			time(&now);

			// find out when will be the next point in time
			// to compute averages for. In addition, we wait 30
			// seconds more, because that gives us the assurance
			// that the data in the sdata table will be ready
			next = now - (now % 300) + 330;

			// wait till this happens
			if (debug)
				mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"next event at %-24.24s, in %d seconds",
					ctime(&next), (int)(next - now));
		} while (0 != sleep(next - now));

		// find the point in the past which to compute the avgs
		next -= next % 300;

		// compute all necessary averages. We have to add offset 
		// to the first argument because the Averager will again
		// substract the same value. Previously, the timekey was
		// to indicate the end of the time interval, but now
		// it points to the beginning, so we have to substract the
		// interval length to get the same behaviour
		avg.add(next - 300, 300, false);
		if (0 == ((next + offset) % 1800)) {
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"adding 1/2h avgs necessary");
			avg.add(next - 1800, 1800, false);
		}
		if (0 == ((next + offset) % 7200)) {
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"adding 2h avgs necessary");
			avg.add(next - 7200, 7200, false);
		}
		if (0 == ((next + offset) % 86400)) {
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"adding 1day avgs necessary");
			avg.add(next - 86400, 86400, false);
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

static int	meteoavg(int argc, char *argv[]) {
	std::string	conffilename(METEOCONFFILE);
	std::string	station;
	int		c, naverages = -1, interval = 0;
	bool		foreground = false;
	bool		daemonmode = false;
	time_t		fromt, tot, t;
	bool		haveavg, average_fake = false, all = false;
	std::string	pidfileprefix("/var/run/meteoavg-");

	// parse the command line					
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
			station = std::string(optarg);
			break;
		case 'd':
			debug++;
			break;
		case 'a':
			all = true;
			if (debug)
				mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"all averages forced");
			break;
		case 'f':
			conffilename = std::string(optarg);
			break;
		case 'F':
			foreground = true;
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
			pidfileprefix = optarg;
			break;
		case 'r':
			naverages = atoi(optarg);
			break;
		case 'n':
			average_fake = true;
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

	// station name is required					
	if (station.empty()) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "no station specified, use -s");
		exit(EXIT_FAILURE);
	}
	meteo::StationInfo	si(station);

	// there should be zero to two more arguments: timestamps for	
	// the range for which we should compute averages		
	switch (argc - optind) {
	case 0:	// no time stamps means that we should run as a daemon	
		// and update the database automagically with the most	
		// recent averages					
		daemonmode = true;
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "running in daemon mode");
		break;
	case 1:
		if ((naverages < 0) || (interval == 0)) {
			mdebug(LOG_CRIT, MDEBUG_LOG, 0,
				"must specify repeats (-r) if if only first "
				"interval end given");
			exit(EXIT_FAILURE);
		}
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "start timestamp: %s",
			argv[optind]);
		fromt = meteo::Timestamp(argv[optind]).getTime();
		tot = fromt + interval * naverages;
		break;
	case 2:
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "interval %s - %s",
			argv[optind], argv[optind + 1]);
		fromt = meteo::Timestamp(argv[optind]).getTime();
		tot = meteo::Timestamp(argv[optind + 1]).getTime();
		break;
	default:
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "wrong number of arguments");
		exit(EXIT_FAILURE);
	}

	// read the configuration file					
	meteo::Configuration	conf(conffilename);

	// consistency checks						
	// interval must be set						
	if ((interval <= 0) && (!daemonmode)) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "interval (-i) not specified");
		exit(EXIT_FAILURE);
	}

	// connect to the database					
	if (daemonmode) {
		// become a daemon					
		meteo::Daemon	daemon(pidfileprefix, station, foreground);

		// start daemon mode for averages			
		avg_daemon(station);

		// if we ever get to this point, we exit
		exit(EXIT_SUCCESS);
	}

	// compute first timestamp, correcting it for the offset. The average
	// for timekey t actually uses data from t - offset to
	// t - offset + interval, so we must add the offset to t to get
	// the intervals we are interested in
	fromt -= (fromt + si.getOffset()) % interval;

	// compute updates in the range according to the command line parameters
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
		"start querying for averages between %d and %d",
		(int)fromt, (int)tot);
	// t is the start time of an interval, it is chosen such that t+offset
	// is a valid time key
	for (t = fromt; t <= tot; t += interval) {
		time_t	timekey = t + si.getOffset();
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"averages for time %d/%d, station '%s'",
			(int)t, interval, station.c_str());

		// create an Averager object
		meteo::Averager	avg(station);
		avg.setFake(average_fake);

		// create necessary averages
		if (!all)
			haveavg = avg.have(timekey, interval);
		else
			haveavg = false;
		if (!haveavg)
			avg.add(timekey, interval, all);
	}

	exit(EXIT_SUCCESS);
}

// main(argc, argv)	wrapper to catch MeteoExceptions thrown inside the
//			real main function. We need the other main function
//			as scope for the Daemon class, since the scope
//			determines how long the PID file will be around
int	main(int argc, char *argv[]) {
	try {
		meteoavg(argc, argv);
	} catch (meteo::MeteoException& me) {
		fprintf(stderr, "MeteoException in meteoavg: %s/%s\n",
			me.getReason().c_str(), me.getAddinfo().c_str());
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
