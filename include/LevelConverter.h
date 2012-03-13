/*
 * LevelConverter.h -- convert level values
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _LevelConverter_h
#define _LevelConverter_h

#include <string>
#include <BasicConverter.h>
#include <LevelValue.h>

namespace meteo {

class	LevelConverter : public BasicConverter {
public:
	LevelConverter(void) : BasicConverter("m") { }
	LevelConverter(const std::string& t) : BasicConverter(t) { }
	virtual ~LevelConverter(void) { }
	virtual double	operator()(const double value, const std::string& from) const;
};

} /* namespace meteo */

#endif /* _LevelConverter_h */
