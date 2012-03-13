/*
 * Query.h -- perform database queries against the averages table
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Query_h
#define _Query_h

#include <string>
#include <Configuration.h>
#include <MeteoValue.h>
#include <vector>
#include <Tdata.h>

namespace meteo {

typedef	std::map<std::string, std::string>	smap_t;

class	Query {
	smap_t	select;
	time_t	start, end;
	int	interval;
public:
	Query(void) { }
	Query(int i, time_t s, time_t e) {
		interval = i; start = s - s%i; end = e - e%i;
	}
	~Query(void) { }

	// accessors
	void	setInterval(int i) { interval = i; }
	int	getInterval(void) const { return interval; }
	void	setStart(time_t s) { start = s; }
	time_t	getStart(void) const { return start; }
	void	setEnd(time_t e) { end = e; }
	time_t	getEnd(void) const { return end; }
	void	addSelect(const std::string& n, const std::string& s) {
		select[n] = s;
	}
	int	getSelectCount(void) const { return select.size(); }

	// more complex accessors 
	std::string	getSelectClause(void) const;
	std::string	getWhereClause(void) const;
	std::string	getQuery(void) const;

	// the QueryProcessor is a friend class so that it can access
	// the internal fields
	friend class QueryProcessor_internal;
};

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

// the QueryProcessor_internal class hides the fact that a MySQL database
// is used, to simplify porting to some other database (code that depends
// on QueryProcessor does not depend on the type of database)
class	QueryProcessor_internal;

class 	QueryProcessor {
	std::string		stationname;
	const Configuration&	configuration;
	QueryProcessor_internal	*internal;
public:
	QueryProcessor(const Configuration& conf, const std::string& name);
	~QueryProcessor(void) { }

	void	perform(const Query& query, QueryResult& result);
};

} /* namespace meteo */

#endif /* _Query_h */
