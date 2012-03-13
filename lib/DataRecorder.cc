/*
 * DataRecorder.cc -- implement accumulator class
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: DataRecorder.cc,v 1.12 2009/01/10 19:00:23 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <DataRecorder.h>
#include <RecorderFactory.h>
#include <mdebug.h>
#include <MeteoException.h>
#include <QueryProcessor.h>

namespace meteo {

// create a new Recorder based on the sensorname
void	DataRecorder::addRecorder(const std::string& sensorname) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "adding Recorder for sensor %s",
		sensorname.c_str());

	// let the RecorderFactory create a new Recorder for this sensor name
	Recorder	r = RecorderFactory::getRecorder(sensorname);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "Recorder for sensor %s created",
		sensorname.c_str());

	// add the recorder to the map
	recorders.insert(recordermap_t::value_type(sensorname, r));
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "Recorder for sensor %s added",
		sensorname.c_str());
}


// the update method just gets the right recorder and hands over the new
// value to it
void	DataRecorder::update(const std::string& sensorname, const Value& v) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "updating %s from %s",
		sensorname.c_str(), v.getClass().c_str());
	// make sure a recorder exists for this sensor
	recordermap_t::iterator	i = recorders.find(sensorname);
	if (i == recorders.end()) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "sensor %s not found",
			sensorname.c_str());
		throw MeteoException("sensor not found", sensorname);
	}

	// update the recorder using the value
	try {
		i->second.update(v);
	} catch (MeteoException& me) {
		mdebug(LOG_INFO, MDEBUG_LOG, 0, "no update: %s, %s",
			me.getReason().c_str(), me.getAddinfo().c_str());
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "update %s complete",
		sensorname.c_str());
}

// retrieve the update queries from all recorders
void	DataRecorder::sendOutlet(Outlet *outlet, const time_t timekey,
	const int sensorid) const {
	// go through the  vector of recorders
	recordermap_t::const_iterator	i;
	for (i = recorders.begin(); i != recorders.end(); i++) {
		i->second.sendOutlet(outlet, timekey, sensorid);
	}
}

// reset all recorders
void	DataRecorder::reset(void) {
	recordermap_t::iterator	i;
	for (i = recorders.begin(); i != recorders.end(); i++) {
		i->second.reset();
	}
}

} /* namespace meteo */
