/*
 * Dewpoint.h -- class encapsulating dewpoint computation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Dewpoint.h,v 1.3 2004/02/25 23:52:34 afm Exp $
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
