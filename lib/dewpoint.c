/*
 * dewpoint.c -- dew point computation from relative humidity and temperature
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: dewpoint.c,v 1.3 2003/06/08 22:32:10 afm Exp $
 */
#include <dewpoint.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <mdebug.h>

#define	UNIT_UNKNOWN	0
#define	UNIT_FAHRENHEIT	1
#define	UNIT_CELSIUS	2

static int	tempunit = 0;

void	find_tempunit(const meteoconf_t *mc, const char *stationname) {
	const char	*unitstr;
	char	xpath[1024];
	/* assigns the tempunit variable the right unit constant	*/
	
	snprintf(xpath, sizeof(xpath),
		"/meteo/station[@name='%s']/unit/temperature", stationname);
	unitstr = xmlconf_get_abs_string(mc, xpath, "C");
	if ((0 == strcmp(unitstr, "F")) ||
		(0 == strcmp(unitstr, "degF")) ||
		(0 == strcmp(unitstr, "degrees Fahrenheit"))) {
		tempunit = UNIT_FAHRENHEIT; return;
	}
	tempunit = UNIT_CELSIUS;
}

double	dewpoint(double humidity, double temp) {
	double	tempC;
	double	ews, dp;

	if (tempunit == UNIT_UNKNOWN)
		mdebug(LOG_ERR, MDEBUG_LOG, 0,
			"find_tempunit was not called in "
			"program, dewpoint may be wrong");

unitknown:
	if (tempunit == UNIT_CELSIUS) {
		tempC = temp;
		goto celsiusknown;
	}
	tempC = 5. * (temp - 32.) / 9.;

celsiusknown:
	ews = humidity * 0.01 * exp((17.502 * tempC)/(240.9 + tempC));
	if (ews < 0)
		return -273.0;
	dp = 240.9 * log(ews)/(17.5 - log(ews));
	if (isnan(dp))
		return -273.0;
	else
		return dp;
}
