/*
 * UVReader.h -- read uv index from a Davis packet.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
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
