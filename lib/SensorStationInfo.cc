/*
 * SensorStationInfo.cc -- access to information about SensorStations
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 *
 * $Id: SensorStationInfo.cc,v 1.6 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <SensorStationInfo.h>
#include <QueryProcessor.h>
#include <Configuration.h>
#include <MeteoException.h>

namespace meteo {

class	ssi {
public:
	std::string	station;
	std::string	sensor;
	int		id;
	stringlist	fieldnames;
};

typedef	std::map<int, ssi>		ssimap_t;
typedef std::map<std::string, int>	ssid_t;

class	SSI_internals {
	QueryProcessor	qp;
	ssimap_t	idcache;
	ssid_t		namecache;
	int	lookup(const std::string& station, const std::string& name);
	int	lookup(const int sensorid);
public:
	SSI_internals(void);
	~SSI_internals(void);
	int	getSensorid(const std::string station, const std::string sensor);
	std::string	getName(int sensorid);
	std::string	getStationname(int sensorid);
	stringlist	getFieldnames(int sensorid);
};

// constructors
SSI_internals::SSI_internals(void) : qp(false) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "MM SSL_internals %p create", this);
}
SSI_internals::~SSI_internals(void) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "MM SSL_internals %p destroy", this);
}

int	SSI_internals::lookup(const std::string& station,
		const std::string& name) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "lookup(%s,%s)", station.c_str(),
		name.c_str());

	// check whether we have this name already in the cache
	std::string	qname = station + "." + name;
	if (namecache.find(qname) != namecache.end()) {
		return namecache.find(qname)->second;
	}
	
	// retrieve the id from the database
	char	query[1024];
	snprintf(query, sizeof(query),
		"select b.id "
		"from station a, sensor b "
		"where a.id = b.stationid "
		"  and a.name = '%s' "
		"  and b.name = '%s'", station.c_str(), name.c_str());
	BasicQueryResult	bqr = qp(query);
	if (bqr.size() == 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "no info for sensor %s.%s",
			station.c_str(), name.c_str());
		throw MeteoException("no info for sensor",
			station + "." + name);
	}

	// look up the rest of the info
	return lookup(atoi((*bqr.begin())[0].c_str()));
}

int	SSI_internals::lookup(const int sensorid) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "lookup(%d)", sensorid);
	// prepare a result structure
	ssi	result;
	result.id = sensorid;

	// check whether this exists
	if (idcache.find(sensorid) != idcache.end()) {
		return sensorid;
	}

	// retrieve info about sensor 
	char	query[1024];
	snprintf(query, sizeof(query),
		"select a.name, b.name "
		"from station a, sensor b "
		"where a.id = b.stationid "
		"  and b.id = %d", sensorid);
	BasicQueryResult	bqr = qp(query);
	if (bqr.size() == 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "no info for sensor %d",
			sensorid);
		throw MeteoException("no info for sensor", "");
	}

	result.station = (*bqr.begin())[0];
	result.sensor = (*bqr.begin())[1];

	// retrieve a list of mfield names that match this sensor, this
	// information is found in the XML configuration file
	std::string	xpath("/meteo/station[@name='" + result.station
		+ "']/sensors/sensor[@name='" + result.sensor + "']/field");
	result.fieldnames = Configuration().getStringList(xpath);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%d mfields in sensor %s",
		result.fieldnames.size(), result.sensor.c_str());

	// now store everything in the caches
	idcache.insert(ssimap_t::value_type(sensorid, result));
	std::string	qname = result.station + "." + result.sensor;
	namecache.insert(ssid_t::value_type(qname, sensorid));
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "name %s cached", qname.c_str());

	// return the sensor id
	return sensorid;
}


int	SSI_internals::getSensorid(const std::string station, const std::string sensor) {
	return lookup(station, sensor);
}
std::string	SSI_internals::getName(int sensorid) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "asking for name for %d", sensorid);
	lookup(sensorid);
	return idcache.find(sensorid)->second.sensor;
}
std::string	SSI_internals::getStationname(int sensorid) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "asking for stationname for %d",
		sensorid);
	lookup(sensorid);
	return idcache.find(sensorid)->second.station;
}
stringlist	SSI_internals::getFieldnames(int sensorid) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "asking for fieldnames for %d",
		sensorid);
	lookup(sensorid);
	return idcache.find(sensorid)->second.fieldnames;
}


//////////////////////////////////////////////////////////////////////
// SensorStationInfo implementation below

SSI_internals	*SensorStationInfo::ssi = NULL;


// create a SensorStation from Stationname and sensorname
SensorStationInfo::SensorStationInfo(const std::string& sn, const std::string& n)
	: stationname(sn), name(n) {
	// construct the internals
	if (ssi == NULL)
		ssi = new SSI_internals();

	// perform a database query to retrieve the sensor id
	sensorid = ssi->getSensorid(sn, n);
}

// create the SensorStation from the id, the id is a key into the sensor table
SensorStationInfo::SensorStationInfo(const int id) : sensorid(id) {
	// construct the internals
	if (ssi == NULL)
		ssi = new SSI_internals();

	// perform a database query to retrieve the sensor id
	name = ssi->getName(id);
	stationname = ssi->getStationname(id);
}

SensorStationInfo::~SensorStationInfo(void) {
}

// access unqualified field names, which is just the raw information from
// the station configuration in the XML configuration file
stringlist	SensorStationInfo::getFieldnames(void) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "ssi=%p, asking for fieldnames", ssi);
	return ssi->getFieldnames(sensorid);
}

// qualified fieldnames contain complete host/sensor/mfield information
stringlist	SensorStationInfo::getQualifiedFieldnames(void) const {
	stringlist	result, fn = getFieldnames();
	stringlist::const_iterator	i;
	for (i = fn.begin(); i != fn.end(); i++) {
		result.push_back(stationname + "." + name + "." + *i);
	}
	return result;
}

// access to averages info (from the configuration file), these are essentially
// XPath lookups
static std::string	getAveragesXpath(const std::string& stationname,
	const std::string& name) {
	return "/meteo/station[@name='" + stationname + "']/averages/"
		"sensor[@name='" + name + "']/average";
}
stringlist	SensorStationInfo::getAverages(void) const {
	return Configuration().getStringList(getAveragesXpath(stationname, name)
		+ "/@name");
}
std::string	SensorStationInfo::getBase(const std::string& average)
	const {
	return Configuration().getString(getAveragesXpath(stationname, name)
		+ "[@name='" + average + "']/@base", "");
}
std::string	SensorStationInfo::getOperator(const std::string&
	average) const {
	return Configuration().getString(getAveragesXpath(stationname, name)
		+ "[@name='" + average + "']/@operator", "");
}

} /* namespace meteo */
