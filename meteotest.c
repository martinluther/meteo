/*
 * test.c -- try to get a sensor image from the station
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sercom.h>
#include <tcpcom.h>
#include <string.h>
#include <errno.h>
#include <meteodata.h>
#include <davis.h>
#include <meteo.h>

extern int	optind;
extern char	*optarg;
char		*location = "(unknown)";
int		inside = 0, outside = 1;

/*
 * get values from weather station
 */
static void	html_report(meteoaccess_t *d) {
	meteovalue_t	*v;
	rain_t		*rain;
	wind_t		*w;
	time_t		now;

	printf("<html>\n<head>\n"
		"<title>Current Weather in %s</title>\n"
		"</head>\n"
		"<body bgcolor=\"#ffffff\">\n", location);
	printf("<h1>Current Weather in %s</h1>\n", location);
	now = time(NULL);
	printf("<p>last measurement: %s</p>\n", ctime(&now));
	printf("<table border=\"1\">\n");
	printf("<tr bgcolor=\"#000000\">\n"
		"<th><font color=\"#ffffff\">Name</font></th>\n"
		"<th><font color=\"#ffffff\">Value</font></th>\n"
		"<th><font color=\"#ffffff\">When</font></th>\n"
		"</tr>\n");

	if (outside) {
		v = get_temperature(d);
		if ((v->flags & METEOVALUE_HASVALUE) || (debug))
			printf("<tr><td>outside temperature:</td>"
				"<td align=\"right\">%7.1f&deg;C</td>"
				"<td>&nbsp;</td></tr>\n",
				unitconvert(v->unit, UNIT_CELSIUS, v->value));
		if ((v->flags & METEOVALUE_HASMAX) || (debug))
			printf("<tr><td>outside temp max:</td>"
				"<td align=\"right\">%7.1f&deg;C</td>"
				"<td>%24.24s</td></tr>\n",
				unitconvert(v->unit, UNIT_CELSIUS, v->max),
				ctime(&v->maxtime));
		if ((v->flags & METEOVALUE_HASMIN) || (debug))
			printf("<tr><td>outside temp min:</td>"
				"<td align=\"right\">%7.1f&deg;C</td>"
				"<td>%24.24s</td></tr>\n",
				unitconvert(v->unit, UNIT_CELSIUS, v->min),
				ctime(&v->mintime));
	}

	if (inside) {
		v = get_temperature_inside(d);
		if ((v->flags & METEOVALUE_HASVALUE) || (debug))
			printf("<tr><td>inside temperature:</td>"
				"<td align=\"right\">%7.1f&deg;C</td>"
				"<td>&nbsp;</td></tr>\n",
				unitconvert(v->unit, UNIT_CELSIUS, v->value));
		if ((v->flags & METEOVALUE_HASMAX) || (debug))
			printf("<tr><td>inside temp max:</td>"
				"<td align=\"right\">%7.1f&deg;C</td>"
				"<td>%24.24s</td></tr>\n",
				unitconvert(v->unit, UNIT_CELSIUS, v->max),
				ctime(&v->maxtime));
		if ((v->flags & METEOVALUE_HASMIN) || (debug))
			printf("<tr><td>inside temp min:</td>"
				"<td align=\"right\">%7.1f&deg;C</td>"
				"<td>%24.24s</td></tr>\n",
				unitconvert(v->unit, UNIT_CELSIUS, v->min),
				ctime(&v->mintime));
	}

	if (outside) {
		v = get_humidity(d);
		if ((v->flags & METEOVALUE_HASVALUE) || (debug))
			printf("<tr><td>outside humidity:</td>"
				"<td align=\"right\">%5.0f %%</td>"
				"<td>&nbsp;</td></tr>\n", v->value);
		if ((v->flags & METEOVALUE_HASMAX) || (debug))
			printf("<tr><td>outside max hum:</td>"
				"<td align=\"right\">%5.0f %%</td>"
				"<td>%24.24s\n", v->max,
				ctime(&v->maxtime));
		if ((v->flags & METEOVALUE_HASMIN) || (debug))
			printf("<tr><td>outside min hum:</td>"
				"<td align=\"right\">%5.0f %%</td>"
				"<td>%24.24s</td></tr>\n",
				v->min,
				ctime(&v->mintime));
	}

	if (inside) {
		v = get_humidity_inside(d);
		if ((v->flags & METEOVALUE_HASVALUE) || (debug))
			printf("<tr><td>inside humidity:</td>"
				"<td align=\"right\">%5.0f %%</td>"
				"<td>&nbsp;</td></tr>\n",
				v->value);
		if ((v->flags & METEOVALUE_HASMAX) || (debug))
			printf("<tr><td>inside max hum:</td>"
				"<td align=\"right\">%5.0f %%</td>"
				"<td> %24.24s</td></tr>\n", v->max,
				ctime(&v->maxtime));
		if ((v->flags & METEOVALUE_HASMIN) || (debug))
			printf("<tr><td>inside min hum:</td>"
				"<td align=\"right\">%5.0f %%</td>"
				"<td>%24.24s</td></tr>\n", v->min,
				ctime(&v->mintime));
	}

	v = get_barometer(d);
	if ((v->flags & METEOVALUE_HASVALUE) || (debug))
		printf("<tr><td>barometer:</td>"
			"<td align=\"right\">%8.1f hPa</td>"
			"<td>&nbsp</td></tr>\n",
		unitconvert(v->unit, UNIT_HPA, v->value));

	if (outside) {
		w = get_wind(d);
		if ((w->flags & METEOVALUE_HASVALUE) || (debug))
			printf("<tr><td>wind speed:</td>"
				"<td align=\"right\">%7.1f m/s</td>"
				"<td>&nbsp;</td></tr>\n",
			unitconvert(w->unit, UNIT_MPS, w->speed));
		if ((w->flags & METEOVALUE_HASDIRECTION) || (debug))
			printf("<tr><td>wind direction:</td>"
				"<td align=\"right\">%5.0f&deg;</td>"
				"<td>&nbsp;</td></tr>\n",
				w->direction);
		if ((w->flags & METEOVALUE_HASVALUE) || (debug))
			printf("<tr><td>max wind speed:</td>"
				"<td align=\"right\">%7.1f m/s</td>"
				"<td>%24.24s</td></tr>\n",
			unitconvert(w->unit, UNIT_MPS, w->speedmax),
			ctime(&w->maxtime));
	}

	if (outside) {
		rain = get_rain(d);
		if ((rain->flags & METEOVALUE_HASVALUE) || (debug))
			printf("<tr><td>rain:</td>"
				"<td align=\"right\">%7.1f mm</td>"
				"<td>&nbsp;</td></tr>\n",
				rain->rain);
		if ((rain->flags & METEOVALUE_HASTOTAL) || (debug))
			printf("<tr><td>total rain:</td>"
				"<td align=\"right\">%7.1f mm</td>"
				"<td>&nbsp;</td></tr>\n",
				rain->raintotal);
		printf("</table>\n");
	}

	printf("<p>Data retrieved with the meteo-0.1 package</p>\n");
	printf("<hr />&copy; 2001 <a href=\"mailto:afm@othello.ch\">"
		"Dr. Andreas M&uuml;ller</a>\n");

	printf("</body></html>\n");
}

