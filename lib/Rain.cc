/*
 * Rain.cc -- encapsulate rain measurements 
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Rain.cc,v 1.12 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <Rain.h>
#include <MeteoException.h>
#include <RainConverter.h>
#include <mdebug.h>

namespace meteo {

Rain::Rain(void) : BasicValue("mm") { }

Rain::Rain(double v) : BasicValue("mm") {
	value = v;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "initialized rain from %.2fmm", v);
}

Rain::Rain(double v, const std::string& u) : BasicValue(u) {
	value = v;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "initialized rain from %.2f%s",
		v, u.c_str());
}

Rain::Rain(const std::string& u) : BasicValue(u) {
}

Rain::~Rain(void) { }

void	Rain::setUnit(const std::string& targetunit) {
	if (hasValue()) {
		RainConverter(targetunit).convert(this);
	}
}

std::string	Rain::getValueString(void) const {
	return BasicValue::getValueString(std::string("%.3f"));
}

} /* namespace meteo */
