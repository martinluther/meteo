/*
 * ShortPacketReader.h -- basic methods to read bytes off a packet and convert
 *                       them to doubles
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _ShortPacketReader_h
#define _ShortPacketReader_h

#include <BasicPacketReader.h>

namespace meteo {

class ShortPacketReader : public BasicPacketReader {
	bool	sign;
	bool	littleendian;
	// littleendian means that the first byte is the least significant,
	// which is the case for Intel processors. SPARC and PPC work the
	// other way round. The endianness of the processor running meteo
	// is of no importance, only the endianness of the data in the weather
	// station packet
public:
	ShortPacketReader(int o, bool s, bool l);
	~ShortPacketReader(void);

	virtual double	value(const std::string& packet) const;
	virtual bool	valid(const std::string& packet) const;
};

} /* namespace meteo */

#endif /* _ShortPacketReader_h */
