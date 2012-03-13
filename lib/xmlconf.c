/*
 * xmlconf.c -- read an XML configuration file and provide access to its
 *              contents
 *
 * (c) 2002 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: xmlconf.c,v 1.5 2003/06/12 23:29:46 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <xmlconf.h>
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <mdebug.h>
#include <errno.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif

/*
 * parse the configuration file
 */
xmlDocPtr	xmlconf_open(const char *filename) {
	xmlDocPtr	result;
	struct stat	sb;
	
	if (stat(filename, &sb) < 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "cannot stat conf file '%s': "
			"%s (%d)", filename, strerror(errno), errno);
		return NULL;
	}
	
	result = xmlParseFile(filename);
	if (result == NULL) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "parse of conf file '%s' failed",
			filename);
	}
	return result;
}


void	xmlconf_close(xmlDocPtr xmlconf) {
	xmlFreeDoc(xmlconf);
}

meteoconf_t	*xmlconf_new(const char *filename, const char *stationname) {
	xmlDocPtr	xp;
	meteoconf_t	*result;

	/* some sanity checks						*/
	if (stationname == NULL) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "station name missing");
		return NULL;
	}
	xp = xmlconf_open(filename);
	if (NULL == xp) {
		mdebug(LOG_CRIT, MDEBUG_LOG, 0, "invalid configuration file");
		return NULL;
	}

	/* create configuration record					*/
	result = (meteoconf_t *)malloc(sizeof(meteoconf_t));
	result->xml = xp;
	result->station = strdup(stationname);

	mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
		"new configuration created for station %s from file %s",
		result->station, filename);
	return result;
}

void	xmlconf_free(meteoconf_t *mc) {
	free(mc->station); mc->station = NULL;
	xmlconf_close(mc->xml); mc->xml = NULL;
	free(mc); mc = NULL;
}

static const ncolor_t	xmlconf_node2color(xmlNodePtr np,
		const ncolor_t defaultcolor) {
	xmlNodePtr	cs;
	ncolor_t	result = defaultcolor;
	/* go through the children of this node, we should find		*/
	/* red/green/blue values					*/
	for (cs = np->children; cs; cs = cs->next) {
		if (0 == strcmp("red", (char *)cs->name)) {
			result.c[0] = atoi((char *)xmlNodeGetContent(cs));
		}
		if (0 == strcmp("green", (char *)cs->name)) {
			result.c[1] = atoi((char *)xmlNodeGetContent(cs));
		}
		if (0 == strcmp("blue", (char *)cs->name)) {
			result.c[2] = atoi((char *)xmlNodeGetContent(cs));
		}
	}
	return result;
}

static xmlNodePtr	xmlconf_get_abs(const xmlDocPtr xc, const char *path) {
	char			rootedpath[1024];
	xmlXPathContextPtr	xp;
	xmlXPathObjectPtr	xop;
	xmlNodePtr		result;

	/* create XPath context						*/
	xp = xmlXPathNewContext(xc);

	/* create a properly rooted path				*/
	snprintf(rootedpath, sizeof(rootedpath), "//meteo/%s", path);

	/* perform the search						*/
	xop = xmlXPathEval((xmlChar *)path, xp);

	/* handle various cases with return values that are not useful	*/
	if (NULL == xop) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"%s not found, using default", path);
		goto cleanup;
	}
	if (XPATH_NODESET != xop->type)
		goto cleanup;
	if (xmlXPathNodeSetIsEmpty(xop->nodesetval))
		goto cleanup;

	/* extract the content of the node found			*/
	result = xmlXPathNodeSetItem(xop->nodesetval, 0);
cleanup:
	if (xop) { xmlXPathFreeObject(xop); xop = NULL; }
	if (xp) { xmlXPathFreeContext(xp); xp = NULL; }
	return result;
}

const char	*xmlconf_get_abs_string(const meteoconf_t *mc, const char *path,
	const char *defaultvalue) {
	xmlNodePtr	n;
	const char	*result = defaultvalue;

	/* get the node associated with the path			*/
	if (NULL == (n = xmlconf_get_abs(mc->xml, path)))
		return result;

	return (const char *)xmlNodeGetContent(n);
}

double	xmlconf_get_abs_double(const meteoconf_t *mc, const char *path,
	double defaultvalue) {
	xmlNodePtr	n;
	int		result = defaultvalue;

	/* get the node associated with the path			*/
	if (NULL == (n = xmlconf_get_abs(mc->xml, path)))
		return result;

	return atof((char *)xmlNodeGetContent(n));
}

int	xmlconf_get_abs_int(const meteoconf_t *mc, const char *path,
	int defaultvalue) {
	xmlNodePtr	n;
	int		result = defaultvalue;

	/* get the node associated with the path			*/
	if (NULL == (n = xmlconf_get_abs(mc->xml, path)))
		return result;

	return atoi((char *)xmlNodeGetContent(n));
}

const ncolor_t	xmlconf_get_abs_color(const meteoconf_t *mc, const char *path,
	const ncolor_t defaultvalue) {
	xmlNodePtr	n;
	ncolor_t	result = defaultvalue;

	/* get the node associated with the path			*/
	if (NULL == (n = xmlconf_get_abs(mc->xml, path)))
		return result;
	return xmlconf_node2color(n, defaultvalue);
}


