/*
 * Confguration.cc -- read configuration file and serve configuration info to
 *                    interested clients
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <Configuration.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <libxml/parser.h>
#include <libxml/xpath.h>
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#include <mdebug.h>
#include <errno.h>
#include <MeteoException.h>

namespace meteo {

class config {
public:
	xmlDocPtr	cf;
	config(const std::string& filename);
};

config::config(const std::string& filename) {
	// open the configuration file
	struct stat	sb;
	if (stat(filename.c_str(), &sb) < 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "cannot stat conf file '%s'"
			"%s (%d)", filename.c_str(), strerror(errno), errno);
		throw MeteoException(std::string("cannot open file ")
			+ filename, strerror(errno));
	}

	cf = xmlParseFile(filename.c_str());
	if (NULL == cf) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "Parse of conf file '%s' failed",
			filename.c_str());
		throw MeteoException("cannot parse file", filename);
	}
}

Configuration::Configuration(const std::string& filename) {
	if (NULL != l) {
		delete l;
	}
	l = new config(filename);
}

config	*Configuration::l = NULL;

double	Configuration::getDouble(const std::string& xpath, double def) const {
	std::vector<std::string>	r = getStringVector(xpath);
	if (0 == r.size()) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%s not found, use default %f",
			xpath.c_str(), def);
		return def;
	} else
		return atof(r[0].c_str());
}

std::vector<double>	Configuration::getDoubleVector(const std::string& xpath) const {
	std::vector<double>	result;
	std::vector<std::string>	r = getStringVector(xpath);
	std::vector<std::string>::const_iterator	i;
	for (i = r.begin(); i != r.end(); i++) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "found %s = %s",
			xpath.c_str(), i->c_str());
		result.push_back(atof(i->c_str()));
	}
	return result;
}

int	Configuration::getInt(const std::string& xpath, int def) const {
	std::vector<std::string>	r = getStringVector(xpath);
	if (0 == r.size()) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%s not found, use default %d",
			xpath.c_str(), def);
		return def;
	} else
		return atoi(r[0].c_str());
}

std::vector<int>	Configuration::getIntVector(const std::string& xpath) const {
	std::vector<int>	result;
	std::vector<std::string>	r = getStringVector(xpath);
	std::vector<std::string>::const_iterator	i;
	for (i = r.begin(); i != r.end(); i++) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "found %s = %s",
			xpath.c_str(), i->c_str());
		result.push_back(atoi(i->c_str()));
	}
	return result;
}

std::string	Configuration::getString(const std::string& xpath, const std::string& def) const {
	std::vector<std::string>	r = getStringVector(xpath);
	if (0 == r.size())
		return def;
	else
		return r[0];
}
std::vector<std::string> Configuration::getStringVector(const std::string& xpath) const {
	std::vector<std::string>	result;
	xmlXPathContextPtr	xpctp = xmlXPathNewContext(l->cf);
	if (NULL == xpctp) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "xmlXPathNewContext failed");
		throw MeteoException("cannt create XPathContextPtr", "");
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "request for %s", xpath.c_str());
	xmlXPathObjectPtr	start = xmlXPathEval((xmlChar *)xpath.c_str(),
					xpctp);
	if (NULL == start) {
		mdebug(LOG_INFO, MDEBUG_LOG, 0, "cannot find xpath %s",
			xpath.c_str());
		xmlXPathFreeContext(xpctp);
		throw MeteoException("path not found", xpath);
	}
	switch (start->type) {
	case XPATH_NODESET:
		if (xmlXPathNodeSetIsEmpty(start->nodesetval)) {
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"node set empty, returning empty vector");
			xmlXPathFreeContext(xpctp);
			xmlXPathFreeObject(start);
			return result;
		}
		for (int i = 0; i < xmlXPathNodeSetGetLength(start->nodesetval);
			i++) {
			xmlNodePtr	np = xmlXPathNodeSetItem(
						start->nodesetval, i);
			std::string	value((char *)xmlNodeGetContent(np));
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "adding item %d: %s",
				i, value.c_str());
			result.push_back(value);
		}
		break;
	case XPATH_STRING:
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "adding single value %s",
			start->stringval);
		result.push_back(std::string((char *)start->stringval));
		break;
	default:
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"don't know what to do with type %d", start->type);
		xmlXPathFreeContext(xpctp);
		xmlXPathFreeObject(start);
		throw MeteoException("initial node of wrong type", xpath);
		break;
	}

	// cleanup
	xmlXPathFreeObject(start);
	xmlXPathFreeContext(xpctp);

	// return the vector we have built
	return result;
}

bool	Configuration::getBool(const std::string& xpath, bool def) const {
	std::string	x = getString(xpath, std::string((def) ? "yes" : "no"));
	if ((x == "yes") || (x == "true") || (x == "y")) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%s is true", xpath.c_str());
		return true;
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%s is false", xpath.c_str());
	return false;
}

bool	Configuration::xpathExists(const std::string& xpath) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "checking existence of %s",
		xpath.c_str());
	xmlXPathContextPtr	xpctp = xmlXPathNewContext(l->cf);
	if (NULL == xpctp) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "xmlXPathNewContext failed");
		throw MeteoException("cannt create XPathContextPtr", "");
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "request for %s", xpath.c_str());
	xmlXPathObjectPtr	start = xmlXPathEval((xmlChar *)xpath.c_str(),
					xpctp);
	if (NULL == start) {
		mdebug(LOG_INFO, MDEBUG_LOG, 0, "cannot find xpath %s",
			xpath.c_str());
		xmlXPathFreeContext(xpctp);
		return false;
	}
	xmlXPathFreeContext(xpctp);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "path %s found", xpath.c_str());
	if (start->type == XPATH_NODESET) {
		if (xmlXPathNodeSetIsEmpty(start->nodesetval)) {
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "but %s is empty",
				xpath.c_str());
			return false;
		}
	}
	return true;
}

const xmlNodePtr	Configuration::getNode(const std::string& xpath) const {
	xmlNodePtr	result = NULL;

	// set up an XPath context for searching
	xmlXPathContextPtr	xpctp = xmlXPathNewContext(l->cf);
	if (NULL == xpctp) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "xmlXPathNewContext failed");
		throw MeteoException("cannt create XPathContextPtr", "");
	}

	// evaluate the xpath
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "request for %s", xpath.c_str());
	xmlXPathObjectPtr	start = xmlXPathEval((xmlChar *)xpath.c_str(),
					xpctp);

	// handle case where we have not found enything
	if (NULL == start) {
		mdebug(LOG_INFO, MDEBUG_LOG, 0, "cannot find xpath %s",
			xpath.c_str());
		xmlXPathFreeContext(xpctp);
		throw MeteoException("path not found", xpath);
	}

	// we can only do something with a node set
	switch (start->type) {
	case XPATH_NODESET:
		if (xmlXPathNodeSetIsEmpty(start->nodesetval)) {
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"node set empty, returning empty vector");
		} else {
			result = xmlXPathNodeSetItem(start->nodesetval, 0);
		}
		break;
	default:
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"don't know what to do with type %d", start->type);
		break;
	}

	// cleanup
	xmlXPathFreeObject(start);
	xmlXPathFreeContext(xpctp);

	if (NULL == result)
		throw MeteoException("node not found", xpath);
	return result;
}

const std::vector<xmlNodePtr>	Configuration::getNodeVector(const std::string& xpath) const {
	std::vector<xmlNodePtr>	result;

	// set up an XPath context for searching
	xmlXPathContextPtr	xpctp = xmlXPathNewContext(l->cf);
	if (NULL == xpctp) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "xmlXPathNewContext failed");
		throw MeteoException("cannt create XPathContextPtr", "");
	}

	// evaluate the xpath
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "request for %s", xpath.c_str());
	xmlXPathObjectPtr	start = xmlXPathEval((xmlChar *)xpath.c_str(),
					xpctp);

	// handle case where we have not found enything
	if (NULL == start) {
		mdebug(LOG_INFO, MDEBUG_LOG, 0, "cannot find xpath %s",
			xpath.c_str());
		xmlXPathFreeContext(xpctp);
		return result;
	}

	// we can only do something with a node set
	xmlNodeSetPtr	ns;
	switch (start->type) {
	case XPATH_NODESET:
		ns = start->nodesetval;
		if (xmlXPathNodeSetIsEmpty(ns)) {
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"node set empty, returning empty vector");
		} else {
			for (int i = 0; i < xmlXPathNodeSetGetLength(ns); i++)
				result.push_back(xmlXPathNodeSetItem(ns, i));
		}
		break;
	default:
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"don't know what to do with type %d", start->type);
		break;
	}

	// cleanup
	xmlXPathFreeObject(start);
	xmlXPathFreeContext(xpctp);

	return result;
}

// convenience methods for access to units definitions
std::string	Configuration::getTemperatureUnit(void) const {
	return getString("/meteo/units/temperatur", std::string("C"));
}
std::string	Configuration::getHumidityUnit(void) const {
	return getString("/meteo/units/humidity", std::string("%"));
}
std::string	Configuration::getPressureUnit(void) const {
	return getString("/meteo/units/pressure", std::string("hPa"));
}
std::string	Configuration::getRainUnit(void) const {
	return getString("/meteo/units/rain", std::string("mm"));
}
std::string	Configuration::getWindUnit(void) const {
	return getString("/meteo/units/wind", std::string("m/s"));
}
std::string	Configuration::getSolarUnit(void) const {
	return getString("/meteo/units/solar", std::string("W/m2"));
}
std::string	Configuration::getUVUnit(void) const {
	return getString("/meteo/units/uv", std::string("index"));
}

// convenience methods for access to database parameters
std::string	Configuration::getDBHostname(void) const {
	std::string	s = getString("/meteo/database/hostname", "unknown");
	if (s == "unknown") {
		throw MeteoException("config tag missing",
			"/meteo/database/hostname");
	}
	return s;
}
std::string	Configuration::getDBName(void) const {
	std::string	s = getString("/meteo/database/dbname", "unknown");
	if (s == "unknown") {
		throw MeteoException("config tag missing",
			"/meteo/database/dbname");
	}
	return s;
}
std::string	Configuration::getDBUser(void) const {
	std::string	s = getString("/meteo/database/user", "unknown");
	if (s == "unknown") {
		throw MeteoException("config tag missing",
			"/meteo/database/user");
	}
	return s;
}
std::string	Configuration::getDBPassword(void) const {
	std::string	s = getString("/meteo/database/password", "unknown");
	if (s == "unknown") {
		throw MeteoException("config tag missing",
			"/meteo/database/password");
	}
	return s;
}
std::string	Configuration::getDBWriter(void) const {
	std::string	s = getString("/meteo/database/writer", "unknown");
	if (s == "unknown") {
		throw MeteoException("config tag missing",
			"/meteo/database/writer");
	}
	return s;
}
std::string	Configuration::getDBWriterpassword(void) const {
	std::string	s = getString("/meteo/database/writerpassword",
		"unknown");
	if (s == "unknown") {
		throw MeteoException("config tag missing",
			"/meteo/database/writerpassword");
	}
	return s;
}
std::string	Configuration::getDBMsgqueue(void) const {
	std::string	s = getString("/meteo/database/msgqueue", "unknown");
	if (s == "unknown") {
		throw MeteoException("config tag missing",
			"/meteo/database/msgqueue");
	}
	return s;
}
std::string	Configuration::getDBUpdatefile(void) const {
	std::string	s = getString("/meteo/database/updatefile", "unknown");
	if (s == "unknown") {
		throw MeteoException("config tag missing",
			"/meteo/database/updatefile");
	}
	return s;
}

} /* namespace meteo */
