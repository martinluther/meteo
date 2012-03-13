/*
 * ExtraTemperatureReader.h -- read extra temperatures from a Davis Vantage
 *                             Pro packet.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _ExtraTemperatureReader_h
#define _ExtraTemperatureReader_h

#include <BytePacketReader.h>

namespace meteo {

class ExtraTemperatureReader : public BytePacketReader {
public:
	ExtraTemperatureReader(int o);
	virtual ~ExtraTemperatureReader(void);
	virtual	double	value(const std::string& packet) const;
};

} /* namespace meteo */

#endif /* _ExtraTemperatureReader_h */
