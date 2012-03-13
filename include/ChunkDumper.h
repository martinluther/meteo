/*
 * ChunkDumper.h -- dump meteo data in chunks, and in various formats, so
 *                  that the contention on the tables remains small
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: ChunkDumper.h,v 1.3 2004/02/25 23:52:34 afm Exp $
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
	int	avg, sdata;
	int	starttime, endtime;
	bool	doavg, dosdata;
	bool	sql;
	int	chunksize;	// time interval size for dump chunk
	void	checkFile(const std::ofstream& file, const char *name);
public:
	ChunkDumper(const std::string prefix, int size);
	~ChunkDumper(void);

	void	enableAvg(void) { doavg = true; }
	void	enableSdata(void) { dosdata = true; }
	void	disableAvg(void) { doavg = false; }
	void	disableSdata(void) { dosdata = false; }
	void	enableRaw(void) { sql = false; }
	void	enableSql(void) { sql = true; }

	void	setStarttime(int s) { starttime = s; }
	int	getStarttime(void) { return starttime; }
	void	setEndtime(int e) { endtime = e; }
	int	getEndtime(void) { return endtime; }

	std::string	out(const std::string& leadin,
		std::vector<std::string>& row) const;
	void	dump(const int sensorid);
	int	dumpAvg(const int start, const int sesorid);
	int	dumpSdata(const int start, const int sensorid);
	void	dumpStation(const std::string& stationname);
	void	dumpStations(const stringlist& stations);
};

} /* namespace meteo */

#endif /* _ChunkDumper_h */
