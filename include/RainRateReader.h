/*
 * RainRateReader.h -- read temperatures from a Davis packet.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
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
