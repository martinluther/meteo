//
// meteomap.cc
//
// (c) 2004 Dr. Andreas Mueller, Beratung und Entwicklung
// $Id: meteomap.cc,v 1.1 2007/08/29 21:00:23 afm Exp $
//
#include <stdlib.h>
#include <stdio.h>
#include <Mapfile.h>
#include <iostream>

int	main(int argc, char *argv[]) {
	// make sure there is exactly one argument, the map file
	if (argc != 2) {
		fprintf(stderr, "wrong number of arguments: map file\n");
		exit(EXIT_FAILURE);
	}

	try {
		// open the mapfile
		meteo::Mapfile	m(argv[1], false);

		// read the map file atomically
		std::string	map = m.read();
		std::cout << map << std::endl;
	} catch (...) {
		std::cerr << "cannot read map file" << std::endl;
		exit(EXIT_FAILURE);
	}

	// that's it
	exit(EXIT_SUCCESS);
}
