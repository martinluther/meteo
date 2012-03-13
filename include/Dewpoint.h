/*
 * Dewpoint.h -- class encapsulating dewpoint computation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Dewpoint_h
#define _Dewpoint_h

#include <Derivedvalue.h>

namespace meteo {

class Dewpoint : public Derivedvalue {
public:
	Dewpoint(void) { }
	~Dewpoint(void) { }
	double	operator()(double humidity, double temperature) const;
};

} /* namespace meteo */

#endif /* _Dewpoint_h */
