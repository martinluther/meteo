/*
 * LeafwetnessValue.h -- Leaf wetness abstraction
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: LeafwetnessValue.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _LeafwetnessValue_h
#define _LeafwetnessValue_h

#include <BasicValue.h>
#include <Timeval.h>
#include <string>

namespace meteo {

class	LeafwetnessValue : public BasicValue {
public:
	LeafwetnessValue(void);
	LeafwetnessValue(double);
	LeafwetnessValue(double, const std::string&);
	LeafwetnessValue(const std::string&);
	virtual	~LeafwetnessValue(void) { }
	virtual void	setUnit(const std::string& unit);
	virtual std::string	getClass(void) const {
		return "LeafwetnessValue";
	}
};

} // namespace meteo

#endif /* _LeafwetnessValue_h */
