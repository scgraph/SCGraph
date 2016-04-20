/* Copyright 2006 Florian Paul Schmidt */

#include "synthdef_file.h"

#include "scgraph.h"

#include <cstdio>
#include <cstring>
#include <iostream>

GSynthDefFile::GSynthDefFile (const std::string &filename)
{
	Options *options = Options::get_instance ();

	if (options->_verbose >= 3)
		std::cout << "[GSynthDefFile]: Reading synthdef file: " << filename << std::endl;

	/* load the file into a chunk of memory and call the other constructor */
	FILE *deffile = fopen (filename.c_str (), "rb");
	if (!deffile)
	{
		throw (std::runtime_error("[GSynthDefFile]: Couldn't open file for reading"));
	}

	// FIXME: do this properly, allocating as much mem as needed
	unsigned char *tmp = new unsigned char [1024*1024];
	size_t num = fread (tmp, sizeof(char), 1024*1024, deffile);

	construct_from_blob (tmp, num);

	delete[] tmp;
	fclose(deffile);
}

GSynthDefFile::GSynthDefFile (unsigned char *data, unsigned long int size)
{
	construct_from_blob (data, size);
}

void GSynthDefFile::construct_from_blob (unsigned char *data, unsigned long int size)
{
	Options *options = Options::get_instance ();
	
	if (size < 10)
		throw (std::runtime_error("[GSynthDefFile]: Malformed synthdef file"));

	if (options->_verbose >= 3)
	{
		std::cout << "[GSynthDefFile]: Constructing from blob" << std::endl;
		std::cout << " size: " << size << std::endl;
	}

	unsigned char *orig_data = data;

	memcpy (_id, data, 4);
	data += 4;

	_version = read_int32_t (&data);

	_num_synthdefs = read_int16_t (&data);

	// FIXME: do checks!!
	if (options->_verbose >= 3)
	{
		std::cout << " id: \"" << *_id << *(_id+1) << *(_id+2) << *(_id+3) << "\"" << std::endl;
		std::cout << " version: " << _version << std::endl;
		std::cout << " num_synthdefs: " << _num_synthdefs << std::endl;
	}

	for (int16_t i = 0; i < _num_synthdefs; ++i)
	{
        try {
            boost::shared_ptr<GSynthDef> synthdef = boost::shared_ptr<GSynthDef>(new GSynthDef(&data, size - (data-orig_data)));
            _synthdefs.push_back (synthdef);
        }
        catch (const std::exception& ex) {
            std::cout << "[SynthDefFile]: Error: " << ex.what() << std::endl;
        }
	}
}


