/*
 * MoistureValue.cc -- implement soil moisture value
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: MoistureValue.cc,v 1.3 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <MoistureValue.h>
#include <MeteoException.h>
#include <MoistureConverter.h>
#include <mdebug.h>

namespace meteo {

MoistureValue::MoistureValue(void) : BasicValue("cb") {
}

MoistureValue::MoistureValue(double v) : BasicValue("cb") {
	setValue(v);
}
MoistureValue::MoistureValue(double v, const std::string& u) : BasicValue(u) {
	setValue(v);
}
MoistureValue::MoistureValue(const std::string& u) : BasicValue(u) {
}
void	MoistureValue::setUnit(const std::string& targetunit) {
	if (hasValue()) {
		MoistureConverter(targetunit).convert(this);
	}
}

} /* namespace meteo */
