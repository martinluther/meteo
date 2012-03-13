/*
 * TemperatureReader.h -- read temperatures from a Davis Vantage
 *                        Pro packet.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TemperatureReader.h,v 1.2 2004/02/25 23:52:35 afm Exp $
 */
#ifndef _TemperatureReader_h
#define _TemperatureReader_h

#include <ShortPacketReader.h>

namespace meteo {

class TemperatureReader : public ShortPacketReader {
public:
	TemperatureReader(int o);
	virtual ~TemperatureReader(void);
	virtual	double	value(const std::string& packet) const;
};

} /* namespace meteo */

#endif /* _TemperatureReader_h */
