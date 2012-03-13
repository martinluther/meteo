/*
 * VoltageReader.h -- read temperatures from a Davis Vantage
 *                        Pro packet.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: VoltageReader.h,v 1.2 2004/02/25 23:52:35 afm Exp $
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
