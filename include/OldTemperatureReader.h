/*
 * OldTemperatureReader.h -- read temperatures from a Davis Vantage
 *                        Pro packet.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: OldTemperatureReader.h,v 1.2 2004/02/25 23:52:34 afm Exp $
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
