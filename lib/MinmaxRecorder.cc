/*
 * MinmaxRecorder.cc -- handle meteo value including maximum and minimum and
 *                   aggregation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: MinmaxRecorder.cc,v 1.12 2009/01/10 21:47:01 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <MinmaxRecorder.h>
#include <MeteoException.h>
#include <mdebug.h>

namespace meteo {

// constructors and destructors
MinmaxRecorder::MinmaxRecorder(const std::string& unit) : BasicRecorder(unit) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creating MinmaxRecorder %s",
		unit.c_str());
	reset();
	storeminmax = false;
}
MinmaxRecorder::~MinmaxRecorder(void) {
}

// update the value, this must now also take into account the minimum and
// maximum value
void	MinmaxRecorder::update(double v) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "updating with value %.2f", v);
	if (isValid()) {
		// update the maximum
		if (v > max)
			max = v;

		// update the minimum
		if (v < min)
			min = v;
	} else {
		min = max = v;
	}
	BasicRecorder::update(v);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "update complete");
}

// set the value, this resets time stamps and max/min
void	MinmaxRecorder::setValue(double v) {
	max = v;
	min = v;
	BasicRecorder::setValue(v);
}

std::string	MinmaxRecorder::getMinString(void) const {
	char	buffer[32];
	if (!isValid())
		return std::string("NULL");
	snprintf(buffer, sizeof(buffer), "%.2f", min);
	return std::string(buffer);
}

std::string	MinmaxRecorder::getMaxString(void) const {
	char	buffer[32];
	if (!isValid())
		return std::string("NULL");
	snprintf(buffer, sizeof(buffer), "%.2f", max);
	return std::string(buffer);
}

double	MinmaxRecorder::getMax(void) const {
	if (!isValid())
		throw MeteoException("no max value", "");
	return value;
}

double	MinmaxRecorder::getMin(void) const {
	if (!isValid())
		throw MeteoException("no min value", "");
	return min;
}

void	MinmaxRecorder::reset(void) {
	max = -1000000.;
	min =  1000000.;
	BasicRecorder::reset();
}

void	MinmaxRecorder::sendOutlet(Outlet *outlet, const time_t timekey,
	const int sensorid, const int fieldid) const {
	// return empty result if no value available
	if (!isValid())
		return;

	// get the BasicRecorder's update query
	BasicRecorder::sendOutlet(outlet, timekey, sensorid, fieldid);

	// get maximum and minimu update query
	if (storeminmax) {
		outlet->send(sensorid, fieldid + 1, min, getUnit());
		outlet->send(sensorid, fieldid + 2, max, getUnit());
	}
}

std::string	MinmaxRecorder::plain(void) const {
	char	result[1024];
	snprintf(result, sizeof(result),
		"%5.1f %s (min: %5.1f, max: %5.1f)",
		value, getUnit().c_str(), min, max);
	return result;
}
std::string	MinmaxRecorder::xml(const std::string& name) const {
	return "<data name=\"" + name
		+ "\" value=\"" + this->getValueString()
		+ "\" unit=\"" + conv.getUnit()
		+ "\" min=\"" + getMinString()
		+ "\" max=\"" + getMaxString()
		+ "/>";
}

} /* namespace meteo */
