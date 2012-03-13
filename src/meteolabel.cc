/*
 * meteolabel.cc -- utility program to convert timestamps into labels
 *                  and navigating up and down
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <Timelabel.h>
#include <MeteoException.h>
#include <iostream>
#include <unistd.h>

int	main(int argc, char *argv[]) {
	int	c;
	bool	outtime = false, outctime = false, outlabel = true,
		outtitle = false;

	// read the command line
	while (EOF != (c = getopt(argc, argv, "scLt")))
		switch (c) {
		case 's':
			// output as timestamp
			outtime = true;
			break;
		case 'c':
			// output ctime string
			outctime = true;
			break;
		case 't':
			// output as string suitable for a title
			outtitle = true;
			break;
		case 'L':
			// output as a label
			outlabel = false;
			break;
		}

	// there must be at least one more arguments
	if (optind >= argc) {
		throw meteo::MeteoException("must specify label argument", "");
	}

	// next argument is a label
	std::string		label = argv[optind++];
	meteo::Timelabel	tl;
	try {
		tl = meteo::Timelabel(label);
	} catch (...) {
		std::cerr << "cannot convert label '" << label << "'"
			<< std::endl;
		exit(EXIT_FAILURE);
	}

	// following arguments are operators
	for (int i = optind; i < argc; i++) {
		if (!strcmp(argv[i], "up")) {
			tl = tl.up();
		}
		if (!strcmp(argv[i], "upup")) {
			tl = tl.upup();
		}
		if (!strcmp(argv[i], "upupup")) {
			tl = tl.upupup();
		}
		if (!strcmp(argv[i], "down")) {
			tl = tl.down();
		}
		if (!strcmp(argv[i], "downdown")) {
			tl = tl.downdown();
		}
		if (!strcmp(argv[i], "downdowndown")) {
			tl = tl.downdowndown();
		}
		if (!strcmp(argv[i], "previous")) {
			tl = tl.previous();
		}
		if (!strcmp(argv[i], "next")) {
			tl = tl.next();
		}
	}

	// output in the desired format
	if (outtime)
		std::cout << tl.getTime() << std::endl;
	if (outctime)
		std::cout << tl.getCtime();	// no endl, ctime includes one
	if (outlabel)
		std::cout << tl.getString() << std::endl;
	if (outtitle)
		std::cout << tl.getTitle() << std::endl;

	exit(EXIT_SUCCESS);
}

