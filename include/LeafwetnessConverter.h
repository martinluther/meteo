/*
 * LeafwetnessConverter.h -- convert leaf wetness values
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _LeafwetnessConverter_h
#define _LeafwetnessConverter_h

#include <string>
#include <LeafwetnessValue.h>
#include <BasicConverter.h>

namespace meteo {

class	LeafwetnessConverter : public BasicConverter {
public:
	LeafwetnessConverter(void) : BasicConverter("index") { }
	LeafwetnessConverter(const std::string& t) : BasicConverter(t) { }
	virtual ~LeafwetnessConverter(void) { }
	virtual double	operator()(const double value, const std::string& from) const;
};

} /* namespace meteo */

#endif /* _LeafwetnessConverter_h */
