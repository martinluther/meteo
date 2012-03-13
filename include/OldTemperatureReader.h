/*
 * OldTemperatureReader.h -- read temperatures from a Davis Vantage
 *                        Pro packet.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _OldTemperatureReader_h
#define _OldTemperatureReader_h

#include <NibblePacketReader.h>

namespace meteo {

class OldTemperatureReader : public NibblePacketReader {
public:
	OldTemperatureReader(int o);
	virtual ~OldTemperatureReader(void);
	virtual	double	value(const std::string& packet) const;
};

} /* namespace meteo */

#endif /* _OldTemperatureReader_h */
