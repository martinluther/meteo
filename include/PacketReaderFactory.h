/*
 * PacketReaderFactory.h -- create a packet based on a specification
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: PacketReaderFactory.h,v 1.3 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _PacketReaderFactory_h
#define _PacketReaderFactory_h

#include <PacketReader.h>

namespace meteo {

class PacketReaderFactory {
public:
	PacketReaderFactory(void) { }
	~PacketReaderFactory(void) { }

	static PacketReader	newPacketReader(const std::string& spec,
		int offset, int length, const std::string& classname,
		const std::string& unit);
};

} /* namespace meteo */

#endif /* _PacketReaderFactory_h */
