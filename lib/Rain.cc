/*
 * Rain.cc -- encapsulate rain measurements 
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
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
