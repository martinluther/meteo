/*
 * Station.cc -- weather station abstraction
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <Station.h>
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#include <MeteoException.h>
#include <mdebug.h>
#include <crc.h>

namespace meteo {

Datarecord::Datarecord(void) {
	setTemperatureUnit("C");
	setHumidityUnit("%");
	setPressureUnit("hPa");
	setRainUnit("mm");
	setWindUnit("m/s");
	setSolarUnit("W/m2");
	setUVUnit("index");
	samples = 0;
}

std::string	Datarecord::updatequery(const std::string& stationname) const {
	char	sb[100];
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "preparing update query");
	std::string	query =
		"insert into stationdata(timekey, station, year, month, mday, "
		"	hour, min, temperature, temperature_inside, "
		"	barometer, humidity, humidity_inside, "
		"	rain, raintotal, windspeed, winddir, windgust, "
		"	windx, windy, duration, solar, uv, samples, "
		"	group300, group1800, group7200, group86400) "
		"values(";
	// timekey and station name
	time_t	current = time(NULL);
	current -= (current % 60);
	snprintf(sb, sizeof(sb), "/*timekey */ %d, ", (int)current);
	query += sb;
	query += "/* stationname */ '" + stationname + "', ";

	// store data stamp
	struct tm	*st = gmtime(&current);
	snprintf(sb, sizeof(sb), "%d, ", 1900 + st->tm_year); query += sb;
	snprintf(sb, sizeof(sb), "%d, ", 1 + st->tm_mon); query += sb;
	snprintf(sb, sizeof(sb), "%d, ", st->tm_mday); query += sb;
	snprintf(sb, sizeof(sb), "%d, ", st->tm_hour); query += sb;
	snprintf(sb, sizeof(sb), "%d, ", st->tm_min); query += sb;

	// meteorological data fields
	query += "/* temp */ " + temperatureoutside.getValueString() + ", ";
	query += "/* tempi */ " + temperatureinside.getValueString() + ", ";
	query += "/* barometer */ " + barometer.getValueString() + ", ";
	query += "/* hum */ " + humidityoutside.getValueString() + ", ";
	query += "/* humi */ " + humidityinside.getValueString() + ", ";
	query += "/* rain */ " + rain.getValueString() + ", ";
	query += "/* raintotal */ " + rain.getTotalString() + ", ";
	if (wind.hasValue()) {
		query += "/* speed */ " + wind.getSpeedString() + ", ";
		query += "/* azi */ " + wind.getAziString() + ", ";
		query += "/* max */ " + wind.getMaxString() + ", ";
		query += "/* x */ " + wind.getXString() + ", ";
		query += "/* y */ " + wind.getYString() + ", ";
		query += "/* duration */" + wind.getDurationString() + ", ";
	} else {
		query += "NULL, NULL, NULL, NULL, NULL, NULL, ";
	}
	query += "/* solar */ " + solar.getValueString() + ", ";
	query += "/* uv */ " + uv.getValueString() + ", ";
	snprintf(sb, sizeof(sb), "/* samples */ %d, ", samples); query += sb;
	time_t	t = time(NULL);
	snprintf(sb, sizeof(sb), "%ld, ", t/300); query += sb;
	snprintf(sb, sizeof(sb), "%ld, ", t/1800); query += sb;
	snprintf(sb, sizeof(sb), "%ld, ", t/7200); query += sb;
	snprintf(sb, sizeof(sb), "%ld)", t/86400); query += sb;

	return query;
}

void	Datarecord::reset(void) {
	samples = 0;
	temperatureinside.reset();
	temperatureoutside.reset();
	humidityinside.reset();
	humidityoutside.reset();
	barometer.reset();
	rain.reset();
	wind.reset();
	solar.reset();
	uv.reset();
}

// concstruction/Destruction of Station
Station::~Station(void) {
	if (NULL != channel)
		delete channel;
}

// check whether the value at the given offset is valid
bool	Station::validByte(const std::string& s, int offset) const {
	unsigned char	c = s[offset];
	return ((c != 0x7f) && (c != 0xff));
}

bool	Station::validShort(const std::string& s, int offset) const {
	unsigned char	c = s[offset + 1];
	return ((c != 0x7f) && (c != 0xff));
}

// retrieve bytes
int	Station::getSignedByte(const std::string& s, int offset) const {
	char	c = s[offset];
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "signed byte %d", c);
	return c;
}
int	Station::getUnsignedByte(const std::string& s, int offset) const {
	unsigned char	c = s[offset];
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "unsigned byte %u", c);
	return c;
}

