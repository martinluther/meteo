/*
 * Health.cc -- Health station implementation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <Health.h>
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#include <Configuration.h>
#include <ChannelFactory.h>
#include <MeteoException.h>
#include <mdebug.h>
#include <ShortPacketReader.h>
#include <NibblePacketReader.h>
#include <BarometerReader.h>

namespace meteo {

Health::Health(const std::string& stationname) : OldDavisStation(stationname, 28) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "create Health(%s, 28)",
		stationname.c_str());

	// now create readers for all the data available in the station
	addAllReaders(ReaderInfo::getReaders("Health"));

	// retrieve speed calibration number from station
	ShortPacketReader	speedcalreader(1, false, true);
	double	speed_cal = speedcalreader(readBytes(0, 0x6a, 2));
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "got speed_cal %f", speed_cal);

	// retrieve the rain calibration number from the station
	ShortPacketReader	raincalreader(1, false, true);
	double	rain_cal = raincalreader(readBytes(1, 0xb9, 2));
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "got raincal %f", rain_cal);

	// retrieve the pressure calibration number from the station
	ShortPacketReader	pressurereader(1, true, true);
	double	pressure_cal = pressurereader(readBytes(1, 0x2c, 2))/1000.;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "got pressurecal %f", pressure_cal);

	// retrieve temperature calibration numbers
	NibblePacketReader	temperaturereader(1, true, 3);
	double	temperature_cal = temperaturereader(readBytes(1, 0x51, 2))/10.;
	double	temperature_inside_cal
		= temperaturereader(readBytes(1, 0x39, 2))/10.;

	// retrieve humidity calibration numbers
	ShortPacketReader	humidityreader(1, true, true);
	double	humidity_cal = humidityreader(readBytes(1, 0x90, 1));

	// calibrate readers
	calibrateReader("inside.temperature",
		Calibrator(1., temperature_inside_cal));
	calibrateReader("outside.temperature", Calibrator(1., temperature_cal));
	calibrateReader("outside.rain", Calibrator(100./rain_cal, 0.));
	calibrateReader("outside.wind", Calibrator(1600./speed_cal, 0.));
	calibrateReader("inside.barometer", Calibrator(1., -pressure_cal));
	Calibrator	humcal(1., humidity_cal);
	humcal.setTopclip(100.);
	humcal.setBottomclip(100.);
	calibrateReader("outside.humidity", humcal);
}

Health::~Health(void) { }

} /* namespace meteo */
