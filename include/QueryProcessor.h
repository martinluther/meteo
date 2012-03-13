/*
 * QueryProcessor.h -- class to database queries against the averages table
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _QueryProcessor_h
#define _QueryProcessor_h

#include <string>
#include <list>
#include <Tdata.h>
#include <Datarecord.h>
#include <Query.h>
#include <QueryResult.h>
#include <BasicQueryResult.h>

namespace meteo {

// the QueryProcessor_internal class hides the fact that a MySQL database
// is used, to simplify porting to some other database (code that depends
// on QueryProcessor does not depend on the type of database)
class	QueryProcessor_internal;

// the QueryProcessor either executes a Query-object and returns a QueryResult
// or it can be used to retrieve a certain DataRecord directly
class 	QueryProcessor {
	QueryProcessor_internal	*internal;
	bool	ignoreerrors;
public:
	QueryProcessor(bool readwrite, bool ie = false);
	~QueryProcessor(void);

	void	perform(const Query& query, QueryResult& result);
	int	perform(const std::string& query);// only returns affected rows
	int	perform(const char *query); // only returns affected rows
	Datarecord	nearRecord(const time_t timekey,
			const std::string& stationname, bool backwards = true,
			int window = 3600);
	Datarecord	lastRecord(const time_t notafter,
			const std::string& stationname, int window = 3600) {
		return nearRecord(notafter, stationname, true, window);
	}
	Datarecord	firstRecord(const time_t notbefore,
			const std::string& stationname, int window = 3600) {
		return nearRecord(notbefore, stationname, false, window);
	}
	BasicQueryResult	operator()(const std::string& query);
};

} /* namespace meteo */

#endif /* _QueryProcessor_h */
