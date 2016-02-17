/* Copyright 2006 Florian Paul Schmidt */

#ifndef SCGRAPH_SYNTHDEFPOOL_HH
#define SCGRAPH_SYNTHDEFPOOL_HH

#include <map>
#include <string>

#include "synthdef.h"
#include "synthdef_file.h"

#include <boost/shared_ptr.hpp>

class SynthDefPool
{
	std::map <std::string, boost::shared_ptr<GSynthDef> > _synthdefs;

	public:
		SynthDefPool ();

		boost::shared_ptr<GSynthDef> get_synthdef (const std::string &name);

		/** this checks for name collisions and properly deallocates
		    the overwritten synth */
		void add_synthdef (boost::shared_ptr<GSynthDef> synthdef);

		void add_synthdef_dir (const std::string &directory);

		void d_free (const std::string &name);
		void d_freeAll ();
};

#endif
