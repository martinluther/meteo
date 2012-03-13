//
// Lock.cc -- lock a process using semaphores
//
// (c) 2004 Dr. Andreas Mueller, Beratung und Entwicklung
//
// $Id: Lock.cc,v 1.4 2004/03/20 08:45:06 afm Exp $
//
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <Lock.h>
#include <fcntl.h>
#include <string>
#include <mdebug.h>
#include <errno.h>
#ifdef HAVE_SYS_SEM_H
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#endif /* HAVE_SYS_SEM_H */
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#endif /* HAVE_ALLOCA_H */

#ifdef HAVE_SYS_SEM_H
#ifdef HAVE_SEMUN
/* semun already defined by including sys/sem.h 			*/
#else /* HAVE_SEMUN */
/* according to X/OPEN, we have to define it ourselves 			*/
union semun	{
	int	val;
	struct semid_ds	*buf;
	unsigned short	*array;
	struct seminfo	*__buf;
};
#endif /* HAVE_SEMUN */
#endif /* HAVE_SYS_SEM_H */

namespace meteo {

void	Lock::setup(const std::string& file, int max, int queue) {
#ifdef HAVE_SYS_SEM_H
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "semaphore based locking");
	/* use the file to lock the semaphore, we have to do this	*/
	/* because System V semaphores have no atomic constructor	*/
	int	mask = umask(0);
	int	f = open(file.c_str(), O_RDWR|O_CREAT, 0666);
	umask(mask);
	if (f < 0) {
		throw MeteoException("cannot create lock file", file);
	}
	struct flock	sf;
	sf.l_type = F_WRLCK;
	sf.l_start = 0;
	sf.l_whence = SEEK_SET;
	sf.l_len = 1;
	if (fcntl(f, F_SETLKW, &sf) < 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "could not obtain lock");
		close(f);
		return;
	}

	/* convert the filename to a semaphore id			*/
	key_t	key = ftok(file.c_str(), 1);
	if (key == (key_t)-1) {
		close(f);
		throw MeteoException("cannot convert file name", file);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "IPC key for semaphore: %d", key);

	/* get the semaphore set, create it if it does not exist yet	*/
	id = semget(key, 2, 0);
	if (id < 0) {
		// handle other errors
		if (errno != ENOENT) {
			close(f);
			mdebug(LOG_ERR, MDEBUG_LOG, 0, "cannot find semaphore "
				"set: %s", strerror(errno));
			throw MeteoException("cannot find semaphore set",
				strerror(errno));
		}
		// create the semaphore set
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creating semaphore set "
			"for key %d", key);
		id = semget(key, 2, 0666 | IPC_CREAT);
		if (id < 0) {
			mdebug(LOG_ERR, MDEBUG_LOG, 0, "cannot create "
				"sempahore set: %s", strerror(errno));
			throw MeteoException("cannot create semaphore set",
				strerror(errno));
		}
		/* initialize the semaphore set				*/
		union semun	s;
		s.val = queue;
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "setting queue to %hd",
			s.val);
		if (semctl(id, 0, SETVAL, s) < 0) {
			mdebug(LOG_ERR, MDEBUG_LOG, 0, "cannot set queue "
				"length: %s (%d)", strerror(errno), errno);
			close(f);
		}
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "created queue of length %d",
			queue);
		s.val = max;
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "setting run slots to %hd",
			s.val);
		if (semctl(id, 1, SETVAL, s) < 0) {
			mdebug(LOG_ERR, MDEBUG_LOG, 0, "cannot set run "
				"length: %s (%d)", strerror(errno), errno);
			close(f);
		}
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "created %d run slots", max);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "semaphore id = %d", id);

	/* release the lock						*/
	close(f); // implicitely releases lock
#else /* HAVE_SYS_SEM_H */
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "file based locking: %s",
		file.c_str());
	int	mask = umask(0);
	id = open(file.c_str(), O_RDWR|O_CREAT, 0666);
	umask(mask);
	if (id < 0) {
		throw MeteoException("cannot create lock file", file);
	}
#endif /* HAVE_SYS_SEM_H */
}

Lock::Lock(const std::string& semfile, int maxprocesses, int maxrunning) {
	setup(semfile, maxrunning, maxprocesses);
}

Lock::Lock(const Configuration& conf) {
	// retrieve info from configuration file
	std::string	file = conf.getString("/meteo/graphs/@lockfile",
		"/var/run/meteodraw.lock");
	int	maxprocesses = conf.getInt("/meteo/graphs/@queued", 10);
	int	maxrunning = conf.getInt("/meteo/graphs/@running", 2);

	// create lock object from configuration data
	setup(file, maxrunning, maxprocesses);
}

Lock::~Lock(void) {
#ifdef HAVE_SYS_SEM_H
	// nothing needs to be done
#else /* HAVE_SYS_SEM_H */
	close(id);
#endif /* HAVE_SYS_SEM_H */
}

bool	Lock::enter(bool block) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "waiting to enter critical region");
#ifdef HAVE_SYS_SEM_H
	/* first try to get into the queue				*/
	struct sembuf	sb;
	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = IPC_NOWAIT | SEM_UNDO;
	if (semop(id, &sb, 1) < 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "no remaining queue slots: "
			"%s (%d)", strerror(errno), errno);
		return false;
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "entered into queue");

	/* now try to become running					*/
	sb.sem_num = 1;
	sb.sem_op = -1;
	sb.sem_flg = SEM_UNDO | ((block) ? 0 : IPC_NOWAIT);
	if (semop(id, &sb, 1) < 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "cannot get into run queue: "
			"%s (%d)", strerror(errno), errno);
		throw MeteoException("cannot get into run queue",
			strerror(errno));
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "became running");

	/* retrieve the values for the two semaphores			*/
	union semun	s;
	s.array = (unsigned short *)alloca(2 * sizeof(unsigned short));
	if (semctl(id, 2, GETALL, s) < 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "cannot get semaphore "
			"values: %s (%d)", strerror(errno), errno);
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "remaining queue slots: %hd, "
		"remaining  run slots: %hd", s.array[0], s.array[1]);

#else /* HAVE_SYS_SEM_H */
	struct flock	sf;
	sf.l_type = F_WRLCK;
	sf.l_start = 0;
	sf.l_whence = SEEK_SET;
	sf.l_len = 1;
	if (fcntl(id, (block) ? F_SETLKW : F_SETLK, &sf) < 0) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "could not obtain lock");
		return false;
	}
#endif /* HAVE_SYS_SEM_H */
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "entering critical region");
	return true;
}

void	Lock::leave(void) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "leaving critical region");
#ifdef HAVE_SYS_SEM_H
	struct sembuf	sb[2];
	sb[0].sem_num = 0; sb[1].sem_num = 1;
	sb[0].sem_op = 1; sb[1].sem_op = 1;
	sb[0].sem_flg = SEM_UNDO; sb[1].sem_flg = SEM_UNDO;
	if (semop(id, sb, 2) < 0) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "cannot release sempahore: %s",
			strerror(errno));
		throw MeteoException("cannot release semaphore",
			strerror(errno));
	}
#else /* HAVE_SYS_SEM_H */
	struct flock	sf;
	sf.l_type = F_UNLCK;
	sf.l_start = 0;
	sf.l_whence = SEEK_SET;
	sf.l_len = 1;
	fcntl(id, F_UNLCK, &sf);
#endif /* HAVE_SYS_SEM_H */
}

} // namespace meteo
