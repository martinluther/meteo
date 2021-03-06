/*
 * LastRecorder.cc -- handle meteo value aggregation according to the last
 *                    algorithm
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: LastRecorder.cc,v 1.2 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <LastRecorder.h>
#include <MeteoException.h>
#include <mdebug.h>

namespace meteo {

// constructors and destructors
LastRecorder::LastRecorder(const std::string& unit) : BasicRecorder(unit) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creating LastRecorder %s",
		unit.c_str());
	reset();
}
LastRecorder::~LastRecorder(void) {
}

// update the value, this must now also take into account the minimum and
// maximum value
void	LastRecorder::update(double v) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "updating with value %.2f", v);
	setValue(v);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "update complete");
}

void	LastRecorder::reset(void) {
	BasicRecorder::reset();
}

} /* namespace meteo */
