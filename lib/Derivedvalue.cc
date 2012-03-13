/*
 * Derivedvalue.cc -- class encapsulating derived value computation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <Derivedvalue.h>
#include <math.h>
#include <QueryProcessor.h>
#include <MeteoException.h>

namespace meteo {

temperature_unit_t	Derivedvalue::tu = temperature_unit_undefined;

temperature_unit_t	Derivedvalue::get(void) {
	QueryProcessor	qp(false);

	// query the database for the temperature unit in use
	BasicQueryResult	qr
		= qp("select unit from mfield where name = 'temperature'");
	if (qr.size() != 1) {
		throw MeteoException("temperature not found", "");
	}
	std::string	t;
	t = (*qr.begin())[0];
	if ((t == "C") || (t == "degC") || (t == "Celsius") || (t == ""))
		return temperature_unit_celsius;
	return temperature_unit_fahrenheit;
}

Derivedvalue::Derivedvalue(void) {
	if (tu == temperature_unit_undefined)
		tu = get();
}

} /* namespace meteo */
