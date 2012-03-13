/*
 * RainRate.cc -- encapsulate rain measurements 
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: RainRate.cc,v 1.4 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <RainRate.h>
#include <MeteoException.h>
#include <RainRateConverter.h>
#include <mdebug.h>

namespace meteo {

RainRate::RainRate(void) : BasicValue("mm/h") { }

RainRate::RainRate(double v) : BasicValue("mm/h") {
	value = v;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "initialized rain from %.2fmm", v);
}

RainRate::RainRate(double v, const std::string& u) : BasicValue(u) {
	value = v;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "initialized rain from %.2f%s",
		v, u.c_str());
}

RainRate::RainRate(const std::string& u) : BasicValue(u) {
}

RainRate::~RainRate(void) { }

void	RainRate::setUnit(const std::string& targetunit) {
	if (hasValue()) {
		RainRateConverter(targetunit).convert(this);
	}
}

std::string	RainRate::getValueString(void) const {
	return BasicValue::getValueString(std::string("%.3f"));
}

} /* namespace meteo */
