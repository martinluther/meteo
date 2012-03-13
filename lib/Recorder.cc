/*
 * Recorder.cc -- reference counted Recorder class that encapsulates all the
 *             Recorder classes but still gives us the benefit of resource
 *             management
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: Recorder.cc,v 1.11 2009/01/10 19:00:25 afm Exp $
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */
#include <Recorder.h>
#include <MeteoException.h>
#include <mdebug.h>

namespace meteo {

// assign a BasicRecorder to a recorder, not that this breaks the reference
// count if the BasicRecorder was already assigned, which is the reason why
// this constructor is private. It also does not set the title and classname
// attributes, or the field id
Recorder::Recorder(BasicRecorder *v) {
	bv = v;
	bv->refcount = 1;
}

// this is just a dummy constructor(void). It will cause a segmentation
// fault if nothing is done to properly initialize the recorder later
Recorder::Recorder(void) {
	bv = NULL;
}

void	Recorder::copy(const Recorder& other) {
	bv = other.bv;
	id = other.id;
	title = other.title;
	classname = other.classname;
	hasmin = other.hasmin;
	hasmax = other.hasmax;
}

Recorder::Recorder(const Recorder& other) {
	copy(other);
	bv->refcount++;
}

// the assignment operator has to make sure that it does not access the bv
// field if it is invalid, or it would not be possible to assign a completely
// initialized Recorder to an uninitilized one.
Recorder&	Recorder::operator=(const Recorder& other) {
	if (NULL == other.bv)
		throw MeteoException("trying to assign uninitilized Recorder",
			"");
	other.bv->refcount++;
	// change own value only if initialized
	if (bv != NULL)
		if (--bv->refcount == 0) delete bv;
	copy(other);
	return *this;
}

// perform resource management on deallocation of a Recorder
Recorder::~Recorder(void) {
	// ignore incompletely initialized Recorders
	if (NULL == bv) return;
	// destroy BasicRecorder at refcount == 0
	if (!--bv->refcount) {
		delete bv;
		bv = NULL;
	}
}

// updating the value depends on the type of the Value and the basic recorder
// within the Recorder
void	Recorder::update(const Value& other) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0,
		"updating %s recorder with %s value %f",
		this->getValueClass().c_str(), other.getClass().c_str(),
		other.getValue());

	// check whether the other value and the present are of the same class
	// we need to go through the this pointer or we may not pick up the
	// virtual function
	if (other.getClass() != this->getValueClass()) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "different class: %s != %s",
			other.getClass().c_str(),
			this->getValueClass().c_str());
		throw MeteoException("cannot update with different class",
			other.getClass());
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "recorder class matches value class");

	// call the update method of the basic recorder, this may be a virtual
	// function 
	if (NULL == bv) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "no basic recorder stuff");
		throw MeteoException("no basic recorder stuff",
			this->getValueClass());
	}
	bv->update(other);
}

// deleget all the other methods to the BasicRecorder payload class
void	Recorder::sendOutlet(Outlet *outlet, const time_t timekey,
		const int sensorid) const {
	bv->sendOutlet(outlet, timekey, sensorid, getId());
}

std::string	Recorder::plain(void) const {
	return title + bv->plain();
}
std::string	Recorder::xml(void) const {
	return bv->xml(name);
}

} /* namespace meteo */
