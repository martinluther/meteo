/*
 * Datasink.h -- unified object the meteo programs can use to send database
 *               updates to
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _Datasink_h
#define _Datasink_h

#include <Configuration.h>
#include <string>
#include <fstream>
#include <iostream>
#include <mysql/mysql.h>
#include <msgque.h>

namespace meteo {

class	Datasink {
public:
	Datasink(void) { }
	virtual	~Datasink(void) { }
	virtual void	receive(const std::string& query);
};

class	FileDatasink : public Datasink {
	std::ofstream	file;
public:
	FileDatasink(std::string& filename) : file(filename.c_str()) { }
	virtual	~FileDatasink(void) { }
	virtual void	receive(const std::string& query) {
		file << query << std::endl;
	}
};

class	MysqlDatasink : public Datasink {
	MYSQL	mysql;
public:
	MysqlDatasink(const Configuration& conf);
	virtual ~MysqlDatasink(void);
	virtual void	receive(const std::string& query);
};

class	QueueDatasink : public Datasink {
	msgque_t	*mq;
public:
	QueueDatasink(const std::string& queuename);
	virtual	~QueueDatasink(void);
	virtual void	receive(const std::string& query);
};

class	dsf;

class	DatasinkFactory {
static dsf	*d;
public:
	DatasinkFactory(void);
	DatasinkFactory(const Configuration& conf);
	DatasinkFactory(const Configuration& conf,
		const std::vector<std::string>& prefs);
	virtual ~DatasinkFactory(void);
	void	setConfiguration(const Configuration& conf);
	const Configuration&	getConfiguration(void) const;
	void	setPreferences(const std::vector<std::string>& prefs);
	const std::vector<std::string>&	getPreferences(void) const;
	Datasink	*newDatasink(const std::string& name) const;
};

} /* namespace meteo */

#endif /* _Datasink_h */
