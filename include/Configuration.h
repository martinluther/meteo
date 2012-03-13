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
	std::string	getString(const std::string& xpath, const std::string& def) const;
	std::vector<std::string>	getStringVector(const std::string& xpath) const;
};

} /* namespace meteo */

#endif /* _Configuration_h */

