/*
 * ConverterFactory.cc -- create a converter class for the named value
 *                        class
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <ConverterFactory.h>
#include <mdebug.h>
#include <MeteoException.h>
#include <TemperatureConverter.h>
#include <HumidityConverter.h>
#include <PressureConverter.h>
#include <WindConverter.h>
#include <RainConverter.h>
#include <RainRateConverter.h>
#include <MoistureConverter.h>
#include <LevelConverter.h>
#include <SolarConverter.h>
#include <UVConverter.h>
#include <LeafwetnessConverter.h>
#include <VoltageConverter.h>

namespace meteo {

BasicConverter	*ConverterFactory::newConverter(const std::string& classname,
			const std::string& targetunit) {
	// depending on the class name, create a new converter 
	if (classname == "BasicValue") {
		return new BasicConverter("");	// delete ~Converter
	}
	if (classname == "TemperatureValue") {
		return new TemperatureConverter(targetunit); // delete ~Converter
	}
	if (classname == "HumidityValue") {
		return new HumidityConverter(targetunit); // delete ~Converter
	}
	if (classname == "PressureValue") {
		return new PressureConverter(targetunit); // delete ~Converter
	}
	if (classname == "Wind") {
		return new WindConverter(targetunit); // delete ~Converter
	}
	if (classname == "WindSpeed") {
		return new WindConverter(targetunit); // delete ~Converter
	}
	if (classname == "Rain") {
		return new RainConverter(targetunit); // delete ~Converter
	}
	if (classname == "RainRate") {
		return new RainRateConverter(targetunit); // delete ~Converter
	}
	if (classname == "MoistureValue") {
		return new MoistureConverter(targetunit); // delete ~Converter
	}
	if (classname == "LevelValue") {
		return new LevelConverter(targetunit); // delete ~Converter
	}
	if (classname == "SolarValue") {
		return new SolarConverter(targetunit); // delete ~Converter
	}
	if (classname == "UVValue") {
		return new UVConverter(targetunit); // delete ~Converter
	}
	if (classname == "LeafwetnessValue") {
		return new LeafwetnessConverter(targetunit); // delete ~Converter
	}
	if (classname == "VoltageValue") {
		return new VoltageConverter(targetunit); // delete ~Converter
	}
	mdebug(LOG_ERR, MDEBUG_LOG, 0, "no converter for class %s",
		classname.c_str());
	throw MeteoException("no converter generated for class", classname);
}


} /* namespace meteo */
