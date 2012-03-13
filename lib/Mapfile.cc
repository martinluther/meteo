//
// Mapfile.cc -- class used to send current station data to a map file
//
// (c) 2004 Dr. Andreas Mueller, Beratung und Entwicklung
// $Id: Mapfile.cc,v 1.3 2006/05/16 11:19:54 afm Exp $
//
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <Mapfile.h>
#include <unistd.h>
#include <fcntl.h>
#include <mdebug.h>
#include <MeteoException.h>
#include <sys/types.h>
#include <errno.h>
#include <Format.h>
#include <Timestamp.h>
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif /* HAVE_ALLOCA_H */

namespace meteo {

// this should be large enough for all currently known davis stations
int	Mapfile::mapsize = 2048;

Mapfile::Mapfile(const std::string& filename, bool writable) {
	int	flags = O_RDONLY;
	if (writable) {
		flags = O_CREAT | O_TRUNC | O_RDWR;
	}
	// open and initialize the map file 
	mapfile = open(filename.c_str(), flags, 0666);
	if (mapfile < 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "cannot open map file %s: %s",
			filename.c_str(), strerror(errno));
		throw MeteoException("cannot open map file", strerror(errno));
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "map file %s created",
		filename.c_str());
}

// publish the data in the scratch area
void	Mapfile::publish(void) {
	Timestamp	t;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "publishing meteo data");
	std::string	result("<?xml version=\"1.0\"?>\n<meteomap station=\"");
	stringprintf(result, "<meteomap station=\"%s\" timestamp=\"%s\"",
		stationname.c_str(), t.ctime().c_str());
	result.append(t.strftime(" day=\"%d\" month=\"%m\" year=\"%Y\" hour=\"%H\" minute=\"%M\" second=\"%S\""));
	result.append(">\n");

	// write the data in the data to the scratch area
	mapdata_t::iterator	i;
	for (i = data.begin(); i != data.end(); i++) {
		char	b[1024];
		snprintf(b, sizeof(b),
			"<data name=\"%s\" value=\"%.3f\" unit=\"%s\"/>\n",
			i->first.c_str(), i->second.first,
			i->second.second.c_str());
		result.append(b);
	}
	result.append("</meteomap>\n");

	// rewind to the beginning of the map
	if (lseek(mapfile, 0, SEEK_SET) < 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "cannot rewind map: %s",
			strerror(errno));
		throw MeteoException("cannot rewind the map file",
			strerror(errno));
	}

	// fill a buffer with the data from the string plus spaces
	char	*buffer = (char *)alloca(mapsize);
	memset(buffer, ' ', mapsize);
	buffer[mapsize - 1] = '\n';
	memcpy(buffer, result.data(), result.length());

	// perform an atomic write to the mapfile
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "writing %d bytes to mapfile", mapsize);
	if (mapsize != write(mapfile, buffer, mapsize)) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "cannot write map: %s",
			strerror(errno));
		throw MeteoException("cannot write map", strerror(errno));
	}
}

// read the data from the map file
std::string	Mapfile::read(void) {
	char	*buffer = (char *)alloca(mapsize);
	int	bytes = ::read(mapfile, buffer, mapsize);
	if (mapsize != bytes) {
		// this really shouldn't happen
		mdebug(LOG_WARNING, MDEBUG_LOG, 0,
			"mapfile incomlete only %d of %d bytes",
			bytes, mapsize);
	}
	return std::string(buffer, mapsize);
}

// add a named value to the mapfile
void	Mapfile::add(const std::string& name, double value,
	const std::string& unit) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "Mapfile::add(%s -> %.f%s)",
		name.c_str(), value, unit.c_str());
	data.insert(mapdata_t::value_type(name, mapvalue_t(value, unit)));
}

// clear the data map
void	Mapfile::clear(void) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "clearing map file");
	data.clear();	// shortcut for data.erase(data.begin(), data.end());
}

} /* namespace meteo */
