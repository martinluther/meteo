/*
 * VantageProWindReader.h -- read wind from a Davis Vantage Pro packet
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: VantageProWindReader.h,v 1.2 2004/02/25 23:52:35 afm Exp $
 */
#ifndef _VantageProWindReader_h
#define _VantageProWindReader_h

#include <ShortPacketReader.h>
#include <BytePacketReader.h>

namespace meteo {

class VantageProWindReader : public BasicPacketReader {
	BytePacketReader	windspeedreader;
	ShortPacketReader	winddirreader;
public:
	VantageProWindReader(void);
	virtual ~VantageProWindReader(void);
	// returns the unix time corresponding to the event
	virtual	double	value(const std::string& packet) const;
	virtual Value	v(const std::string& packet) const;
	virtual bool	valid(const std::string& packet) const;
};

} /* namespace meteo */

#endif /* _VantageProWindReader_h */
