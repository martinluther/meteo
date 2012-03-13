/*
 * FQField.h -- oracle that gives us information about fields
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _FQField_h
#define _FQField_h

#include <string>
#include <map>

namespace meteo {

// the fieldid is a triple consisting of the station id, sensor id and
// the mfield type id
class fieldid {
public:
	int	stationid;
	int	sensorid;
	int	mfieldid;
	fieldid(void) {
		stationid = -1; sensorid = -1; mfieldid = -1;
	}
	fieldid(int t, int s, int m) {
		stationid = t; sensorid = s; mfieldid = m;
	}
	bool	operator<(const fieldid& other) const;
};

// the fqfieldname_t encapsulates a broken down field name
class fqfieldname {
public:
	std::string	stationname;
	std::string	sensorname;
	std::string	fieldname;
	fqfieldname(void) { }
	fqfieldname(const std::string& st, const std::string se,
		const std::string fi) {
		stationname = st; sensorname = se; fieldname = fi;
	}
	std::string	getString(void) const {
		return stationname + "." + sensorname + "." + fieldname; 
	}
};

// forward declartion for internal class that is needed for cache implementation
class	FQField_internals;

// publicly available class
class	FQField {
	static FQField_internals	*fi;
public:
	// constructors
	FQField(void);
	~FQField(void);

	// main fieldid retrieveal function
	fieldid	getFieldid(const std::string& fieldname);
	fieldid	getFieldid(int sensorid, int mfieldid);
	fqfieldname	getFqfieldname(const fieldid& fid);
	fqfieldname	getFqfieldname(const std::string& fieldname);
};

} /* namespace meteo */

#endif /* _FQField_h */
