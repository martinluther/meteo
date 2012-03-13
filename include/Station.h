/*
 * Station.h -- encode Station information
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Station_h
#define _Station_h

#include <Channel.h>
#include <Vector.h>
#include <string>
#include <MeteoValue.h>
#include <Rain.h>
#include <Wind.h>
#include <Configuration.h>
#include <Datarecord.h>

namespace meteo {

class Station {
	Datarecord	r;
	Channel		*channel;
	std::string	name;
	int		packets;
public:
	// construction
	Station(const std::string& n) : name(n) {
		r.setStationname(n);
	}
	virtual	~Station(void);

	// accumulate data
	virtual void	update(const std::string &);
	void	reset(void) { r.reset(); }
	std::string	updatequery() {
		return r.updatequery();
	}

	// the station reads a packet from the channel	
	Channel	*getChannel(void) { return channel; }
	void	setChannel(Channel *ch) { channel = ch; }
	std::string	getPacket(void) { packets--;
		return this->readPacket();
	}
	virtual std::string	readPacket(void) = 0;

	// read bytes from the packet and convert to 
	bool	validByte(const std::string& s, int offset) const;
	int	getSignedByte(const std::string& s, int offset) const;
	int	getUnsignedByte(const std::string& s, int offset) const;
	bool	validShort(const std::string& s, int offset) const;
	int	getSignedShort(const std::string& s, int offset) const;
	int	getUnsignedShort(const std::string& s, int offset) const;

	// set units we want to use after it has been read from the station
	void	setTemperatureUnit(const std::string& u) {
		r.setTemperatureUnit(u);
	}
	void	setHumidityUnit(const std::string& u) {
		r.setHumidityUnit(u);
	}
	void	setPressureUnit(const std::string& u) {
		r.setPressureUnit(u);
	}
	void	setRainUnit(const std::string& u) {
		r.setRainUnit(u);
	}
	void	setWindUnit(const std::string& u) {
		r.setWindUnit(u);
	}
	void	setSolarUnit(const std::string& u) {
		r.setSolarUnit(u);
	}
	void	setUVUnit(const std::string& u) {
		r.setUVUnit(u);
	}

	// access the data in the packet
	virtual TemperatureValue	getInsideTemperature(const std::string&) const = 0;
	virtual TemperatureValue	getOutsideTemperature(const std::string&) const = 0;
	virtual HumidityValue	getInsideHumidity(const std::string&) const = 0;
	virtual HumidityValue	getOutsideHumidity(const std::string&) const = 0;
	virtual PressureValue	getBarometer(const std::string&) const = 0;
	virtual SolarValue	getSolar(const std::string&) const = 0;
	virtual UVValue		getUV(const std::string&) const = 0;
	virtual Wind		getWind(const std::string&) const = 0;
	virtual Rain		getRain(const std::string&) const = 0;

	// controlling the packet loop
	virtual void	startLoop(int p);
	bool	expectMorePackets(void) { return (packets > 0); }
	int	getPacketCount(void) { return packets; }

	// the station factory may change private things
	friend class StationFactory;
};

class	WMII : public Station {
	int	raincal;
	int	pressurecal;
public:
	WMII(const std::string& n);
	virtual	~WMII(void);

	// read a packet from the station
	virtual std::string	readPacket(void);

	// accessors
	virtual TemperatureValue	getInsideTemperature(const std::string&) const;
	virtual TemperatureValue	getOutsideTemperature(const std::string&) const;
	virtual HumidityValue	getInsideHumidity(const std::string&) const;
	virtual HumidityValue	getOutsideHumidity(const std::string&) const;
	virtual PressureValue	getBarometer(const std::string&) const;
	virtual SolarValue	getSolar(const std::string&) const;
	virtual UVValue		getUV(const std::string&) const;
	virtual Wind		getWind(const std::string&) const;
	virtual Rain		getRain(const std::string&) const;

	// loop control
	virtual void	startLoop(int p);
};

class	VantagePro : public Station {
public:
	VantagePro(const std::string& n) : Station(n) { }
	virtual	~VantagePro(void);

	// read a packet from the station
	virtual std::string	readPacket(void);

	// accessors
	virtual TemperatureValue	getInsideTemperature(const std::string&) const;
	virtual TemperatureValue	getOutsideTemperature(const std::string&) const;
	virtual HumidityValue	getInsideHumidity(const std::string&) const;
	virtual HumidityValue	getOutsideHumidity(const std::string&) const;
	virtual PressureValue	getBarometer(const std::string&) const;
	virtual SolarValue	getSolar(const std::string&) const;
	virtual UVValue		getUV(const std::string&) const;
	virtual Wind		getWind(const std::string&) const;
	virtual Rain		getRain(const std::string&) const;

	// loop control
	virtual void	startLoop(int p);
};

class	StationFactory {
	const Configuration&	conf;
public:
	StationFactory(const Configuration& c) : conf(c) { }
	Station	*newStation(const std::string& name) const;  // connected station
};

} /* namespace meteo */

#endif /* _Station_h */
