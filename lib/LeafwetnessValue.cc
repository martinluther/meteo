/*
 * LeafwetnessValue.cc -- leaf wetness value
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: LeafwetnessValue.cc,v 1.2 2004/02/25 23:48:05 afm Exp $
 */
#include <LeafwetnessValue.h>
#include <MeteoException.h>
#include <LeafwetnessConverter.h>
#include <mdebug.h>

namespace meteo {

LeafwetnessValue::LeafwetnessValue(void) : BasicValue("index") {
}
LeafwetnessValue::LeafwetnessValue(double v) : BasicValue("index") {
	setValue(v);
}

LeafwetnessValue::LeafwetnessValue(double v, const std::string& u)
	: BasicValue(u) {
	setValue(v);
}

LeafwetnessValue::LeafwetnessValue(const std::string& u) : BasicValue(u) {
}

void	LeafwetnessValue::setUnit(const std::string& targetunit) {
	if (hasValue()) {
		LeafwetnessConverter(targetunit).convert(this);
	}
}

} /* namespace meteo */
