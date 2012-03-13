/*
 * PacketReader.h -- wrapper class for packet reader resource management
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 */
#ifndef _PacketReader_h
#define _PacketReader_h

#include <BasicPacketReader.h>

namespace meteo {

class PacketReader {
	BasicPacketReader	*bpr;
	PacketReader(BasicPacketReader *b);
public:
	PacketReader(void);
	~PacketReader(void);
	PacketReader(const PacketReader& other);
	PacketReader&	operator=(const PacketReader& other);

	double	operator()(const std::string& packet) const;
	Value	v(const std::string& packet) const;
	bool	valid(const std::string& packet) const;
	void	calibrate(const Calibrator& cal);

	friend class PacketReaderFactory;
};

} /* namespace meteo */

#endif /* _PacketReader_h */
