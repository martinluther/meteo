/*
 * RainRecoder.cc -- encapsulate rain measurements 
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: RainRecorder.cc,v 1.10 2009/01/10 21:47:01 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <RainRecorder.h>
#include <MeteoException.h>
#include <RainConverter.h>
#include <mdebug.h>

namespace meteo {

RainRecorder::RainRecorder(void) : BasicRecorder("mm") { startvalue = 0.; }

RainRecorder::RainRecorder(double v) : BasicRecorder("mm") {
	startvalue = v;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "initialized rain from %.2fmm", v);
}

RainRecorder::RainRecorder(double v, const std::string& u) : BasicRecorder(u) {
	startvalue = v;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "initialized rain from %.2f%s",
		v, u.c_str());
}

RainRecorder::RainRecorder(const std::string& u) : BasicRecorder(u) {
}

RainRecorder::~RainRecorder(void) { }

// override the update method, we have to keep track of startvalue and possible
// wrap around
void	RainRecorder::update(double rain) {
	if (!isValid())
		startvalue = rain;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "updating rain to %.1f", rain);
	// if a wrap around occurs, we have to correct the startvalue so
	// that it still reflects the correct rain total
	if (rain < value) {
		// compute the rain that had fallen until the wrap around
		// occured
		double	rainsofar = value - startvalue;

		// we don't know exactly what happened since the last time
		// we looked at rain, so the wrap around may have happend
		// some time after we last look, in which case we will miss
		// a little rain. However, since we are checking every 2
		// seconds normally, it is extremely unlikely that we miss
		// a tick

		// the current rain value reflects the rain that has fallen
		// since the wrap around, so we set the startvalue to
		// - what was before, to get the correct result
		startvalue = -rainsofar;
	}
	value = rain;
	setValid(true);
	lastupdate.now();
}

// override the reset method: it should reset the start value to the last
// read rain value, or we may loose some information. In particular, the
// rain value will be valid even if no rain fell
void	RainRecorder::reset(void) {
	if (isValid())
		startvalue = value;
		// this causes getValue() to return 0. until some more rain
		// falls, i.e. value increases
	else
		startvalue = 0.;
}

// accessor methods (overrides the BasicRecorder methods, since we are not
// interested in the value field, but only in value - startvalue
double	RainRecorder::getValue(void) const {
	if (!isValid())
		throw MeteoException("no rain value", "");
	return value - startvalue;
}

double	RainRecorder::getTotal(void) const {
	if (!isValid())
		throw MeteoException("no rain value", "");
	return value;
}

double	RainRecorder::getRate(void) const {
	if (!isValid())
		throw MeteoException("no rain value", "");
	double	duration = (lastupdate - start).getValue();
	if (duration == 0.)
		throw MeteoException("zero duration", "");
	return getValue()/duration;
}

std::string	RainRecorder::getValueString(void) const {
	return BasicRecorder::getValueString(std::string("%.3f"));
}

std::string	RainRecorder::getTotalString(void) const {
	if (!isValid())
		return std::string("NULL");
	char	buffer[32];
	snprintf(buffer, sizeof(buffer), "%.3f", getTotal());
	return std::string(buffer);
}

void	RainRecorder::sendOutlet(Outlet *outlet, const time_t timekey,
		const int sensorid, const int fieldid) const {
	// return an empty query set immediately if we have nothing to display
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "asking for rain update query");
	if (!isValid()) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "no valid rain data");
		return;
	}

	// only add the query if the current value is nonzero, this saves
	// quite a few rows in the database
	if (getValue() > 0) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "no rain in last interval, "
			"suppressing db update");
		outlet->send(sensorid, fieldid, getValue(), getUnit());
	}

	// add total rain to the database
	outlet->send(sensorid, fieldid + 1, getTotal(), getUnit());
}

std::string	RainRecorder::plain(void) const {
	char	result[1024];
	snprintf(result, sizeof(result),
		"%5.1f %s (total: %6.1f)", getValue(), getUnit().c_str(),
		getTotal());
	return result;
}
std::string	RainRecorder::xml(const std::string& name) const {
	return "<data name=\"" + name
		+ "\" value=\"" + this->getValueString()
		+ "\" unit=\"" + conv.getUnit()
		+ "\" total=\"" + getTotalString()
		+ "/>";
}

} /* namespace meteo */
