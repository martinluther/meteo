/* 
 * RainRate.h -- encapsulate how rain rate is measured
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 *
 * $Id: RainRate.h,v 1.3 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _RainRate_h
#define _RainRate_h

#include <string>
#include <Timeval.h>
#include <mdebug.h>
#include <BasicValue.h>

namespace meteo {

class	RainRate : public BasicValue {
public:
	RainRate(void);
	RainRate(double v);
	RainRate(double v, const std::string& u);
	RainRate(const std::string& u);
	~RainRate(void);

	std::string	getClass(void) const { return "RainRate"; }

	// get string form
	virtual std::string	getValueString(void) const;

	// unit accessor
	virtual void	setUnit(const std::string& u);

	friend class RainRateConverter;
};

} /* namespace meteo */

#endif /* _RainRate_h */
