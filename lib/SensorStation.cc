/*
 * SensorStation.cc -- A SensorStation represents a set of sensors on a weather
 *                     station.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: SensorStation.cc,v 1.10 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <SensorStation.h>
#include <SensorStationInfo.h>
#include <Station.h>
#include <mdebug.h>
#include <MeteoException.h>

namespace meteo {

SensorStation::SensorStation(const std::string& n, Station *parent) : name(n) {
	// remember the parent station, which we need for access to the readers
	parentstation = parent;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creating SensorStation %s.%s",
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

void	SensorStation::sendOutlet(Outlet *outlet, time_t timekey) const {
	r.sendOutlet(outlet, timekey, sensorid);
}

void	SensorStation::reset(void) {
	r.reset();
}

void	SensorStation::update(const std::string& packet, Mapfile *m) {
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
			// find the corresponding value object
			Value	value = parentstation->readValue(readername,
				packet);
			i->second.update(value);

			// mapfile updates
			if (NULL != m) {
				mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"mapfile update for %s",
					readername.c_str());
				// update the mapfile
				m->add(readername, i->second.getValue(),
					i->second.getUnit());

				// handle the special case of wind, which also
				// has a direction
				if (value.getClass() == "Wind") {
					m->add(readername + "dir", ((Wind *)value
						.getBasicValue())->getAzideg(),
						std::string("deg"));
				}
			}
		} catch (MeteoException& me) {
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"exception during update: %s, %s",
				me.getReason().c_str(), me.getAddinfo().c_str());
		}

	}
	// at this point we have updated all sensors in this particular 
	// SensorStation
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "all data values updated");
}

} /* namespace meteo */

