//
// Lock.h -- lock a process using semaphores
//
// (c) 2004 Dr. Andreas Mueller, Beratung und Entwicklung
//
// $Id: Lock.h,v 1.1 2004/03/19 22:25:36 afm Exp $
//
#ifndef _Lock_h
#define _Lock_h

#include <MeteoException.h>
#include <Configuration.h>
#include <string>

#ifdef HAVE_SEM_H

#endif

namespace meteo {

class Lock {
	int	id;
	void	setup(const std::string& file, int max, int queue);
public:
	// create a semaphore set, use file locking on the semaphore file
	Lock(const std::string& semfile, int maxprocesses, int maxrunning);
	Lock(const Configuration& conf);
	~Lock(void);

	// enter and leave the critical region
	bool	enter(bool block);
	void	leave(void);
};

} // namespace meteo

#endif /* _Lock_h */
