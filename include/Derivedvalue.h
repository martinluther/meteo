/*
 * Derivedvalue.h -- class encapsulating derived value computation, based
 *                   on temperature
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Derivedvalue.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _Derivedvalue_h
#define _Derivedvalue_h

namespace meteo {

typedef	enum	temperature_unit_e {
	temperature_unit_undefined = -1,
	temperature_unit_celsius = 0,
	temperature_unit_fahrenheit = 1
} temperature_unit_t;

class Derivedvalue {
	temperature_unit_t	get(void);
protected:
	static temperature_unit_t	tu;
public:
	Derivedvalue(void);
	~Derivedvalue(void) { }
};

} /* namespace meteo */

#endif /* _Derivedvalue_h */
