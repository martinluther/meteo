/*
 * Configuration.h -- read configuration file and serve configuration info to
 *                    interested clients
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Configuration.h,v 1.10 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _Configuration_h
#define _Configuration_h

#include <MeteoTypes.h>
#include <libxml/tree.h>

namespace meteo {

class	config;

class	Configuration {
	static config	*l;
public:
	// construction and destruction
	Configuration(void) { }
	Configuration(const std::string& filename);
	~Configuration(void) { }

	// accessors
	double	getDouble(const std::string& xpath, double def) const;
	doublelist	getDoubleList(const std::string& xpath) const;

	int	getInt(const std::string& xpath, int def) const;
	intlist	getIntList(const std::string& xpath) const;

	std::string	getString(const std::string& xpath,
				const std::string& def) const;
	stringlist	getStringList(const std::string&
		xpath) const;

	bool	getBool(const std::string& xpath, bool def = false) const;
	bool	xpathExists(const std::string& xpath) const;

	// some convenience methods to retrieve database parameters more
	// easily
	std::string	getDBHostname(void) const;
	std::string	getDBName(void) const;
	std::string	getDBUser(void) const;
	std::string	getDBPassword(void) const;
	std::string	getDBWriter(void) const;
	std::string	getDBWriterpassword(void) const;
	std::string	getDBMsgqueue(void) const;
	std::string	getDBUpdatefile(void) const;

	// some convenience functions for graph parameter access
private:
	std::string	graphXpath(const std::string& graphname,
				const std::string& tag,
				const std::string& attribute) const;
	std::string	graphXpath(const std::string& graphname,
				const std::string& tag, int interval,
				const std::string& attribute) const;
public:
	bool	hasGraphTag(const std::string& graphname, int interval,
			const std::string& tag) const;
	bool	hasGraphTag(const std::string& graphname,
			const std::string tag) const;
	double	getGraphDouble(const std::string& graphname, int interval,
		std::string& tag, const std::string& attribute,
		double def) const;
	doublelist	getGraphDoubleList(const std::string& graphname,
			int interval, const std::string& tag,
			const std::string& attribute) const;
	int	getGraphInt(const std::string& graphname, int interval,
		std::string& tag, const std::string& attribute, int def) const;
	intlist	getGraphIntList(const std::string& graphname, int interval,
			const std::string& tag, const std::string& attribute)
			const;
	std::string	getGraphString(const std::string& graphname,
			int interval, std::string& tag,
			const std::string& attribute,
			const std::string& def) const;
	stringlist	getGraphStringList(const std::string& graphname,
			int interval, const std::string& tag,
			const std::string& attribute) const;

	// low level primitives, primarily used for the Dataset stuff where
	// we want to perform operations on data according to the XML tree
	const xmlNodePtr	getNode(const std::string& xpath) const;
	const std::list<xmlNodePtr>	getNodeList(const std::string& xpath) const;
};

} /* namespace meteo */

#endif /* _Configuration_h */

