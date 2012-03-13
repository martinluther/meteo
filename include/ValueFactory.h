/*
 * ValueFactory.h -- creates value objects based on the configuration
 *                   
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: ValueFactory.h,v 1.3 2004/02/25 23:52:35 afm Exp $
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
