/*
 * Dewpoint.h -- class encapsulating dewpoint computation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Dewpoint_h
#define _Dewpoint_h

#include <Configuration.h>

namespace meteo {

typedef	enum	temperature_unit_e {
	temperature_unit_celsius = 0,
	temperature_unit_fahrenheit = 1
} temperature_unit_t;

class Dewpoint {
	temperature_unit_t	tu;
	temperature_unit_t	fromConfiguration(const Configuration& conf);
public:
	Dewpoint(void);
	Dewpoint(const Configuration& conf);
	~Dewpoint(void) { }
	double	operator()(double humidity, double temperature) const;
};

} /* namespace meteo */

#endif /* _Dewpoint_h */
