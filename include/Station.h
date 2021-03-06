/*
 * Station.h -- encode Station information
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Station.h,v 1.20 2008/09/07 15:18:52 afm Exp $
 */
#ifndef _Station_h
#define _Station_h

#include <Channel.h>
#include <Vector.h>
#include <list>
#include <string>
#include <map>
#include <Rain.h>
#include <Wind.h>
#include <DataRecorder.h>
#include <PacketReader.h>
#include <SensorStation.h>
#include <ReaderInfo.h>
#include <Mapfile.h>
#include <Outlet.h>

namespace meteo {

typedef	std::map<std::string, PacketReader>	readermap_t;
typedef	std::map<std::string, SensorStation>	sensormap_t;

class Station {
	Channel		*channel;
	std::string	name;	// station name
	int		stationid;
	int		offset;
	int		packets;
	sensormap_t	sensors;
	Mapfile		*mapfile;
	std::list<Outlet *>	outlets;
protected:
	// the readers map contains all the readers, and allows the station
	// to read data symbolically. Since real stations may redefine the
	// the protocol, the derived classes need access to the readers
	readermap_t	readers;
	void	addReader(const std::string& readername,
			const std::string& readerclass, int byteoffset,
			int length, const std::string& unit);
	int	addAllReaders(const stationreaders_t *sr);
	bool	hasReader(const std::string& readername) const;
	void	calibrateReader(const std::string& readername,
			const Calibrator& cal);

public:
	// construction, also retrieves the stationid
	Station(const std::string& n);
	virtual	~Station(void);

	// some accessors
	const std::string&	getName(void) const { return name; }
	int	getId(void) const { return stationid; }
	const int	getOffset(void) { return offset; }

	// add a mapfile (takes ownership of the mapfile)
	void	addMapfile(Mapfile *_mapfile) {
		mapfile = _mapfile;
	}

	// read values from packets
	Value	readValue(const std::string& readername,
		const std::string& packet) const;

	// managing outlets
	void	addOutlet(Outlet *outlet) {
		outlets.push_back(outlet);
	}
	void	sendOutlets(time_t timekey);
	void	sendOutlet(Outlet *outlet, time_t timekey) const;

	// accumulate data
	void	update(const std::string &packet);
	void	reset(void);

	// the station reads a packet from the channel	
	Channel	*getChannel(void) { return channel; }
	void	setChannel(Channel *ch) { channel = ch; }
	std::string	getPacket(void) {
		packets--; return this->readPacket();
	}
	virtual std::string	readPacket(void) = 0;

	// read data value by name
	double	read(const std::string& packet, const std::string& name) const;
	bool	valid(const std::string& packet, const std::string& name) const;

	// controlling the packet loop
	virtual void	startLoop(int p);
	bool	expectMorePackets(void) { return (packets > 0); }
	int	getPacketCount(void) { return packets; }

	// the station factory may change private things
	friend class StationFactory;
};

} /* namespace meteo */

#endif /* _Station_h */
