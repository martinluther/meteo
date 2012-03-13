/*
 * Derivedvalue.cc -- class encapsulating derived value computation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Derivedvalue.cc,v 1.3 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <Derivedvalue.h>
#ifdef HAVE_MATH_H
#include <math.h>
#endif /* HAVE_MATH_H */
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
