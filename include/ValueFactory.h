/*
 * ValueFactory.h -- creates value objects based on the configuration
 *                   
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _ValueFactory_h
#define _ValueFactory_h

#include <Value.h>

namespace meteo {

class ValueFactory {
public:
	ValueFactory(void) { }
	~ValueFactory(void) { }
	static Value	getValue(const std::string& classname, double value,
			const std::string& unit);
	static Value	getValue(const std::string& classname,
			const std::string& unit);
	static Value	getValue(const Vector& wind,
			const std::string& unit);
	static Value	copyValue(const Value& other);
};

} /* namespace meteo */

#endif /* ValueFactory_h */
