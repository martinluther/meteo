/*
 * BasicConverter.cc -- base class for converters
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <BasicConverter.h>
#include <mdebug.h>

namespace meteo {

BasicConverter::BasicConverter(const std::string& unit) : targetunit(unit) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creating BasicConverter %p", this);
}

BasicConverter::~BasicConverter(void) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "destroying BasicConverter %p", this);
}

double	BasicConverter::operator()(const double v,
		const std::string& sourceunit) const {
	return v;
}

void	BasicConverter::convert(BasicValue *b) const {
	b->setValue(this->operator()(b->getValue(), b->getUnit()));
}

} /* namespace meteo */
