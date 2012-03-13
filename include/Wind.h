/*
 * Wind.h -- wind class
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Wind_h
#define _Wind_h

#include <Vector.h>
#include <WindSpeed.h>
#include <Timeval.h>
#include <string>

namespace meteo {

class	Wind : public WindSpeed {
	double	azi;	// this is always in radians, so that trigonometric
			// computations are easier
public:
	// construction and destruction
	Wind(void);
	Wind(const Vector& v0);
	Wind(const Vector& v0, const std::string& u);
	Wind(const std::string& u);
	~Wind(void);

	// accessors
	virtual void	setUnit(const std::string& u);
	void		setValue(const Vector& vv);
	Vector	getVectorValue(void) const;
	std::string	getClass(void) const { return "Wind"; }

	// additional accessors
	double	getX(void) const;
	double	getY(void) const;
	double	getAzi(void) const { return azi; }
	double	getAzideg(void) const;	// degrees

	// string representation
	std::string	getSpeedString(void) const;
	std::string	getXString(void) const;
	std::string	getYString(void) const;
	std::string	getAziString(void) const; // degrees, since this is
			// only used for display purposes, or database updates
			// where we want to store degree angles

	// output
	virtual std::string	plain(const std::string& name) const;
	virtual std::string	xml(const std::string& name) const;

	// friends
	friend class	WindConverter;
};

} // namespace meteo

#endif /* _Wind_h */
