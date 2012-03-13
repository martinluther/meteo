/*
 * BytePacketReader.h -- basic methods to read bytes off a packet and convert
 *                       them to doubles
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: BytePacketReader.h,v 1.2 2004/02/25 23:52:34 afm Exp $
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
