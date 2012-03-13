/*
 * XmlOutletFactory.cc -- 
 *
 * (c) 2008 Prof Dr Andreas Mueller
 * $Id: XmlOutletFactory.cc,v 1.2 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <XmlOutletFactory.h>

namespace meteo {

XmlOutlet	*XmlOutletFactory::get(const std::string& stationname,
	const std::string& xmlfile) {
	if (xmlfile[0] == '|')  {
		return new XmlOutlet(stationname,
			new XmlPipeOutletDelegate(xmlfile.substr(1)));
	} else {
		return new XmlOutlet(stationname,
			new XmlFileOutletDelegate(xmlfile));
	}
}

} // namespace meteo
