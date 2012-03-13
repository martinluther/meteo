/*
 * LastRecorder.h -- recorder class that always remembers the last value seen
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: LastRecorder.h,v 1.1 2004/02/26 14:00:19 afm Exp $
 */
#ifndef _LastRecorder_h
#define _LastRecorder_h

#include <BasicRecorder.h>
#include <Timeval.h>

namespace meteo {

class Converter;

class	LastRecorder : public BasicRecorder {

public:
	// construction and destruction
	LastRecorder(const std::string& unit);
	virtual ~LastRecorder(void);

	// update the meteovalue with a new value
	void	reset(void);
	virtual void	update(double v);
};

} // namespace meteo

#endif /* _LastRecorder_h */
