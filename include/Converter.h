/*
 * Converter.h -- converter classes that allow to convert Meteo values
 *                between differen units
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Converter.h,v 1.5 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _Converter_h
#define _Converter_h

#include <string>
#include <Value.h>

namespace meteo {

// virtual base class for converters, just to make it possible to
// write generic methods for converters
class	Converter {
	std::string	targetunit;
public:
	Converter(const std::string& t) : targetunit(t) { }
	virtual ~Converter(void) { }

	Value	operator()(const Value& v) const;
	const std::string&	getUnit(void) const { return targetunit; }
};

} /* namespace meteo */

#endif /* _Converter_h */
