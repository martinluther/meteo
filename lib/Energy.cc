/*
 * Energy.cc -- Energy station implementation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <Energy.h>
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#include <Configuration.h>
#include <ChannelFactory.h>
#include <MeteoException.h>
#include <mdebug.h>
#include <ShortPacketReader.h>
#include <BytePacketReader.h>
#include <NibblePacketReader.h>
#include <BarometerReader.h>

namespace meteo {

Energy::Energy(const std::string& stationname) : OldDavisStation(stationname, 30) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "create Energy(%s, 30)",
		stationname.c_str());

	// now create readers for all the data available in the station
	addAllReaders(ReaderInfo::getReaders("Energy"));

	// retrieve the rain calibration number from the station
	ShortPacketReader	raincalreader(1, false, true);
	double	rain_cal = raincalreader(readBytes(1, 0xca, 2));
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "got raincal %f", rain_cal);

	// retrieve the pressure calibration number from the station
	ShortPacketReader	pressurereader(1, true, true);
	double	pressure_cal = pressurereader(readBytes(1, 0x18, 2));
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "got pressurecal %f", pressure_cal);

	// retrieve temperature calibration numbers
	NibblePacketReader	temperaturereader(1, true, 3);
	double	temperature_cal = temperaturereader(readBytes(1, 0x45, 2))/10.;
	double	temperature_inside_cal
		= temperaturereader(readBytes(1, 0x33, 2))/10.;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "temp cal in/out: %f/%f",
		temperature_inside_cal, temperature_cal);

	// retrieve humidity calibration numbers
	BytePacketReader	humidityreader(1, true);
	double	humidity_cal = humidityreader(readBytes(1, 0x94, 1));
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "humidity cal %f", humidity_cal);

	// wind speed calibration number
	ShortPacketReader	speedreader(1, false, true);
	double	speed_cal = speedreader(readBytes(0, 0x6a, 2));
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "speed calibration number: %f",
		speed_cal);

	// add calibrators
	calibrateReader("temperature", Calibrator(0., temperature_cal));
	calibrateReader("temperature_inside",
		Calibrator(0., temperature_inside_cal));
	calibrateReader("rain", Calibrator(100./rain_cal, 0.));
	calibrateReader("wind", Calibrator(1600./speed_cal, 0.));
	Calibrator	humcal(1., humidity_cal);
	humcal.setTopclip(100.);
	humcal.setBottomclip(0.);
	calibrateReader("humidity", humcal);
}

Energy::~Energy(void) { }

} /* namespace meteo */
