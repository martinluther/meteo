/*
 * ChunkDumper.h -- dump meteo data in chunks, and in various formats, so
 *                  that the contention on the tables remains small
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _ChunkDumper_h
#define _ChunkDumper_h

#include <QueryProcessor.h>
#include <MeteoTypes.h>
#include <string>
#include <fstream>

namespace meteo {

class	ChunkDumper {
	QueryProcessor	qp;
	std::ofstream	avgfile;
	std::ofstream	sdatafile;
	std::ofstream	headerfile;
	int	avg, sdata, header;
	bool	doavg, dosdata, doheader;
	bool	sql;
	int	chunksize;
	void	checkFile(const std::ofstream& file, const char *name);
public:
	ChunkDumper(const std::string prefix, int size);
	~ChunkDumper(void);

	void	enableAvg(void) { doavg = true; }
	void	enableHeader(void) { doheader = true; }
	void	enableSdata(void) { dosdata = true; }
	void	disableAvg(void) { doavg = false; }
	void	disableHeader(void) { doheader = false; }
	void	disableSdata(void) { dosdata = false; }
	void	enableRaw(void) { sql = false; }
	void	enableSql(void) { sql = true; }

	std::string	out(const std::string& leadin,
		std::vector<std::string>& row) const;
	int	dump(const int timekey, const int sensorid);
	void	dumpStation(const std::string& stationname);
	void	dumpStations(const stringlist& stations);
};

} /* namespace meteo */

#endif /* _ChunkDumper_h */
