/*
 * Heatindex.h -- class encapsulating dewpoint computation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Heatindex.h,v 1.2 2004/02/25 23:52:34 afm Exp $
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
