/*
 * SensorStation.cc -- A SensorStation represents a set of sensors on a weather
 *                     station.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <SensorStation.h>
#include <SensorStationInfo.h>
#include <Station.h>
#include <mdebug.h>
#include <MeteoException.h>

namespace meteo {

SensorStation::SensorStation(const std::string& n, Station *parent) : name(n) {
	// remember the parent station, which we need for access to the readers
	parentstation = parent;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creatin SensorStation %s.%s",
		parent->getName().c_str(), name.c_str());

	// find SensorStation id in database
	SensorStationInfo	ssi(parent->getName(), name);
	sensorid = ssi.getId();
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "sensorid of %s is %d", name.c_str(),
		sensorid);

	// retrieve all field names for this sensor from configuration file
	stringlist	sensors = ssi.getFieldnames();

	// add a recorder for each field
	stringlist::iterator	i;
	for (i = sensors.begin(); i != sensors.end(); i++) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"adding mfield %s to SensorStation %s", i->c_str(),
			name.c_str());
		r.addRecorder(*i);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "SensorStation %s.%d complete",
		parent->getName().c_str(), name.c_str());
}

SensorStation::~SensorStation(void) {
	parentstation = NULL;
}

stringlist	SensorStation::updatequery(time_t timekey) const {
	return r.updatequery(timekey, sensorid);
}

void	SensorStation::reset(void) {
	r.reset();
}

void	SensorStation::update(const std::string& packet) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "updating SensorStation %s",
		name.c_str());
	// go through the DataRecorder, and update every Recorder using the
	// readers of the parent station
	recordermap_t::iterator	i;
	for (i = r.begin(); i != r.end(); i++) {
		// construct the name of the corresponding reader
		std::string	readername = name + "." + i->first;
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "updating DataRecorder for %s",
			readername.c_str());
		
		// use the reader to get a value
		try {
			i->second.update(parentstation->readValue(readername,
				packet));
		} catch (MeteoException& me) {
			mdebug(LOG_INFO, MDEBUG_LOG, 0,
				"exception during update: %s, %s",
				me.getReason().c_str(), me.getAddinfo().c_str());
		}
	}
	// at this point we have updated all sensors in this particular 
	// SensorStation
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "all data values updated");
}

} /* namespace meteo */

