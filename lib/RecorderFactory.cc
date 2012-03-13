/*
 * RecorderFactory.cc -- factory to create Recorders based on the information
 *                       in the database
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: RecorderFactory.cc,v 1.11 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <RecorderFactory.h>
#include <Field.h>
#include <MeteoException.h>
#include <mdebug.h>
#include <HumidityRecorder.h>
#include <LeafwetnessRecorder.h>
#include <LevelRecorder.h>
#include <MinmaxRecorder.h>
#include <MoistureRecorder.h>
#include <PressureRecorder.h>
#include <RainRecorder.h>
#include <RainRateRecorder.h>
#include <SolarRecorder.h>
#include <TemperatureRecorder.h>
#include <UVRecorder.h>
#include <WindRecorder.h>
#include <VoltageRecorder.h>
#include <TransmitterRecorder.h>
#include <TimeRecorder.h>

namespace meteo {

Recorder	RecorderFactory::getRecorder(const std::string& mfieldname) {
	// retrieve Sensor information from the database
	std::string	classname = Field().getClass(mfieldname);
	std::string	unit = Field().getUnit(mfieldname);

	// create a Recorder based on the value class name and unit
	BasicRecorder	*br = NULL;
	if (classname == "TemperatureValue") {
		br = new TemperatureRecorder(unit); // delete by ~Recorder
	}
	if (classname == "HumidityValue") {
		br = new HumidityRecorder(unit); // delete by ~Recorder
	}
	if (classname == "PressureValue") {
		br = new PressureRecorder(unit); // delete by ~Recorder
	}
	if (classname == "SolarValue") {
		br = new SolarRecorder(unit);	// delete by ~Recorder
	}
	if (classname == "UVValue") {
		br = new UVRecorder(unit);	// delete by ~Recorder
	}
	if (classname == "MoistureValue") {
		br = new MoistureRecorder(unit); // delete by ~Recorder
	}
	if (classname == "LeafwetnessValue") {
		br = new LeafwetnessRecorder(unit); // delete by ~Recorder
	}
	if (classname == "Rain") {
		br = new RainRecorder(unit);	// delete by ~Recorder
	}
	if (classname == "RainRate") {
		br = new RainRateRecorder(unit);	// delete by ~Recorder
	}
	if (classname == "Wind") {
		br = new WindRecorder(unit);	// delete by ~Recorder
	}
	if (classname == "VoltageValue") {
		br = new VoltageRecorder(unit);	// delete by ~Recorder
	}
	if (classname == "TransmitterStatus") {
		br = new TransmitterRecorder(unit);	// delete by ~Recorder
	}
	if (classname == "TimeValue") {
		br = new TimeRecorder(unit);	// delete by ~Recorder
	}
	if (br == NULL) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "cannot create class %s",
			classname.c_str());
		throw MeteoException("cannot create class", classname);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "BasicRecorder %s created",
		br->getValueClass().c_str());

	// set the remaining fields from the Sensor information
	Recorder	result(br);
	result.name = mfieldname;
	result.id = Field().getId(mfieldname);
	result.classname = classname;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "sensor %s Recorder has id %d",
		mfieldname.c_str(), result.id);

	// return the new Recorder object
	return result;
}

} /* namespace meteo */
