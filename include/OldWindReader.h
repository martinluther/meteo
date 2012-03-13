/*
 * OldWindReader.h -- read wind from a Davis Weather Monitor II packet
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: OldWindReader.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _OldWindReader_h
#define _OldWindReader_h

#include <NibblePacketReader.h>
#include <BytePacketReader.h>

namespace meteo {

class OldWindReader : public BasicPacketReader {
	NibblePacketReader	winddirreader;
	BytePacketReader	windspeedreader;
public:
	OldWindReader(int o);
	virtual ~OldWindReader(void);
	// returns the unix time corresponding to the event
	virtual	double	value(const std::string& packet) const;
	virtual Value	v(const std::string& packet) const;
	virtual bool	valid(const std::string& packet) const;
};

} /* namespace meteo */

#endif /* _OldWindReader_h */
