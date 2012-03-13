/*
 * MeteoException.h -- common exception class for Meteo Project
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: MeteoException.h,v 1.3 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _MeteoException_h 
#define _MeteoException_h

#include <string>

namespace meteo {

class MeteoException {
	std::string	reason;
	std::string	addinfo;
public:
	MeteoException(void) { }
	MeteoException(const std::string& r, const std::string& a) {
		this->reason = r;
		this->addinfo = a;
	}
	~MeteoException(void) { }

	// accessors
	void	setReason(const std::string& r) { reason = r; }
	void	setAddinfo(const std::string& a) { addinfo = a; }
	const std::string&	getReason(void) const { return reason; }
	const std::string&	getAddinfo(void) const { return addinfo; }
};

} /* namespace meteo */

#endif /* _MeteoException_h */
