/*
 * MoistureConverter.h -- convert soil moisture values
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _MoistureConverter_h
#define _MoistureConverter_h

#include <string>
#include <MoistureValue.h>
#include <BasicConverter.h>

namespace meteo {

class	MoistureConverter : public BasicConverter {
public:
	MoistureConverter(void) : BasicConverter("cb") { }
	MoistureConverter(const std::string& t) : BasicConverter(t) { }
	virtual ~MoistureConverter(void) { }
	virtual double	operator()(const double value, const std::string& from) const;
};

} /* namespace meteo */

#endif /* _MoistureConverter_h */
