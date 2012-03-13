/*
 * meteolast.c c -- get the most recent update to the database and format it
 *                  in an XML based format
 * 
 * (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: meteolast.cc,v 1.14 2004/02/27 16:03:50 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#include <unistd.h>
#include <string>
#include <list>
#include <mdebug.h>
#include <printver.h>
#include <Configuration.h>
#include <Query.h>
#include <QueryProcessor.h>
#include <MeteoException.h>
#include <Timestamp.h>
#include <iostream>

void	usage(const char *progname) {
}

static int	meteolast(int argc, char *argv[]) {
	std::string	conffilename(METEOCONFFILE);
	std::string	stationname("undefined");
	std::string	logurl("file:///-");
	bool		xmloutput = false;
	bool		searchbackwards = true;
	bool		addxmlheader = false;
	time_t		timekey;
	int		c;
	int		window = 3600;

	// initialize the logging stuff
	mdebug_setup("meteolast", logurl.c_str());
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "set up logging to %s",
		logurl.c_str());

	// by default, we will work from the current time
	timekey = time(NULL);

	// parse command line
	while (EOF != (c = getopt(argc, argv, "l:df:Vh?t:T:+w:xH")))
		switch (c) {
		case 'l':
			logurl = optarg;
			break;
		case 'H':
			addxmlheader = true;
			break;
		case 'd':
			debug++;
			break;
		case 'f':
			conffilename = optarg;
			break;
		case 'V':
			fprintver(stdout);
			exit(EXIT_SUCCESS);
			break;
		case 'h':
		case '?':
			usage(argv[0]);
			exit(EXIT_SUCCESS);
			break;
		case 'T':
			do { // creates a block, keeps compiler from complaining
			     // about skipping ts initialization
				meteo::Timestamp	ts(optarg);
				timekey = ts.getTime();
			} while (0);
			break;
		case 't':
			timekey = atoi(optarg);
			break;
		case 'x':
			xmloutput = true;
			break;
		case '+':
			searchbackwards = false;
			break;
		case 'w':
			window = atoi(optarg);
			break;
		}

	// log search base
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "base timekey %d: %s", timekey,
		ctime(&timekey));

	// create a list of station names from the remaining arguments
	std::list<std::string>	stationlist;
	for (int i = optind; i < argc; i++)
		stationlist.push_back(std::string(argv[i]));

	// create a configuration object from the config filename
	meteo::Configuration	conf(conffilename);

	// iterate through the station names
	for (std::list<std::string>::const_iterator i = stationlist.begin();
		i != stationlist.end(); i++) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "processing station %s",
			i->c_str());

		// create a QueryProcessor
		meteo::QueryProcessor	qp(false);

		// retrieve a record for this station
		meteo::Datarecord	r;
		if (searchbackwards)
			r = qp.lastRecord(timekey, *i, window);
		else
			r = qp.firstRecord(timekey, *i, window);

		// display in the right format
		if (xmloutput) {
			if (addxmlheader)
				std::cout << "<?xml version=\"1.0\"?>\n"
					<< "<meteolist>\n";
			std::cout << r.xml(*i) ;
			if (addxmlheader)
				std::cout << "</meteolist>\n";
		} else {
			std::cout << r.plain(*i);
		}
	}

	exit(EXIT_SUCCESS);
}

int	main(int argc, char *argv[]) {
	try {
		meteolast(argc, argv);
	} catch (meteo::MeteoException &me) {
		fprintf(stderr, "MeteoException in meteolast: %s/%s\n",
			me.getReason().c_str(), me.getAddinfo().c_str());
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
