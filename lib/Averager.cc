/*
 * Averager.cc -- compute averages for a given station
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Averager.cc,v 1.16 2009/01/10 19:00:23 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */
#include <Averager.h>
#include <StationInfo.h>
#include <SensorStationInfo.h>
#include <QueryProcessor.h>
#include <Field.h>
#include <MeteoException.h>
#include <mdebug.h>

namespace meteo {

// report on the interval for which data is taken to compute averages and the
// timekey under which it is to be added
static void	reportinterval(const std::string& tag, int interval,
			time_t from, time_t to, time_t timekey, int line) {
	if (0 == debug) return;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
		"[%d] %s %d-averages at %s(%ld): data from %s(%ld) to %s(%ld)",
		line, tag.c_str(), interval,
		std::string(ctime(&timekey)).substr(4, 15).c_str(), timekey,
		(from > 0) ? std::string(ctime(&from)).substr(4, 15).c_str()
			: "(unknown)", from,
		(to > 0) ? std::string(ctime(&to)).substr(4, 15).c_str()
			: "(unknown)", to);
}

// construct an Averager object
Averager::Averager(const std::string& s) : queryproc(true, true), station(s) {
	fake = false;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "writable QueryProcessor created");
	offset = StationInfo(station).getOffset();
	mdebug(LOG_ERR, MDEBUG_LOG, 0, "offset for %s: %d", s.c_str(),
		offset);
}

// affected rows reporting function
static void	reportrows(const char *file, const int line, const int rows) {
	if (rows)
		mdebug(LOG_DEBUG, file, line, 0, "affected rows: %d", rows);
	else
		mdebug(LOG_DEBUG, file, line, 0, "no rows added");
}
#define	REPORT(rows)	reportrows(__FILE__, __LINE__, rows)

// averaging takes all the elements that belong the same averageing group
// together and averages then. The averaging group is defined as
//
//      (unixtime + offset)/interval
//
// The group is identified by a unixtime value divisible by the interval
// that lies in the interval. If the offset is nonzero, there is only one
// unixtime value in the interval. timekey - offset is the unix time of the
// begining of the sampling interval, thus its group number is
//
//	(timekey - offset + offset) / interval = timekey / interval
//
class groupinfo {
	time_t	timekey;
	time_t	from;	// time value
	time_t	to;	// time value
	int	group;
	int	interval;
	int	offset;
public:
	groupinfo(time_t tk, int i, int o) {
		interval = i;
		offset = o;
		timekey = tk - tk % interval;
		from = timekey - offset;
		to = from + interval;
		reportinterval("groupinfo", interval, from, to, timekey,
			__LINE__);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "timekey -> group%d: %d",
			interval, timekey);
	}
	time_t	getTimekey(void) const { return timekey; }
	time_t	getFrom(void) const { return from; }
	time_t	getFromKey(void) const { return from + offset; }
	time_t	getTo(void) const { return to; }
	time_t	getToKey(void) const { return to + offset; }
	int	getInterval(void) const { return interval; }
};

// add a simple value
int	Averager::addSimpleValue(const time_t timekey, const int interval,
		const int sensorid, const int targetid, const double value) {
	int	rows = 0;

	// create the insert query
	char	query[1024];
	snprintf(query, sizeof(query),
		"insert into avg(timekey, intval, sensorid, fieldid, value) "
		"values (%ld, %d, %d, %d, %.5f)",
		timekey, interval, sensorid, targetid, value);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "simple query [id = (%d,%d)]: %s",
		sensorid, targetid, query);

	// send the query off to the database
	if (!fake)
		rows = queryproc.perform(query);
	REPORT(rows);
	return rows;
}

// addGeneric	add computed values to the database
int	Averager::addGeneric(const time_t timekey, const int interval,
		const int sensorid, const int targetid, const int baseid,
		const std::string& op) {
	int	rows = 0;

	// some variables need for the queries
	groupinfo	gi(timekey, interval, offset);

	// create the insert query
	char	query[1024];
	snprintf(query, sizeof(query),
		"insert into avg(timekey, intval, sensorid, fieldid, value) "
		"select %ld, %d, %d, %d, %s(value) "
		"from sdata b "
		"where b.timekey >= %ld and b.timekey <= %ld "
		"  and b.sensorid = %d and b.fieldid = %d "
		"group by 1, 2, 3, 4",
		gi.getTimekey(), gi.getInterval(), sensorid,
			targetid, op.c_str(),
		gi.getFrom(), gi.getTo(),
		sensorid, baseid);
	reportinterval("addGeneric", interval, gi.getFrom(), gi.getTo(),
		gi.getTimekey(), __LINE__);

	// log the insert query
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "query for %s is %s",
		op.c_str(), query);

	// get a query processor to send the query
	if (!fake)
		rows = queryproc.perform(query);
	REPORT(rows);
	return rows;
}

