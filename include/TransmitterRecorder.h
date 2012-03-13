/*
 * TransmitterRecorder.h -- encapsulate temperature
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TransmitterRecorder.h,v 1.2 2004/02/25 23:52:35 afm Exp $
 */
#ifndef _TransmitterRecorder_h
#define _TransmitterRecorder_h

#include <BasicRecorder.h>
#include <string>

namespace meteo {

class	TransmitterRecorder : public BasicRecorder {
public:
	TransmitterRecorder(void);
	TransmitterRecorder(double);
	TransmitterRecorder(double, const std::string&);
	TransmitterRecorder(const std::string&);
	virtual ~TransmitterRecorder(void) { }
	virtual std::string	getValueClass(void) const {
		return "BasicValue";
	}
	virtual void	update(const Value& v);
};

} // namespace meteo

#endif /* _TransmitterRecorder_h */
