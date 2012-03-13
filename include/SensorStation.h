/*
 * SensorStation.h -- Stations are sets of SensorStations, which have their own
 *                 station name and id
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: SensorStation.h,v 1.4 2004/02/25 23:52:35 afm Exp $
 */
#ifndef _SensorStation_h
#define _SensorStation_h

#include <string>
#include <DataRecorder.h>

namespace meteo {

class	Station;

class SensorStation {
	Station	*parentstation;
	std::string	name;
	int		sensorid;
	DataRecorder	r;
public:
	SensorStation(const std::string& sensorname, Station *parent);
	~SensorStation(void);

	stringlist	updatequery(time_t timekey) const;

	void	update(const std::string& packet);
	void	reset(void);
};

} /* namespace meteo */

#endif /* _SensorStation_h */
