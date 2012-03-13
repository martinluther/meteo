/*
 * Datasink.cc -- data sink class: where do we send our data
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <Datasink.h>
#include <mdebug.h>
#include <MeteoException.h>

namespace meteo {

//////////////////////////////////////////////////////////////////////
// Datasink base class methods
//////////////////////////////////////////////////////////////////////
void	Datasink::receive(const std::string& query) {
	mdebug(LOG_INFO, MDEBUG_LOG, 0, "update query: %s", query.c_str());
}

//////////////////////////////////////////////////////////////////////
// FileDatasink (no additional methods)
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// MysqlDatasink
//////////////////////////////////////////////////////////////////////
MysqlDatasink::MysqlDatasink(const Configuration& conf) {
	mysql_init(&mysql);
	// connect and authenticate
	std::string	host = conf.getDBHostname();
	std::string	dbname = conf.getDBName();
	std::string	user = conf.getDBUser();
	std::string	pw = conf.getDBPassword();
	if (NULL == mysql_real_connect(&mysql, host.c_str(), user.c_str(),
		pw.c_str(), dbname.c_str(), 0, NULL, 0)) {
		throw MeteoException("cannot create database",
			mysql_error(&mysql));
	}
}

MysqlDatasink::~MysqlDatasink(void) {
	mysql_close(&mysql);
}

void	MysqlDatasink::receive(const std::string& query) {
	int	res = mysql_query(&mysql, query.c_str());
	if (res) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "mysql query failed: %s",
			mysql_error(&mysql));
		throw MeteoException("mysql query failed", mysql_error(&mysql));
	}
}

//////////////////////////////////////////////////////////////////////
// QueueDatasink
//////////////////////////////////////////////////////////////////////
QueueDatasink::QueueDatasink(const std::string& queuename) {
	mq = msgque_setup(queuename.c_str());
	if (NULL == mq) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "cannot setup msg queue");
		throw MeteoException("cannot setup msg queue", "");
	}
}
QueueDatasink::~QueueDatasink(void) {
	msgque_cleanup(mq);
}
void	QueueDatasink::receive(const std::string& query) {
	msgque_sendquery(mq, query.c_str(), query.length() + 1);
}

//////////////////////////////////////////////////////////////////////
// DatasinkFactory
//////////////////////////////////////////////////////////////////////
class dsf {
	Configuration			configuration;
	std::vector<std::string>	preferences;
public:
	dsf(const Configuration& conf, const std::vector<std::string>& pref) {
		configuration = conf;
		preferences = pref;
	}
	~dsf(void) { }
	void	setConfiguration(const Configuration& conf) {
		configuration = conf;
	}
	void	setPreferences(const std::vector<std::string>& pref) {
		preferences = pref;
	}
	const Configuration& getConfiguration(void) {
		return configuration;
	}
	const std::vector<std::string>& getPreferences(void) {
		return preferences;
	}
};
dsf	*DatasinkFactory::d = NULL;
DatasinkFactory::DatasinkFactory(void) {
	if (NULL == d) {
		throw MeteoException(
			"Datasink Factory not correctly initialized", "");
	}
}
DatasinkFactory::DatasinkFactory(const Configuration& conf) {
	std::vector<std::string>	defprefs;
	defprefs.push_back("msgqueue");
	defprefs.push_back("mysql");
	defprefs.push_back("file");
	defprefs.push_back("debug");
	d = new dsf(conf, std::vector<std::string>());
}
DatasinkFactory::DatasinkFactory(const Configuration& conf,
	const std::vector<std::string>& prefs) {
	d = new dsf(conf, prefs);
}
DatasinkFactory::~DatasinkFactory(void) {
}
const Configuration&	DatasinkFactory::getConfiguration(void) const {
	return d->getConfiguration();
}
const std::vector<std::string>&	DatasinkFactory::getPreferences(void) const {
	return d->getPreferences();
}

Datasink	*DatasinkFactory::newDatasink(const std::string& name) const {
	// make sure we do have some preferences
	const std::vector<std::string>	preferences = getPreferences();
	if (0 == preferences.size()) {
		throw MeteoException("no Datasink preferences set", "");
	}

	// go through the preferences vector
	std::vector<std::string>::const_iterator	i;
	for (i = preferences.begin(); i != preferences.end(); i++) {
		// create a message queue datasink
		if ("msgqueue" == *i) {
			std::string	msgqueue = getConfiguration()
							.getDBMsgqueue();
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
				return new MysqlDatasink(getConfiguration());
			} catch (MeteoException e) {
				mdebug(LOG_ERR, MDEBUG_LOG, 0,
					"cannot connect to database");
			}
		}
		// create a file datasink
		if ("file" == *i) {
			std::string	filename = getConfiguration()
							.getDBUpdatefile();
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
