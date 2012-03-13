/*
 * XmlOutlet.cc -- send data to an XML file
 *
 * (c) 2008 Prof Dr Andreas Mueller
 * $Id: XmlOutlet.cc,v 1.3 2009/01/10 21:47:01 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <XmlOutlet.h>
#include <SensorStationInfo.h>
#include <Field.h>
#include <set>
#include <list>
#include <Format.h>
#include <mdebug.h>
#include <iostream>

namespace meteo {

XmlOutlet::XmlOutlet(const std::string& _stationname) : stationname(_stationname) {
	delegate = NULL;
}
XmlOutlet::XmlOutlet(const std::string& _stationname, XmlOutletDelegate *_delegate)
	: stationname(_stationname) {
	delegate = _delegate;
}

XmlOutlet::~XmlOutlet() {
	if (NULL != delegate) {
		delete delegate;
	}
}

void	XmlOutlet::flush(const time_t timekey) {
	std::list<outlet_t>::iterator	i;
	char	val[1024];
	std::string	result;
	result.append(stringprintf("<?xml version=\"1.0\"?>\n"));
	result.append(stringprintf(
		"<meteo-outlet station=\"%s\" version=\"%s\" time=\"%ld\">\n",
		stationname.c_str(), VERSION, timekey));

	// found out which sensor ids appear in the list
	std::set<int>	sensorids;
	for (i = batch.begin(); i != batch.end(); i++) {
		sensorids.insert(i->sensorid);
	}

	// iterate through station ids
	std::set<int>::iterator	j;
	for (j = sensorids.begin(); j != sensorids.end(); j++) {
		SensorStationInfo	sensor(*j);
		result.append(stringprintf("<sensorstation "
			"name=\"%s\">\n", sensor.getName().c_str()));
		Field	f;
		for (i = batch.begin(); i != batch.end(); i++) {
			if (i->sensorid == *j) {
				result.append(stringprintf("<value name=\"%s\" unit=\"%s\">%f</value>\n",
					f.getName(i->fieldid).c_str(),
					i->unit.c_str(), i->value));
			}
		}
		result.append(std::string("</sensorstation>\n"));
	}
	result.append(std::string("</meteo-outlet>\n"));
	if (NULL != delegate) {
		delegate->write(result);
	}
	Outlet::flush(timekey);
}


} // namespace meteo
