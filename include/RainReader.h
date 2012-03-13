/*
 * RainReader.h -- read rain level from a Davis Vantage Pro packet.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
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
