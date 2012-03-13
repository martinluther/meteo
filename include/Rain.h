/* 
 * Rain.h -- encapsulate how rain is measured
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 */
#ifndef _Rain_h
#define _Rain_h

#include <string>
#include <Timeval.h>
#include <mdebug.h>

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
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "initialized rain from %.2fmm",
			v);
	}
	Rain(double v, const std::string& u) {
		value = svalue = v; start.now(); hasvalue = true;
		lastupdate.now();
		unit = u;
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "initialized rain from %.2f%s",
			v, u.c_str());
	}
	Rain(const std::string& u) {
		start.now(); hasvalue = false;
		unit = u;
	}
	~Rain(void) { }

	// primitive accessors
	void	setValue(double v) { value = v; svalue = 0; hasvalue = true; }
	void	setTotal(double v) {
		double d = value - svalue;
		value = v; svalue = value - d;
	}

	// update
	void	reset(void);
	void	update(double rain);
	void	update(const Rain&);

	// access data
	double	getValue(void) const;
	double	getTotal(void) const;
	double	getRate(void) const;
	const std::string&	getUnit(void) const { return unit; }
	void	setUnit(const std::string& u);
	bool	hasValue(void) const { return hasvalue; }

	// get string form
	std::string	getValueString(void) const;
	std::string	getTotalString(void) const;

	friend class RainConverter;
};

} /* namespace meteo */

#endif /* _Rain_h */