// retrieve short
int	Station::getSignedShort(const std::string& s, int offset) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "signed short bytes: %02x%02x",
		(unsigned char)s[offset], (unsigned char)s[offset + 1]);
	unsigned short	s0, s1;
	s0 = (unsigned char)s[offset];
	s1 = (unsigned char)s[offset + 1];
	unsigned short	ss = ((s1 << 8) | s0);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "singed short retrieved: %hd",
		*(signed short *)&ss);
	return	(int)ss;
}

int	Station::getUnsignedShort(const std::string& s, int offset) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "unsigned short bytes: %02x%02x",
		(unsigned char)s[offset], (unsigned char)s[offset + 1]);
	unsigned short	s0, s1;
	s0 = (unsigned char)s[offset];
	s1 = (unsigned char)s[offset + 1];
	unsigned short	ss = ((s1 << 8) | s0);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "unsigned short retrieved: %hu", ss);
	return	(int)ss;
}

void	Station::update(const std::string& packet) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "starting record update");
	// retrieve data values, convert them to the station unit and
	// update the existing value
	r.temperatureinside.update(this->getInsideTemperature(packet));
	r.temperatureoutside.update(this->getOutsideTemperature(packet));
	r.humidityinside.update(this->getInsideHumidity(packet));
	r.humidityoutside.update(this->getOutsideHumidity(packet));
	r.barometer.update(this->getBarometer(packet));
	r.rain.update(this->getRain(packet));
	r.wind.update(this->getWind(packet));
	// maybe we don't have to do anything... for the following two
	r.solar.update(this->getSolar(packet));
	r.uv.update(this->getUV(packet));
	r.update();
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "all data values updated");
}

#define	ACK	6
void	Station::startLoop(int p) {
	// remember the number of packets to read
	packets = p;

	// wait for a an ack on the channel
	char	*expect = "\r\nOK\n\r";
	unsigned int	idx = 0;
	bool	ackfound = false;
	do {
		char	c = channel->recvChar();
		if (c < 0)
			throw MeteoException("failed to read ACK", "");
		if (c == ACK)
			ackfound = true;
		if (c == expect[idx])
			idx++;
		if (idx == strlen(expect))
			ackfound = true;
	} while (!ackfound);
}

