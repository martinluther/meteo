//
// Mapfile.h -- class to send current station data to a map file for
//              more interactive interfaces
//
// (c) 2004 Dr. Andreas Mueller, Beratung und Entwicklung
// $Id: Mapfile.h,v 1.2 2006/05/07 21:48:14 afm Exp $
// 
#ifndef _Mapfile_h
#define _Mapfile_h

#include <string>
#include <map>

namespace meteo {

typedef std::pair<double, std::string>	mapvalue_t;
typedef std::map<std::string, mapvalue_t>	mapdata_t;

class	Mapfile {
	int		mapfile;
	static int	mapsize;
	mapdata_t	data;
	std::string	stationname;
public:
	Mapfile(const std::string& filename, bool writeable);
	void	setStationname(const std::string& s) { stationname = s; }
	std::string	getStationname() const { return stationname; }
	void	add(const std::string& dataname, double value,
		const std::string& unit);
	void	publish(void);
	void	clear(void);
	std::string	read(void);
};

} // namespace meteo

#endif /* _Mapfile_h */
