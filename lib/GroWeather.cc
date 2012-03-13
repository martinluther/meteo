/*
 * GroWeather.cc -- GroWeather station implementation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <GroWeather.h>
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#include <Configuration.h>
#include <ChannelFactory.h>
#include <MeteoException.h>
#include <mdebug.h>
#include <ShortPacketReader.h>
#include <NibblePacketReader.h>
#include <TemperatureReader.h>
#include <BarometerReader.h>

namespace meteo {

GroWeather::GroWeather(const std::string& stationname) : OldDavisStation(stationname, 36) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creating GroWeather(%s, 36)",
		stationname.c_str());

	// now create readers for all the data available in the station
	addAllReaders(ReaderInfo::getReaders("GroWeather"));

	// retrieve the rain calibration number from the station
	ShortPacketReader	raincalreader(1, false, true);
	double	rain_cal = raincalreader(readBytes(1, 0xcf, 2));
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "got raincal %f", rain_cal);
	Calibrator	raincal(100./rain_cal, 0.);

	// retrieve the pressure calibration number from the station
	ShortPacketReader	pressurereader(1, true, true);
	double pressure_cal = pressurereader(readBytes(1, 0x20, 2));
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "got pressurecal %f", pressure_cal);
	Calibrator	pressurecal(1., pressure_cal);

	// retrieve temperature calibration numbers
	NibblePacketReader	temperaturereader(1, true, 3);
	double temperature_cal = temperaturereader(readBytes(1, 0x51, 2));
	double soiltemperature_cal = temperaturereader(readBytes(1, 0x3f, 2));
	Calibrator	tempcal(1., temperature_cal);
	Calibrator	soiltempcal(1., soiltemperature_cal);

	// retrieve humidity calibration numbers
	ShortPacketReader	humidityreader(1, true, true);
	double humidity_cal = humidityreader(readBytes(1, 0x85, 1));
	Calibrator	humcal(1., humidity_cal);
	humcal.setTopclip(100.);
	humcal.setBottomclip(0.);

	// wind speed calibration
	ShortPacketReader	speedreader(1, false, true);
	double	speed_cal = speedreader(readBytes(0, 0x6a, 2));
	Calibrator	windcal(1600./speed_cal, 0.);

	// add calibrators
	calibrateReader("soil.temperature", soiltempcal);
	calibrateReader("outside.temperature", tempcal);
	calibrateReader("inside.barometer", pressurecal);
	calibrateReader("outside.humidity", humcal);
	calibrateReader("outside.wind", windcal);
	calibrateReader("outside.rain", raincal);
}

GroWeather::~GroWeather(void) { }

} /* namespace meteo */
