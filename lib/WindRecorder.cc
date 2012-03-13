/*
 * WindRecorder.cc -- wind 
 * 
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 *
 * $Id: WindRecorder.cc,v 1.14 2009/01/10 22:11:40 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <WindRecorder.h>
#include <WindConverter.h>
#include <mdebug.h>
#include <MeteoException.h>
#include <Field.h>

namespace meteo {

WindRecorder::WindRecorder(void) : BasicRecorder("m/s") {
	samples = 0;
	start.now();
}
WindRecorder::WindRecorder(const Vector& v0) : BasicRecorder("m/s") {
	samples = 0;
	start.now();
	v = v0;
	max = v0;
	lastupdate.now();
	setValid(true);
}
WindRecorder::WindRecorder(const Vector& v0, const std::string& u)
	: BasicRecorder(u) {
	samples = 0;
	start.now();
	v = v0;
	max = v0;
	setValid(true);
	lastupdate.now();
}
WindRecorder::WindRecorder(const std::string& u) : BasicRecorder(u) {
	samples = 0;
	start.now();
}
WindRecorder::~WindRecorder(void) { }


void	WindRecorder::reset(void) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "resetting duration timer for wind");
	v = Vector(0., (unsigned short)0);
	max = Vector(0., (unsigned short)0);
	samples = 0;
	BasicRecorder::reset();
}

void	WindRecorder::update(const Vector& v0) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
		"WindRecorder udpate (start = %.f, last = %.f) with (%.1f, %.1f)",
		start.getValue(), lastupdate.getValue(), v0.getX(), v0.getY());
	// handle the case where don't have anyhting yet (except a start
	// time)
	if (!isValid()) {
		v = v0; max = v0; lastupdate.now(); setValid(true);
		return;
	}
	double	t1 = getDuration();
	Timeval	n; n.now();
	double	t2 = (n - lastupdate).getValue();

	// update the vector
	double	duration = t1 + t2;
	if (duration > 0.1) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "wind update: "
			"(%.1f * (%.1f, %.1f) + %.1f * (%.1f, %.1f))/%.1f)",
			t1, v.getX(), v.getY(), t2, v0.getX(), v0.getY(),
			getDuration());
		v = (1/duration) * (t1 * v + t2 * v0);
	} else {
		// ignore wind if duration is less than 0.1 sec
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "ignoring %f secs of wind",
			duration);
		v = v0;
	}

	// update maximum values
	double m = v0.getAbs();
	if (m > getMax()) {
		max = v0;
	}

	// remember this update
	samples++;
	lastupdate.now();
}

// overrides the update from a Value method of BasicRecorder, since the
// WindRecorder needs to take the wind direction into account
void	WindRecorder::update(const Value& w) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "WindRecorder::update(const Value&)");
	// first check whether we have a wind value
	if (w.getClass() != "Wind") {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "cannot update Wind from %s "
			"class", w.getClass().c_str());
		throw MeteoException("cannot update Wind from class",
			w.getClass());
	}

	// convert the Wind to the units of this recorder
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "converting to unit %s",
		conv.getUnit().c_str());
	Value	w2(conv(w));

	// extract the Wind part from the Value and update using the vector
	// valued updater, for this it is
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "updating from vector");
	Wind	*wp = dynamic_cast<Wind *>(w2.bv);
	update(wp->getVectorValue());
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "Wind update complete");
}

double	WindRecorder::getDuration(void) const {
	if (isValid()) {
		double	dur = (lastupdate - start).getValue();
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "duration: %.1f", dur);
		return dur;
	} else
		throw MeteoException("WindRecorder does not have value", "");
}

std::string	WindRecorder::getSpeedString(void) const {
	char	sb[100];
	if (isValid()) {
		snprintf(sb, sizeof(sb), "%.1f", v.getAbs());
		return std::string(sb);
	} else
		return "NULL";
}
std::string	WindRecorder::getXString(void) const {
	char	sb[100];
	if (isValid()) {
		snprintf(sb, sizeof(sb), "%.1f", v.getX());
		return std::string(sb);
	} else
		return "NULL";
}
std::string	WindRecorder::getYString(void) const {
	char	sb[100];
	if (isValid()) {
		snprintf(sb, sizeof(sb), "%.1f", v.getY());
		return std::string(sb);
	} else
		return "NULL";
}
std::string	WindRecorder::getAziString(void) const {
	char	sb[100];
	if (isValid()) {
		snprintf(sb, sizeof(sb), "%.1f", v.getAzideg());
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "wind azi: %s", sb);
		return std::string(sb);
	} else {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "return NULL");
		return "NULL";
	}
}
std::string	WindRecorder::getMaxString(void) const {
	char	sb[100];
	if (isValid()) {
		snprintf(sb, sizeof(sb), "%.1f", max.getAbs());
		return std::string(sb);
	} else
		return "NULL";
}
std::string	WindRecorder::getDurationString(void) const {
	char	sb[100];
	if (isValid()) {
		snprintf(sb, sizeof(sb), "%.1f", getDuration());
		return std::string(sb);
	} else
		return "NULL";
}

// the updatequery method builds the five queries that are needed to add
// all what we know about wind to the sdata table. Note that this sensor
// must be refered to as windspeed, and all the other fields have ids
// larger than that
void	WindRecorder::sendOutlet(Outlet *outlet, const time_t timekey,
	const int sensorid, const int fieldid) const {
	stringlist	result;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "asking for wind queries");
	// return immediately if we don't have a value
	if (!isValid()) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "no wind queries");
		return;
	}

	// wind speed
	outlet->send(sensorid, fieldid, v.getAbs(), getUnit());

	// wind direction
	outlet->send(sensorid, fieldid + 1, v.getAzideg(), std::string("deg"));

	// maximum wind speed (windgust)
	outlet->send(sensorid, fieldid + 2, max.getAbs(), getUnit());

	// wind x and y coordinates
	outlet->send(sensorid, fieldid + 3, getDuration() * v.getX(),
		getUnit());
	outlet->send(sensorid, fieldid + 4, getDuration() * v.getY(),
		getUnit());

	// insert duration and samples into database
	outlet->send(sensorid, Field().getId("duration"), getDuration(),
		std::string("s"));
	outlet->send(sensorid, Field().getId("samples"), samples, "");
}

// output methods
std::string	WindRecorder::plain(void) const {
	char	result[1024];
	snprintf(result, sizeof(result),
		"%5.1f %s, azi: %.0f deg, max: %5.1f %s",
		getValue(), getUnit().c_str(), getAzideg(),
		getMax(), getUnit().c_str());
	return result;
}

std::string	WindRecorder::xml(const std::string& name) const {
	return "<data name=\"" + name
		+ "\" value=\"" + getSpeedString()
		+ "\" unit=\"" + conv.getUnit()
		+ "\" windgust=\"" + getMaxString()
		+ "/>\n"
		+ "<data name=\"" + name + "dir"
		+ "\" value=\"" + getAziString()
		+ "\" unit=\"deg\"/>";
}

} /* namespace meteo */

