/*
 * StationFactory.cc -- create weather station classes
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <StationFactory.h>
#include <WMII.h>
#include <GroWeather.h>
#include <Health.h>
#include <Energy.h>
#include <Configuration.h>
#include <VantagePro.h>
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#include <ChannelFactory.h>
#include <MeteoException.h>
#include <mdebug.h>

namespace meteo {

Station	*StationFactory::newStation(const std::string& name) const {
	Configuration	conf;
	Station	*result;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "createing new station named %s",
		name.c_str());
	// find out what type of station this is
	std::string	stationxpath = "/meteo/station[@name='"
				+ name + "']/type";
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "looking for station %s via xpath %s",
		name.c_str(), stationxpath.c_str());
	std::string	stationtype = conf.getString(stationxpath, "undefined");
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "station type: %s",
		stationtype.c_str());
	if (stationtype == "WMII") {
		result = new WMII(name);	// explicit delete
	} else if (stationtype == "Perception") {
		result = new WMII(name);	// explicit delete
	} else if (stationtype == "Wizard") {
		result = new WMII(name);	// explicit delete
	} else if (stationtype == "GroWeather") {
		result = new GroWeather(name);	// explicit delete
	} else if (stationtype == "Energy") {
		result = new Energy(name);	// explicit delete
	} else if (stationtype == "Health") {
		result = new Health(name);	// explicit delete
	} else if (stationtype == "VantagePro") {
		result = new VantagePro(name);	// explicit delete
/* add other station types here,
 *	} else if (stationtype == "LevelStation") {
 *		result = new LevelStation(name);	// explicit delete
 */
	} else {
		throw MeteoException("unknown station type", stationtype);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "station %s created", name.c_str());

	// return the complete station
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creating of new station %s complete",
		name.c_str());
	return result;
}

} /* namespace meteo */
