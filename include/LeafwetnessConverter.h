/*
 * LeafwetnessConverter.h -- convert leaf wetness values
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: LeafwetnessConverter.h,v 1.2 2004/02/25 23:52:34 afm Exp $
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
