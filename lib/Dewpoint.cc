/*
 * Dewpoint.cc -- class encapsulating dew point computation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Dewpoint.cc,v 1.6 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <Dewpoint.h>
#ifdef HAVE_MATH_H
#include <math.h>
#endif /* HAVE_MATH_H */
#include <QueryProcessor.h>

namespace meteo {

double	Dewpoint::operator()(double humidity, double temp) const {
	double	tempC, resultC;
	double	ews, dp;

	// convert to celsius
	if (tu == temperature_unit_fahrenheit)
		tempC = 5. * (temp - 32.) / 9.;
	else
		tempC = temp;

	// compute the dew point (in the celsius scale)
	ews = humidity * 0.01 * ::exp((17.502 * tempC)/(240.9 + tempC));
	if (ews < 0)
		return -273.0;
	dp = 240.9 * ::log(ews)/(17.5 - ::log(ews));
	if (isnan(dp))
		resultC = -273.0;
	else
		resultC = dp;

	// convert back to Fahrenheit if necessary
	if (tu == temperature_unit_fahrenheit)
		return 32. + resultC * 9. / 5.;
	return resultC;
}

} /* namespace meteo */
