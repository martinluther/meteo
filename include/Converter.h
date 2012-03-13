/*
 * Converter.h -- converter classes that allow to convert Meteo values
 *                between differen units
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Converter_h
#define _Converter_h

#include <string>
#include <MeteoValue.h>
#include <Rain.h>
#include <Wind.h>

namespace meteo {

// virtual base class for converters, just to make it possible to
// write generic methods for converters
class	Converter {
	std::string	unit;
public:
	Converter(void) { }
	Converter(const std::string& t) : unit(t) { }
	virtual ~Converter(void) { }
	virtual double	convert(const double& value, const std::string& from) const = 0;
	void	setUnit(const std::string& t) { unit = t; }
	const std::string&	getUnit(void) const { return unit; }
protected:
	void	convertAll(const MeteoValue *from, MeteoValue *to) const;
};

class	TemperatureConverter : public Converter {
public:
	TemperatureConverter(void) { setUnit("C"); }
	TemperatureConverter(const std::string& t) : Converter(t) { }
	virtual ~TemperatureConverter(void) { }
	virtual double	convert(const double& value, const std::string& from) const;
	TemperatureValue	operator()(const TemperatureValue&) const;
};

class	HumidityConverter : public Converter {
public:
	HumidityConverter(void) { setUnit("%"); }
	HumidityConverter(const std::string& t) : Converter(t) { }
	virtual ~HumidityConverter(void) { }
	virtual double	convert(const double& value, const std::string& from) const;
	HumidityValue	operator()(const HumidityValue&) const;
};

class	PressureConverter : public Converter {
public:
	PressureConverter(void) { setUnit("hPa"); }
	PressureConverter(const std::string& t) : Converter(t) { }
	virtual ~PressureConverter(void) { }
	virtual double	convert(const double& value, const std::string& from) const;
	PressureValue	operator()(const PressureValue&) const;
};

class	SolarConverter : public Converter {
public:
	SolarConverter(void) { setUnit("W/m2"); }
	SolarConverter(const std::string& t) : Converter(t) { }
	virtual ~SolarConverter(void) { }
	virtual double	convert(const double& value, const std::string& from) const;
	SolarValue	operator()(const SolarValue&) const;
};

class	UVConverter : public Converter {
public:
	UVConverter(void) { setUnit("index"); }
	UVConverter(const std::string& t) : Converter(t) { }
	virtual ~UVConverter(void) { }
	virtual double	convert(const double& value, const std::string& from) const;
	UVValue	operator()(const UVValue&) const;
};

class	RainConverter : public Converter {
public:
	RainConverter(void) { setUnit("mm"); }
	RainConverter(const std::string& t) : Converter(t) { }
	virtual ~RainConverter(void) { }
	virtual double	convert(const double& value, const std::string& from) const;
	Rain	operator()(const Rain&) const;
};

class	WindConverter : public Converter {
public:
	WindConverter(void) { setUnit("m/s"); }
	WindConverter(const std::string& t) : Converter(t) { }
	virtual ~WindConverter(void) { }
	virtual double	convert(const double& value, const std::string& from) const;
	Wind	operator()(const Wind&) const;
};

} /* namespace meteo */

#endif /* _Converter_h */
