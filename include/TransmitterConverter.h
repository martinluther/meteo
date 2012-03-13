/*
 * TransmitterConverter.h -- convert voltage values
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _TransmitterConverter_h
#define _TransmitterConverter_h

#include <string>
#include <BasicConverter.h>
#include <TransmitterStatus.h>

namespace meteo {

class	TransmitterConverter : public BasicConverter {
public:
	TransmitterConverter(void) : BasicConverter("") { }
	TransmitterConverter(const std::string& unit) : BasicConverter(unit) { }
	virtual ~TransmitterConverter(void) { }
};

} /* namespace meteo */

#endif /* _TransmitterConverter_h */
