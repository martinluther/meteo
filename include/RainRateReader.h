/*
 * RainRateReader.h -- read temperatures from a Davis packet.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: RainRateReader.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _RainRateReader_h
#define _RainRateReader_h

#include <BytePacketReader.h>

namespace meteo {

class RainRateReader : public BytePacketReader {
public:
	RainRateReader(int o);
	virtual ~RainRateReader(void);
	virtual	double	value(const std::string& packet) const;
};

} /* namespace meteo */

#endif /* _RainRateReader_h */
