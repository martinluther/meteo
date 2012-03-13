/*
 * meteodbdump.cc -- dump the meteo database for a given set of stations
 *                   to suitable load files, and provide load stations
 *                   that will load the files again
 *
 *                   This program is written so that it can be compiled and
 *                   run independently of the meteo library. I use it to
 *                   synchronize the database on my notebook with timon's.
 *                   It isn't documented in a manual page either, because it
 *                   is hardly useful anybody but me.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 * $Id: meteodbdump.cc,v 1.12 2004/02/25 23:41:13 afm Exp $
 */
#include <stdio.h>
#include <stdlib.h>
#include <ChunkDumper.h>
#include <Configuration.h>
#include <MeteoException.h>
#include <StationInfo.h>
#include <string>
#include <vector>
#include <errno.h>
#include <strings.h>
#include <unistd.h>
#include <mdebug.h>
#include <fstream>

static int	meteodbdump(int argc, char *argv[]) {
	meteo::stringlist	stations;
	std::string	basename("./");
	std::string	logurl("file:///-");
	std::string	conffile(METEOCONFFILE);
	int	chunksize = 86400; 	// one day worth of data
	int	c;
	bool	raw = false;

	// parse the command line
	while (EOF != (c = getopt(argc, argv, "db:l:s:f:c:r")))
		switch (c) {
		case 'f':
			conffile = std::string(optarg);
			break;
		case 'l':
			logurl = std::string(optarg);
			break;
		case 'd':
			debug++;
			break;
		case 's':
			stations.push_back(std::string(optarg));
			break;
		case 'b':
			basename = std::string(optarg);
			break;
		case 'c':
			chunksize = atoi(optarg);
			break;
		case 'r':
			raw = !raw;
			break;
		}

	// set up logging
	mdebug_setup("meteopoll", logurl.c_str());
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "set up logging to %s",
		logurl.c_str());

	// open configuration
	meteo::Configuration	conf(conffile);

	// create a chunkdumper for basename and chunksize
	meteo::ChunkDumper	cd(basename, chunksize);

	// set some parameters for this station
	if (raw)
		cd.enableRaw();

	// if there are more arguments, use them to select the tables we
	// want to dump
	if (argc > optind) {
		cd.disableAvg(); cd.disableSdata();
		for (int i = optind; i < argc; i++) {
			if (0 == strcmp(argv[i], "avg"))
				cd.enableAvg();
			if (0 == strcmp(argv[i], "sdata"))
				cd.enableSdata();
		}
	}

	// do the work in a function, so that destructors are properly
	// called when it completes
	cd.dumpStations(stations);

	// that's it
	exit(EXIT_SUCCESS);
}

int	main(int argc, char *argv[]) {
	try {
		meteodbdump(argc, argv);
	} catch (meteo::MeteoException& me) {
		fprintf(stderr, "MeteoException in meteodbdump: %s/%s\n",
			me.getReason().c_str(), me.getAddinfo().c_str());
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
