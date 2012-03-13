/*
 * ReaderInfo.cc -- show information about readers of a station
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: ReaderInfo.cc,v 1.8 2004/02/26 23:43:12 afm Exp $
 */
#include <ReaderInfo.h>
#include <mdebug.h>

namespace meteo {

static	stationreaders_t	vantageproreaders[46] = {
	{ "console.barometer", "BarometerReader", 7, 2, "inHg" },
	{ "console.temperature", "TemperatureReader", 9, 2, "F" },
	{ "console.humidity", "SignedBytePacketReader", 11, 1, "%" },
	{ "iss.temperature", "TemperatureReader", 12, 2, "F" },
	{ "iss.wind", "VantageProWindReader", 14, 3, "mph" },
	{ "extra1.temperature", "ExtraTemperatureReader", 18, 1, "F" },
	{ "extra2.temperature", "ExtraTemperatureReader", 19, 1, "F" },
	{ "extra3.temperature", "ExtraTemperatureReader", 20, 1, "F" },
	{ "extra4.temperature", "ExtraTemperatureReader", 21, 1, "F" },
	{ "extra5.temperature", "ExtraTemperatureReader", 22, 1, "F" },
	{ "extra6.temperature", "ExtraTemperatureReader", 23, 1, "F" },
	{ "extra7.temperature", "ExtraTemperatureReader", 24, 1, "F" },
	{ "soil1.temperature", "ExtraTemperatureReader", 25, 1, "F" },
	{ "soil2.temperature", "ExtraTemperatureReader", 26, 1, "F" },
	{ "soil3.temperature", "ExtraTemperatureReader", 27, 1, "F" },
	{ "soil4.temperature", "ExtraTemperatureReader", 28, 1, "F" },
	{ "leaf1.temperature", "ExtraTemperatureReader", 29, 1, "F" },
	{ "leaf2.temperature", "ExtraTemperatureReader", 30, 1, "F" },
	{ "leaf3.temperature", "ExtraTemperatureReader", 31, 1, "F" },
	{ "leaf4.temperature", "ExtraTemperatureReader", 32, 1, "F" },
	{ "iss.humidity", "SignedBytePacketReader", 33, 1, "%" },
	{ "extra1.humidity", "SignedBytePacketReader", 34, 1, "%" },
	{ "extra2.humidity", "SignedBytePacketReader", 35, 1, "%" },
	{ "extra3.humidity", "SignedBytePacketReader", 36, 1, "%" },
	{ "extra4.humidity", "SignedBytePacketReader", 37, 1, "%" },
	{ "extra5.humidity", "SignedBytePacketReader", 38, 1, "%" },
	{ "extra6.humidity", "SignedBytePacketReader", 39, 1, "%" },
	{ "extra7.humidity", "SignedBytePacketReader", 40, 1, "%" },
	{ "iss.rainrate", "RainRateReader", 41, 2, "in/h" },
	{ "iss.uv", "UVReader", 43, 1, "index" },
	{ "iss.solar", "SignedLittleEndianShortPacketReader", 44, 1, "W/m2" },
	{ "iss.rain", "RainReader", 50, 1, "in" },
	{ "soil1.moisture", "UnsignedBytePacketReader", 62, 1, "cb" },
	{ "soil2.moisture", "UnsignedBytePacketReader", 63, 1, "cb" },
	{ "soil3.moisture", "UnsignedBytePacketReader", 64, 1, "cb" },
	{ "soil4.moisture", "UnsignedBytePacketReader", 65, 1, "cb" },
	{ "leaf1.wetness", "UnsignedBytePacketReader", 66, 1, "index" },
	{ "leaf2.wetness", "UnsignedBytePacketReader", 67, 1, "index" },
	{ "leaf3.wetness", "UnsignedBytePacketReader", 68, 1, "index" },
	{ "leaf4.wetness", "UnsignedBytePacketReader", 69, 1, "index" },
	{ "iss.transmitter", "UnsignedBytePacketReader", 86, 1, "" },
	{ "console.battery", "VoltageReader", 87, 2, "V" },
	{ "console.sunrise", "TimeReader", 91, 2, "unix" },
	{ "console.sunset", "TimeReader", 93, 2, "unix" },
	{ NULL, NULL, 0, 0, NULL }
};

static stationreaders_t	wmiireaders[8] = {
	{ "inside.temperature", "TemperatureReader", 1, 2, "F" },
	{ "outside.temperature", "TemperatureReader", 3, 2, "F" },
	{ "outside.wind", "WMIIWindReader", 5 /* not really used */, 2, "mph" },
	{ "inside.barometer", "BarometerReader", 8, 2, "inHg" },
	{ "inside.humidity", "SignedBytePacketReader", 10, 1, "%" },
	{ "outside.humidity", "SignedBytePacketReader", 11, 1, "%" },
	{ "outside.rain", "RainReader", 12, 2, "in" },
	{ NULL, NULL, 0, 0, NULL }
};

static stationreaders_t	groweatherreaders[9] = {
	{ "soil.temperature", "OldTemperatureReader", 4, 2, "F" },
	{ "outside.temperature", "OldTemperatureReader", 6, 2, "F" },
	{ "outside.wind", "OldWindReader", 8, 3, "mph" },
	{ "inside.barometer", "UnsignedLittleEndianShortPacketReader",
		11, 2, "inHg" },
	{ "outside.rainrate", "RainRateReader", 13, 1, "in/h" },
	{ "outside.humidity", "SignedBytePacketReader", 14, 2, "%" },
	{ "outside.rain", "RainReader", 15, 2, "in" },
	{ "outside.solar", "SignedLittleEndianShortPacketReader", 17, 2, "W/m2" },
	{ NULL, NULL, 0, 0, NULL }
};

static stationreaders_t	energyreaders[9] = {
	{ "inside.temperature", "TemperatureReader", 4, 2, "F" },
	{ "outside.temperature", "TemperatureReader", 6, 2, "F" },
	{ "outside.wind", "OldWindReader", 8, 3, "mph" },
	{ "inside.barometer", "UnsignedLittleEndianShortPacketReader",
		11, 2, "inHg" },
	{ "outside.rainrate", "RainRateReader", 13, 1, "in/h" },
	{ "outside.humidity", "SignedBytePacketReader", 14, 1, "%" },
	{ "outside.rain", "RainReader", 15, 2, "in" },
	{ "outside.solar", "SignedLittleEndianShortPacketReader", 17, 2, "W/m2" },
	{ NULL, NULL, 0, 0, NULL }
};

static stationreaders_t	healthreaders[11] = {
	{ "inside.temperature", "OldTemperatureReader", 4, 2, "F" },
	{ "outside.temperature", "OldTemperatureReader", 6, 2, "F" },
	{ "outside.wind", "OldWindReader", 8, 2, "mph" },
	{ "inside.barometer", "UnsignedLittleEndianShortPacketReader",
		11, 2, "inHg" },
	{ "outside.rainrate", "RainRateReader", 13, 1, "in/h" },
	{ "outside.rain", "RainReader", 14, 2, "in" },
	{ "outside.solar", "SignedLittleEndianShortPacketReader", 16, 2, "W/m2" },
	{ "inside.humidity", "SignedBytePacketReader", 18, 1, "%" },
	{ "outside.humidity", "SignedBytePacketReader", 19, 1, "%" },
	{ "outside.uv", "UnsignedBytePacketReader", 20, 1, "index" },
	{ NULL, NULL, 0, 0, NULL }
};

// getReaders	retrieve a list of all the readers for a station type, which
//		can be used with the addAllReaders method of the Station
stationreaders_t *ReaderInfo::getReaders(const std::string& stationtype) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "retrieve reader info for type %s",
		stationtype.c_str());
	// return the right array based on the type
	if (stationtype == "WMII") { return wmiireaders; }
	if (stationtype == "Perception") { return wmiireaders; }
	if (stationtype == "Wizard") { return wmiireaders; }
	if (stationtype == "GroWeather") { return groweatherreaders; }
	if (stationtype == "Energy") { return energyreaders; }
	if (stationtype == "Health") { return healthreaders; }
	if (stationtype == "VantagePro") { return vantageproreaders; }
	mdebug(LOG_ERR, MDEBUG_LOG, 0, "unknown station type: %s",
		stationtype.c_str());
	return NULL;
}

stringlist	ReaderInfo::knownTypes(void) {
	stringlist	result;
	result.push_back("WMII");
	result.push_back("Perception");
	result.push_back("Wizard");
	result.push_back("GroWeather");
	result.push_back("Energy");
	result.push_back("Health");
	result.push_back("VantagePro");
	return result;
}

} /* namespace meteo */
