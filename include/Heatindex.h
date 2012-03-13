/*
 * Heatindex.h -- class encapsulating dewpoint computation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Heatindex_h
#define _Heatindex_h

#include <Derivedvalue.h>

namespace meteo {

class Heatindex : public Derivedvalue {
public:
	Heatindex(void) { }
	~Heatindex(void) { }
	double	operator()(double humidity, double temperature) const;
};

} /* namespace meteo */

#endif /* _Heatindex_h */
