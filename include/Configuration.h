/*
 * Configuration.h -- read configuration file and serve configuration info to
 *                    interested clients
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Configuration_h
#define _Configuration_h

#include <string>
#include <vector>
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
	std::vector<double>	getDoubleVector(const std::string& xpath) const;
	int	getInt(const std::string& xpath, int def) const;
	std::vector<int>	getIntVector(const std::string& xpath) const;
	std::string	getString(const std::string& xpath,
				const std::string& def) const;
	std::vector<std::string>	getStringVector(const std::string& xpath) const;
	bool	getBool(const std::string& xpath, bool def = false) const;
	bool	xpathExists(const std::string& xpath) const;

	// some convenience methods to retrieve standard parameters from
	// the configuration file
	std::string	getTemperatureUnit(void) const;
	std::string	getHumidityUnit(void) const;
	std::string	getPressureUnit(void) const;
	std::string	getRainUnit(void) const;
	std::string	getWindUnit(void) const;
	std::string	getSolarUnit(void) const;
	std::string	getUVUnit(void) const;

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

	// low level primitives, primarily used for the Dataset stuff where
	// we want to perform operations on data according to the XML tree
	const xmlNodePtr	getNode(const std::string& xpath) const;
	const std::vector<xmlNodePtr>	getNodeVector(const std::string& xpath) const;
};

} /* namespace meteo */

#endif /* _Configuration_h */

