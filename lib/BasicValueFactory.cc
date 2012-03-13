/*
 * BasicValueFactory.cc -- create a new BasicValue based on class name and unit
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: BasicValueFactory.cc,v 1.11 2009/01/10 19:00:23 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <BasicValueFactory.h>
#include <BasicValue.h>
#include <TemperatureValue.h>
#include <HumidityValue.h>
#include <PressureValue.h>
#include <SolarValue.h>
#include <UVValue.h>
#include <MoistureValue.h>
#include <LeafwetnessValue.h>
#include <LevelValue.h>
#include <VoltageValue.h>
#include <Wind.h>
#include <Rain.h>
#include <RainRate.h>
#include <MeteoException.h>
#include <TransmitterStatus.h>
#include <VoltageValue.h>
#include <TimeValue.h>

namespace meteo {

static BasicValue	*getBasicValueFromClassnameAndUnit(
			const std::string& classname, const std::string& unit) {
	// depending on type, create the right object
	if (classname == "BasicValue") {
		return (BasicValue *)new BasicValue(""); // delete ~Value
	}
	if (classname == "TemperatureValue") {
		return (BasicValue *)new TemperatureValue(unit); // delete ~Value
	}
	if (classname == "HumidityValue") {
		return (BasicValue *)new HumidityValue(unit); // delete ~Value
	}
	if (classname == "PressureValue") {
		return (BasicValue *)new PressureValue(unit); // delete ~Value
	}
	if (classname == "Wind") {
		return (BasicValue *)new Wind(unit); // delete ~Value
	}
	if (classname == "WindSpeed") {
		return (BasicValue *)new WindSpeed(unit); // delete ~Value
	}
	if (classname == "Rain") {
		return (BasicValue *)new Rain(unit); // delete ~Value
	}
	if (classname == "RainRate") {
		return (BasicValue *)new RainRate(unit); // delete ~Value
	}
	if (classname == "SolarValue") {
		return (BasicValue *)new SolarValue(unit); // delete ~Value
	}
	if (classname == "UVValue") {
		return (BasicValue *)new UVValue(unit); // delete ~Value
	}
	if (classname == "LevelValue") {
		return (BasicValue *)new LevelValue(unit); // delete ~Value
	}
	if (classname == "MoistureValue") {
		return (BasicValue *)new MoistureValue(unit); // delete ~Value
	}
	if (classname == "LeafwetnessValue") {
		return (BasicValue *)new LeafwetnessValue(unit); // delete ~Value
	}
	if (classname == "VoltageValue") {
		return (BasicValue *)new VoltageValue(unit); // delete ~Value
	}
	if (classname == "TransmitterStatus") {
		return (BasicValue *)new TransmitterStatus(unit); // delete ~Value
	}
	if (classname == "TimeValue") {
		return (BasicValue *)new TimeValue(unit); // delete ~Value
	}
	throw MeteoException("unknown field class: ", classname);
}

static BasicValue	*getBasicValueFromOld(const BasicValue *other) {
	std::string	classname = other->getClass();
	// depending on type, create the right object
	if (classname == "TemperatureValue") {
		return (BasicValue *)
			new TemperatureValue(*(TemperatureValue *)other); // delete ~Value
	}
	if (classname == "HumidityValue") {
		return (BasicValue *)
			new HumidityValue(*(HumidityValue *)other); // delete ~Value
	}
	if (classname == "PressureValue") {
		return (BasicValue *)
			new PressureValue(*(PressureValue *)other); // delete ~Value
	}
	if (classname == "Wind") {
		return (BasicValue *)
			new Wind(*(Wind *)other); // delete ~Value
	}
	if (classname == "WindSpeed") {
		return (BasicValue *)
			new WindSpeed(*(WindSpeed *)other); // delete ~Value
	}
	if (classname == "Rain") {
		return (BasicValue *)
			new Rain(*(Rain *)other); // delete ~Value
	}
	if (classname == "RainRate") {
		return (BasicValue *)
			new RainRate(*(RainRate *)other); // delete ~Value
	}
	if (classname == "SolarValue") {
		return (BasicValue *)
			new SolarValue(*(SolarValue *)other); // delete ~Value
	}
	if (classname == "UVValue") {
		return (BasicValue *)
			new UVValue(*(UVValue *)other); // delete ~Value
	}
	if (classname == "LevelValue") {
		return (BasicValue *)
			new LevelValue(*(LevelValue *)other); // delete ~Value
	}
	if (classname == "MoistureValue") {
		return (BasicValue *)
			new MoistureValue(*(MoistureValue *)other); // delete ~Value
	}
	if (classname == "LeafwetnessValue") {
		return (BasicValue *)
			new LeafwetnessValue(*(LeafwetnessValue *)other); // delete ~Value
	}
	if (classname == "VoltageValue") {
		return (BasicValue *)
			new VoltageValue(*(VoltageValue *)other); // delete ~Value
	}
	if (classname == "TransmitterStatus") {
		return (BasicValue *)
			new TransmitterStatus(*(TransmitterStatus *)other); // delete ~Value
	}
	if (classname == "TimeValue") {
		return (BasicValue *)
			new TimeValue(*(TimeValue *)other); // delete ~Value
	}
	throw MeteoException("unknown field class: ", classname);
}

BasicValue	*BasicValueFactory::newBasicValue(const std::string& classname,
			const std::string& unit) const {
	BasicValue	*result = getBasicValueFromClassnameAndUnit(classname,
				unit);
	return result;
}

BasicValue	*BasicValueFactory::newBasicValue(const std::string& classname,
			double value, const std::string& unit) const {
	BasicValue	*result = getBasicValueFromClassnameAndUnit(classname,
				unit);
	result->setValue(value);
	return result;
}

BasicValue	*BasicValueFactory::copyBasicValue(const BasicValue *other) const {
	return getBasicValueFromOld(other);
}

} /* namespace meteo */


