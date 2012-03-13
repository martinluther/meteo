/*
 * UVRecorder.h -- base class for meteorological values retrieved from
 *                 a weather station, allows for updates and knows about
 *                 about maximum and minimum
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _UVRecorder_h
#define _UVRecorder_h

#include <MinmaxRecorder.h>
#include <string>

namespace meteo {

class	UVRecorder : public MinmaxRecorder {
public:
	UVRecorder(void);
	UVRecorder(double);
	UVRecorder(double, const std::string&);
	UVRecorder(const std::string&);
	virtual	~UVRecorder(void) { }
	virtual std::string	getValueClass(void) const { return "UVValue"; }
};

} // namespace meteo

#endif /* _UVRecorder_h */
