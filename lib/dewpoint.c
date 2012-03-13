/*
 * dewpoint.c -- dew point computation from relative humidity and temperature
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: dewpoint.c,v 1.2 2002/03/03 22:09:38 afm Exp $
 */
#include <dewpoint.h>
#include <math.h>

double	dewpoint(double humidity, double tempC) {
	double	ews, dp;
	ews = humidity * 0.01 * exp((17.502 * tempC)/(240.9 + tempC));
	if (ews < 0)
		return -273.0;
	dp = 240.9 * log(ews)/(17.5 - log(ews));
	if (isnan(dp))
		return -273.0;
	else
		return dp;
}