int	main(int argc, char *argv[]) {
	meteocom_t	*m;
	meteodata_t	*md;
	meteoaccess_t	*d;
	int		c;
	char		*url;
	int		archive = 0;
	int		speed;

	/* parse command line						*/
	while (EOF != (c = getopt(argc, argv, "adiol:")))
		switch (c) {
		case 'a':
			archive = 1;
			break;
		case 'd':
			debug++;
			break;
		case 'i':
			inside = 1 - inside;
			break;
		case 'o':
			outside = 1 - outside;
			break;
		case 'l':
			location = optarg;
			break;
		}

	/* check for data source argument				*/
	if ((argc - optind) < 1) {
		fprintf(stderr, "%s:%d: wrong number of arguments\n",
			__FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}
	url = argv[optind];
	if (debug)
		fprintf(stderr, "%s:%d: connecting to %s\n", __FILE__,
			__LINE__, url);
	if (0 == strncmp(url, "file://", 7)) {
		speed = 2400;
		m = sercom_new(url, speed);
	} else {
		m = tcpcom_new(url);
	}
	if (m == NULL) {
		fprintf(stderr, "%s:%d: cannot open device: %s (%d)\n",
			__FILE__, __LINE__, strerror(errno), errno);
		exit(EXIT_FAILURE);
	}

	d = davis_new(m);

	if (archive) {
		md = get_update(d);
		printf("Baro:    %8.1f\n",
			unitconvert(md->barometer->unit,
			UNIT_HPA, md->barometer->value));
		printf("Hin:     %8.1f\n",
			unitconvert(md->humidity_inside->unit,
			UNIT_NONE, md->humidity_inside->value));
		printf("Hout:    %8.1f\n",
			unitconvert(md->humidity->unit,
			UNIT_NONE, md->humidity->value));
		printf("Rain:    %8.1f\n",
			unitconvert(md->rain->unit, UNIT_MM, md->rain->rain));
		printf("TinAvg:  %8.1f\n",
			unitconvert(md->temperature_inside->unit,
			UNIT_CELSIUS, md->temperature_inside->value));
		printf("TOutAvg: %8.1f\n",
			unitconvert(md->temperature->unit,
			UNIT_CELSIUS, md->temperature->value));
		printf("WspAvg:  %8.1f\n",
			unitconvert(md->wind->unit,
			UNIT_MPS, md->wind->speed));
		printf("Wdir:    %8.1f\n", (double)md->wind->direction);
		printf("Gust:    %8.1f\n", unitconvert(md->wind->unit,
			UNIT_MPS, md->wind->speedmax));
	} else {
		html_report(d);
	}

	exit(EXIT_SUCCESS);
}
