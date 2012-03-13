/*
 * BarometerReader.h -- read pressure from a Davis Vantage Pro packet.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: BarometerReader.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _BarometerReader_h
#define _BarometerReader_h

#include <ShortPacketReader.h>

namespace meteo {

class BarometerReader : public ShortPacketReader {
public:
	BarometerReader(int o);
	virtual ~BarometerReader(void);
	virtual	double	value(const std::string& packet) const;
};

} /* namespace meteo */

#endif /* _BarometerReader_h */
