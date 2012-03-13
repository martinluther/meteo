/*
 * Averager.h -- compute averages for a given station
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
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

	// basic add function, useful for individual values
	int	addSimpleValue(time_t timekey, int interval, int sensorid,
		int mfieldid, double value);

	// add averages for a set of fields and given operation
	int	addGeneric(const time_t timekey, const int interval,
			const int sensorid, const int mfieldid,
			const std::string& op);
	int	addSum(const time_t timekey, const int interval,
			const int sensorid, const int mfieldid);
	int	addMax(const time_t timekey, const int interval,
			const int sensorid, const int mfieldid);
	int	addMin(const time_t timekey, const int interval,
			const int sensorid, const int mfieldid);
	int	addAvg(const time_t timekey, const int interval,
			const int sensorid, const int mfieldid);

	// add wind for a given sensor
	int	addWindVector(const time_t timekey, const int interval,
			const int offset, const int sensorid,
			const Vector& v);
	int	addWind(const time_t timekey, const int inteval,
			const int offset, const int sensorid);

public:
	Averager(const std::string& s);
	~Averager(void) { }

	// add averages to a single sensor on a station
	int	add(const time_t timekey, const int interval,
			const int sensorid, const int mfieldid,
			const std::string& fieldname);
	int	add(const time_t timekey, const int interval,
			const int sensorid, const bool replace);
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
