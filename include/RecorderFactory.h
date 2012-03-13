/*
 * RecorderFactory.h -- create a Recorder based on the value class it should
 *                      server
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _RecorderFactory_h
#define _RecorderFactory_h

#include <string.h>
#include <Recorder.h>

namespace meteo {

class RecorderFactory {
public:
	static Recorder	getRecorder(const std::string& sensorname);
};

} /* namespace meteo */

#endif /* _RecorderFactory_h */
