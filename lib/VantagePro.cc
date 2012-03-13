/*
 * VantagePro.cc -- weather station abstraction
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: VantagePro.cc,v 1.13 2004/02/25 23:48:06 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <VantagePro.h>
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#include <MeteoException.h>
#include <BytePacketReader.h>
#include <ExtraTemperatureReader.h>
#include <mdebug.h>
#include <crc.h>

namespace meteo {

// construct a VantagePro station: this adds all the packet readers
// to the station, so that the updater can read the fields symbolically
VantagePro::VantagePro(const std::string& n) : DavisStation(n, 99) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "create VantagePro(%s, 99)",
		n.c_str());

	// add all readers
	addAllReaders(ReaderInfo::getReaders("VantagePro"));

	// read rain collector type, and other calibration numbers
	int	raincoll = (0x30 & eeprom(0x2b, 1)[0]) >> 4;
	switch (raincoll) {
	case 0:	// 0.01in rain collector, no calibration required
		break;
	case 1: // 0.2mm rain collector, divide by 1.27
		calibrateReader("iss.rain", Calibrator(1/1.27, 0.));
		calibrateReader("iss.rainrate", Calibrator(1/1.27, 0.));
		break;
	case 2:	// 0.1mm rain collector, devide by 2.54
		calibrateReader("iss.rain", Calibrator(1/2.54, 0.));
		calibrateReader("iss.rainrate", Calibrator(1/2.54, 0.));
		break;
	default:
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "unknown rain collector type %d",
			raincoll);
		break;
	}

	// read the temperature and humidity calibration numbers
	std::string	packet = eeprom(0x32, 29);
#define	CAL_OFFSET	0x32
#define	TEMP_IN		51
#define	TEMP_OUT_CAL	52
#define	TEMP_CAL	53
#define	HUM_IN_CAL	68
#define	HUM_CAL		69
#define	DIR_CAL		77
#define	CAL(a)		(a - CAL_OFFSET)

	// calibrate ordinary temperature sensors
	calibrateReader("console.temperature",
		Calibrator(1., BytePacketReader(CAL(TEMP_IN), true)
			.value(packet)/10.));
	calibrateReader("iss.temperature",
		Calibrator(1., BytePacketReader(CAL(TEMP_CAL), true)
			.value(packet)/10.));

	// extra temperature calibration
	for (int i = 0; i < 7; i++) {
		char	f[128];
		snprintf(f, sizeof(f), "extra%d.temperature", i + 1);
		if (hasReader(f)) {
			calibrateReader(f, Calibrator(1.,
				BytePacketReader(CAL(TEMP_CAL) + i, true).value(packet)));
		}
	}

	// soil temperature calibration
	for (int i = 0; i < 4; i++) {
		char	f[128];
		snprintf(f, sizeof(f), "soil%d.temperature", i + 1);
		if (hasReader(f)) {
			calibrateReader(f, Calibrator(1.,
				BytePacketReader(CAL(TEMP_CAL) + 7 + i, true)
					.value(packet)/10.));
		}
	}

	// leaf temperature calibration
	for (int i = 0; i < 4; i++) {
		char	f[128];
		snprintf(f, sizeof(f), "leaf%d.temperature", i + 1);
		if (hasReader(f)) {
			calibrateReader(f, Calibrator(1.,
				BytePacketReader(CAL(TEMP_CAL) + 11 + i, true)
					.value(packet)/10.));
		}
	}

	// inside humidity calibration
	Calibrator	humcal(1., BytePacketReader(CAL(HUM_IN_CAL), true)
		.value(packet));
	humcal.setTopclip(100.);
	humcal.setBottomclip(0.);
	calibrateReader("console.humidity", humcal);

	// outside humidity calibration
	Calibrator	humcal2(1., BytePacketReader(CAL(HUM_CAL), true)
		.value(packet));
	humcal2.setTopclip(100.);
	humcal2.setBottomclip(0.);
	calibrateReader("iss.humidity", humcal2);

	// calibrate the extrahumidity sensors
	for (int i = 0; i < 7; i++) {
		char	f[128];
		snprintf(f, sizeof(f), "extra%d.humidity", i + 1);
		if (hasReader(f)) {
			Calibrator	cal(1.,
				BytePacketReader(CAL(HUM_CAL) + 1 + i, true)
					.value(packet));
			cal.setTopclip(100.);
			cal.setBottomclip(0.);
			calibrateReader(f, cal);
		}
	}
}

VantagePro::~VantagePro(void) { }

// to conserver energy, the Vantage Pro very often goes to sleep. So before
// doing anything, we should wake it up by sending a \r, the station will
// reply by \r\n
void	VantagePro::wakeup(int iterations) {
	// before every loop iteration, the vantage pro needs to be
	// waked up. This is necessary because the Vantage Pro goes
	// to sleep after it has executed a LOOP command.
	// Send \r bytes, read \n\r
	std::string	response;
	int	loops = iterations;
	do {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"start Vantage Pro wake up sequence");
		getChannel()->sendChar('\n');
		try {
			response = getChannel()->recvString(2);
		} catch (MeteoException& me) {
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "read timeout, try again");
			response = "";
		}
		if (response != "\n\r") {
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"problem in wake up sequence, trying drain");
			getChannel()->drain(10);
		}
	} while ((response != "\n\r") && (loops-- > 0));
	if (loops <= 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "wakeup failed");
		throw MeteoException("wakeup failed", "");
	}
}

void	VantagePro::startLoop(int p) {
	// before every loop iteration, the vantage pro needs to be
	// waked up. This is necessary because the Vantage Pro goes
	// to sleep after it has executed a LOOP command.
	// Send \r bytes, read \n\r
	wakeup(3);

	// format a loop command
	char	lcmd[30];
	snprintf(lcmd, sizeof(lcmd), "LOOP %d\n", p);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "sending LOOP command: %s", lcmd);
	getChannel()->sendString(lcmd);

	// the startLoop command also reads the ACK from the station
	DavisStation::startLoop(p);
}

std::string	VantagePro::eeprom(const int eeaddr, const int bytes) {
	char	command[128];
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "retrieving %d bytes @ %02x\n", eeaddr,
		bytes, eeaddr);
	// construct the command we would like to send to the station
	snprintf(command, sizeof(command), "EEBRD %02x %02x\n", eeaddr, bytes);

	// send the command
	getChannel()->sendString(command);

	// wait for a bytes+1 reply
	std::string	response = getChannel()->recvString(bytes + 1 + 2);

	// return the substring starting at offset 1 (not 0)
	return response.substr(1);
}

std::string	VantagePro::readPacket(void) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "waiting for Davis packet (%d bytes)",
		getPacketsize());
	std::string	packet = getChannel()->recvString(getPacketsize());
	// verify the header
	if (packet.substr(0, 3) != "LOO") {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"VantagePro packet does not start with LOO");
		throw MeteoException("VantagePro packet does not start "
			"with LOO", "");
	}

	// verify the check sum
	crc_t	crc = (unsigned char)packet[getPacketsize() - 1] |
		((unsigned char)packet[getPacketsize() - 2] << 8);
	if (0 != crc_check(&crc, (unsigned char *)packet.substr(0,
		getPacketsize() - 2).c_str(), getPacketsize() - 2)) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "wrong CRC on packet");
		throw MeteoException("wrong CRC on packet", "");
	}

	// return the packet
	return packet;
}

} /* namespace meteo */