//////////////////////////////////////////////////////////////////////
// WMII
//////////////////////////////////////////////////////////////////////
WMII::WMII(const std::string& stationname) : Station(stationname) {
	// temporarily open a channel
	Configuration	conf;
	Channel	*ch = ChannelFactory(conf).newChannel(stationname);

	// drain the channel
	ch->drain(10);

	// retrieve the rain calibration number from the station
	unsigned char	cmd[6];
	cmd[0] = 'W'; cmd[1] = 'R'; cmd[2] = 'D';
	cmd[3] = 0x44; cmd[4] = 0xd6; cmd[5] = 0xd;
	std::string	cmdstr((char *)cmd, 6);

	// send the command through the cannel
	ch->sendString(cmdstr);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "sent WRD command");
	std::string	reply = ch->recvString(3);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "got %d bytes reply to WRD",
		reply.length());

	// wait for an ACK reply
	if (ACK != reply[0]) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "cannot read RainCAL number");
		delete ch;
		throw MeteoException("cannot read RainCAL number", "");
	}

	// read two bytes from the channel and store as an unsigned int
	raincal = getUnsignedShort(reply, 1);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "got raincal %d", raincal);

	// retrieve the pressure calibration number from the station
	cmd[0] = 'W'; cmd[1] = 'R'; cmd[2] = 'D';
	cmd[3] = 0x44; cmd[4] = 0x2c; cmd[5] = 0xd;
	std::string cmdstr1((char *)cmd, 6);

	// send the command through the channel
	ch->sendString(cmdstr1);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "send WRD command to retrieve "
		"pressure cal number");

	reply = ch->recvString(3);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "got %d bytes reply to WRD",
		reply.length());

	// wait for an ACK reply
	if (ACK != reply[0]) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "cannot read PressureCAL "
			"number");
		delete ch;
		throw MeteoException("cannot read PressureCal number", "");
	}

	// read two bytes from the channel and store as a signed int
	pressurecal = (short)getSignedShort(reply, 1);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "got pressurecal %d", pressurecal);

	delete ch;
}
WMII::~WMII(void) { }
void	WMII::startLoop(int p) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "requesting %d packets from WMII", p);
	unsigned short	n = 65536 - p;
	unsigned char	b1 = n & 0xff;
	unsigned char	b2 = (n & 0xff00) >> 8;
	unsigned char	el = 0xd;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "sending command LOOP %02x %02x \\r",
		b1, b2);

	std::string	cmd = std::string("LOOP")
				+ std::string((char *)&b1, 1)
				+ std::string((char *)&b2, 1)
				+ std::string((char *)&el, 1);
	
	// format the command as a hex string for easier debugging
	if (debug) {
		std::string	scmd;
		char		b[8];
		for (unsigned int i = 0; i < cmd.size(); i++) {
			snprintf(b, sizeof(b), "%02x ", (unsigned char)cmd[i]);
			scmd += b;
		}
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "WMII LOOP command; %s",
			scmd.c_str());
	}
	getChannel()->sendString(cmd);
	Station::startLoop(p);
}
std::string	WMII::readPacket(void) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
		"waiting for WMII packet (18 bytes)");
	std::string	packet = getChannel()->recvString(18);
	// verify the header
	if (packet[0] != 0x01) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"WMII packet does not start with 0x01");
		throw MeteoException("WMII packet doesn't start wih 0x01", "");
	}
	// verify the check sum
	crc_t	crc = (unsigned char)packet[17] |
		((unsigned char)packet[16] << 8);
	if (0 != crc_check(&crc, (unsigned char *)packet.substr(1, 15).c_str(),
		15)) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "wrong CRC on WMII packet");
		throw MeteoException("wrong CRC on WMII packet", "");
	}

	// return the packet
	return packet;
}
TemperatureValue	WMII::getInsideTemperature(const std::string& s) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "get inside temperature");
	if (validShort(s, 1))
		return TemperatureValue(getSignedShort(s, 1)/10., "F");
	else
		return TemperatureValue("F");
}
TemperatureValue	WMII::getOutsideTemperature(const std::string& s) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "get outside temperature");
	if (validShort(s, 3))
		return TemperatureValue(getSignedShort(s, 3)/10., "F");
	else
		return TemperatureValue("F");
}
HumidityValue	WMII::getInsideHumidity(const std::string& s) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "get inside humidity");
	if (validByte(s, 10) && ((unsigned char)s[10] != 0x80))

		return HumidityValue(getUnsignedByte(s, 10), "%");
	else
		return HumidityValue();
}
HumidityValue	WMII::getOutsideHumidity(const std::string& s) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "get outside humidity");
	if (validByte(s, 11) && ((unsigned char)s[11] != 0x80))
		return HumidityValue(getUnsignedByte(s, 11), "%");
	else
		return HumidityValue();
}
PressureValue	WMII::getBarometer(const std::string& s) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "get pressure");
	if (validShort(s, 8))
		return PressureValue((getUnsignedShort(s, 8) - pressurecal)/1000., "inHg");
	else
		return PressureValue("inHg");
}
SolarValue	WMII::getSolar(const std::string& s) const {
	return SolarValue("W/m2");
}
UVValue	WMII::getUV(const std::string& s) const {
	return UVValue("index");
}
Wind	WMII::getWind(const std::string& s) const {
	if (validShort(s, 6) && validByte(s, 5)) {
		double	speed = getUnsignedByte(s, 5);
		double	azideg = getUnsignedShort(s, 6);
		double	azi = 3.1415926535 * azideg/180.;
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "wind azi %.f, speed %.1f",
			azideg, speed);
		return Wind(Vector(sin(azi) * speed, cos(azi) * speed), "mph");
	} else
		return Wind("mph");
}
Rain	WMII::getRain(const std::string& s) const {
	if (validShort(s, 12)) {
		Rain	rr(((double)getUnsignedShort(s, 12))/raincal, "in");
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "WMII rain value: %.2f",
			rr.getValue());
		return rr;
	} else
		return Rain("in");
}


//////////////////////////////////////////////////////////////////////
// VantagePro
//////////////////////////////////////////////////////////////////////
VantagePro::~VantagePro(void) { }

void	VantagePro::startLoop(int p) {
	// before every loop iteration, the vantage pro needs to be
	// waked up. This is necessary because the Vantage Pro goes
	// to sleep after it has executed a LOOP command.
	// Send \r bytes, read \n\r
	std::string	response;
	int	loops = 20;
	do {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"start Vantage Pro wake up sequence");
		getChannel()->sendChar('\r');
		response = getChannel()->recvString(2);
		if (response != "\n\r") {
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"problem in wake up sequence, trying drain");
			getChannel()->drain(10);
		}
	} while ((response != "\n\r") && (loops-- > 0));

	// send a loop command
	unsigned char	el = 0xd;
	if (p > 9)
		p = 9;
	unsigned char	b1 = '0' + p;

	std::string	cmd = std::string("LOOP1")
				+ std::string((char *)&b1, 1)
				+ std::string((char *)&el, 1);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "sending LOOP command: %s",
		cmd.c_str());
	getChannel()->sendString(cmd);

	// the startLoop command also reads the ACK from the station
	Station::startLoop(p);
}

