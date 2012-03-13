/*
 * ValueFactory.h -- create a new value object of the right type
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <ValueFactory.h>

#include <MeteoException.h>
#include <BasicValueFactory.h>
#include <Wind.h>
#include <string>
#include <mdebug.h>

namespace meteo {

Value	ValueFactory::getValue(const std::string& classname, double value,
		const std::string& unit) {
	BasicValueFactory	bvf;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "building %s value from %f %s",
		classname.c_str(), value, unit.c_str());
	BasicValue	*bv = bvf.newBasicValue(classname, value, unit);
	Value	result(bv);
	return result;
}

Value	ValueFactory::copyValue(const Value& other) {
	BasicValueFactory	bvf;
	BasicValue	*bv = bvf.copyBasicValue(other.bv);
	Value	result(bv);
	return result;
}

Value	ValueFactory::getValue(const std::string& classname,
		const std::string& unit) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "building %s value from unit %s",
		classname.c_str(), unit.c_str());
	return getValue(classname, 0., unit);
}

Value	ValueFactory::getValue(const Vector& wind, const std::string& unit) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "building Wind value from Vector, "
		"unit %s", unit.c_str());
	Wind	*w = new Wind(wind, unit);	// delete by ~Value
	Value	result(w);
	return result;
}

} /* namespace meteo */
