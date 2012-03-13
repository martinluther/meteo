/*
 * QueryResult.cc -- perform database queries against 
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: QueryResult.cc,v 1.3 2004/02/25 23:48:05 afm Exp $
 */
#include <QueryResult.h>
#include <mdebug.h>
#include <MeteoException.h>
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif

namespace meteo {

bool	QueryResult::existsItem(const std::string& dataname, time_t timekey) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "existence query for data %s at %d",
		dataname.c_str(), timekey);
	dmap_t::const_iterator	i = data.find(dataname);
	if (i == data.end())
		return false;
	const tdata_t&	td = i->second.getData();
	return (td.find(timekey) != td.end());
}

double	QueryResult::fetchItem(const std::string& dataname, time_t timekey) const {
	dmap_t::const_iterator	i = data.find(dataname);
	if (i == data.end()) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0,
			"data name %s does not exist", dataname.c_str());
		throw MeteoException("data item does not exist", "");
	}
	const tdata_t&	td = i->second.getData();
	tdata_t::const_iterator	j = td.find(timekey);
	if (j == td.end()) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0,
			"data item %s at %d does not exist", dataname.c_str(),
				timekey);
		throw MeteoException("data item does not exist", "");
	}
	return j->second;
}

} /* namespace meteo */
