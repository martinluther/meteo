/*
 * Datarecord.cc -- weather station data abstraction
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <Datarecord.h>
#include <MeteoException.h>
#include <mdebug.h>
#include <time.h>
#include <Timelabel.h>

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
	timekey = time(NULL);
	stationname = "";
}

std::string	Datarecord::updatequery() {
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
	timekey = current;
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

std::string	Datarecord::xml(void) const {
	std::string	result;
	if (temperatureoutside.hasValue())
		result += "<temperatureoutside unit=\""
				+ temperatureoutside.getUnit() + "\">"
				+ temperatureoutside.getValueString()
			+ "</temperatureoutside>\n";
	if (temperatureinside.hasValue())
		result += "<temperatureinside unit=\""
				+ temperatureinside.getUnit() + "\""
				+ temperatureinside.getValueString()
			+ "</temperatureinside>\n";
	if (barometer.hasValue())
		result += "<barometer unit=\"" + barometer.getUnit() + "\">"
				+ barometer.getValueString()
			+ "</barometer>\n";
	if (humidityinside.hasValue())
		result += "<humidityinside unit=\""
				+ humidityinside.getUnit() + "\">"
				+ humidityinside.getValueString()
			+ "</humidityinside>\n";
	if (humidityoutside.hasValue())
		result += "<humidityoutside unit=\""
				+ humidityoutside.getUnit() + "\">"
				+ humidityoutside.getValueString()
			+ "</humidityoutside>\n";
	if (solar.hasValue())
		result += "<solar unit=\"" + solar.getUnit() + "\">"
				+ solar.getValueString()
			+ "</solar>\n";
	if (uv.hasValue())
		result += "<uv unit=\"" + uv.getUnit() + "\">"
				+ uv.getValueString()
			+ "</uv>\n";
	// XXX we should add interval info to wind data, otherwise the max
	//     does not really make sense
	if (wind.hasValue()) {
		std::string	windresult;
		windresult += "<speed>" + wind.getSpeedString() + "</speed>\n";
		windresult += "<azi>" + wind.getAziString() + "</azi>\n";
		windresult += "<max>" + wind.getSpeedString() + "</max>\n";
		windresult += "<x>" + wind.getXString() + "</x>\n";
		windresult += "<y>" + wind.getYString() + "</y>\n";
		result += "<wind unit=\"" + wind.getUnit() + "\">\n"
				+ windresult
			+ "</wind>\n";
	}

	// XXX we should really add info about the interval to rain because
	//     the total rain does not make sense (similar to max above)
	if (rain.hasValue())
		result += "<rain unit=\"" + rain.getUnit() + "\">\n"
				+ "<current>"
					+ rain.getValueString()
				+ "</current>\n"
				+ "<total>"
					+ rain.getTotalString()
				+ "</total>\n"
			+ "</rain>\n";

	// format the timekey
	char	buffer[12];
	snprintf(buffer, sizeof(buffer), "%d", timekey);

	return std::string("<meteodata time=\"") + buffer + "\" timelabel=\"" +
		Timelabel(timekey, Level(day)).getString()
		+ "\" station=\"" + stationname + "\">" + result
		+ "</meteodata>\n";
}

std::string	Datarecord::plain(void) const {
	std::string	result;
	if (temperatureoutside.hasValue())
		result += "Temperature          = "
				+ temperatureoutside.getValueString()
				+ " "
				+ temperatureoutside.getUnit()
				+ "\n";
	if (temperatureinside.hasValue())
		result += "Temperature inside   = "
				+ temperatureinside.getValueString()
				+ " "
				+ temperatureinside.getUnit()
				+ "\n";
	if (barometer.hasValue())
		result += "Pressure             = "
				+ barometer.getValueString()
				+ " "
				+ barometer.getUnit()
				+ "\n";
	if (humidityoutside.hasValue())
		result += "Humidity             = "
				+ humidityoutside.getValueString()
				+ " "
				+ humidityoutside.getUnit()
				+ "\n";
	if (humidityinside.hasValue())
		result += "Humidity inside      = "
				+ humidityinside.getValueString()
				+ " "
				+ humidityinside.getUnit()
				+ "\n";
	if (solar.hasValue())
		result += "Temperature          = "
				+ solar.getValueString()
				+ " "
				+ solar.getUnit()
				+ "\n";
	if (uv.hasValue())
		result += "Temperature          = "
				+ uv.getValueString()
				+ " "
				+ uv.getUnit()
				+ "\n";
	// XXX we should add interval info to wind data, otherwise the max
	//     does not really make sense
	if (wind.hasValue()) {
		result += "Wind speed           = "
				+ wind.getSpeedString()
				+ " "
				+ wind.getUnit()
				+ "\n";
		result += "Wind azimut          = "
				+ wind.getAziString()
				+ "\n";
		result += "Wind maximum         = "
				+ wind.getSpeedString()
				+ " "
				+ wind.getUnit()
				+ "\n";
	}

	// XXX we should really add info about the interval to rain because
	//     the total rain does not make sense (similar to max above)
	if (rain.hasValue())
		result += "Rain                 = "
				+ rain.getValueString()
				+ " "
				+ rain.getUnit()
				+ "\n";

	return result;
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
	timekey = time(NULL);
}

} /* namespace meteo */
