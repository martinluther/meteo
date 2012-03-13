/*
 * dewpoint.c -- dew point computation from relative humidity and temperature
 *
 * (c) 2001 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: dewpoint.c,v 1.1 2002/01/18 23:34:29 afm Exp $
 */
#include <dewpoint.h>
#include <math.h>

double	dewpoint(double humidity, double tempC) {
	double	ews;
	ews = humidity * 0.01 * exp((17.502 * tempC)/(240.9 + tempC));
	return 240.9 * log(ews)/(17.5 - log(ews));
}
