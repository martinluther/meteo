/*
 * Datasink.h -- unified object the meteo programs can use to send database
 *               updates to
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Datasink.h,v 1.7 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _Datasink_h
#define _Datasink_h

#include <MeteoTypes.h>
#include <fstream>
#include <iostream>
#include <QueryProcessor.h>
#include <MsgQueuer.h>

namespace meteo {

class	Datasink {
public:
	Datasink(void) { }
	virtual	~Datasink(void) { }
	virtual void	receive(const std::string& query);
	virtual void	receive(const stringlist queries);
};

class	FileDatasink : public Datasink {
	std::ofstream	file;
public:
	FileDatasink(std::string& filename) : file(filename.c_str()) { }
	virtual	~FileDatasink(void) { }
	virtual void	receive(const std::string& query) {
		file << query << ";" << std::endl;
	}
};

class	MysqlDatasink : public Datasink {
	QueryProcessor	qp;
public:
	MysqlDatasink(void);
	virtual ~MysqlDatasink(void);
	virtual void	receive(const std::string& query);
};

class	QueueDatasink : public Datasink {
	MsgQueuer	mq;
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
	DatasinkFactory(const stringlist& prefs);
	virtual ~DatasinkFactory(void);
	void	setPreferences(const stringlist& prefs);
	const stringlist&	getPreferences(void) const;
	Datasink	*newDatasink(const std::string& name) const;
};

} /* namespace meteo */

#endif /* _Datasink_h */
