/*
 * Outlet.c -- 
 *
 * (c) 2008 Prof Dr Andreas Mueller
 * $Id: Outlet.cc,v 1.3 2009/01/10 21:47:01 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <Outlet.h>
#include <mdebug.h>

namespace meteo {

Outlet::Outlet() {
}
Outlet::~Outlet() {
}

void	Outlet::send(const int sensorid, const int fieldid, const float value, const std::string& unit) {
	outlet_t	o;
	o.sensorid = sensorid;
	o.fieldid = fieldid;
	o.value = value;
	o.unit = unit;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "Outlet::send(%d, %d, %f, \"%s\")",
		sensorid, fieldid, value, unit.c_str());
	batch.push_back(o);
}

void	Outlet::flush(const time_t timekey) {
	// just remove everything from the batch
	batch.clear();
}

} // namespace meteo
