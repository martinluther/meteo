/*
 * Query.h -- perform database queries against the averages table
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Query_h
#define _Query_h

#include <string>
#include <list>
#include <Tdata.h>
#include <Datarecord.h>
#include <FQField.h>

namespace meteo {

typedef	std::map<std::string, std::string>	smap_t;
typedef std::map<fieldid, std::string>		imap_t;

// The Query class encapsulates queries for a set of data fields and a given
// interval. It thus represents a vertical section through the table. This
// class is not very useful to access all data in a row, the QueryProcessor
// class is capable of retrieving entire records
class	Query {
	// the smap_t select maps symbolic names to field names in the old
	// structure of the database. In the new structure, these field
	// names no longer exist, so the query object now converts the
	// fully qualified field names to field ids and sensor ids
	smap_t	select;
	// the imap_t idmap maps field ids to the symbolic name of the field
	// in select, so that the query processor can map ids retrieved
	// from the database to the right dataset
	imap_t	idmap;
	time_t	start, end;
	int	interval;
	int	offset;
public:
	Query(void) { offset = 0; }
	Query(int i, time_t s, time_t e) {
		interval = i; start = s - s % i; end = e - e % i; offset = 0;
	}
	~Query(void) { }

	// accessors
	void	setInterval(int i) { interval = i; }
	int	getInterval(void) const { return interval; }
	void	setStart(time_t s) { start = s; }
	time_t	getStart(void) const { return start; }
	void	setEnd(time_t e) { end = e; }
	time_t	getEnd(void) const { return end; }
	void	addSelect(const std::string& n, const std::string& fqfieldname);
	int	getSelectCount(void) const { return select.size(); }
	void	setOffset(int o) { offset = o; }
	int	getOffset(void) const { return offset; }
	std::string	getNameById(const fieldid& id) const;

	// more complex accessors 
	std::string	getQuery(void) const;

	// the QueryProcessor is a friend class so that it can access
	// the internal fields
	friend class QueryProcessor_internal;
};

} /* namespace meteo */

#endif /* _Query_h */
