/*
 * PacketReaderFactory.cc -- creates packet reader from a specification
 *
 * (c) 2003 Dr Andreas Mueller, Beratung und Entwicklung 
 */
#include <PacketReaderFactory.h>
#include <mdebug.h>
#include <MeteoException.h>
#include <BytePacketReader.h>
#include <ShortPacketReader.h>
#include <NibblePacketReader.h>
#include <TemperatureReader.h>
#include <ExtraTemperatureReader.h>
#include <BarometerReader.h>
#include <RainReader.h>
#include <RainRateReader.h>
#include <UVReader.h>
#include <WMIIWindReader.h>
#include <VantageProWindReader.h>
#include <OldWindReader.h>
#include <OldTemperatureReader.h>
#include <VoltageReader.h>
#include <TimeReader.h>

namespace meteo {

PacketReader	PacketReaderFactory::newPacketReader(const std::string& spec,
			int offset, int length, const std::string& classname,
			const std::string& unit) {
	BasicPacketReader	*bpr = NULL;

	// construct reader according to specification
	if (spec == "SignedBytePacketReader") {
		bpr = new BytePacketReader(offset, true);	// delete ~PacketReader
	}
	if (spec == "UnsignedBytePacketReader") {
		bpr = new BytePacketReader(offset, false);	// delete ~PacketReader
	}
	if (spec == "SignedLittleEndianShortPacketReader") {
		bpr = new ShortPacketReader(offset, true, true);	// delete ~PacketReader
	}
	if (spec == "UnsignedLittleEndianShortPacketReader") {
		bpr = new ShortPacketReader(offset, false, true);	// delete ~PacketReader
	}
	if (spec == "SignedBigEndianShortPacketReader") {
		bpr = new ShortPacketReader(offset, true, false);	// delete ~PacketReader
	}
	if (spec == "UnsignedBigEndianShortPacketReader") {
		bpr = new ShortPacketReader(offset, false, false);	// delete ~PacketReader
	}
	if (spec == "SignedNibblePacketReader") {
		bpr = new NibblePacketReader(offset, true, length);	// delete ~PacketReader
	}
	if (spec == "UnsignedNibblePacketReader") {
		bpr = new NibblePacketReader(offset, false, length);	// delete ~PacketReader
	}
	if (spec == "TemperatureReader") {
		bpr = new TemperatureReader(offset);	// delete ~PacketReader
	}
	if (spec == "ExtraTemperatureReader") {
		bpr = new ExtraTemperatureReader(offset); // delete ~PacketReader
	}
	if (spec == "OldTemperatureReader") {
		bpr = new OldTemperatureReader(offset);	// delete ~PacketReader
	}
	if (spec == "BarometerReader") {
		bpr = new BarometerReader(offset);	// delete ~PacketReader
	}
	if (spec == "RainReader") {
		bpr = new RainReader(offset);	// delete ~PacketReader
	}
	if (spec == "RainRateReader") {
		bpr = new RainRateReader(offset);	// delete ~PacketReader
	}
	if (spec == "UVReader") {
		bpr = new UVReader(offset);	// delete ~PacketReader
	}
	if (spec == "VoltageReader") {
		bpr = new VoltageReader(offset);	// delete ~PacketReader
	}
	if (spec == "TimeReader") {
		bpr = new TimeReader(offset);	// delete ~PacketReader
	}
	if (spec == "OldWindReader") {
		bpr = new OldWindReader(offset);	// delete ~PacketReader
	}
	if (spec == "WMIIWindReader") {
		bpr = new WMIIWindReader();	// delete ~PacketReader
	}
	if (spec == "VantageProWindReader") {
		bpr = new VantageProWindReader();	// delete ~PacketReader
	}

	// make sure we have a packet reader
	if (bpr == NULL) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "don't understand packet "
			"reader spec %s", spec.c_str());
		throw MeteoException("dont understand packet reader spec",
			spec);
	}

	// set value class and unit
	bpr->setValueClass(classname);
	bpr->setUnit(unit);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "class: %s, unit %s",
		bpr->getValueClass().c_str(), bpr->getUnit().c_str());

	// assign a PacketReader
	PacketReader	result(bpr);
	return result;
}

} /* namespace meteo */
