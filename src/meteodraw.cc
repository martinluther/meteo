/*
 * meteodraw.cc -- create meteo drawings
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 * $Id: meteodraw.cc,v 1.21 2004/03/20 01:15:55 afm Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <time.h>
#include <vector>
#include <set>
#include <algorithm>
#include <Configuration.h>
#include <Graphics.h>
#include <mdebug.h>
#include <Timestamp.h>
#include <printver.h>
#include <errno.h>
#include <iostream>
#include <Timelabel.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <Lock.h>
#include <MeteoException.h>


// some type definitions that will be handy
typedef	std::list<meteo::Timelabel>	timelabellist_t;
typedef std::set<std::string>	stringset_t;
typedef	std::vector<int>	intvector_t;

// some global flags, should rather wrap them into an object
static bool	withtimestamps = false;
static bool	imagemap = false;
static bool	images = true;
static bool	imagetag = false;
time_t		end;
std::string	stationname("undefined");
std::string	prefix;
std::string	url;

// format a time_t as a string
static std::string	timestamp(time_t t) {
	char		tsbuffer[32];
	struct tm	*lt = localtime(&t);
	strftime(tsbuffer, sizeof(tsbuffer), "%Y%m%d%H%M%S", lt);
	return std::string(tsbuffer);
}

// test whether an image file is old, compared to the interval. It is old
// if new data could have arrived in the meantime
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

// readintervals goes through the command line to read the intervals for which
// graphs should be produced, and adds them to the intervals set
static void	readintervals(int argc, char *argv[],
			intvector_t& intervals, bool havelabeled) {
	// read all the remaining arguments, they should be valid interval
	// numbers (check this)
	if (optind == argc) {
		// when optind is argc, there are no arguments left. This means
		// that there aren't any intervals specified. Unless we have
		// some labels set, we need to dd all intervals to the intervals
		// set
		if (!havelabeled) {
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
		// this is the case where we have read the required intervals
		// from the command line
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
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "read all remaining arguments");
}

// usage displays a usage message on stdout when an error is found or the
// help option is invoked
static void	usage(void) {
	printf(
"usage: meteodraw [ -dtVmIx ] [ -l logurl ] [ -c dir ] [ -G graph ] \n"
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
"	-f config	use configuration file config\n"
"	-p prefix	store images in files with this prefix\n"
"	-u url		use this URL prefix for image maps\n"
"	-e endtime	specify end time of graph\n"
"	-x		lock processes according to graphs attributes\n");
}

// draw a graph based on a timelabel
void	drawlabeled(const std::string& currentgraph, const meteo::Timelabel& ti) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "working on graph %s",
		currentgraph.c_str());
	// write the graph to a file
	std::string	outfilename = prefix + currentgraph + "-"
		+ ti.getString() + ".png";
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "outfilename %s", outfilename.c_str());

	// check age of the graph
	bool	imgold = imageIsOld(outfilename, ti.getInterval());
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "image is %sold",
		(imgold) ? "" : "not ");
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "(images || imgold) = %s",
		(images || imgold) ? "true" : "false");

	// make sure we catch any problems that happen during
	// a call graph generation
	try {
		// compute the graph
		meteo::Graphics	graph(ti.getInterval(), ti.getTime(), true,
			currentgraph, (images || imgold));
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "graph %s constructed",
			currentgraph.c_str());

		if (images || imgold) {
			graph.toFile(outfilename);
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "graph written to %s",
				outfilename.c_str());
		}

		// output image map if available
		if (imagemap) {
			std::cout << graph.mapString(url, stationname);
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "image map written");
		}

		// output the image tag if asked to do so
		if (imagetag) {
			std::cout << graph.imageTagString( outfilename);
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "IMG tag written");
		}
	}
	catch (meteo::MeteoException& ex) {
		// report the problem
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "MeteoException caught: %s, %s",
			ex.getReason().c_str(), ex.getAddinfo().c_str());
	}
}

// get the file name suffix based on the interval
std::string	getsuffix(const int interval) {
	switch (interval) {
		case 60:	return std::string("-hour"); break;
		case 300:	return std::string("-day"); break;
		case 1800:	return std::string("-week"); break;
		case 7200:	return std::string("-month"); break;
		case 86400:	return std::string("-year"); break;
	}
	throw meteo::MeteoException("illegal interval found", "");
}

// draw a graph based on the interval
static void	drawinterval(const std::string& currentgraph, int interval) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "processing graph %s, interval %d",
		currentgraph.c_str(), interval);
	// construct the file name
	std::string	outfilename = prefix + currentgraph
		+ getsuffix(interval);

	// add a timestamp if requested
	if (withtimestamps) {
		outfilename += "-" + timestamp(end);
	}

	// add the png extension, we only do png, so we don't have to worry
	// about possible other formats
	outfilename += ".png";
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "outfilename %s", outfilename.c_str());

	// find out whether the image exists, and is less than the interval old
	bool	imgold = imageIsOld(outfilename, interval);

	// again, catch all errors during graph generation
	try {
		// compute the graph
		meteo::Graphics	graph(interval, end, false, currentgraph,
			(images || imgold));

		if (images || imgold)
			graph.toFile(outfilename);
	}
	catch (meteo::MeteoException& ex) {
		// report the problem
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "MeteoException caught: %s, %s",
			ex.getReason().c_str(), ex.getAddinfo().c_str());
	}
}

// the main program, it does the following
// - check the command line arguments, and set various variables according
//   the flags found
// - set up logging
// - open the Configuration
// - check which intervals should be drawn
static int	meteodraw(int argc, char *argv[]) {
	int		c;
	std::string	conffilename(METEOCONFFILE);
	std::string	logurl("file:///-");
	stringset_t	requested;
	intvector_t	intervals;
	timelabellist_t	labeled;
	meteo::Timelabel	tl;
	bool		exclusive = false;

	// remember current time
	time(&end);

	// parse the command line
	while (EOF != (c = getopt(argc, argv, "adc:e:f:g:iIl:L:mp:ts:V?u:x")))
		switch (c) {
		case '?':
			usage();
			exit(EXIT_SUCCESS);
			break;
		case 'l':
			logurl = std::string(optarg);
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
			end = meteo::Timestamp(optarg).getTime();
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
		case 'x':
			exclusive = true;
			break;
		default:
			mdebug(LOG_ERR, MDEBUG_LOG, 0, "unkown option %c", c);
			break;
		}

	// set up logging
	mdebug_setup("meteodraw", logurl.c_str());
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "set up logging to %s",
		logurl.c_str());

	// warn if we have no station name, some things will not work, or
	// at least will produce strange results
	if ("undefined" == stationname) {
		fprintf(stderr, "%s: station name not set\n", argv[0]);
		mdebug(LOG_WARNING, MDEBUG_LOG, 0, "station name not specified");
	}

	// make sure we have a configuration file, and initialize the config
	// reader
	meteo::Configuration	conf(conffilename);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "read configuration %s",
		conffilename.c_str());

	// create lock, so the system cannot be flooded by requests
	meteo::Lock	lock(conf);
	if (exclusive)
		// wait until we can fetch the lock
		if (!lock.enter(true)) {
			mdebug(LOG_ERR, MDEBUG_LOG, 0, "lock failed");
			throw meteo::MeteoException("lock failed", "");
		}

	// read the intervals from the command line
	readintervals(argc, argv, intervals, (labeled.size() == 0));

	// do all the labeled graphs
	timelabellist_t::const_iterator	ti;
	for (ti = labeled.begin(); ti != labeled.end(); ti++) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "working on label %s",
			ti->getString().c_str());
		// go through the list of requested graph names
		stringset_t::iterator	j;
		for (j = requested.begin(); j != requested.end(); j++) {
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "processing graph %s",
				j->c_str());
			// actually draw the timelabeled graph
			drawlabeled(*j, *ti);

		}
	}

	// if we had any labeled graphs, we should stop here
	if (labeled.size() != 0) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "labeled graphs drawn, "
			"suppressing any interval graphing, exiting");
		if (exclusive)
			lock.leave();
		exit(EXIT_SUCCESS);
	}

	// do the same for all intervals
	intvector_t::iterator	i;
	for (i = intervals.begin(); i != intervals.end(); i++) {
		int	interval = *i;
		stringset_t::iterator	j;
		for (j = requested.begin(); j != requested.end(); j++) {
			std::string	currentgraph = *j;
			// actually draw the graph
			drawinterval(currentgraph, interval);
		}
	}

	// release the lock
	if (exclusive)
		lock.leave();

	exit(EXIT_SUCCESS);
}

int	main(int argc, char *argv[]) {
	try {
		meteodraw(argc, argv);
	} catch (meteo::MeteoException& me) {
		fprintf(stderr, "MeteoException in meteodraw: %s/%s\n",
			me.getReason().c_str(), me.getAddinfo().c_str());
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
