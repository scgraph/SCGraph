/* Copyright 2006 Florian Paul Schmidt */

#ifndef SCGRAPH_SYNTHDEF_FILE
#define SCGRAPH_SYNTHDEF_FILE

#include <string>
#include <vector>

#include "synthdef.h"
#include "util.h"	

#include <boost/shared_ptr.hpp>

/** This class represents a SynthDef-File and can be constructed
    either by reading a binary blob from the file system
    or directly from a binary blob.
    The constructors throw an exception when something
    went wrong */
struct GSynthDefFile
{
	char                   _id[4];
	int32_t                _version;
	int16_t                _num_synthdefs;

	std::vector <boost::shared_ptr<GSynthDef> > _synthdefs;


	GSynthDefFile (const std::string &filename);
	GSynthDefFile (unsigned char *data, unsigned long int size);

	void construct_from_blob (unsigned char *data, unsigned long int size);
};

#endif
