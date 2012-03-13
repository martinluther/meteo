/*
 * Rain.cc -- encapsulate rain measurements 
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <Rain.h>
#include <MeteoException.h>
#include <Converter.h>
#include <mdebug.h>

namespace meteo {

void	Rain::update(double rain) {
	if (!hasvalue)
		svalue = rain;
	hasvalue = true;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "updating rain to %.1f", rain);
	if (rain < svalue) // a reset has occured
		svalue = 0.;
	value = rain;
	lastupdate.now();
}

void	Rain::update(const Rain& r) {
	update(RainConverter(unit)(r).getValue());
}

void	Rain::reset(void) {
	hasvalue = false;
	start.now();
}

double	Rain::getValue(void) const {
	if (!hasvalue)
		throw MeteoException("no rain value", "");
	return value - svalue;
}
double	Rain::getTotal(void) const {
	if (!hasvalue)
		throw MeteoException("no rain value", "");
	return svalue;
}

double	Rain::getRate(void) const {
	if (!hasvalue)
		throw MeteoException("no rain value", "");
	double	duration = (lastupdate - start).getValue();
	if (duration == 0.)
		throw MeteoException("zero duration", "");
	return getValue()/duration;
}

std::string	Rain::getValueString(void) const {
	if (!hasvalue)
		return std::string("NULL");
	char	buffer[32];
	snprintf(buffer, sizeof(buffer), "%.3f", getValue());
	return std::string(buffer);
}

std::string	Rain::getTotalString(void) const {
	if (!hasvalue)
		return std::string("NULL");
	char	buffer[32];
	snprintf(buffer, sizeof(buffer), "%.3f", getTotal());
	return std::string(buffer);
}

} /* namespace meteo */
