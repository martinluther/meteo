/*
 * BytePacketReader.h -- basic methods to read bytes off a packet and convert
 *                       them to doubles
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _BytePacketReader_h
#define _BytePacketReader_h

#include <BasicPacketReader.h>

namespace meteo {

class BytePacketReader : public BasicPacketReader {
	bool	sign;
public:
	BytePacketReader(int o, bool s);
	virtual ~BytePacketReader(void);

	virtual double	value(const std::string& packet) const;
	virtual bool	valid(const std::string& packet) const;
};

} /* namespace meteo */

#endif /* _BytePacketReader_h */
