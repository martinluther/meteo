//
// Format.h -- format numbers as std::string
//
#ifndef _Format_h
#define _Format_h

#include <string>

namespace meteo {

extern std::string stringprintf(const char *format, ...);
extern std::string& stringprintf(std::string& s, const char *format, ...);

} // namespace meteo

#endif	/* _Format_h */
