/*
 * BasicQueryResult.h -- result of a primitive query
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: BasicQueryResult.h,v 1.3 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _BasicQueryResult_h
#define _BasicQueryResult_h

#include <string>
#include <vector>

namespace meteo {

// we also want to use the QueryProcessor to process any kind of query that
// we like, so we need a more generic way to return the result
typedef	std::vector<std::string>	Row;
typedef	std::vector<Row>	BasicQueryResult;

} /* namespace meteo */

#endif /* _BasicQueryResult_h */
