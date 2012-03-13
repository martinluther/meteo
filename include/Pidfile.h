/*
 * Pidfile.h -- C++ implementation of pidfile logic
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
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
