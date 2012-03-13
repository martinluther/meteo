/*
 * MeteoValue.h -- base class for meteorological values retrieved from
 *                 a weather station, allows for updates and knows about
 *                 about maximum and minimum
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _MeteoValue_h
#define _MeteoValue_h

#include <Timeval.h>
#include <string>


namespace meteo {

class Converter;

class	MeteoValue {
	Timeval		start;
	Timeval		lastupdate;
	std::string	unit;
	bool	hasvalue;

protected:
	// current/average value
	double	value;

	// maximum value
	double	max;

	// minimum value
	double	min;

public:
	// construction and destruction
	MeteoValue(void);
	virtual ~MeteoValue(void);

	// update the meteovalue with a new value
	void	reset(void);
	virtual void	update(double v);

	// retrieve the values
	double	getValue(void) const;
	double	getMax(void) const;
	double	getMin(void) const;
	void	setValue(double v);

	// retrieve a std::string form (suitable for SQL queries)
	std::string	getValueString(void) const;
	std::string	getMinString(void) const;
	std::string	getMaxString(void) const;

	// other accessors
	const std::string&	getUnit(void) const { return unit; }
	void	setUnit(const std::string& u) { unit = u; }

	// check for availability of a value
	bool	hasValue(void) const { return hasvalue; }
	friend class	Converter;
};

class	TemperatureValue : public MeteoValue {
public:
	TemperatureValue(void) { }
	TemperatureValue(double);
	TemperatureValue(double, const std::string&);
	TemperatureValue(const std::string&);
	virtual ~TemperatureValue(void) { }
	void	update(const TemperatureValue&);
};

class	HumidityValue : public MeteoValue {
public:
	HumidityValue(void) { }
	HumidityValue(double);
	HumidityValue(double, const std::string&);
	HumidityValue(const std::string&);
	virtual	~HumidityValue(void) { }
	void	update(const HumidityValue&);
	TemperatureValue	Dewpoint(const TemperatureValue&);
};

class	PressureValue : public MeteoValue {
public:
	PressureValue(void) { }
	PressureValue(double);
	PressureValue(double, const std::string&);
	PressureValue(const std::string&);
	virtual	~PressureValue(void) { }
	void	update(const PressureValue&);
};

class	SolarValue : public MeteoValue {
public:
	SolarValue(void) { }
	SolarValue(double);
	SolarValue(double, const std::string&);
	SolarValue(const std::string&);
	virtual	~SolarValue(void) { }
	void	update(const SolarValue&);
};

class	UVValue : public MeteoValue {
public:
	UVValue(void) { }
	UVValue(double);
	UVValue(double, const std::string&);
	UVValue(const std::string&);
	virtual	~UVValue(void) { }
	void	update(const UVValue&);
};

} // namespace meteo

#endif /* _MeteoValue_h */
