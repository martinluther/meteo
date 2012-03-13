/*
 * UVValue.cc -- implement UV value
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: UVValue.cc,v 1.2 2004/02/25 23:48:06 afm Exp $
 */
#include <UVValue.h>
#include <MeteoException.h>
#include <UVConverter.h>
#include <mdebug.h>

namespace meteo {

UVValue::UVValue(void) : BasicValue("index") {
}
UVValue::UVValue(double v) : BasicValue("index") {
	setValue(v);
}
UVValue::UVValue(double v, const std::string& u) :BasicValue(u) {
	setValue(v);
}
UVValue::UVValue(const std::string& u) : BasicValue(u) {
}
void	UVValue::setUnit(const std::string& targetunit) {
	if (hasValue()) {
		UVConverter(targetunit).convert(this);
	}
}

} /* namespace meteo */
