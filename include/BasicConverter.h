/*
 * BasicConverter.h -- base class for the converters that convert values
 *                     in a basic class
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: BasicConverter.h,v 1.4 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _BasicConverter_h
#define _BasicConverter_h

#include <string>
#include <BasicValue.h>

namespace meteo {

class BasicConverter {
	std::string	targetunit;
	BasicConverter(const BasicConverter&);
	BasicConverter&	operator=(const BasicConverter&);
public:
	BasicConverter(const std::string& unit);
	virtual ~BasicConverter(void);
	const std::string&	getUnit(void) const { return targetunit; }
	virtual double	operator()(const double v,
				const std::string& sourceunit) const;
	void	convert(BasicValue *b) const;
};

} /* namespace meteo */

#endif /* _BasicConverter_h */
