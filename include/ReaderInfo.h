/*
 * ReaderInfo.h -- information about readers
 *
 * (c) 2003 Dr. Andreas Mueller, Beratung und Entwicklung
 *
 * $Id: ReaderInfo.h,v 1.3 2009/01/10 19:32:20 afm Exp $
 */
#ifndef _ReaderInfo_h
#define _ReaderInfo_h

#include <MeteoTypes.h>

namespace meteo {

struct stationreaders_s {
	const char	*readername;	// the name of the reader, of the form
					// sensorstationname.mfieldname
	const char	*classname;	// the reader class that is supposed to 
					// handle this field
	int		param1;		// usually offset and length of the data
	int		param2;		// to be read from the packet, may use
					// other meanings for other stations
	const char	*readerunit;	// the unit found in the packet, the
					// reader generates Values of this unit,
					// the Recorders will have to convert
					// to database units
};
typedef struct stationreaders_s	stationreaders_t;

class ReaderInfo {
public:
	static stationreaders_t	*getReaders(const std::string& stationtype);
	static stringlist	knownTypes(void);
};

}

#endif /* _ReaderInfo_h */
