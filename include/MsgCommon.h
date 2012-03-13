/*
 * MsgCommon.h -- class that takes a query and sends it to a queue
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: MsgCommon.h,v 1.2 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _MsgCommon_h
#define _MsgCommon_h

#include <string>

namespace meteo {

class MsgCommon {
protected:
	int		id;
	std::string	queuename;
public:
	MsgCommon(const std::string& name);
	~MsgCommon(void);
};

} /* namespace meteo */

#endif /* _MsgCommon_h */
