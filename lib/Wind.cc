/*
 * Wind.cc -- wind 
 * 
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 */
#include <Wind.h>
#include <Converter.h>
#include <mdebug.h>
#include <MeteoException.h>

namespace meteo {

void	Wind::reset(void) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "resetting duration timer for wind");
	start.now();
	hasvalue = false;
}

void	Wind::update(const Vector& v0) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
		"Wind udpate (start = %.f, last = %.f) with (%.1f, %.1f)",
		start.getValue(), lastupdate.getValue(), v0.getX(), v0.getY());
	// handle the case where don't have anyhting yet (except a start
	// time)
	if (!hasvalue) {
		v = v0; max = v0; lastupdate.now(); hasvalue = true;
		return;
	}
	double	t1 = getDuration();
	Timeval	n; n.now();
	double	t2 = (n - lastupdate).getValue();

	// update the vector
	double	duration = t1 + t2;
	if (duration > 0) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "wind update: "
			"(%.1f * (%.1f, %.1f) + %.1f * (%.1f, %.1f))/%.1f)",
			t1, v.getX(), v.getY(), t2, v0.getX(), v0.getY(),
			duration);
		v = (1/duration) * (t1 * v + t2 * v0);
	} else
		v = v0;

	// update maximum values
	double m = v0.getAbs();
	if (m > getMax()) {
		max = v0;
	}

	// remember this update
	lastupdate.now();
}

void	Wind::update(const Wind& w) {
	update(WindConverter(getUnit())(w).getValue());
}

double	Wind::getDuration(void) const {
	if (hasvalue) {
		double	dur = (lastupdate - start).getValue();
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "duration: %.1f", dur);
		return dur;
	} else
		throw MeteoException("Wind does not have value", "");
}

std::string	Wind::getSpeedString(void) const {
	char	sb[100];
	if (hasvalue) {
		snprintf(sb, sizeof(sb), "%.1f", v.getAbs());
		return std::string(sb);
	} else
		return "NULL";
}
std::string	Wind::getXString(void) const {
	char	sb[100];
	if (hasvalue) {
		snprintf(sb, sizeof(sb), "%.1f", v.getX());
		return std::string(sb);
	} else
		return "NULL";
}
std::string	Wind::getYString(void) const {
	char	sb[100];
	if (hasvalue) {
		snprintf(sb, sizeof(sb), "%.1f", v.getY());
		return std::string(sb);
	} else
		return "NULL";
}
std::string	Wind::getAziString(void) const {
	char	sb[100];
	if (hasvalue) {
		snprintf(sb, sizeof(sb), "%.1f", v.getAzi());
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "wind azi: %s", sb);
		return std::string(sb);
	} else {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "return NULL");
		return "NULL";
	}
}
std::string	Wind::getMaxString(void) const {
	char	sb[100];
	if (hasvalue) {
		snprintf(sb, sizeof(sb), "%.1f", max.getAbs());
		return std::string(sb);
	} else
		return "NULL";
}
std::string	Wind::getDurationString(void) const {
	char	sb[100];
	if (hasvalue) {
		snprintf(sb, sizeof(sb), "%.1f", getDuration());
		return std::string(sb);
	} else
		return "NULL";
}

} /* namespace meteo */
