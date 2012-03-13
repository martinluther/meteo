/*
 * Station.cc -- weather station abstraction
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <Station.h>
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#include <Configuration.h>
#include <StationInfo.h>
#include <SensorStationInfo.h>
#include <MeteoException.h>
#include <PacketReaderFactory.h>
#include <QueryProcessor.h>
#include <FQField.h>
#include <Field.h>
#include <ChannelFactory.h>
#include <mdebug.h>

namespace meteo {


// construction/Destruction of Station
Station::Station(const std::string& n) : name(n) {
	// set channel to well defined state, or the destructor may erroneously
	// try to destroy a nonexisting channel, leading to a segmentation
	// fault
	channel = NULL;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creating station %s", name.c_str());

	// retrieve all the field names this station is supposed to know about
	StationInfo	si(name);
	stationid = si.getId();
	offset = si.getOffset();
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "stationid = %d, offset = %d",
		stationid, offset);

	// retrieve the sensor information
	stringlist	sensornames = si.getSensornames();
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "got %d sensor names",
		sensornames.size());

	// add all the sensor stations this station is supposed to know about
	stringlist::iterator	s;
	for (s = sensornames.begin(); s != sensornames.end(); s++) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "adding sensor %s",
			(*s).c_str());
		SensorStation	sensorstation(*s, this);
		sensors.insert(sensormap_t::value_type(*s, sensorstation));
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "sensorstation %s added",
			(*s).c_str());
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "Station::Station(%s) complete",
		name.c_str());

	// create a channel (using the ChannelFactory)
	channel = ChannelFactory().newChannel(name);
}

Station::~Station(void) {
	if (NULL != channel) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "destroying channel %p",
			channel);
		delete channel;
		channel = NULL;
	}
}

// add a reader to the reader map, the arguments have the following meaning
// fieldname	- the  name by which this reader will be referenced, of the
//		  form sensorid.mfield
// readerspec	- often a class name, tells the ReaderFactory what kind of 
//		  reader to create (some readers are differently parametrized
//		  instances of the same class)
// byteoffset	- position inside the packet where the reader should start
//		  reading
// length	- home many bytes should be read by the reader (most readers
//		  ignore this)
// unit		- unit of measurement one can find in the packet. This is
//		  needed when a Value is to be read: the fieldname specifies
//		  the Value type that should be created and the unit field
//		  interprets the value in the packet as the correct physical
//		  entity
void	Station::addReader(const std::string& fieldname,
		const std::string& readerspec, int byteoffset, int length,
		const std::string& unit) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "adding reader %s (%s)",
		fieldname.c_str(), unit.c_str());
	// first send a warning if there is reader name collision
	readermap_t::const_iterator	i = readers.find(fieldname);
	if (i != readers.end()) {
		mdebug(LOG_WARNING, MDEBUG_LOG, 0, "warning: reader %s already "
			"defined", fieldname.c_str());
	}

	// find out the value class and unit for this sensor,
	fieldid	id = FQField().getFieldid(name + "." + fieldname);
	std::string	classname = Field().getClass(id.mfieldid);

	// now add a new reader
	PacketReader	p = PacketReaderFactory::newPacketReader(readerspec,
				byteoffset, length, classname, unit);
	readers.insert(readermap_t::value_type(fieldname, p));
}

// add all the stations found in the stationspec
int	Station::addAllReaders(const stationreaders_t *stationspec) {
	// check that we got something
	if (NULL == stationspec) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "no readers specfied");
		throw MeteoException("no readers specified", "addAllReaders");
	}

	// add the readers found in the array
	int	counter = 0;
	for (const stationreaders_t *srp = stationspec; srp->readername; srp++) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "add reader (%s)%s [%s] %d-%d",
			srp->classname, srp->readername, srp->readerunit,
			srp->param1, srp->param2);
		try {
			addReader(srp->readername, srp->classname,
				srp->param1, srp->param2, srp->readerunit);
			counter++;
		} catch (MeteoException& me) {
			mdebug(LOG_INFO, MDEBUG_LOG, 0, "reader %s not added",
				srp->readername);
		}
	}

	// report the number of readers added
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%d readers added", counter);
	return counter;
}

// check for the existence of a reader
bool	Station::hasReader(const std::string& fieldname) const {
	// check whether the reader exists in the map
	readermap_t::const_iterator	i = readers.find(fieldname);
	return (i != readers.end());
}

// calibrate a reader, the fieldname here must concide with the one
// give when adding the Reader with addReader
void	Station::calibrateReader(const std::string& fieldname,
		const Calibrator& cal) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "calibrating %s with %f/%f",
		fieldname.c_str(), cal.getSlope(), cal.getY0());
	// check whether reader exists
	readermap_t::iterator	i = readers.find(fieldname);
	if (i == readers.end()) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "reader %s not found",
			fieldname.c_str());
		throw MeteoException("reader not found", fieldname);
	}

	// now calibrate the reader
	i->second.calibrate(cal);
}

// read a Value from the packet
Value	Station::readValue(const std::string& readername,
	const std::string& packet) const {
	readermap_t::const_iterator	j = readers.find(readername);
	if (j == readers.end()) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "no reader for %s",
			readername.c_str());
		throw MeteoException("no reader found", readername);
	}

	return j->second.v(packet);
}

// read data from the packet
double	Station::read(const std::string& packet,
		const std::string& fieldname) const {
	readermap_t::const_iterator	i = readers.find(fieldname);

	// find the reader associated with this name
	if (i == readers.end()) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "cannot find reader for %s",
			name.c_str());
		throw MeteoException("reader not found for name", fieldname);
	}

	// read the data using the reader
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "reading field %s from %d byte packet",
		fieldname.c_str(), packet.size());
	return i->second(packet);
}

bool	Station::valid(const std::string& packet,
		const std::string& fieldname) const {
	readermap_t::const_iterator	i = readers.find(fieldname);

	// find the reader associated with this name
	if (i == readers.end()) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "cannot find reader for %s",
			fieldname.c_str());
		throw MeteoException("reader not found for name", fieldname);
	}

	// read the data using the reader
	return i->second.valid(packet);
}

// update method
void	Station::update(const std::string& packet) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "starting record update");
	// go through the sensormap
	sensormap_t::iterator	i;
	for (i = sensors.begin(); i != sensors.end(); i++) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "updating sensor %s",
			i->first.c_str());
		// update the current SensorStation from the packet
		i->second.update(packet);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "all data values updated");
}

static bool	timekeyheader(const time_t timekey) {
	char	query[1024];
	snprintf(query, sizeof(query),
		"select count(*) from header where timekey = %ld", timekey);

	QueryProcessor  qp(false);
	BasicQueryResult	bqr = qp(query);
	return (1 == atoi((*bqr.begin())[0].c_str()));
}

// updatequery method, constructs the union of all the queries
stringlist	Station::updatequery(time_t timekey) const {
	stringlist	result;
	// add the query for the header to the result list
	if (!timekeyheader(timekey)) {
		char    query[1024];
		snprintf(query, sizeof(query),
			"insert into header(timekey, group300, group1800, "
			"	group7200, group86400) values "
			"	(%ld, %ld, %ld, %ld, %ld)",
			timekey,
			(timekey + offset)/300, (timekey + offset)/1800,
			(timekey + offset)/7200, (timekey + offset)/86400);
		result.push_back(query);
	}

	// go through the sensor map, and add every element of the sensors
	// update queries to the result
	sensormap_t::const_iterator	i;
	for (i = sensors.begin(); i != sensors.end(); i++) {
		stringlist	a = i->second.updatequery(timekey);
		// splice this query at the end of the result list
		result.splice(result.end(), a);
	}

	// return the accumulated results
	return result;
}


// reset all recorders
void	Station::reset(void) {
	sensormap_t::iterator	i;
	for (i = sensors.begin(); i != sensors.end(); i++) {
		i->second.reset();
	}
}

void	Station::startLoop(int p) {
	// remember the number of packets to read
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "starting loop for %d packets", p);
	packets = p;
}

} /* namespace meteo */
