/*
 * Converter.cc -- class to convert values from one system to another
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <Converter.h>
#include <MeteoException.h>
#include <mdebug.h>

namespace meteo {

void	Converter::convertAll(const MeteoValue *from, MeteoValue *to) const {
	const std::string	target = from->getUnit();
	to->value = this->convert(from->value, target);
	to->min = this->convert(from->min, target);
	to->max = this->convert(from->max, target);
}

double	TemperatureConverter::convert(const double& value, const std::string& from) const {
	if (from == getUnit())
		return value;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "converting %.2f from %s to %s",
		value, from.c_str(), getUnit().c_str());

	// convert to intermediate value
	double	intermediate;
	if ((from == "degrees Fahrenheit") || (from == "degF")
		|| (from == "F")) {
		intermediate = 273.15 + (5. * (value - 32.) / 9.);
	} else if ((from == "degrees Celsius") || (from == "degC")
		|| (from == "C")) {
		intermediate = value + 273.15;
	} else if ((from == "Kelvin") || (from == "K")) {
		intermediate = value;
	} else {
		throw MeteoException("unknown temperature unit `from'", from);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "absolute temperature %.1f",
		intermediate);

	// convert to target value
	const std::string&	to = getUnit();
	if ((to == "degrees Fahrenheit") || (to == "degF") || (to == "F")) {
		return 9. * (intermediate - 273.15) / 5. + 32.;
	} else if ((to == "degrees Celsius") || (to == "degC") || (to == "C")) {
		return intermediate - 273.15;
	} else if ((from == "Kelvin") || (from == "K")) {
		return intermediate;
	} else {
		throw MeteoException("unknown temperature unit `to'", to);
	}
}
TemperatureValue	TemperatureConverter::operator()(const TemperatureValue& t) const {
	TemperatureValue	result = t;
	if (t.hasValue())
		convertAll(&t, &result);
	else
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "no temp to convert");
	return result;
}

double	HumidityConverter::convert(const double& value, const std::string& from) const {
	if (from == getUnit())
		return value;

	if (from != "%")
		throw MeteoException("unknown Humidity unit `from'", from);
	if (getUnit() != "%")
		throw MeteoException("unknown Humidity unit `to'", getUnit());
	return value;
}
HumidityValue	HumidityConverter::operator()(const HumidityValue& t) const {
	HumidityValue	result = t;
	if (t.hasValue())
		convertAll(&t, &result);
	return result;
}

double	PressureConverter::convert(const double& value, const std::string& from) const {
	if (from == getUnit())
		return value;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "converting %.2f from %s to %s",
		value, from.c_str(), getUnit().c_str());

	// convert to intermediate value
	double	intermediate;
	if (from == "hPa") {
		intermediate = value;
	} else if (from == "mmHg") {
		intermediate = 1.333224 * value;
	} else if (from == "inHg") {
		intermediate = 33.8639 * value;
	} else {
		throw MeteoException("unknown Pressure unit `from'", from);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "hPa pressure: %.2f", intermediate);

	// convert intermediate value to target unit
	const std::string&	to = getUnit();
	if (to == "hPa") {
		return intermediate;
	} else if (to == "mmHg") {
		return intermediate / 1.333224;
	} else if (to == "inHg") {
		return intermediate * 0.03970079 / 1.333224;
	} else {
		throw MeteoException("unknown Pressure unit `to'", to);
	}
}
PressureValue	PressureConverter::operator()(const PressureValue& t) const {
	PressureValue	result = t;
	if (t.hasValue())
		convertAll(&t, &result);
	return result;
}


double	SolarConverter::convert(const double& value, const std::string& from) const {
	if (from == getUnit())
		return value;

	if (from != "W/m2")
		throw MeteoException("unknown solar radiation unit `from'", from);
	if (getUnit() != "W/m2")
		throw MeteoException("unknown solar radiation unit `to'", getUnit());
	return value;
}
SolarValue	SolarConverter::operator()(const SolarValue& t) const {
	SolarValue	result = t;
	if (t.hasValue())
		convertAll(&t, &result);
	return result;
}


double	UVConverter::convert(const double& value, const std::string& from) const {
	if (from == getUnit())
		return value;

	if (from != "index")
		throw MeteoException("unknown uv unit `from'", from);
	if (getUnit() != "index")
		throw MeteoException("unknown uv unit `to'", getUnit());
	return value;
}
UVValue	UVConverter::operator()(const UVValue& t) const {
	UVValue	result = t;
	if (t.hasValue())
		convertAll(&t, &result);
	return result;
}

double	RainConverter::convert(const double& value, const std::string& from) const {
	if (from == getUnit())
		return value;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "converting %.2f from %s to %s",
		value, from.c_str(), getUnit().c_str());

	// convert to intermediate value (in mm)
	double	intermediate;
	if (from == "in") {
		intermediate = value * 25.4;
	} else if (from == "mm") {
		intermediate = value;
	} else {
		throw MeteoException("unknown rain unit `from'", from);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "mm rain %.1f", intermediate);

	// convert intermediate value to target unit
	const std::string&	to = getUnit();
	if (to == "in") {
		return intermediate / 25.4;
	} else if (to == "mm") {
		return intermediate;
	} else {
		throw MeteoException("unknown rain unit `to'", getUnit());
	}
}
Rain	RainConverter::operator()(const Rain& r) const {
	Rain	result = r;
	if (r.hasValue())
		result.value = convert(r.value, r.unit);
	return result;
}

#define	MILE	1.609344
double	WindConverter::convert(const double& value, const std::string& from) const {
	if (from == getUnit())
		return value;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "convert %.2f from %s to %s",
		value, from.c_str(), getUnit().c_str());

	// convert to intermediate value in meters per second
	double	intermediate;
	if (from == "m/s") {
		intermediate = value;
	} else if (from == "mph") {
		intermediate = (value * MILE) / 3.6;
	} else if (from == "km/h") {
		intermediate = value / 3.6;
	} else {
		throw MeteoException("unknown wind speed unit `from'", from);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "wind speed m/s: %.1f", intermediate);

	// convert intermediate value to target unit
	const std::string&	to = getUnit();
	if (to == "m/s") {
		return intermediate;
	} else if (to == "mph") {
		return 3.6 * intermediate / MILE;
	} else if (to == "km/h") {
		return 3.6 * intermediate;
	} else {
		throw MeteoException("unknown wind speed unit `to'", to);
	}
	return value;
}
Wind	WindConverter::operator()(const Wind& w) const {
	Wind	result = w;
	result.unit = getUnit();
	if (w.hasValue()) {
		result.v = Vector(convert(w.v.getX(), w.getUnit()),
			convert(w.v.getY(), w.getUnit()));
		result.max = Vector(convert(w.max.getX(), w.getUnit()),
			convert(w.max.getY(), w.getUnit()));
	}
	return result;
}

} /* namespace meteo */
