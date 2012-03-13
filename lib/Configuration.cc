/*
 * Confguration.cc -- read configuration file and serve configuration info to
 *                    interested clients
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <Configuration.h>
#include <stdlib.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <sys/stat.h>
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
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "cannot stat conf file '%s'",
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
	if (0 == r.size())
		return def;
	else
		return atof(r[0].c_str());
}

std::vector<double>	Configuration::getDoubleVector(const std::string& xpath) const {
	std::vector<double>	result;
	std::vector<std::string>	r = getStringVector(xpath);
	std::vector<std::string>::const_iterator	i;
	for (i = r.begin(); i != r.end(); i++) {
		result.push_back(atof(i->c_str()));
	}
	return result;
}

int	Configuration::getInt(const std::string& xpath, int def) const {
	std::vector<std::string>	r = getStringVector(xpath);
	if (0 == r.size())
		return def;
	else
		return atoi(r[0].c_str());
}

std::vector<int>	Configuration::getIntVector(const std::string& xpath) const {
	std::vector<int>	result;
	std::vector<std::string>	r = getStringVector(xpath);
	std::vector<std::string>::const_iterator	i;
	for (i = r.begin(); i != r.end(); i++) {
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

} /* namespace meteo */
