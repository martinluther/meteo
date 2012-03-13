/* 
 * Rain.h -- encapsulate how rain is measured
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 */
#ifndef _Rain_h
#define _Rain_h

#include <string>
#include <Timeval.h>

namespace meteo {

class	Rain {
	bool		hasvalue;
	std::string	unit;
	Timeval		start;
	Timeval		lastupdate;
	double		value;
	double		svalue;
public:
	Rain(void) { value = svalue = 0.; start.now(); hasvalue = false; }
	Rain(double v) {
		value = svalue = v; start.now(); hasvalue = true;
		lastupdate.now();
		unit = "mm";
	}
	Rain(double v, const std::string& u) {
		value = svalue = v; start.now(); hasvalue = true;
		lastupdate.now();
		unit = u;
	}
	Rain(const std::string& u) {
		start.now(); hasvalue = false;
		unit = u;
	}
	~Rain(void) { }

	// update
	void	reset(void);
	void	update(double rain);
	void	update(const Rain&);

	// access data
	double	getValue(void) const;
	double	getTotal(void) const;
	double	getRate(void) const;
	const std::string&	getUnit(void) { return unit; }
	void	setUnit(const std::string& u) { unit = u; }
	bool	hasValue(void) const { return hasvalue; }

	// get string form
	std::string	getValueString(void) const;
	std::string	getTotalString(void) const;

	friend class RainConverter;
};

} /* namespace meteo */

#endif /* _Rain_h */