/*
 * Dataset.cc
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <Dataset.h>
#include <vector>
#include <MeteoException.h>
#include <mdebug.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

namespace meteo {

static xmlNodePtr	xmlFirstElementNode(xmlNodePtr cur) {
	// dont follow a NULL pointer
	if (NULL == cur) return cur;

	// check whe
	while (cur) {
		if (cur->type == XML_ELEMENT_NODE)
			return cur;
		cur = cur->next;
	}
	return cur;
}
static xmlNodePtr	xmlNextElementNode(xmlNodePtr cur) {
	return xmlFirstElementNode(cur->next);
}

static const Tdata	processNode(Dataset *dset, xmlNodePtr cur) {
	xmlNodePtr	cp;
	// skip forward to the first element node
	cur = xmlFirstElementNode(cur);
	if (NULL == cur) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"no element nodes among children");
		throw MeteoException("no element nodes among children",
			(char *)cur->name);
	}

	// data node: just call again with the first child
	if ((!xmlStrcmp(cur->name, (const xmlChar *)"data"))) {
		if (NULL == cur->xmlChildrenNode) {
			mdebug(LOG_ERR, MDEBUG_LOG, 0, "data node without "
				"children");
			throw MeteoException("incorrect data node",
				"no children");
		}
		return processNode(dset, cur->xmlChildrenNode);
	}

	// constant node: just return a constant Tdata for the attribute
	//                value
	if ((!xmlStrcmp(cur->name, (const xmlChar *)"constant"))) {
		xmlChar	*value = xmlGetProp(cur, (const xmlChar *)"value");
		if (NULL == value) {
			mdebug(LOG_ERR, MDEBUG_LOG, 0,
				"constant without value");
			throw MeteoException("constant without value", "");
		}
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "constant node %s", value);
		double	v = atof((char *)value);
		xmlFree(value);
		return	Tdata(v);
	}

	// value node: retrieve the Tdata from the map
	if ((!xmlStrcmp(cur->name, (const xmlChar *)"value"))) {
		xmlChar	*name = xmlGetProp(cur, (const xmlChar *)"name");
		if (NULL == name) {
			mdebug(LOG_ERR, MDEBUG_LOG, 0, "name attr missing");
			throw MeteoException("name attr missing in value", "");
		}
		std::string	dataname((char *)name);
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "value node @ %s", name);
		xmlFree(name);
		return dset->getData(dataname);
	}


	// function node: there must be exactly one child, the argument to
	//                the function
	if ((!xmlStrcmp(cur->name, (const xmlChar *)"function"))) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "function node");
		cp = xmlFirstElementNode(cur->xmlChildrenNode);
		Tdata	d = processNode(dset, cp);

		// find the function name
		xmlChar	*name = xmlGetProp(cur, (const xmlChar *)"name");
		if (NULL == name) {
			mdebug(LOG_ERR, MDEBUG_LOG, 0,
				"function name not found");
			throw MeteoException("function name not found", "");
		}
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "function name is %s", name);

		// check for a known function name
		if ((!xmlStrcmp(name, (const xmlChar *)"sqr"))) {
			xmlFree(name);
			return d.sqr();
		}
		if ((!xmlStrcmp(name, (const xmlChar *)"sqrt"))) {
			xmlFree(name);
			return d.sqrt();
		}
		if ((!xmlStrcmp(name, (const xmlChar *)"sin"))) {
			xmlFree(name);
			return d.sin();
		}
		if ((!xmlStrcmp(name, (const xmlChar *)"cos"))) {
			xmlFree(name);
			return d.cos();
		}
		if ((!xmlStrcmp(name, (const xmlChar *)"tan"))) {
			xmlFree(name);
			return d.tan();
		}
		xmlChar	*limitname = xmlGetProp(cur, (const xmlChar *)"limit");
		if ((!xmlStrcmp(name, (const xmlChar *)"ceil"))) {
			xmlFree(name);
			if (NULL == limitname)
				throw MeteoException("limit arg missing", "");
			double limit = atof((char *)limitname);
			xmlFree(limitname);
			return d.ceil(limit);
		}
		if ((!xmlStrcmp(name, (const xmlChar *)"floor"))) {
			xmlFree(name);
			if (NULL == limitname)
				throw MeteoException("limit arg missing", "");
			double limit = atof((char *)limitname);
			xmlFree(limitname);
			return d.floor(limit);
		}
	}

	// binary function node: there must be exactly two children, the
	//                       two arguments to the function
	if ((!xmlStrcmp(cur->name, (const xmlChar *)"function2"))) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "binary function node");
		cp = xmlFirstElementNode(cur->xmlChildrenNode);
		Tdata	arg1 = processNode(dset, cp);
		Tdata	arg2 = processNode(dset, xmlNextElementNode(cp));

		// find the name of the function
		xmlChar	*name = xmlGetProp(cur, (const xmlChar *)"name");
		if (NULL == name) {
			mdebug(LOG_ERR, MDEBUG_LOG, 0,
				"function name not found");
			throw MeteoException("function name not found", "");
		}
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "function name is %s", name);

		// check for a known function name
		if ((!xmlStrcmp(name, (const xmlChar *)"atan2"))) {
			xmlFree(name);
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "computing atan2");
			return atan2(arg1, arg2);
		}
		if ((!xmlStrcmp(name, (const xmlChar *)"azi"))) {
			xmlFree(name);
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "computing azi");
			return azi(arg1, arg2);
		}
		if ((!xmlStrcmp(name, (const xmlChar *)"hypot"))) {
			xmlFree(name);
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "computing hypot");
			return hypot(arg1, arg2);
		}
		if ((!xmlStrcmp(name, (const xmlChar *)"dewpoint"))) {
			xmlFree(name);
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "computing dewpoint");
			return dewpoint(arg1, arg2);
		}
		if ((!xmlStrcmp(name, (const xmlChar *)"max"))) {
			xmlFree(name);
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "computing max");
			return max(arg1, arg2);
		}
		if ((!xmlStrcmp(name, (const xmlChar *)"min"))) {
			xmlFree(name);
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "computing min");
			return min(arg1, arg2);
		}
	}

	// sum node: retrieve all Tdata subnodes and compute sum
	if ((!xmlStrcmp(cur->name, (const xmlChar *)"sum"))) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "sum node");
		Tdata	result;
		bool	first = true;
		for (cp = xmlFirstElementNode(cur->xmlChildrenNode); NULL != cp;
			cp = xmlNextElementNode(cp)) { 
			if (first) {
				result = processNode(dset, cp);
				first = false;
			} else
				result = result + processNode(dset, cp);
		}
	}

	// multipliation node: retrieve all Tdata subnodes and compute sum
	if ((!xmlStrcmp(cur->name, (const xmlChar *)"mult"))) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "mult node");
		Tdata	result;
		bool	first = true;
		for (cp = xmlFirstElementNode(cur->xmlChildrenNode); NULL != cp;
			cp = xmlNextElementNode(cp)) {
			if (first) {
				result = processNode(dset, cp);
				first = false;
			} else
				result = result * processNode(dset, cp);
		}
	}

	// difference node: retrieve the two Tdata subnodes and compute diff
	if ((!xmlStrcmp(cur->name, (const xmlChar *)"diff"))) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "difference node");
		cp = xmlFirstElementNode(cur->xmlChildrenNode);
		return processNode(dset, cp)
				- processNode(dset, xmlNextElementNode(cp));
	}

	// quotient node: retrieve the two Tdata subnodes and compute quotient
	if ((!xmlStrcmp(cur->name, (const xmlChar *)"quot"))) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "quotient node");
		cp = xmlFirstElementNode(cur->xmlChildrenNode);
		return processNode(dset, cp) /
			processNode(dset, xmlNextElementNode(cp));
	}

	mdebug(LOG_ERR, MDEBUG_LOG, 0, "unknown node %s", cur->name);
	throw MeteoException("dont know how to process node",
		(const char *)cur->name);
}

void	Dataset::addData(const Configuration& conf, const std::string& xpath) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "processing expression xpath %s",
		xpath.c_str());

	// search the xpath in the configuration and preform the actions 
	// described in the XML tree
	xmlNodePtr	startnode = conf.getNode(xpath);
	xmlChar	*name = xmlGetProp(startnode, (const xmlChar *)"name");
	if (NULL == name) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "no name for %s",
			xpath.c_str());
		throw MeteoException("no name for xpath", xpath);
	}

	// process the tree starting from the startnode, and add the result to
	// the data map
	data[(char *)name] = processNode(this, startnode);
	xmlFree(name);
}

void	Dataset::addAlldata(const Configuration& conf, const std::string& xpath) {
	// retrieve a list of names
	std::vector<std::string>	datanames = conf.getStringVector(xpath
					+ "/data/@name");
	// for each of the data names, do an addData
	for (int i = 0; i < (int)datanames.size(); i++) {
		addData(conf, xpath + "/data[@name='" + datanames[i] + "']");
	}
}

const Tdata&	Dataset::getData(std::string& name) const {
	dmap_t::const_iterator	i;

	// look for the data in our own data
	i = data.find(name);
	if (i != data.end()) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "found %s in primary data",
			name.c_str());
		return (i->second);
	}

	// check for it in the source data
	i = srcdata.find(name);
	if (i != srcdata.end()) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "found %s in secondary data",
			name.c_str());
		return (i->second);
	}

	// throw an exception
	throw MeteoException("cannot find requested dataset name", name);
}

} /* namespace meteo */
