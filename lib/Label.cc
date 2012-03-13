/*
 * Label.cc -- represent labels at left and right of a graph
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Label.cc,v 1.3 2009/01/10 19:00:24 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <Label.h>
#include <mdebug.h>
#include <Configuration.h>

namespace meteo {

// read a label specification from an XML configuration file
Label::Label(const std::string& xpath) {
	Configuration	conf;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "constructing label from %s",
		xpath.c_str());
	std::string	alignment = conf.getString(xpath + "/@align", "center");
	if ("center" == alignment)
		align = center;
	if ("top" == alignment)
		align = top;
	if ("bottom" == alignment)
		align = bottom;
	text = conf.getString(xpath, "label");
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "label text: %s", text.c_str());
}

} /* namespace meteo */