// addSum	add sum (as computed by the sum method of the database)
//		to the avg table
int	Averager::addSum(const time_t timekey, const int interval,
		const int sensorid, const int baseid) {
	return addGeneric(timekey, interval, sensorid, baseid, baseid, "sum");
}

// addMax	add maxima (as computed by the max method of the database)
//		to the avg table
int	Averager::addMax(const time_t timekey, const int interval,
		const int sensorid, const int baseid) {
	return addGeneric(timekey, interval, sensorid, baseid + 2, baseid,
		"max");
}

// addMin	add minima (as computed by the min method of the database)
//		to the avg table
int	Averager::addMin(const time_t timekey, const int interval,
		const int sensorid, const int baseid) {
	return addGeneric(timekey, interval, sensorid, baseid + 1, baseid,
		"min");
}

// addAvg	add averages (as computed by the avg method of the database)
//		to the avg table
int	Averager::addAvg(const time_t timekey, const int interval,
		const int sensorid, const int baseid) {
	return addGeneric(timekey, interval, sensorid, baseid, baseid,
		"avg");
}

// addWindVector	add a single wind vector to the avg table
int	Averager::addWindVector(const time_t timekey, const int interval,
		const int sensorid, const Vector& v) {
	int	rows = 0;

	// get ids for wind direction and speed components
	int	windspeedid = Field().getId("wind");
	int	winddirid = Field().getId("winddir");

	// add wind azimut and speed
	rows += addSimpleValue(timekey, interval, sensorid, winddirid,
			v.getAzideg());
	rows += addSimpleValue(timekey, interval, sensorid, windspeedid,
			v.getAbs());
	
	// return the number of rows changed
	return rows;
}

// addWind	add wind averages and maxima
int	Averager::addWind(const time_t timekey, const int interval,
		const int sensorid) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "add wind from sensor %d", sensorid);
	int	rows = 0;
	// some variables need for the queries
	groupinfo	gi(timekey, interval, offset);

	// get the ids for windx and windy
	int	windxid = Field().getId("windx");
	int	windyid = Field().getId("windy");

	// note: the following has as a consequence that duration and
	// samples must be associated with the wind sensor station, this
	// is also what the meteodbmigration tool assumes
	int	durationid = Field().getId("duration");
	int	samplesid = Field().getId("samples");

	// build a query to compute the wind averages, and also sample number,
	// while we are at it
	char	query[2048];
	snprintf(query, sizeof(query),
		"select sum(c.value) , sum(d.value), "	/* windx, windy */
		"       sum(b.value), sum(e.value) " 	/* duration,  samples */
		"from sdata b, sdata c, sdata d, sdata e "
		"where b.timekey = c.timekey "
		"  and c.timekey = d.timekey "
		"  and d.timekey = e.timekey "
		"  and b.sensorid = c.sensorid "
		"  and c.sensorid = d.sensorid "
		"  and d.sensorid = e.sensorid "
		"  and b.fieldid = %d " /* durationid */
		"  and c.fieldid = %d "	/* windxid */
		"  and d.fieldid = %d " /* windyid */
		"  and e.fieldid = %d " /* samplesid */
		"  and b.sensorid = %d " /* sensorid */
		"  and b.timekey >= %ld and b.timekey <= %ld",
		durationid, windxid, windyid, samplesid,
		sensorid, 
		gi.getFrom(), gi.getTo());
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "wind averaging query is %s", query);

	// send the query to the database, and receive the result
	BasicQueryResult	bqr = queryproc(query);

	// go through the query result to compute averages for this station
	BasicQueryResult::const_iterator	i;
	for (i = bqr.begin(); i != bqr.end(); i++) {
		double	duration = atof((*i)[2].c_str());
		double	samples = atof((*i)[3].c_str());
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "duration: %f", duration);
		if (duration > 0) {
			// compute the wind vector
			double	x, y;
			x = atof((*i)[0].c_str())/duration;
			y = atof((*i)[1].c_str())/duration;
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"wind components: %f, %f", x, y);
			Vector	v(x, y);

			// add the vector
			reportinterval("wind Vector", interval, 
				gi.getFrom(), gi.getTo(), timekey, __LINE__);
			rows += addWindVector(timekey, interval, sensorid, v);

			// add duration and samples
			rows += addSimpleValue(timekey, interval, sensorid,
				durationid, duration);
			rows += addSimpleValue(timekey, interval, sensorid,
				samplesid, samples);
		}
	}

	// return the number of rows added
	return rows;
}

