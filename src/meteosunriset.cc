//
// meteosunriset.cc -- program to compute sunrise and sunset for a given
//                     station
//
// (c) 2004 Dr. Andreas Mueller, Beratung und Entwicklung
// $Id: meteosunriset.cc,v 1.3 2006/05/07 19:47:22 afm Exp $
//
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <printver.h>
#include <string>
#include <Configuration.h>
#include <MeteoException.h>
#include <StationInfo.h>
#include <Timelabel.h>
#include <Sun.h>
#include <unistd.h>
#include <stdlib.h>

// main function for sunriset program
void	meteosunriset(int argc, char *argv[]) {
	std::string	station;
	std::string	conffilename(METEOCONFFILE);
	std::string	formatstring("%H:%M");
	double		elevation = -0.8333;
	int		c;

	// parse command line
	while (EOF != (c = getopt(argc, argv, "e:s:f:F:V")))
		switch (c) {
		case 'e':
			elevation = atof(optarg);
			break;
		case 's':
			station = std::string(optarg);
			break;
		case 'f':
			conffilename = std::string(optarg);
			break;
		case 'F':
			formatstring = std::string(optarg);
			break;
		case 'V':
			fprintver(stdout);
			exit(EXIT_SUCCESS);
			break;
		}

	// make sure the station was specified
	if (station.empty()) {
		throw meteo::MeteoException("must specify station", "");
	}

	// open the configuration file
	meteo::Configuration	conf(conffilename);

	// get the latitude and longitude from the database
	meteo::StationInfo	si(station);
	double	longitude = si.getLongitude();
	double	latitude = si.getLatitude();

	// create a Sun object
	meteo::Sun	thesun(longitude, latitude, elevation);

	// the remaining arguments are a list of datestamps
	for (int i = optind; i < argc; i++) {
		char	buffer[1024];
		// get the time
		printf("%-10.10s ", argv[i]);
		meteo::Timelabel	tl(argv[i]);

		// compute sunrise and sunset times
		time_t	sunrise	= thesun.sunrise(tl.getTime());
		time_t	sunset	= thesun.sunset(tl.getTime());

		// format the strings for output
		struct tm	*tp = localtime(&sunrise);
		strftime(buffer, sizeof(buffer), formatstring.c_str(), tp);
		printf("%s ", buffer);
		tp = localtime(&sunset);
		strftime(buffer, sizeof(buffer), formatstring.c_str(), tp);
		printf("%s\n", buffer);
	}
}

int	main(int argc, char *argv[]) {
	try {
		meteosunriset(argc, argv);
	} catch (meteo::MeteoException& me) {
		fprintf(stderr, "MeteoException in meteosunriset: %s/%s\n",
			me.getReason().c_str(), me.getAddinfo().c_str());
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
