/*
 * BasicValueFactory.h -- construct a new Basic Value based on class name
 *                        and unit
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _BasicValueFactory_h
#define _BasicValueFactory_h

#include <BasicValue.h>
#include <string>

namespace meteo {

class BasicValueFactory {
public:
	BasicValue	*newBasicValue(const std::string& classname,
				const std::string& unit) const;
	BasicValue	*newBasicValue(const std::string& classname,
				double value, const std::string& unit) const;
	BasicValue	*copyBasicValue(const BasicValue *other) const;
};

} /* namespace meteo */

#endif /* _BasicValueFactory_h */

