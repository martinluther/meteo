/*
 * Dataset.h -- perform operations on Tdata objects based on the specification
 *              found in an XML node
 *
 * (c) 2003 Dr. Andreas Mueller
 *
 * $Id: Dataset.h,v 1.3 2004/02/25 23:52:34 afm Exp $
 */
#ifndef _Dataset_h
#define _Dataset_h

#include <Tdata.h>
#include <QueryResult.h>

namespace meteo {

class Dataset {
	const dmap_t&	srcdata;
	dmap_t	data;

public:
	// constructors and destructors
	Dataset(const dmap_t& sourcedata) : srcdata(sourcedata) { }
	Dataset(const QueryResult& qr) : srcdata(qr.getData()) { }
	~Dataset(void) { }

	// add a data channel based on the information inside an XML node
	void	addData(const std::string& xpath);
	void	addAlldata(const std::string& xpath);

	// access to data channels
	const Tdata&	getData(std::string& name) const;
};

} /* namespace meteo */

#endif /* _Dataset_h */