// add the averages for a given sensorstation and field
int	Averager::add(const time_t timekey, const int interval,
		const int sensorid, const int targetid, const int baseid,
		const std::string& op) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "add averages of %d = %s(%d)",
		targetid, op.c_str(), baseid);

	// handle the special case of wind
	if (op == "wind") {
		//  handle wind
		return addWind(timekey, interval, sensorid);
	}

	// handle all the other fields in a generic way
	return addGeneric(timekey, interval, sensorid, targetid, baseid, op);
}

// remove existing averages
int	Averager::remove(const time_t timekey, const int interval,
		const int sensorid) {
	int	deletedrows = 0;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
		"removing old avg at %ld, interval %d for sensor %d",
		timekey, interval, sensorid);
	char	 query[1024];
	snprintf(query, sizeof(query),
		"delete from avg "
		"where timekey = %ld "
		"  and intval = %d "
		"  and sensorid = %d",
		timekey, interval, sensorid);
	if (!fake) {
		deletedrows = queryproc.perform(query);
		REPORT(-deletedrows);
	}
	return -deletedrows;
}

// add all averages for a given sensor station
int	Averager::add(const time_t timekey, const int interval,
		const std::string& sensor, const bool replace) {
	int	a = 0;
	// get all sensors for this sensor
	SensorStationInfo	si(station, sensor);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "looking for target names");
	stringlist	targetnames = si.getAverages();
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%s has %d fields", sensor.c_str(),
		targetnames.size());

	// find the sensor id
	int	sensorid = si.getId();

	// if we are replacing, we should first clean out the averages table
	// for this sensorid
	if (replace) {
		remove(timekey, interval, sensorid);
	}

	// for each field name, retrieve the field id (we already know
	// the sensor id)
	stringlist::iterator	i;
	for (i = targetnames.begin(); i != targetnames.end(); i++) {
		// report on the name of the field we are about to add
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "adding field name %s",
			i->c_str());
		int	targetid = Field().getId(*i);

		// now find out the operator and the base
		std::string	base = si.getBase(*i);
		std::string	op = si.getOperator(*i);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "computing %s(%s)",
			op.c_str(), base.c_str());

		// now add the data for each of the target fields
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"adding data for %s.%s based on %s(%s)",
			sensor.c_str(), i->c_str(), op.c_str(), base.c_str());
		int	baseid = Field().getId(base);
		a += add(timekey, interval, sensorid, targetid, baseid, op);
	}
	return a;
}

// add	add all averages this station needs, i.e. all averages for sensors
//	of the station
int	Averager::add(const time_t timekey, const int interval,
		const bool replace) {
	int	a = 0;
	StationInfo	si(station);

	// find out whether this timekey really is a legal timekey
	if (0 != (timekey % interval)) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%d not a valid timekey, "
			"skipping avg computation for intval %d", timekey,
			interval);
		return a;
	}

	// find out about the fields we should process, and what we should
	// do about them
	stringlist	sensors = si.getSensornames();
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "found %d sensors for station %s",
		sensors.size(), station.c_str());

	// go through the sensors and add all averages for the sensor
	stringlist::iterator	i;
	for (i = sensors.begin(); i != sensors.end(); i++) {
		a += add(timekey, interval, *i, replace);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "added %d averages", a);
	return a;
}

bool	Averager::have(const time_t timekey, const int interval,
		const int sensorid) {
	// send a query to the database to count the number of rows in avg for
	// the given station, interval and timekey
	char	query[1024];
	snprintf(query, sizeof(query),
		"select count(*) "
		"from avg a "
		"where a.timekey = %ld "
		"  and a.sensorid = %d "
		"  and a.intval = %d", timekey, sensorid, interval);
	BasicQueryResult	qr = queryproc(query);
	if (qr.size() == 0) {
		return false;
	}
	return (0 < atoi((*qr.begin())[0].c_str()));
}

bool	Averager::have(const time_t timekey, const int interval) {
	// send a query to the database to count the number of rows in avg for
	// the given station, interval and timekey
	char	query[1024];
	snprintf(query, sizeof(query),
		"select count(*) "
		"from avg a, sensor b, station c "
		"where a.timekey = %ld "
		"  and a.sensorid = b.id "
		"  and b.stationid = c.id "
		"  and c.name = '%s' "
		"  and intval = %d", timekey, station.c_str(), interval);
	BasicQueryResult	qr = queryproc(query);
	if (qr.size() == 0)
		return false;
	return (0 < atoi((*qr.begin())[0].c_str()));
}

} /* namespace meteo */
