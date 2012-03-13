/*
 * LevelValue.cc -- implement water level
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <LevelValue.h>
#include <MeteoException.h>
#include <LevelConverter.h>
#include <mdebug.h>

namespace meteo {

LevelValue::LevelValue(void) : BasicValue("m") {
}
LevelValue::LevelValue(double v) : BasicValue("m") {
	setValue(v);
}

LevelValue::LevelValue(double v, const std::string& u) : BasicValue(u) {
	setValue(v);
}

LevelValue::LevelValue(const std::string& u) : BasicValue(u) {
}

void	LevelValue::setUnit(const std::string& targetunit) {
	if (hasValue()) {
		LevelConverter(targetunit).convert(this);
	}
}

} /* namespace meteo */
