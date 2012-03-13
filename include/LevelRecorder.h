/*
 * LevelRecorder.h -- base class for meteorological values retrieved from
 *                    a weather station, allows for updates and knows about
 *                    about maximum and minimum
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _LevelRecorder_h
#define _LevelRecorder_h

#include <MinmaxRecorder.h>
#include <string>

namespace meteo {

class	LevelRecorder : public MinmaxRecorder {
public:
	LevelRecorder(void);
	LevelRecorder(double);
	LevelRecorder(double, const std::string&);
	LevelRecorder(const std::string&);
	virtual	~LevelRecorder(void) { }
	virtual std::string	getValueClass(void) const {
		return "LevelValue";
	}
};

} // namespace meteo

#endif /* _LevelRecorder_h */
