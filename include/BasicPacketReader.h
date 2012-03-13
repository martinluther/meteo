/*
 * BasicPacketReader.h -- read numeric data from a packet, this is only the
 *                        base class for a series of classes that implement
 *                        various ways to actually read data from a packet
 *
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _BasicPacketReader_h
#define _BasicPacketReader_h

#include <string>
#include <Calibrator.h>
#include <Value.h>

namespace meteo {

class BasicPacketReader {
	int		refcount;
	std::string	unit;
	std::string	classname;
	Calibrator	cal;
	BasicPacketReader(const BasicPacketReader&);
	BasicPacketReader&	operator=(const BasicPacketReader&);
protected:
	int		offset;
public:
	BasicPacketReader();
	BasicPacketReader(int o);
	virtual ~BasicPacketReader(void);

	// accessors
	void	setValueClass(const std::string& c) { classname = c; }
	const std::string&	getValueClass(void) const { return classname; }
	void	setUnit(const std::string& u) { unit = u; }
	const std::string&	getUnit(void) const { return unit; }

	void	calibrate(const Calibrator& c);

	double	operator()(const std::string& packet) const;
	virtual double	value(const std::string& packet) const = 0;
	virtual Value	v(const std::string& packet) const;
	virtual bool	valid(const std::string& packet) const = 0;

	friend class PacketReader;
};

} /* namespace meteo */

#endif /* _BasicPacketReader_h */ 

