/*
 * Heatindex.cc -- class encapsulating dew point computation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Heatindex.cc,v 1.2 2004/02/25 23:48:05 afm Exp $
 */
#include <Heatindex.h>
#include <math.h>
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
