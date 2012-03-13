/*
 * Value.h -- general value type. This class performs resource management
 *            and acts as a smart pointer for more refined value types as they
 *            are defined in MinmaxValue.h, Wind.h and Rain.h
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Value.h,v 1.4 2006/05/07 19:47:22 afm Exp $
 */
#ifndef _Value_h
#define _Value_h

#include <BasicValue.h>
#include <TemperatureValue.h>
#include <HumidityValue.h>
#include <PressureValue.h>
#include <SolarValue.h>
#include <UVValue.h>
#include <MoistureValue.h>
#include <LeafwetnessValue.h>
#include <LevelValue.h>
#include <Wind.h>
#include <Rain.h>
#include <string>

namespace meteo {

class	Value {
	BasicValue	*bv;
	// methods used by the ValueFactory are private. If we would publish
	// the Value(BasicValue *) method, we would loose control over
	// allocation and deallocation of the basic value, which is to be 
	// considered internal
	Value(BasicValue *bv);
	void	setValue(double v);
public:
	Value();
	~Value(void);
	// implement intelligent pointer, only copy constructor and assignment
	// are public, so a Value can only be instantiated by copying it
	Value(const Value&);
	Value&	operator=(const Value&);

	// accessor methods
	const std::string	getClass(void) const;
	const std::string	getUnit(void) const { return bv->getUnit(); }

	// access to the basic value
	BasicValue	*getBasicValue(void) { return bv; }

	// retrieve the value (forgetting about the unit)
	double	getValue(void) const;

	// display methods: display the information in this value either as
	//                  an XML string or as plain text
	std::string	xml(const std::string& name) const;
	std::string	plain(const std::string& name) const;

	//  make sure the Factory can modify the members
	friend class ValueFactory;	// uses the Value(BasicValue*) constr.
	friend class WindRecorder;	// needs to read the bv
	friend class Converter;		// allow acceses to setValue method
};

} /* namespace meteo */

#endif /* _Value_h */
