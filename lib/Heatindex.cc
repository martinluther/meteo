/*
 * Heatindex.cc -- class encapsulating dew point computation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Heatindex.cc,v 1.3 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <Heatindex.h>
#ifdef HAVE_MATH_H
#include <math.h>
#endif /* HAVE_MATH_H */
#include <QueryProcessor.h>

namespace meteo {

double	Heatindex::operator()(double humidity, double temp) const {
	double	tempF;

	// make sure we have Celsius
	if (tu == temperature_unit_celsius)
		tempF = 32. + temp * 9. / 5.;
	else
		tempF = temp;

	double	heat_index =
		-42.379
		+ 2.04901523 * tempF
		+ 10.14333127                * humidity
		- 0.22475541 * tempF         * humidity
		- 0.00683783 * tempF * tempF
		- 0.05481717                 * humidity * humidity
		+ 0.00122874 * tempF * tempF * humidity
		+ 0.00085282 * tempF         * humidity * humidity
		- 0.00000199 * tempF * tempF * humidity * humidity;
	return(heat_index);
}

} /* namespace meteo */
