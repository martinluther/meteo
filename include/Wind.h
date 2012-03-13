/*
 * Wind.h -- wind class
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Wind_h
#define _Wind_h

#include <Vector.h>
#include <Timeval.h>
#include <string>

namespace meteo {

class	Wind {
	Timeval		start;
	Timeval		lastupdate;
	std::string	unit;
	bool		hasvalue;

	// current/average value
	Vector		v;

	// maximum value
	Vector		max;

public:
	// construction and destruction
	Wind(void) {
		start.now();
		hasvalue = false;
	}
	Wind(const Vector& v0) {
		start.now();
		hasvalue = true;
		v = v0;
		max = v0;
		lastupdate.now();
		unit = "mps";
	}
	Wind(const Vector& v0, const std::string& u) {
		start.now();
		hasvalue = true;
		v = v0;
		max = v0;
		lastupdate.now();
		unit = u;
	}
	Wind(const std::string& u) {
		start.now();
		hasvalue = false;
		unit = u;
	}
	~Wind(void) { }

	// updaters
	void	reset(void);
	void	update(const Vector& v);
	void	update(const Wind& w);

	// accessors
	const Vector&	getValue(void) const { return v; }
	double		getMax(void) const { return max.getAbs(); }
	double		getMaxAzimut(void) const { return max.getArg(); }
	const std::string&	getUnit(void) const { return unit; }
	void		setUnit(const std::string& u);
	bool		hasValue(void) const { return hasvalue; }
	double		getDuration(void) const;
	void		setValue(const Vector& vv) { v = vv; hasvalue = true; }
	void		setMax(const Vector& vv) { max = vv; }

	// string representation
	std::string	getSpeedString(void) const;
	std::string	getXString(void) const;
	std::string	getYString(void) const;
	std::string	getAziString(void) const;
	std::string	getMaxString(void) const;
	std::string	getDurationString(void) const;

	// friends
	friend class	WindConverter;
};

} // namespace meteo

#endif /* _Wind_h */
