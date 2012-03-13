/*
 * UVReader.h -- read uv index from a Davis packet.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: UVReader.h,v 1.2 2004/02/25 23:52:35 afm Exp $
 */
#ifndef _UVReader_h
#define _UVReader_h

#include <BytePacketReader.h>

namespace meteo {

class UVReader : public BytePacketReader {
public:
	UVReader(int o);
	virtual ~UVReader(void);
	virtual	double	value(const std::string& packet) const;
};

} /* namespace meteo */

#endif /* _UVReader_h */
