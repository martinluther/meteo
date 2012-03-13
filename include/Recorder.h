/*
 * Recorder.h -- general value type. This class performs resource management
 *            and acts as a smart pointer for more refined value types as they
 *            are defined in MinmaxRecorder.h, Wind.h and Rain.h
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Recorder.h,v 1.7 2006/05/16 11:19:54 afm Exp $
 */
#ifndef _Recorder_h
#define _Recorder_h

#include <BasicRecorder.h>
#include <string>

namespace meteo {

class	Recorder {
	BasicRecorder	*bv;
	// the following fields are 
	int		id;	// the field id of the prinicial value, must
				// be retrieved from the database
	std::string	name;	// the field name as found in the mfield table
				// in the database
	std::string	title;	// the field title, used in printing
	std::string	classname;	// the class for acceptable value
					// classes
	bool		hasmin, hasmax;
	// methods used by the RecorderFactory are private. If we would publish
	// the Recorder(BasicRecorder *) method, we would loose control over
	// allocation and deallocation of the basic value, which is to be 
	// considered internal
	Recorder(BasicRecorder *bv);
	void	copy(const Recorder& other);
public:
	Recorder();
	~Recorder(void);
	// implement intelligent pointer, only copy constructor and assignment
	// are public, so a Recorder can only be instantiated by copying it
	Recorder(const Recorder&);
	Recorder&	operator=(const Recorder&);

	// accessor methods (there are no set methods for these fields),
	// they are set by the RecorderFactory
	const int	getId(void) const { return id; }
	const bool	hasMin(void) const { return hasmin; }
	const bool	hasMax(void) const { return hasmax; }
	const std::string&	getTitle(void) const { return title; }
	const std::string&	getValueClass(void) const { return classname; }

	// reset method
	void	reset(void) { bv->reset(); }

	// update method: update the present value from a value reference
	//                given as argument
	void	update(const Value& other);
	stringlist	updatequery(const time_t timekey,
		const int sensorid) const;

	// access data
	double	getValue() const { return bv->getValue(); }
	const std::string&	getUnit() const { return bv->getUnit(); }

	// display methods: display the information in this value either as
	//                  an XML string or as plain text
	std::string	xml(void) const;
	std::string	plain(void) const;

	//  make sure the Factory can modify the members
	friend class RecorderFactory;
};

} /* namespace meteo */

#endif /* _Recorder_h */
