/*
 * meteolast.c c -- get the most recent update to the database and format it
 *                  in an XML based format
 * 
 * (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <meteo.h>
#include <mysql.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#include <string>
#include <list>
#include <mdebug.h>
#include <printver.h>
#include <Configuration.h>
#include <Query.h>
#include <iostream>

void	usage(const char *progname) {
}

int	main(int argc, char *argv[]) {
	std::string	conffilename(METEOCONFFILE);
	std::string	stationname("undefined");
	std::string	logurl("file:///-");
	bool		englishUnits = false;
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
	while (EOF != (c = getopt(argc, argv, "l:EMdf:Vh?t:+w:xH")))
		switch (c) {
		case 'l':
			logurl = optarg;
			break;
		case 'E':
			englishUnits = true;
			break;
		case 'M':
			englishUnits = false;
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
		meteo::QueryProcessor	qp(conf, *i);

		// retrieve a record for this station
		meteo::Datarecord	r;
		if (searchbackwards)
			r = qp.lastRecord(timekey, window);
		else
			r = qp.firstRecord(timekey, window);

		// convert units if necessary

		// display in the right format
		if (xmloutput) {
			if (addxmlheader)
				std::cout << "<?xml version=\"1.0\"?>\n"
					<< "<meteolist>\n";
			std::cout << r.xml() ;
			if (addxmlheader)
				std::cout << "</meteolist>\n";
		} else {
			std::cout << r.plain();
		}
	}

	exit(EXIT_SUCCESS);
}
