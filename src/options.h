/* Copyright 2006 Florian Paul Schmidt */

#ifndef SCGRAPH_OPTIONS_HH
#define SCGRAPH_OPTIONS_HH

#include <string>
#include <vector>
#include <getopt.h>

extern char *optarg;
extern int optind, opterr, optopt;

class Options
{
	static Options *_instance;
	// methods
	public:
		static Options *get_instance (int argc, char *argv[]);
		static Options *get_instance ();

	protected:
		Options (int argc, char *argv[]);

		void print_usage ();

	// members
	public:
		unsigned int  _port;

		int           _verbose;

		unsigned int  _control_rate;
		unsigned int  _control_busses;
		unsigned int  _control_prio;

		unsigned int  _graphics_rate;
		unsigned int  _graphics_busses;

		std::vector<std::string>
		              _persistent_synthdef_filenames;

#ifdef HAVE_JACK
		unsigned int  _jack_ports;
#endif

		std::string   _synthdef_path;
		std::string   _plugin_path;
		std::string   _sc_plugin_path;
		std::string   _texture_path;
		std::string   _shader_path;
		std::string   _recording_path;
};

#endif
