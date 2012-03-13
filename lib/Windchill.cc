/*
 * Windchill.cc -- class encapsulating windchill computation, formulas 
 *                 contributed by Jeff Wabik
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Windchill.cc,v 1.3 2004/02/25 23:48:06 afm Exp $
 */
#include <Windchill.h>
#include <math.h>
#include <TemperatureConverter.h>
#include <WindConverter.h>
#include <MeteoException.h>
#include <mdebug.h>
#include <Field.h>

namespace meteo {

std::string	Windchill::wu = "undefined";

std::string	Windchill::getWindunit(void) {
	// query the database for the temperature unit in use
	return	Field().getUnit("wind");
}

Windchill::Windchill(void) {
	if (wu == "undefined")
		wu = getWindunit();
}

double	Windchill::operator()(double windspeed, double temperature) const {
	// convert temperature to F
	double	tempF = temperature;
	if (tu == temperature_unit_celsius)
		tempF = TemperatureConverter("F")(temperature, "C");
	// convert windspeed to mph
	double	windmph = WindConverter("mph")(windspeed, wu);

	// compute windchill based on formula contributed by Jeff Wabik
	double	wcf =	35.74 +
			(0.6215 * tempF) -
			(35.57 + 0.4275 * tempF) * pow(windmph ,0.16);

	// check for reasonable ranges
	double	chill;
	if (tempF < 60)
		chill = wcf;
	else
		chill = tempF;

	// make sure chill is smaller than current temperature
	if (chill > tempF)
		chill = tempF;

	// the chill value so obtained is the most reasonable value
	if (tu == temperature_unit_celsius)
		chill = TemperatureConverter("C")(tempF, "F");
	return chill;
}

} /* namespace meteo */
