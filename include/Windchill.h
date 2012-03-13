/*
 * Windchill.h -- class encapsulating derived value computation, based
 *                   on temperature
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Windchill_h
#define _Windchill_h

#include <Derivedvalue.h>
#include <string>

namespace meteo {

class Windchill : public Derivedvalue {
	std::string	getWindunit(void);
protected:
	static std::string	wu;
public:
	Windchill(void);
	~Windchill(void) { }
	double	operator()(double humidity, double temperature) const;
};

} /* namespace meteo */

#endif /* _Windchill_h */
