/*
 * TimeValue.cc -- implement Time value
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TimeValue.cc,v 1.1 2004/02/26 14:00:19 afm Exp $
 */
#include <TimeValue.h>
#include <MeteoException.h>
#include <TimeConverter.h>
#include <mdebug.h>

namespace meteo {

TimeValue::TimeValue(void) : BasicValue("unix") {
}
TimeValue::TimeValue(double v) : BasicValue("unix") {
	setValue(v);
}
TimeValue::TimeValue(double v, const std::string& u) :BasicValue(u) {
	setValue(v);
}
TimeValue::TimeValue(const std::string& u) : BasicValue(u) {
}
void	TimeValue::setUnit(const std::string& targetunit) {
	if (hasValue()) {
		TimeConverter(targetunit).convert(this);
	}
}

} /* namespace meteo */
