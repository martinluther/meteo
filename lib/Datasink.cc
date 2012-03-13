/*
 * Datasink.cc -- data sink class: where do we send our data
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Datasink.cc,v 1.9 2009/01/10 19:00:23 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <Datasink.h>
#include <Configuration.h>
#include <mdebug.h>
#include <MeteoException.h>
#include <MsgQueuer.h>

namespace meteo {

//////////////////////////////////////////////////////////////////////
// Datasink base class methods
//////////////////////////////////////////////////////////////////////
void	Datasink::receive(const std::string& query) {
	mdebug(LOG_INFO, MDEBUG_LOG, 0, "update query: %s", query.c_str());
}

void	Datasink::receive(stringlist queries) {
	stringlist::const_iterator	i;
	for (i = queries.begin(); i != queries.end(); i++) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "sending query '%s' to db",
			i->c_str());
		// use the receive method, make sure virual methods get
		// called!
		this->receive(*i);
	}
}

//////////////////////////////////////////////////////////////////////
// FileDatasink (no additional methods)
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// MysqlDatasink
//////////////////////////////////////////////////////////////////////
MysqlDatasink::MysqlDatasink(void) : qp(true) {
}

MysqlDatasink::~MysqlDatasink(void) {
}

void	MysqlDatasink::receive(const std::string& query) {
	qp(query);
}

//////////////////////////////////////////////////////////////////////
// QueueDatasink
//////////////////////////////////////////////////////////////////////
QueueDatasink::QueueDatasink(const std::string& queuename)
	: mq(queuename) {
}
QueueDatasink::~QueueDatasink(void) {
}
void	QueueDatasink::receive(const std::string& query) {
	mq(query);
}

//////////////////////////////////////////////////////////////////////
// DatasinkFactory
//////////////////////////////////////////////////////////////////////
class dsf {
	stringlist	preferences;
public:
	dsf(const stringlist& pref) : preferences(pref) { }
	~dsf(void) { }
	void	setPreferences(const stringlist& pref) {
		preferences = pref;
	}
	const stringlist& getPreferences(void) {
		return preferences;
	}
};
dsf	*DatasinkFactory::d = NULL;
DatasinkFactory::DatasinkFactory(void) {
	if (NULL != d) return;
	stringlist	defprefs;
	defprefs.push_back("msgqueue");
	defprefs.push_back("mysql");
	defprefs.push_back("file");
	defprefs.push_back("debug");
	d = new dsf(stringlist());  // delete ~DatasinkFactory
}
DatasinkFactory::DatasinkFactory(const stringlist& prefs) {
	d = new dsf(prefs);	// delete ~DatasinkFactory
}
DatasinkFactory::~DatasinkFactory(void) {
	delete d; d = NULL;
}
const stringlist&	DatasinkFactory::getPreferences(void) const {
	return d->getPreferences();
}

Datasink	*DatasinkFactory::newDatasink(const std::string& name) const {
	Configuration	conf;
	// make sure we do have some preferences
	const stringlist	preferences = getPreferences();
	if (0 == preferences.size()) {
		throw MeteoException("no Datasink preferences set", "");
	}

	// go through the preferences list
	stringlist::const_iterator	i;
	for (i = preferences.begin(); i != preferences.end(); i++) {
		// create a message queue datasink
		if ("msgqueue" == *i) {
			std::string	msgqueue = conf.getDBMsgqueue();
			try {
				mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"trying msg queue");
				return new QueueDatasink(msgqueue);
			} catch (MeteoException& e){
				mdebug(LOG_ERR, MDEBUG_LOG, 0,
					"setting up msg queue failed");
			}
		}
		// create a mysql database datasink
		if ("mysql" == *i) {
			try {
				mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
					"trying mysql database");
				return new MysqlDatasink();
			} catch (MeteoException& e) {
				mdebug(LOG_ERR, MDEBUG_LOG, 0,
					"cannot connect to database");
			}
		}
		// create a file datasink
		if ("file" == *i) {
			std::string	filename = conf.getDBUpdatefile();
			try {
				mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "trying file");
				return new FileDatasink(filename);
			} catch (...) {
				mdebug(LOG_ERR, MDEBUG_LOG, 0,
					"cannot open update file");
			}
		}
		// use the debug log for the data sink (base class)
		if ("debug" == *i) {
			mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "using debug log");
			return new Datasink();
		}
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "no datasink created");
	throw MeteoException("no datasink created", "bad preferences?");
}

} /* namespace meteo */
