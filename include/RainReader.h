/*
 * RainReader.h -- read rain level from a Davis Vantage Pro packet.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: RainReader.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _RainReader_h
#define _RainReader_h

#include <ShortPacketReader.h>

namespace meteo {

class RainReader : public ShortPacketReader {
public:
	RainReader(int o);
	virtual ~RainReader(void);
	virtual	double	value(const std::string& packet) const;
};

} /* namespace meteo */

#endif /* _RainReader_h */
