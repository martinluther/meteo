/*
 * Datarecord.cc -- weather station data abstraction
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Datarecord.cc,v 1.9 2009/01/10 19:00:23 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <Datarecord.h>
#include <ValueFactory.h>
#include <StationInfo.h>
#include <MeteoException.h>
#include <mdebug.h>
#ifdef HAVE_TIME_H
#include <time.h>
#endif /* HAVE_TIME_H */
#include <Timelabel.h>

namespace meteo {

void	Datarecord::addField(const std::string& fieldname, const Value& v) {
	// check wether this value object already exists
	if (data.find(fieldname) != data.end()) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "field %s already exists",
			fieldname.c_str());
		throw MeteoException("field already exists", fieldname);
	}

	// ask the ValueFactory to create a suitable value object for us
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "adding %s -> %f", fieldname.c_str(),
		v.getValue());
	data.insert(valuemap_t::value_type(fieldname, v));
}

std::string	Datarecord::xml(const std::string& stationname) const {
	std::string	rawdata;
	valuemap_t::const_iterator	i;
	for (i = data.begin(); i != data.end(); i++) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "adding %s", i->first.c_str());
		rawdata += "  " + i->second.xml(i->first) + "\n";
	}

	// format the timekey
	char	buffer[12];
	snprintf(buffer, sizeof(buffer), "%ld", timekey);

	return std::string("<meteodata time=\"") + buffer + "\" timelabel=\"" +
		Timelabel(timekey, Level(day)).getString()
		+ "\" station=\"" + stationname + "\">\n" + rawdata
		+ "</meteodata>\n";
}

std::string	Datarecord::plain(const std::string& stationname) const {
	std::string	result;
	valuemap_t::const_iterator	i;
	for (i = data.begin(); i != data.end(); i++) {
		result += i->second.plain(i->first) + "\n";
	}
	return result;
}

Value	Datarecord::getValue(const std::string& fieldname) const {
	// find out whether the Value is really in the datarecord
	valuemap_t::const_iterator	i;
	i = data.find(fieldname);
	if (i == data.end()) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "field %s not found",
			fieldname.c_str());
		throw MeteoException("field not found", fieldname);
	}

	// retrieve the Value
	return i->second;
}

} /* namespace meteo */
