/*
 * SensorStation.h -- Stations are sets of SensorStations, which have their own
 *                 station name and id
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: SensorStation.h,v 1.5 2006/05/07 19:47:22 afm Exp $
 */
#ifndef _SensorStation_h
#define _SensorStation_h

#include <string>
#include <DataRecorder.h>
#include <Mapfile.h>

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

	void	update(const std::string& packet, Mapfile *m);
	void	reset(void);
};

} /* namespace meteo */

#endif /* _SensorStation_h */
