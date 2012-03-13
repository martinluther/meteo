/*
 * WMIIWindReader.h -- read wind from a Davis Weather Monitor II packet
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _WMIIWindReader_h
#define _WMIIWindReader_h

#include <ShortPacketReader.h>
#include <BytePacketReader.h>

namespace meteo {

class WMIIWindReader : public BasicPacketReader {
	ShortPacketReader	winddirreader;
	BytePacketReader	windspeedreader;
public:
	WMIIWindReader(void);
	virtual ~WMIIWindReader(void);
	// returns the unix time corresponding to the event
	virtual	double	value(const std::string& packet) const;
	virtual Value	v(const std::string& packet) const;
	virtual bool	valid(const std::string& packet) const;
};

} /* namespace meteo */

#endif /* _WMIIWindReader_h */
