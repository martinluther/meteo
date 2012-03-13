/*
 * Pidfile.h -- C++ implementation of pidfile logic
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Pidfile.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _Pidfile_h
#define _Pidfile_h

#include <string>

namespace meteo {

class Pidfile {
public:
	Pidfile(const std::string& filename);
	~Pidfile(void) { }
};

} /* namespace meteo */

#endif /* _Pidfile_h */
