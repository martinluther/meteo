/*
 * Dewpoint.cc -- class encapsulating dew point computation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <Dewpoint.h>
#include <math.h>

namespace meteo {

temperature_unit_t	Dewpoint::fromConfiguration(const Configuration& conf) {
	std::string	t;
	// here is a limitation: since we don't know what station we are
	// working with when we are called, we just take the first
	// station description found in the XML file. This may be wrong
	// if different stations use different units.
	t = conf.getString("/meteo/station/unit/temperature", "");
	if ((t == "C") || (t == "degC") || (t == "Celsius") || (t == ""))
		return temperature_unit_celsius;
	return temperature_unit_fahrenheit;
}

Dewpoint::Dewpoint(const Configuration& conf) {
	tu = fromConfiguration(conf);
}

Dewpoint::Dewpoint(void) {
	tu = fromConfiguration(Configuration());
}

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
