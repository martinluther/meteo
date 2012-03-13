/*
 * ExtraTemperatureReader.h -- read extra temperatures from a Davis Vantage
 *                             Pro packet.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: ExtraTemperatureReader.h,v 1.2 2004/02/25 23:52:34 afm Exp $
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
