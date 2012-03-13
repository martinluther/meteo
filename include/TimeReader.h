/*
 * TimeReader.h -- read temperatures from a Davis Vantage
 *                        Pro packet.
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _TimeReader_h
#define _TimeReader_h

#include <ShortPacketReader.h>

namespace meteo {

class TimeReader : public ShortPacketReader {
public:
	TimeReader(int o);
	virtual ~TimeReader(void);
	// returns the unix time corresponding to the event
	virtual	double	value(const std::string& packet) const;
	virtual bool	valid(const std::string& packet) const;
};

} /* namespace meteo */

#endif /* _TimeReader_h */
