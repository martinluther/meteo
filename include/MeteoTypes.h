/*
 * MeteoTypes.h -- types that don't really fit anywhere else
 *
 * (c) 2003 Dr. Andreas Mueller, BEratung und Entwicklung
 *
 * $Id: MeteoTypes.h,v 1.4 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _MeteoTypes_h
#define _MeteoTypes_h

#include <string>
#include <list>

namespace meteo {

typedef	std::list<std::string>	stringlist;
typedef	std::list<int>		intlist;
typedef	std::list<double>	doublelist;

} /* namespace meteo */

#endif /* _MeteoTypes_h */
