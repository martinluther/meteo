/*
 * XmlOutletFactory.h -- contains the XmlOutletFactory class that is capable of
 *                       creating XmlOutlet classes with suitable 
 *      
 * (c) 2008 Prof Dr Andreas Mueller
 * $Id: XmlOutletFactory.h,v 1.1 2008/09/07 15:18:52 afm Exp $
 */
#ifndef _XmlOutletFactory_h 
#define _XmlOutletFactory_h

#include <XmlOutlet.h>

namespace meteo {

class XmlOutletFactory {
public:
	static XmlOutlet	*get(const std::string& stationname, const std::string& xmlfile);
};

} // namespace meteo

#endif /* _XmlOutletFactory_h */