static xmlNodePtr	xmlconf_getcontainer(xmlXPathContextPtr xp,
	xmlNodePtr current, const char *element, const char *name) {
	char			relname[1024];
	xmlXPathObjectPtr	xop = NULL;
	xmlNodePtr		result;

	/* stop when we encounter a NULL current node			*/
	if (current == NULL) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "arrived at NULL node");
		return NULL;
	}

	/* construct a search path that finds an immediate child with	*/
	/* with the right name						*/
	if (name)
		snprintf(relname, sizeof(relname), "./%s[@name='%s']",
			element, name);
	else
		snprintf(relname, sizeof(relname), "./%s",
			element);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "searching for %s", relname);

	/* perform a search for the current node			*/
	xp->node = current;
	xop = xmlXPathEval((xmlChar *)relname, xp);
	
	/* if we found something, return it				*/
	if (xop) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "found a node");
		if (XPATH_NODESET != xop->type)
			goto tryparent;
		if (xmlXPathNodeSetIsEmpty(xop->nodesetval))
			goto tryparent;

		/* clean up after the search				*/
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "found target node");
		result = xmlXPathNodeSetItem(xop->nodesetval, 0);
		xmlXPathFreeObject(xop);

		/* return the first node				*/
		return result;
	} else {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "search returns NULL");
	}

	/* search the parent node for the same stuff			*/
tryparent:
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "retry in parent");
	if (xop)
		xmlXPathFreeObject(xop);
	return xmlconf_getcontainer(xp, current->parent, element, name);
}

static xmlNodePtr	xmlconf_get(xmlXPathContextPtr xpc,
				const char *format, ...) {
	va_list			ap;
	xmlXPathObjectPtr	start;
	xmlNodePtr		startnode;
	char			path[10240];

	/* format the search XPath					*/
	va_start(ap, format);
	vsnprintf(path, sizeof(path), format, ap);
	va_end(ap);
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "search base: %s", path);

	/* search for a node with this path, this must succeed!		*/
	start = xmlXPathEval((xmlChar *)path, xpc);
	if (NULL == start) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "initial path %s not found",
			path);
		return NULL;
	}

	/* retrieve a node from the object pointer, must have a node	*/
	/* set								*/
	if (XPATH_NODESET != start->type) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "initial node %s has wrong type",
			path);
		xmlXPathFreeObject(start);
		return NULL;
	}
	if (xmlXPathNodeSetIsEmpty(start->nodesetval)) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
			"initial node set for %s is emtpy", path);
		xmlXPathFreeObject(start);
		return NULL;
	}

	/* now pick the first node from the node set (we are not 	*/
	/* interestedin the other ones					*/
	startnode = xmlXPathNodeSetItem(start->nodesetval, 0);
	xmlXPathFreeObject(start);

	return startnode;
}

static xmlNodePtr	xmlconf_getnode(const meteoconf_t *mc,
	const char *initialpath, const char *element, const char *name,
	int interval) {
	xmlNodePtr		startnode = NULL, resnode;
	xmlXPathContextPtr	xpctp;
	const char		*realname = name;

	/* create an xmlXPathContext					*/
	xpctp = xmlXPathNewContext(mc->xml);

	/* build the correct search to start with			*/
	if (interval > 0) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "search with interval");
		startnode = xmlconf_get(xpctp,
			"/meteo/station[@name='%s']/%s/interval[@width='%d']",
			mc->station, initialpath, interval);
	}
	if (NULL == startnode) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
				"interval node not found, trying plain");
		startnode = xmlconf_get(xpctp,
			"/meteo/station[@name='%s']%s%s",
			mc->station, (strlen(initialpath) > 0) ? "/" : "",
			initialpath);
	}
	if (NULL == startnode) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "no base node found");
		xmlXPathFreeContext(xpctp);
		return NULL;
	}

	/* now recursively perform xpath searches from the current node	*/
	/* for objects of name "element", stepping up if nothing found	*/
	if (name)
		realname = (strlen(name) > 0) ? name : NULL;
	resnode = xmlconf_getcontainer(xpctp, startnode, element, realname);

	/* clean up the xmlXPathContext					*/
	xmlXPathFreeContext(xpctp);

	/* this node must contain the element we asked for		*/
	return resnode;
}

/* access to data							*/
double	xmlconf_get_double(const meteoconf_t *mc,
		const char *initialpath, const char *element, const char *name,
		int interval, double defaultvalue) {
	xmlNodePtr	np;
	char		*c;

	np = xmlconf_getnode(mc, initialpath, element, name, interval);
	if (!np)
		return defaultvalue;

	/* the content of this node must be nonempty and a double	*/
	c = (char *)xmlNodeGetContent(np);
	if (c)
		return atof(c);
	return defaultvalue;
}

int	xmlconf_get_int(const meteoconf_t *mc,
		const char *initialpath, const char *element, const char *name,
		int interval, int defaultvalue) {
	xmlNodePtr	np;
	char		*c;

	np = xmlconf_getnode(mc, initialpath, element, name, interval);
	if (!np)
		return defaultvalue;

	/* the content of this node must be nonempty and an integer	*/
	c = (char *)xmlNodeGetContent(np);
	if (c)
		return atoi(c);
	return defaultvalue;
}

const char	*xmlconf_get_string(const meteoconf_t *mc,
		const char *initialpath, const char *element, const char *name,
		int interval, const char *defaultvalue){
	xmlNodePtr	np;
	char		*c;

	np = xmlconf_getnode(mc, initialpath, element, name, interval);
	if (!np)
		return defaultvalue;

	/* the content of this node must be nonempty and a string	*/
	c = (char *)xmlNodeGetContent(np);
	if (c)
		return c;
	return defaultvalue;
}

ncolor_t	xmlconf_get_color(const meteoconf_t *mc,
		const char *initialpath, const char *element, const char *name,
		int interval, ncolor_t defaultvalue) {
	xmlNodePtr	np;

	np = xmlconf_getnode(mc, initialpath, element, name, interval);
	if (!np)
		return defaultvalue;

	/* go through the children of this node, we should find		*/
	/* red/green/blue values					*/
	return xmlconf_node2color(np, defaultvalue);
}
