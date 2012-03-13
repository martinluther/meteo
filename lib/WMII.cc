/*
 * WMII.cc -- Weather Monitor II station implementation
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: WMII.cc,v 1.16 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <WMII.h>
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif /* HAVE_ARPA_INET_H */
#include <Configuration.h>
#include <ChannelFactory.h>
#include <MeteoException.h>
#include <mdebug.h>
#include <ShortPacketReader.h>
#include <TemperatureReader.h>
#include <BarometerReader.h>

namespace meteo {

WMII::WMII(const std::string& stationname) : OldDavisStation(stationname, 18) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creating WMII(%s, 18)",
		stationname.c_str());

	// now create readers for all the data available in the station
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "start adding readers");
	addAllReaders(ReaderInfo::getReaders("WMII"));

	// retrieve the rain calibration number from the station
	ShortPacketReader	raincalreader(1, false, true);
	double	rain_cal = raincalreader(readBytes(1, 0xd6, 2));
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "got raincal %f", rain_cal);
	Calibrator	raincal(100./rain_cal, 0.);

	// retrieve the pressure calibration number from the station
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "reading pressure calibration number");
	ShortPacketReader	pressurereader(1, true, true);
	double	pressure_cal = pressurereader(readBytes(1, 0x2c, 2))/1000.;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "got pressurecal %f", pressure_cal);
	Calibrator	pressurecal(1., -pressure_cal);

	// retrieve temperature calibration numbers
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "reading temp calibration number");
	TemperatureReader	temperaturereader(1);
	double	temperature_cal = temperaturereader(readBytes(1, 0x78, 2));
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "got temperature_cal = %f",
		temperature_cal);
	Calibrator	tempcal(1., temperature_cal);
	double temperature_inside_cal = temperaturereader(readBytes(1, 0x52, 2));
	Calibrator	tempcal_inside(1., temperature_inside_cal);

	// retrieve humidity calibration numbers
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "reading humidity calibration number");
	ShortPacketReader	humidityreader(1, true, true);
	double	humidity_cal = humidityreader(readBytes(1, 0xda, 1));
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "got humidity_cal = %f", humidity_cal);
	Calibrator	humcal(1, humidity_cal);
	humcal.setTopclip(100.);
	humcal.setBottomclip(0.);

	// add Calibrators
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "start adding calibrators");
	calibrateReader("outside.rain", raincal);
	calibrateReader("outside.temperature", tempcal);
	calibrateReader("inside.temperature", tempcal_inside);
	calibrateReader("outside.humidity", humcal);
	calibrateReader("inside.barometer", pressurecal);

	// inform user
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "WMII created");
}

WMII::~WMII(void) { }

} /* namespace meteo */
