/*
 * Datarecord.h -- encode Station information
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Datarecord_h
#define _Datarecord_h

#include <Channel.h>
#include <Vector.h>
#include <string>
#include <MeteoValue.h>
#include <Rain.h>
#include <Wind.h>
#include <Configuration.h>

namespace meteo {

class	Datarecord {
public:
	TemperatureValue	temperatureinside;
	TemperatureValue	temperatureoutside;
	TemperatureValue	soiltemperature;
	HumidityValue		humidityinside;
	HumidityValue		humidityoutside;
	PressureValue		barometer;
	Rain			rain;
	Wind			wind;
	SolarValue		solar;
	UVValue			uv;
	int			samples;
private:
	time_t			timekey;
	std::string		stationname;
public:
	// construction
	Datarecord(void);
	~Datarecord(void) { }

	// update and reset
	void	update(void) { samples++; }
	void	reset(void);

	// accessors for non meteo data
	time_t	getTimekey(void) const { return timekey; }
	const std::string&	getStationname(void) const {
		return stationname;
	}
	void	setTimekey(time_t key) { timekey = key; }
	void	setStationname(const std::string& n) { stationname = n; }

	// set temperature units
	void	setTemperatureUnit(const std::string& unit) {
		temperatureinside.setUnit(unit);
		temperatureoutside.setUnit(unit);
		soiltemperature.setUnit(unit);
	}
	void	setHumidityUnit(const std::string& unit) {
		humidityinside.setUnit(unit);
		humidityoutside.setUnit(unit);
	}
	void	setPressureUnit(const std::string& unit) {
		barometer.setUnit(unit); 
	}
	void	setRainUnit(const std::string& unit) { rain.setUnit(unit); }
	void	setWindUnit(const std::string& unit) { wind.setUnit(unit); }
	void	setSolarUnit(const std::string& unit) { solar.setUnit(unit); }
	void	setUVUnit(const std::string& unit){ uv.setUnit(unit); }

	// accessor
	std::string	updatequery(void);
	std::string	xml(void) const;
	std::string	plain(void) const;

	// show stuff on stdout
};

} /* namespace meteo */

#endif /* _Datarecord_h */
