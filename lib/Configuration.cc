/*
 * Confguration.cc -- read configuration file and serve configuration info to
 *                    interested clients
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Configuration.cc,v 1.15 2009/05/03 11:20:28 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <Configuration.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif /* HAVE_STDLIB_H */
#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif /* HAVE_STDIO_H */
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#include <libxml/parser.h>
#include <libxml/xpath.h>
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif /* HAVE_SYS_STAT_H */
#include <mdebug.h>
#ifdef HAVE_ERRNO_H
#include <errno.h>
#endif /* HAVE_ERRNO_H */
#include <MeteoException.h>
#include <map>

namespace meteo {

// Originally there was a memory leak in the code below, which caused
// the processes to slowly (or sometimes amazingly quickly) grow. As
// a workaround,  I introduced the two caches existscache and resultcache.
// Since there are only finitely many possible lookups in the configuration
// file, the possibility of memory exhaustion is removed, however there is 
// some memory overhead for the caches. The caches can be disabled by
// undefining the CONFIG_USECACHE symbol
//
// However, since the lookup in the cache is much faster than the XML
// XPath evaluation, the caches improve the performance of the Configuration
// class at the expense of a little memory. So we leave this feature on.
// The additional memory requirement is negligible: 8kB.

#define	CONFIG_USECACHE

#ifdef CONFIG_USECACHE
typedef	std::map<std::string, bool>	existsmap_t;
typedef std::map<std::string, stringlist>	resultmap_t;
#endif /* CONFIG_USECACHE */

// config is a payload class that carries the cache (its a singleton)
class config {
public:
	xmlDocPtr	cf;
	config(const std::string& filename);
#ifdef CONFIG_USECACHE
	existsmap_t	existscache;
	resultmap_t	resultcache;
#endif /* CONFIG_USECACHE */
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
	stringlist	r = getStringList(xpath);
	if (0 == r.size()) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%s not found, use default %f",
			xpath.c_str(), def);
		return def;
	} else
		return atof(r.begin()->c_str());
}

doublelist	Configuration::getDoubleList(const std::string& xpath) const {
	doublelist	result;
	stringlist	r = getStringList(xpath);
	stringlist::const_iterator	i;
	for (i = r.begin(); i != r.end(); i++) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "found %s = %s",
			xpath.c_str(), i->c_str());
		result.push_back(atof(i->c_str()));
	}
	return result;
}

int	Configuration::getInt(const std::string& xpath, int def) const {
	stringlist	r = getStringList(xpath);
	if (0 == r.size()) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "%s not found, use default %d",
			xpath.c_str(), def);
		return def;
	} else
		return atoi(r.begin()->c_str());
}

intlist	Configuration::getIntList(const std::string& xpath) const {
	intlist	result;
	stringlist	r = getStringList(xpath);
	stringlist::const_iterator	i;
	for (i = r.begin(); i != r.end(); i++) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "found %s = %s",
			xpath.c_str(), i->c_str());
		result.push_back(atoi(i->c_str()));
	}
	return result;
}

std::string	Configuration::getString(const std::string& xpath, const std::string& def) const {
	stringlist	r = getStringList(xpath);
	if (0 == r.size())
		return def;
	else
		return *r.begin();
}
stringlist Configuration::getStringList(const std::string& xpath) const {
#ifdef CONFIG_USECACHE
	// check cache
	if (l->resultcache.find(xpath) != l->resultcache.end()) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "cache hit for  %s",
			xpath.c_str());
		return l->resultcache.find(xpath)->second;
	}
#endif /* CONFIG_USECACHE */

	// declare the pointer we will later need
	xmlXPathContextPtr	xpctp = NULL;
	xmlXPathObjectPtr	start = NULL;

	// log
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "getStringList(%s)", xpath.c_str());
	stringlist	result;

	// create an XPathContextPtr
	xpctp = xmlXPathNewContext(l->cf);
	if (NULL == xpctp) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "xmlXPathNewContext failed");
		throw MeteoException("cannt create XPathContextPtr", "");
	}

	// evaluate the xpath
	start = xmlXPathEval((xmlChar *)xpath.c_str(), xpctp);
	if (NULL == start) {
		mdebug(LOG_INFO, MDEBUG_LOG, 0, "cannot find xpath %s",
			xpath.c_str());
		xmlXPathFreeContext(xpctp);
		throw MeteoException("path not found", xpath);
	}

	// handle various cases for the result type of the search
	switch (start->type) {
	case XPATH_NODESET:
		if (xmlXPathNodeSetIsEmpty(start->nodesetval)) {
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"node set empty, returning empty list");
			xmlXPathFreeContext(xpctp);
			xmlXPathFreeObject(start);
			return result;
		}
		for (int i = 0; i < xmlXPathNodeSetGetLength(start->nodesetval);
			i++) {
			xmlNodePtr	np = xmlXPathNodeSetItem(
						start->nodesetval, i);
			xmlChar	*cont = xmlNodeGetContent(np);
			std::string	value((char *)cont);
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "adding item %d: %s",
				i, value.c_str());
			result.push_back(value);
			// this missing xmlFree was causing the memory leak
			// aluded to in the introduction. Because of the
			// performance improvement it is still recommended
			// to use the caches
			xmlFree(cont);
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

	// return the list we have built
#ifdef CONFIG_USECACHE
	l->resultcache.insert(resultmap_t::value_type(xpath, result));
#endif /* CONFIG_USECACHE */
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

