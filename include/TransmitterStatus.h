/*
 * TransmitterStatus.h -- encapsulate the VantagePro transmitter status
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: TransmitterStatus.h,v 1.2 2004/02/25 23:52:35 afm Exp $
 */
#ifndef _TransmitterStatus_h
#define _TransmitterStatus_h

#include <BasicValue.h>
#include <Timeval.h>
#include <string>

namespace meteo {

class	TransmitterStatus : public BasicValue {
public:
	TransmitterStatus(void);
	TransmitterStatus(double T);
	TransmitterStatus(double T, const std::string& u);
	TransmitterStatus(const std::string& u);
	virtual ~TransmitterStatus(void) { }
	virtual void	setUnit(const std::string& u);
	virtual std::string	getClass(void) const {
		return "TransmitterStatus";
	}
	virtual std::string	plain(const std::string& name) const;
	virtual std::string	xml(const std::string& name) const;
};

} // namespace meteo

#endif /* _TransmitterStatus_h */
