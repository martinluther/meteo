/*
 * PacketReader.cc -- wrapper class for resource Management of packet reader
 *                    classes
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#include <PacketReader.h>
#include <MeteoException.h>
#include <mdebug.h>

namespace meteo {

// assign this packet reader the basic packet reader specified as argument
// not that this brakes resource management for the packet reader class if
// the BasicPacketReader was already assigned to something different.
PacketReader::PacketReader(BasicPacketReader *b) {
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "creating PacketReader for %s",
		b->classname.c_str());
	if (b->refcount > 0) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "BasicPacketReader already "
			"managed by some other packet reader");
		throw MeteoException("BasicPacketReader already managed by "
			"some other PacketReader", "");
	}
	bpr = b;
	bpr->refcount = 1;
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "packet reader created");
}

// void constructor, creates an empty Packet reader, will throw an exception
// if used later
PacketReader::PacketReader(void) {
	bpr = NULL;
}

// destructor, only does something if PacketReader is assigned, in which case
// it makes sure memory is freed when the last reference is destroyed
PacketReader::~PacketReader(void) {
	// check whether the Reader has been assigned
	if (bpr == NULL) {
		mdebug(LOG_INFO, MDEBUG_LOG, 0,
			"destroying unassigned PacketReader");
		return;
	}
	if (!--bpr->refcount) {
		delete bpr;
		bpr = NULL;
	}
}

// copy operations
PacketReader::PacketReader(const PacketReader& other) {
	if (other.bpr != NULL) {
		bpr = other.bpr;
		bpr->refcount++;
	}
}

PacketReader&	PacketReader::operator=(const PacketReader& other) {
	// if the other PacketReader is unassigned, we forget about our
	// own assignment
	if (other.bpr != NULL)
		other.bpr->refcount++;
	if (bpr != NULL)
		if (--bpr->refcount == 0) delete bpr;
	bpr = other.bpr;
	return *this;
}

// delegate operator() to the real packet reader
double	PacketReader::operator()(const std::string& packet) const {
	if (bpr == NULL) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "PacketReader payload is NULL");
		throw MeteoException("PacketReader has no payload", "");
	}
	return bpr->operator()(packet);
}

// retrieve a value object
Value	PacketReader::v(const std::string& packet) const {
	if (bpr == NULL) {
		mdebug(LOG_ERR, MDEBUG_LOG, 0, "PacketReader payload is NULL");
		throw MeteoException("PacketReader has no payload", "");
	}
	mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "apply (BasicPacketReader*)%p->v()",
		bpr);
	return bpr->v(packet);
}

// check for validity of value in packet
bool	PacketReader::valid(const std::string& packet) const {
	if (bpr == NULL) {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "PacketReader has no valid "
			"payload to check validity");
		throw MeteoException("cannot check validity without payload",
			"");
	}
	return bpr->valid(packet);
}

// delegate the calibration stuff to the BasicReader
void	PacketReader::calibrate(const Calibrator& cal) {
	// check for payload
	if (bpr == NULL)  {
		mdebug(LOG_DEBUG, MDEBUG_LOG, 0, "PacketReader has no valid "
			"payload to calibrate");
		throw MeteoException("cannot calibrate without payload", "");
	}

	// delegate
	bpr->calibrate(cal);
}

} /* namespace meteo */

