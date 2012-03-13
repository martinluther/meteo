/*
 * Converter.cc -- class to convert values from one system to another
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Converter.cc,v 1.6 2009/01/10 19:00:23 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <Converter.h>
#include <BasicConverter.h>
#include <ValueFactory.h>
#include <ConverterFactory.h>
#include <MeteoException.h>
#include <mdebug.h>

namespace meteo {

Value	Converter::operator()(const Value& v) const {
	// find the target class to which we are supposed to convert
	std::string	targetclass = v.getClass();

	// create a suitable converter object for the associated BasicValue
	// class
	BasicConverter	*bc = ConverterFactory::newConverter(targetclass,
		targetunit);

	// create an object of the target class
	Value	result = ValueFactory().copyValue(v);
	result.bv->setHasvalue(false);

	// set the new units (since the value is invalid, it will not be
	// converted, which would cause an infinite loop, as we are the
	// converter)
	result.bv->setUnit(getUnit());

	// set the converted value in the result object
	result.setValue(bc->operator()(v.getValue(), v.getUnit()));

	// destroy the converter, so as not to create a memory leak
	delete bc;

	// return the converted Value
	return result;
}

} /* namespace meteo */
