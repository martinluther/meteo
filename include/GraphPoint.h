/*
 * GraphPoint.h -- abstracts points insode the graphs, which uses the
 *                 application domain coordinate system
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: GraphPoint.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _GraphPoint_h
#define _GraphPoint_h

#include <time.h>

namespace meteo {

class	GraphPoint {
	time_t	t;
	double	v;
public:
	GraphPoint(void) { t = 0; v = 0.; }
	GraphPoint(time_t tt, double vv) { t = tt; v = vv; }
	~GraphPoint(void) { }
	time_t	getTime(void) const { return t; }
	double	getValue(void) const { return v; }
};

} /* namespace meteo */

#endif /* _GraphPoint_h */
