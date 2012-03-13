/*
 * VoltageValue.cc -- implement voltage (for VantagePro console battery)
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: VoltageValue.cc,v 1.2 2004/02/25 23:48:06 afm Exp $
 */
#include <VoltageValue.h>
#include <MeteoException.h>
#include <VoltageConverter.h>
#include <mdebug.h>

namespace meteo {

// VoltageValue methods
VoltageValue::VoltageValue(void) : BasicValue("C") {
}
VoltageValue::VoltageValue(double v) : BasicValue("C") {
	setValue(v);
}

VoltageValue::VoltageValue(double v, const std::string& u)
	: BasicValue(u) {
	setValue(v);
}

VoltageValue::VoltageValue(const std::string& u) : BasicValue(u) {
}

void	VoltageValue::setUnit(const std::string& targetunit) {
	if (hasValue()) {
		VoltageConverter(targetunit).convert(this);
	}
}

} /* namespace meteo */
