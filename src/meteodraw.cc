/*
 * meteodraw.cc -- create meteo drawings
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <vector>
#include <set>
#include <algorithm>
#include <Configuration.h>
#include <Graphics.h>
#include <mdebug.h>
#include <timestamp.h>
#include <printver.h>
#include <errno.h>
#include <iostream>
#include <Timelabel.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <MeteoException.h>

#define	NGRAPHNAMES	8
char	*graphname[NGRAPHNAMES] = {
	"temperature", "temperature_inside", "humidity", "humidity_inside",
	"pressure", "radiation", "rain", "wind"
};

static std::string	timestamp(time_t t) {
	char		tsbuffer[32];
	struct tm	*lt = localtime(&t);
	strftime(tsbuffer, sizeof(tsbuffer), "%Y%m%d%H%M%S", lt);
	return std::string(tsbuffer);
}

static bool	imageIsOld(const std::string& filename, int interval) {
	struct stat	sb;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "checking age of %s",
		filename.c_str());
	// find out whether file exists
	if (stat(filename.c_str(), &sb) < 0) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "file %s does not exist yet",
			filename.c_str());
		return true;
	}

	// compute current time
	time_t	now = time(NULL);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "file age: %d - %d = %d",
		now, sb.st_ctime, now - sb.st_ctime);
	return ((now - sb.st_ctime) > interval);
}

static void	usage(void) {
	printf(
"usage: meteograph [ -dtVmI ] [ -l logurl ] [ -c dir ] [ -G graph ] \n"
"    [ -s station ] [ -p prefix ] [ -g graph ] [ -f config ] [ -u url ] \n"
"    [ -e endtime ] [ -L label ] [ interval ... ]\n"
"	-d		increase debug level\n"
"	-t		include timestamp in generated file name\n"
"	-s stationname	name of the station for which to compute graphs\n"
"	-V		display version and exit\n"
"	-I		create graphs only of the last update is more than\n"
"			the interval length back\n"
"	-l logfile	write debug to log destionation logurl\n"
"	-L label	compute the image for the label\n"
"	-m              write image map info to stdout\n"
"	-c dir		change directory to dir before writing any image files\n"
"	-g graph	include only graph (can be given multiple times)\n"
"	-G graph	exclude graph (can be given multiple times)\n"
"			[ -g and -G can not be used simulatineouly ]\n"
"	-f config	use configuration file config\n"
"	-p prefix	store images in files with this prefix\n"
"	-u url		use this URL prefix for image maps\n"
"	-e endtime	specify end time of graph\n");
}

int	main(int argc, char *argv[]) {
	int		c;
	bool		withtimestamps = false;
	std::string	conffilename(METEOCONFFILE);
	std::string	stationname("undefined");
	std::string	logurl("file:///-");
	std::string	url;
	std::string	prefix;
	time_t		end;
	std::set<std::string>	requested;
	std::set<std::string>	notrejected;
	std::vector<int>	intervals;
	std::list<meteo::Timelabel>	labeled;
	bool		imagemap = false;
	bool		images = true;
	bool		imagetag = false;
	meteo::Timelabel	tl;

	// remember current time
	time(&end);

	// add all known graphs to the list of graphs
	for (int k = 0; k < NGRAPHNAMES; k++) {
		notrejected.insert(std::string(graphname[k]));
	}

	// parse the command line
	while (EOF != (c = getopt(argc, argv, "adc:e:f:g:iIl:L:mG:p:ts:V?u:")))
		switch (c) {
		case '?':
			usage();
			exit(EXIT_SUCCESS);
			break;
		case 'l':
			if (mdebug_setup("meteodraw", optarg) < 0) {
				fprintf(stderr, "%s: cannot init log to %s\n",
					argv[0], optarg);
				exit(EXIT_FAILURE);
			}
			break;
		case 'c':
			if (chdir(optarg) < 0) {
				mdebug(LOG_CRIT, MDEBUG_LOG, 0,
					"cannot chdir(%s): %s", optarg,
					strerror(errno));
				exit(EXIT_FAILURE);
			}
			break;
		case 'd':
			debug++;
			break;
		case 'e':
			end = localtime2time(optarg);
			if (debug)
				mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"end timestamp: %d", end);
			break;
		case 't':
			withtimestamps = true;
			break;
		case 'f':
			conffilename = std::string(optarg);
			break;
		case 'g':
			requested.insert(std::string(optarg));
			break;
		case 'G':
			// find the graph name an remove it from the 
			notrejected.erase(std::string(optarg));
			break;
		case 's':
			stationname = std::string(optarg);
			break;
		case 'V':
			fprintver(stdout);
			exit(EXIT_SUCCESS);
			break;
		case 'i':
			imagetag = true;
			break;
		case 'I':
			images = false;
			break;
		case 'm':
			imagemap = true;
			break;
		case 'L':
			tl = meteo::Timelabel(optarg);
			labeled.push_back(tl);
			break;
		case 'p':
			prefix = std::string(optarg);
			break;
		case 'u':
			url = std::string(optarg);
			break;
		default:
			mdebug(LOG_ERR, MDEBUG_LOG, 0, "unkown option %c", c);
			break;
		}

	// set up logging
	mdebug_setup("meteodraw", logurl.c_str());
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "set up logging to %s",
		logurl.c_str());

	// make sure we have a station name
	if ("undefined" == stationname) {
		fprintf(stderr, "%s: station name not set\n", argv[0]);
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "station name not specified");
		exit(EXIT_FAILURE);
	}

	// make sure we have a configuration file, and initialize the config
	// reader
	meteo::Configuration	conf(conffilename);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "read configuration %s",
		conffilename.c_str());

	// read all the remaining arguments, they should be valid interval
	// numbers (check this)
	if (optind == argc) {
		if (labeled.size() == 0) {
			// the 60 seconds interval is not pushed by default
			// because it behaves rather differently: There are
			// no maximum/minimum values in this interval, so
			// in most cases a separate configuration file will
			// be needed. We don't want to have to exclude 60 secs
			// interval manually (would be an incompatible change)
			intervals.push_back(300);
			intervals.push_back(1800);
			intervals.push_back(7200);
			intervals.push_back(86400);
		}
	} else {
		for (; optind < argc; optind++) {
			switch (int interval = atoi(argv[optind])) {
			case 60:
			case 300:
			case 1800:
			case 7200:
			case 86400:
				intervals.push_back(interval);
				break;
			default:
				mdebug(LOG_CRIT, MDEBUG_LOG, 0,
					"illegal interval %s", argv[optind]);
				exit(EXIT_FAILURE);
				break;
			}
		}
	}

	// compute the set of requested graphs
	std::set<std::string>	g;
	set_intersection(requested.begin(), requested.end(),
		notrejected.begin(), notrejected.end(),
		std::insert_iterator< std::set<std::string> >(g, g.begin()));

	// do all the labeled graphs
	std::set<std::string>::iterator	j;
	std::list<meteo::Timelabel>::const_iterator	ti;
	for (ti = labeled.begin(); ti != labeled.end(); ti++) {
		for (j = g.begin(); j != g.end(); j++) {
			std::string	currentgraph = *j;
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "processing graph %s",
				currentgraph.c_str());
			// write the graph to a file
			std::string	outfilename = prefix + stationname + "-"
				+ currentgraph + "-" +ti->getString() + ".png";
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "outfilename %s",
				outfilename.c_str());

			// check age of the graph
			bool	imgold = imageIsOld(outfilename,
						ti->getInterval());

			// make sure we catch any problems that happen during
			// a call graph generation
			try {
				// compute the graph
				meteo::Graphics	graph(conf, stationname,
					ti->getInterval(), ti->getTime(), true,
					currentgraph, (images || imgold));

				if (images || imgold)
					graph.toFile(outfilename);

				// output image map if available
				if (imagemap)
					std::cout << graph.mapString(url);

				// output the image tag if asked to do so
				if (imagetag)
					std::cout << graph.imageTagString(
						outfilename);
			}
			catch (meteo::MeteoException& ex) {
				// report the problem
				mdebug(LOG_CRIT, MDEBUG_LOG, 0,
					"MeteoException caught: %s, %s",
					ex.getReason().c_str(),
					ex.getAddinfo().c_str());
			}
		}
	}

	// draw all the graphs
	std::vector<int>::iterator	i;
	for (j = g.begin(); j != g.end(); j++) {
		std::string	currentgraph = *j;
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "processing graph %s",
			currentgraph.c_str());
		for (i = intervals.begin(); i != intervals.end(); i++) {
			int	interval = *i;
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"processing interval %d", interval);
			// construct the file name
			std::string	outfilename = prefix + stationname + "-"
						+ currentgraph;
			switch (*i) {
			case 60:	outfilename += "-hour"; break;
			case 300:	outfilename += "-day"; break;
			case 1800:	outfilename += "-week"; break;
			case 7200:	outfilename += "-month"; break;
			case 86400:	outfilename += "-year"; break;
			}
			if (withtimestamps) {
				outfilename += "-" + timestamp(end);
			}
			outfilename += ".png";
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "outfilename %s",
				outfilename.c_str());

			// find out whether the image exists, and is less
			// than the interval old
			bool	imgold = imageIsOld(outfilename, interval);

			// again, catch all errors during graph generation
			try {
				// compute the graph
				meteo::Graphics	graph(conf, stationname,
					interval, end, false, currentgraph,
					(images || imgold));

				if (images || imgold)
					graph.toFile(outfilename);
			}
			catch (meteo::MeteoException& ex) {
				// report the problem
				mdebug(LOG_CRIT, MDEBUG_LOG, 0,
					"MeteoException caught: %s, %s",
					ex.getReason().c_str(),
					ex.getAddinfo().c_str());
			}
		}
	}

	exit(EXIT_SUCCESS);
}
