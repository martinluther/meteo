/*
 * Averager.h -- compute averages for a given station
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Averager.h,v 1.7 2004/02/26 23:43:12 afm Exp $
 */
#ifndef _Averager_h
#define _Averager_h

#include <string>
#include <list>
#include <Vector.h>
#include <QueryProcessor.h>
#include <Field.h>

namespace meteo {

class Averager {
	// fields 
	bool		fake;
	QueryProcessor	queryproc;
	std::string	station;
	int		offset;

	// remove existing averages for a timekey, interval and sensor id
	int	remove(const time_t timekey, const int interval,
		const int sensorid);

	// basic add function, useful for individual values
	int	addSimpleValue(const time_t timekey, const int interval,
			const int sensorid, const int mfieldid,
			const double value);

	// add averages for a set of fields and given operation
	int	addGeneric(const time_t timekey, const int interval,
			const int sensorid, const int targetid,
			const int baseid, const std::string& op);
	int	addSum(const time_t timekey, const int interval,
			const int sensorid, const int baseid);
	int	addMax(const time_t timekey, const int interval,
			const int sensorid, const int baseid);
	int	addMin(const time_t timekey, const int interval,
			const int sensorid, const int baseid);
	int	addAvg(const time_t timekey, const int interval,
			const int sensorid, const int baseid);

	// add wind for a given sensor
	int	addWindVector(const time_t timekey, const int interval,
			const int sensorid, const Vector& v);
	int	addWind(const time_t timekey, const int inteval,
			const int sensorid);

public:
	Averager(const std::string& s);
	~Averager(void) { }

	// add averages to a single sensor on a station
	int	add(const time_t timekey, const int interval,
			const int sensorid, const int targetid,
			const int baseid, const std::string& op);
	int	add(const time_t timekey, const int interval,
			const std::string& sensor, const bool replace);
	bool	have(const time_t timekey, const int interval,
			const int sensorid);

	// add averages for all the sensors on a station
	int	add(const time_t timekey, const int interval,
			const bool replace);
	bool	have(const time_t timekey, const int interval);

	bool	getFake(void) const { return fake; }
	void	setFake(const bool f) { fake = f; }

	// some convenience accessors
	int	getOffset(void) const { return offset; }
};

} /* namespace meteo */

#endif /* _Averager_h */
