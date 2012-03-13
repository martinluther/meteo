/*
 * ConverterFactory.h -- create a converter class for a named value class
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _ConverterFactory_h
#define _ConverterFactory_h

#include <BasicConverter.h>

namespace meteo {

class ConverterFactory {
public:
	static BasicConverter	*newConverter(const std::string& classname,
		const std::string& targetunit);
};

} /* namespace meteo */

#endif /* _ConverterFactory_h */