std::string	VantagePro::readPacket(void) {
	// read 99 characters from the station (length of a Vantage Pro 
	// LOOP packet)
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
		"waiting for Vantage Pro packet (99 bytes)");
	std::string	packet = getChannel()->recvString(99);
	// verify the header
	if (packet.substr(0, 3) != "LOO") {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "packet is not a LOOP packet");
		throw MeteoException("not a loop packet", packet.substr(0, 3));
	}
	// verify the check sum
	crc_t	crc = (unsigned char)packet[98] |
		((unsigned char)packet[97] << 8);
	if (0 != crc_check(&crc, (unsigned char *)packet.substr(0, 97).c_str(),
		97)) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "wrong CRC on Vantage packet");
		throw MeteoException("wrong CRC on Vantage packet", "");
	}
	// return the packet
	return packet;
}
TemperatureValue VantagePro::getInsideTemperature(const std::string& s) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "get inside temperature");
	if (validShort(s, 9))
		return TemperatureValue(getSignedShort(s, 9)/10., "F");
	else
		return TemperatureValue("F");
}
TemperatureValue VantagePro::getOutsideTemperature(const std::string& s) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "get outside temperature");
	if (validShort(s, 12))
		return TemperatureValue(getSignedShort(s, 12)/10., "F");
	else
		return TemperatureValue("F");
}
HumidityValue	VantagePro::getInsideHumidity(const std::string& s) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "get inside humitiy");
	if (validByte(s, 11) && ((unsigned char)s[11] != 0x80))
		return HumidityValue(getUnsignedByte(s, 11), "%");
	else
		return HumidityValue("%");
}
HumidityValue	VantagePro::getOutsideHumidity(const std::string& s) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "get outside humitiy");
	if (validByte(s, 33))
		return HumidityValue(getUnsignedByte(s, 33), "%");
	else
		return HumidityValue("%");
}
PressureValue	VantagePro::getBarometer(const std::string& s) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "get pressure");
	if (validShort(s, 7))
		return PressureValue(getUnsignedShort(s, 7)/1000., "inHg");
	else
		return PressureValue("inHg");
}
SolarValue	VantagePro::getSolar(const std::string& s) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "get solar radiation");
	if (validShort(s, 44))
		return SolarValue(getUnsignedShort(s, 44), "W/m2");
	else
		return SolarValue("W/m2");
}
UVValue	VantagePro::getUV(const std::string& s) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "get uv radiation");
	if (validByte(s, 43))
		return UVValue(getUnsignedByte(s, 43)/10., "index");
	else
		return UVValue("index");
	
}
Wind	VantagePro::getWind(const std::string& s) const {
	if (validByte(s, 14) && validShort(s, 16)) {
		double	speed = getUnsignedByte(s, 14);
		double	azideg = getUnsignedShort(s, 16);
		double	azi = 3.1415926535 * azideg/180.;
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "wind azi %.f, speed %.1f",
			azideg, speed);
		return Wind(Vector(sin(azi) * speed, cos(azi) * speed), "mph");
	} else
		return Wind("mph");
}
Rain	VantagePro::getRain(const std::string& s) const {
	if (validShort(s, 50))
		return Rain(getUnsignedShort(s, 50)/100., "in");
	else
		return Rain("in");
}


Station	*StationFactory::newStation(const std::string& name) const {
	Station	*result;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "createing new station named %s",
		name.c_str());
	// find out what type of station this is
	std::string	stationxpath = "/meteo/station[@name='"
				+ name + "']/type";
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "looking for station %s via xpath %s",
		name.c_str(), stationxpath.c_str());
	std::string	stationtype = conf.getString(stationxpath, "undefined");
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "station type: %s",
		stationtype.c_str());
	if (stationtype == "WMII") {
		result = new WMII(name);
	} else if (stationtype == "VantagePro") {
		result = new VantagePro(name);
	} else {
		throw MeteoException("unknown station type", stationtype);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "station %s created", name.c_str());

	// retrieve the unit settings from the configuration file
	std::string	station = "/meteo/station[@name='" + name + "']/unit/";
	result->setTemperatureUnit(conf.getString(station + "temperature", "C"));
	result->setHumidityUnit(conf.getString(station + "humidity", "%"));
	result->setPressureUnit(conf.getString(station + "pressure", "hPa"));
	result->setRainUnit(conf.getString(station + "rain", "mm"));
	result->setWindUnit(conf.getString(station + "wind", "m/s"));
	result->setSolarUnit(conf.getString(station + "solar", "W/m2"));
	result->setUVUnit(conf.getString(station + "uv", "index"));

	// open a channel to the station
	Channel	*channel = ChannelFactory(conf).newChannel(name);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creating new channel to %s",
		name.c_str());
	result->setChannel(channel);

	// return the complete station
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creating of new station %s complete",
		name.c_str());
	return result;
}


} /* namespace meteo */
