/*
 * QueryResult.h -- perform database queries against the averages table
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: QueryResult.h,v 1.3 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _QueryResult_h
#define _QueryResult_h

#include <string>
#include <list>
#include <Tdata.h>
#include <Datarecord.h>

namespace meteo {

class	QueryResult {
	dmap_t	data;
public:
	QueryResult(void) { }
	~QueryResult(void) { }
	void	addTdata(const std::string& n, const Tdata& d) { data[n] = d; }
	bool	existsItem(const std::string& dataname, time_t timekey) const;
	double	fetchItem(const std::string& dataname, time_t timekey) const;
	const dmap_t&	getData(void) const { return data; }
	// allow the QueryProcessor to directly work with the data fields
	// to save copying
	friend class QueryProcessor_internal;
};

} /* namespace meteo */

#endif /* _QueryResult_h */