// the following method checks for the existence of a Xpath in the config
bool	Configuration::xpathExists(const std::string& xpath) const {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "xpathExists(%s)", xpath.c_str());
	// the following two variables must carefully be deallocated when
	// exiting from this function
	xmlXPathContextPtr	xpctp = NULL;
	xmlXPathObjectPtr	start = NULL;

	// here we prepare the result
	bool	result = false;

	// first look in the cache
#ifdef CONFIG_USECACHE
	if (l->existscache.find(xpath) != l->existscache.end()) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "cache hit for %s",
			xpath.c_str());
		return l->existscache.find(xpath)->second;
	}
#endif /* CONFIG_USECACHE */

	// now try the configuration file, first create a PathContext
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "checking existence of %s",
		xpath.c_str());
	xpctp = xmlXPathNewContext(l->cf);
	if (NULL == xpctp) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "xmlXPathNewContext failed");
		throw MeteoException("cannt create XPathContextPtr", "");
	}

	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "request for %s", xpath.c_str());
	start = xmlXPathEval((xmlChar *)xpath.c_str(), xpctp);
	if (NULL == start) {
		mdebug(LOG_INFO, MDEBUG_LOG, 0, "cannot find xpath %s",
			xpath.c_str());
		goto alldone;
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "path %s found", xpath.c_str());
	if (start->type == XPATH_NODESET) {
		if (xmlXPathNodeSetIsEmpty(start->nodesetval)) {
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "but %s is empty",
				xpath.c_str());
			goto alldone;
		}
	}
	result = true;
alldone:
	// deallocate that objects that are still allocated
	if (xpctp) { xmlXPathFreeContext(xpctp); xpctp = NULL; }
	if (start) { xmlXPathFreeObject(start); start = NULL; }

	// store the result of the lookup in the cache
#ifdef CONFIG_USECACHE
	l->existscache.insert(existsmap_t::value_type(xpath, result));
#endif /* CONFIG_USECACHE */
	return result;
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
				"node set empty, returning empty list");
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

const std::list<xmlNodePtr>	Configuration::getNodeList(const std::string& xpath) const {
	std::list<xmlNodePtr>	result;

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
				"node set empty, returning empty list");
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

// xpath concstruction methods
std::string	Configuration::graphXpath(const std::string& graphname,
			const std::string& tag, int interval,
			const std::string& attribute) const {
	char	ii[10];
	snprintf(ii, sizeof(ii), "%d", interval);
	return "/meteo/graphs/graph[@name='" + graphname + "']/"
		+ tag + "[@interval='" + std::string(ii) + "']/@" + attribute;
}
std::string	Configuration::graphXpath(const std::string& graphname,
			const std::string& tag,
			const std::string& attribute) const {
	return "/meteo/graphs/graph[@name='" + graphname + "']/"
		+ tag + "/@" + attribute;
}

// graph access convennience functions
bool	Configuration::hasGraphTag(const std::string& graphname,
		int interval, const std::string& tag) const {
	char	ii[10];
	snprintf(ii, sizeof(ii), "%d", interval);
	return xpathExists("/meteo/graphs/graph[@name='" + graphname + "']/"
		+ tag + "[@interval='" + std::string(ii) + "']");
}

bool	Configuration::hasGraphTag(const std::string& graphname,
		const std::string tag) const {
	return xpathExists("/meteo/graphs/graph[@name='" + graphname + "']/"
		+ tag);
}

double	Configuration::getGraphDouble(const std::string& graphname,
	int interval, std::string& tag, const std::string& attribute,
	double def) const {
	if (hasGraphTag(graphname, interval, tag)) {
		return getDouble(graphXpath(graphname, tag, interval,
			attribute), def);
	} else {
		return getDouble(graphXpath(graphname, tag, attribute), def);
	}
}

doublelist	Configuration::getGraphDoubleList(const std::string& graphname,
			int interval, const std::string& tag,
			const std::string& attribute) const {
	if (hasGraphTag(graphname, interval, tag)) {
		return getDoubleList(graphXpath(graphname, tag, interval,
			attribute));
	} else {
		return getDoubleList(graphXpath(graphname, tag, attribute));
	}
}

int	Configuration::getGraphInt(const std::string& graphname, int interval,
		std::string& tag, const std::string& attribute, int def) const {
	if (hasGraphTag(graphname, interval, tag)) {
		return getInt(graphXpath(graphname, tag, interval, attribute),
			def);
	} else {
		return getInt(graphXpath(graphname, tag, attribute), def);
	}
}

intlist	Configuration::getGraphIntList(const std::string& graphname,
		int interval, const std::string& tag,
		const std::string& attribute) const {
	if (hasGraphTag(graphname, interval, tag)) {
		return getIntList(graphXpath(graphname, tag, interval,
			attribute));
	} else {
		return getIntList(graphXpath(graphname, tag, attribute));
	}

}

std::string	Configuration::getGraphString(const std::string& graphname,
			int interval, std::string& tag,
			const std::string& attribute, const std::string& def)
			const {
	if (hasGraphTag(graphname, interval, tag)) {
		return getString(graphXpath(graphname, tag, interval,
			attribute), def);
	} else {
		return getString(graphXpath(graphname, tag, attribute), def);
	}
}

stringlist	Configuration::getGraphStringList(const std::string& graphname,
			int interval, const std::string& tag,
			const std::string& attribute) const {
	if (hasGraphTag(graphname, interval, tag)) {
		return getStringList(graphXpath(graphname, tag, interval,
			attribute));
	} else {
		return getStringList(graphXpath(graphname, tag, attribute));
	}
}


} /* namespace meteo */
