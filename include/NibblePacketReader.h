/*
 * NibblePacketReader.h -- basic methods to read bytes off a packet and convert
 *                       them to doubles
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: NibblePacketReader.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _NibblePacketReader_h
#define _NibblePacketReader_h

#include <BasicPacketReader.h>

namespace meteo {

class NibblePacketReader : public BasicPacketReader {
	bool	sign;
	int	nibbles;
public:
	NibblePacketReader(int o, bool s, int n);
	virtual ~NibblePacketReader(void);

	unsigned int	getNibble(int i, const std::string& packet) const;
	unsigned int	rawvalue(const std::string& packet) const;

	virtual double	value(const std::string& packet) const;
	virtual bool	valid(const std::string& packet) const;
};

} /* namespace meteo */

#endif /* _NibblePacketReader_h */
