/*
 * Wind.cc -- wind 
 * 
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 *
 * $Id: Wind.cc,v 1.15 2006/05/16 11:19:54 afm Exp $
 */
#include <Wind.h>
#include <WindConverter.h>
#include <mdebug.h>
#include <MeteoException.h>

namespace meteo {

// constructors
Wind::Wind(void) : WindSpeed("m/s") {
	azi = 0.;
}
Wind::Wind(const Vector& v0) : WindSpeed("m/s") {
	WindSpeed::setValue(v0.getAbs());
	azi = v0.getAzi();
}
Wind::Wind(const Vector& v0, const std::string& u) : WindSpeed(u) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creating Wind from Vector");
	azi = v0.getAzi();
	WindSpeed::setValue(v0.getAbs());
}
Wind::Wind(const std::string& u) : WindSpeed(u) {
}

Wind::~Wind(void) { }

// setting values from various types
void	Wind::setValue(const Vector& v) {
	azi = v.getAzi();
	WindSpeed::setValue(v.getAbs());
}

void	Wind::setUnit(const std::string& targetunit) {
	if (hasValue()) {
		WindConverter(targetunit).convert(this);
	}
}

// retrieve values in different types
double	Wind::getAzideg(void) const {
	return 180. * getAzi() / PI;
}
double	Wind::getX(void) const {
	if (!hasValue())
		throw MeteoException("cannot retrieve getX without value", "");
	return sin(azi) * getValue();
}

double	Wind::getY(void) const {
	if (!hasValue())
		throw MeteoException("cannot retrieve getY without value", "");
	return cos(azi) * getValue();
}

Vector	Wind::getVectorValue(void) const {
	if (!hasValue())
		throw MeteoException("cannot retrieve Vector without value",
			"");
	return Vector(getValue(), (short unsigned)getAzideg(), true);
}

// get string forms for display purposes
std::string	Wind::getSpeedString(void) const {
	char	sb[100];
	if (hasValue()) {
		snprintf(sb, sizeof(sb), "%.1f", getValue());
		return std::string(sb);
	} else
		return "NULL";
}
std::string	Wind::getXString(void) const {
	char	sb[100];
	if (hasValue()) {
		snprintf(sb, sizeof(sb), "%.1f", getX());
		return std::string(sb);
	} else
		return "NULL";
}
std::string	Wind::getYString(void) const {
	char	sb[100];
	if (hasValue()) {
		snprintf(sb, sizeof(sb), "%.1f", getY());
		return std::string(sb);
	} else
		return "NULL";
}
std::string	Wind::getAziString(void) const {
	char	sb[100];
	if (hasValue()) {
		snprintf(sb, sizeof(sb), "%.1f", getAzideg());
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "wind azi: %s", sb);
		return std::string(sb);
	} else {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "return NULL");
		return "NULL";
	}
}

// output methods
std::string	Wind::plain(const std::string& name) const {
	char	result[1024];
	snprintf(result, sizeof(result),
		"%s.speed %5.1f %s\n%s.azi %.0f",
		name.c_str(), getValue(), getUnit().c_str(), name.c_str(),
		getAzideg());
	return result;
}

std::string	Wind::xml(const std::string& name) const {
	return "<data name=\"" + name + ".speed"
		+ "\" unit=\"" + getUnit()
		+ "\" value=\"" + getSpeedString()
		+ "</data>\n"
		+ "<data name=\"" + name + ".dir"
		+ "\" value=\"" + getAziString()
		+ "\" unit=\"deg\"/>";
}

} /* namespace meteo */
