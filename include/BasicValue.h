 /*
 * BasicValue.h -- base class for all the value classes. The class knows about
 *                 units (specified in string form) and updating. We use this
 *                 as a common base class so we can create a simpler interface
 *                 for the station classes
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _BasicValue_h
#define _BasicValue_h

#include <string>

namespace meteo {

class BasicValue {
	bool		hasvalue;
	std::string	unit;
	int		refcount;
	// define the assignment and copy constructors private, because
	// we don't want them to be used at all (copy values, not BasicValues)
protected:
	double		value;
public:
	// construction and destruction
	BasicValue(const std::string& u);
	virtual	~BasicValue(void);
	BasicValue(const BasicValue&);
	BasicValue&	operator=(const BasicValue&);

	// we need a type field, which we implement by a getClass method
	virtual std::string	getClass(void) const { return "BasicValue"; }

	// accessors to the value field
	double		getValue(void) const;
	std::string	getValueString(const std::string& format) const;
	std::string	getValueString(void) const;

	// setting the value may change other things besides the value
	void	setValue(double v);

	// unit accessors, the setUnit accessor implicitely performs conversion
	const std::string&	getUnit(void) const { return unit; }
	virtual void	setUnit(const std::string& u) { unit = u; }
	
	// hasValue accessor
	bool	hasValue(void) const { return hasvalue; }
	void	setHasvalue(bool h) { hasvalue = h; }

	// output
	virtual std::string	plain(const std::string& name) const;
	virtual std::string	xml(const std::string& name) const;

	// allow access to the data fields to the Converter class
	friend class	Value;	// may update the reference count
};

} /* namespace meteo */

#endif /* _BasicValue_h */
