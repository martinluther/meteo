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
#include <BasicValue.h>

namespace meteo {

class	Rain : public BasicValue {
public:
	Rain(void);
	Rain(double v);
	Rain(double v, const std::string& u);
	Rain(const std::string& u);
	~Rain(void);

	std::string	getClass(void) const { return "Rain"; }

	// get string form
	virtual std::string	getValueString(void) const;

	// unit accessor
	virtual void	setUnit(const std::string& u);

	friend class RainConverter;
};

} /* namespace meteo */

#endif /* _Rain_h */
