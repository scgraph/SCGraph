/* Copyright 2006 Florian Paul Schmidt */

#include "synthdef_pool.h"

#include <iostream>

#include <sys/types.h>
#include <dirent.h>

#include "scgraph.h"

SynthDefPool::SynthDefPool ()
{
	Options *options = Options::get_instance ();

	add_synthdef_dir (options->_synthdef_path);
}

boost::shared_ptr<GSynthDef> SynthDefPool::get_synthdef (const std::string &name)
{
	if (_synthdefs.find (name) == _synthdefs.end ())
	{
		throw ("[SynthDefPool]: Error: SynthDef not found!");
	}
	return _synthdefs [name];
}

void SynthDefPool::d_free (const std::string &name)
{
	if (_synthdefs.find (name) == _synthdefs.end ())
	{
		throw ("[SynthDefPool]: Error: SynthDef not found!");
	}
	_synthdefs.erase (name);	
}

void SynthDefPool::d_freeAll ()
{
	// Simply replace the map with a new one. The shared_ptr's take care of the rest
	_synthdefs = std::map<std::string, boost::shared_ptr<GSynthDef> >();
}

void SynthDefPool::add_synthdef (boost::shared_ptr<GSynthDef> synthdef)
{
	Options *options = Options::get_instance ();

	if (options->_verbose >= 2)
		std::cout << "[SynthDefPool]: Adding SynthDef: " << synthdef->_name << std::endl;

	if (_synthdefs.find (synthdef->_name) != _synthdefs.end ())
	{
		if (options->_verbose >= 2)
			std::cout << "[SynthDefPool]: Exists. Overwriting..." << std::endl;

		_synthdefs.erase (synthdef->_name);
	}

	_synthdefs [synthdef->_name] = synthdef;
}

void SynthDefPool::add_synthdef_dir (const std::string &directory)
{
	Options *options = Options::get_instance ();

	/* first we check whether our synthdef dir exists. */
	DIR *dir = opendir (directory.c_str ());
	if (!dir)
	{
		if (options->_verbose >= 1)
			std::cout << "[SynthDefPool]: SynthDef directory not found. Trying to create it (\"" << directory << "\")" << std::endl;

		std::string command = std::string ("install -d ") += directory.c_str ();

		if (!(system (command.c_str ()) == 0))
		{
			std::cout << "[SynthDefPool]: Error: Couldn't create synthdef directory (\"" << directory << "\"). Exiting..." << std::endl;
			exit (EXIT_FAILURE);
		}
	}

	if (options->_verbose >= 2)
		std::cout << "[SynthDefPool]: Loading SynthDefs in directory: \"" << directory << "\"" << std::endl;

	/* then we scan the directory recursively */
	struct dirent *dirent;
	while ((dirent = readdir(dir)))
	{
		if ((std::string (dirent->d_name) == std::string (".")) || (std::string (dirent->d_name) == std::string ("..")))
			continue;
 
		if (dirent->d_type == DT_DIR)
			add_synthdef_dir (directory + "/" + std::string (dirent->d_name));

		else
		{
			std::string filename = directory + "/" + std::string (dirent->d_name);

			try
			{
				GSynthDefFile file (filename);
				for (size_t i = 0; i < file._synthdefs.size (); ++i)
					add_synthdef (file._synthdefs [i]);
			}
			catch (const char *s)
			{
				std::cout << "[SynthDefPool]: Error: " << s << std::endl;
			}
		

		}
	}

	closedir (dir);
}

