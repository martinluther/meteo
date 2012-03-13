/*
 * Field_internals.h -- internal stuff for field info access
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung 
 *
 * $Id: Field_internals.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _Field_internals_h
#define _Field_internals_h

#include <Field.h>
#include <QueryProcessor.h>
#include <MeteoException.h>
#include <map>

namespace meteo {

// internally, we create a cache of the most often used info about mfields
struct mfield_s {
	int		id;
	std::string	name;
	std::string	classname;
	std::string	unit;
	bool		hasmax;
	bool		hasmin;
	mfield_s(void) { hasmax = false; hasmin = false; }
};
typedef struct mfield_s mfield_t;

// map types:	from fieldname to fieldid (which contains mfieldid)
//		from mfieldid to mfield
typedef	std::map<std::string, int>	fieldidmap_t;
typedef	std::map<int, mfield_t>		mfieldmap_t;

class Field_internals {
	QueryProcessor	qp;
	fieldidmap_t	fieldidmap;
	mfieldmap_t	mfieldmap;
	// the lookup functions maintain the cache, and return the id of
	// the field being looked up
	int	lookup(const int mfieldid);
	int	lookup(const std::string& fieldname);
public:
	Field_internals(void);
	~Field_internals(void);
	Field_internals&	operator=(const Field_internals&);

	// two basic lookup functions: based on id and name
	mfield_t	getField(const int mfieldid);
	mfield_t	getField(const std::string& fieldname);

	// retrieve id and name, which are the most common
	int	getId(const std::string& fieldname);
	std::string	getName(const int mfieldid);
};

} /* namespace meteo */

#endif /* _Field_internals_h */
