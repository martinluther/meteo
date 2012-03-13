/*
 * Field.h -- oracle that gives us information about fields
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Field_h
#define _Field_h

#include <string>
#include <map>

namespace meteo {

// forward declartion for internal class that is needed for cache implementation
class	Field_internals;

// publicly available class
class	Field {
	static Field_internals	*fi;
public:
	// constructors
	Field(void);
	~Field(void);


	// access to field only data, key fields first
	int		getId(const std::string& fieldname);
	std::string	getName(const int mfieldid);

	// secondary fields
	std::string	getClass(const std::string& fieldname);
	std::string	getClass(const int mfieldid);
	std::string	getUnit(const std::string& fieldname);
	std::string	getUnit(const int mfieldid);
	bool	hasMaximum(const std::string& fieldname);
	bool	hasMaximum(const int mfieldid);
	bool	hasMinimum(const std::string& fieldname);
	bool	hasMinimum(const int mfieldid);
	bool	isWind(const std::string& fieldname);
	bool	isWind(const int mfieldid);
	bool	isRain(const std::string& fieldname);
	bool	isRain(const int mfieldid);
};

} /* namespace meteo */

#endif /* _Field_h */
