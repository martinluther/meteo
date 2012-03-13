/*
 * PacketReaderFactory.h -- create a packet based on a specification
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
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
