/*
 * VoltageReader.h -- read temperatures from a Davis Vantage
 *                        Pro packet.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _VoltageReader_h
#define _VoltageReader_h

#include <ShortPacketReader.h>

namespace meteo {

class VoltageReader : public ShortPacketReader {
public:
	VoltageReader(int o);
	virtual ~VoltageReader(void);
	virtual	double	value(const std::string& packet) const;
};

} /* namespace meteo */

#endif /* _VoltageReader_h */
