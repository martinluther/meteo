/*
 * TimeValue.cc -- implement Time value
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TimeValue.cc,v 1.2 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
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
