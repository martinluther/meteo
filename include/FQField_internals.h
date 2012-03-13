/*
 * FQField_internals.h -- oracle that gives us information about fields
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: FQField_internals.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _FQField_internals_h
#define _FQField_internals_h

#include <FQField.h>
#include <QueryProcessor.h>
#include <string>
#include <map>

namespace meteo {

// map types:	from fieldname to fieldid (which contains mfieldid)
//		from mfieldid to fieldname, more details about a field
//		are retrieved using the Field classes
typedef	std::map<std::string, fieldid>	string2field_t;
typedef	std::map<fieldid, fqfieldname>	field2fqfield_t;
typedef std::map<fieldid, int>		field2station_t;

class FQField_internals {
	QueryProcessor	qp;
	field2fqfield_t	field2fqmap;
	string2field_t	string2fieldmap;
	field2station_t	stationmap;
	// cache management functions
	fieldid	lookup(const std::string& fieldname);
	fieldid	lookup(const fieldid mfieldid);
public:
	FQField_internals(void);
	~FQField_internals(void);
	FQField_internals&	operator=(const FQField_internals&);

	// lookup based on name
	fieldid	getFieldid(const std::string& fieldname);
	fqfieldname	split(const std::string& fieldname) const;

	// lookup based on (partial) fieldid
	fieldid		getFieldid(int sensorid, int mfieldid);
	fqfieldname	getFqfieldname(const fieldid& fid);
};

} /* namespace meteo */

#endif /* _FQField_internals_h */
