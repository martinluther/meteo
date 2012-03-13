/*
 * DataRecorder.h -- accumulator class that handles all the Recorders of
 *                   a station
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: DataRecorder.h,v 1.4 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _DataRecorder_h
#define _DataRecorder_h

#include <MeteoTypes.h>
#include <map>
#include <Recorder.h>

namespace meteo {

typedef std::map<std::string, Recorder>	recordermap_t;

class DataRecorder {
	// for each field name,
	recordermap_t	recorders;
public:
	DataRecorder() { }
	~DataRecorder() { }

	// add recorders
	void	addRecorder(const std::string& sensorname);

	// update recorders by name
	void	update(const std::string& sensorname, const Value& newvalue);

	// access to recorders
	recordermap_t::iterator	begin(void) { return recorders.begin(); }
	recordermap_t::iterator	end(void) { return recorders.end(); }

	// retrieve update queries for all recorders
	stringlist	updatequery(const time_t timekey,
		const int sensorid) const;

	// reset the recorder
	void	reset(void);
};

} /* namespace meteo */

#endif /* _DataRecorder_h */
