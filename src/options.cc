/* Copyright 2006 Florian Paul Schmidt */

#include "options.h"

#include <iostream>
#include <sstream>
#include <cstdlib>

Options *Options::_instance = 0;

Options *Options::get_instance (int argc, char *argv[])
{
	if (!_instance)
	{
		_instance = new Options (argc, argv);
		return _instance;
	}
	else
	{
		std::cout << "[Options]: Error: Instance exists. Use get_instance (). Exiting..." << std::endl;
		exit (EXIT_FAILURE);
	}
}

Options *Options::get_instance ()
{
	if (!_instance)
	{
		std::cout << "[Options]: Error: Must be constructed with get_instance (argc, argv). Exiting..." << std::endl;
		exit (EXIT_FAILURE);
	}
	else
	{
		return _instance;
	}
}


Options::Options (int argc, char *argv[]) :
	_port                   (37291),
	_verbose                (0),
	_control_rate           (100),
	_control_busses         (1024),
	_control_prio           (0),
	_graphics_rate          (50),
	_graphics_busses        (10),
#ifdef HAVE_JACK
	_jack_ports             (2),
#endif
	_plugin_path         (PREFIX "/lib/scgraph/plugins"),
	_sc_plugin_path      (SC3_PLUGIN_PATH)
{
	char options[] = "u:c:v:g:f:r:p:j:s:h";

	int ret;

	while ((ret = getopt (argc, argv, options)) != -1)
	{
		switch (ret)
		{
			case 'c':
				_control_busses = atoi (optarg);
			break;

		    case 'g':
				_graphics_busses = atoi (optarg);
			break;


			case 'p':
				_control_prio = atoi (optarg);
			break;

			case 'u':
				_port = atoi (optarg);
			break;

			case 'r':
				_graphics_rate = atoi (optarg);
			break;

			case 's':
				_persistent_synthdef_filenames.push_back(optarg);
			break;

#ifdef HAVE_JACK
			case 'j':
				_jack_ports = atoi (optarg);
			break;
#endif

			case 'f':
				_control_rate = atoi (optarg);
			break;

			case 'v':
				_verbose = atoi (optarg);
			break;

			case 'h':
				print_usage ();
				exit (EXIT_SUCCESS);
			break;

			case '?':
				print_usage ();
				exit (EXIT_FAILURE);
			break;

			default:
			break;
		}
	}

	_synthdef_path = getenv ("HOME");
	_synthdef_path += "/.scgraph/synthdefs";

	_texture_path = getenv ("HOME");
	_texture_path += "/.scgraph/textures";

	_shader_path = getenv ("HOME");
	_shader_path += "/.scgraph/shaders";

	_recording_path = getenv ("HOME");
	_recording_path += "/.scgraph/recordings";


	/* check environment vars */
	char *tmp;
	if ((tmp = getenv ("SCGRAPH_SYNTHDEF_PATH")))
	{
		_synthdef_path = tmp;
	}

	if ((tmp = getenv ("SCGRAPH_TEXTURE_PATH")))
	{
		_texture_path = tmp;
	}

	if ((tmp = getenv ("SCGRAPH_SHADER_PATH")))
	{
		_shader_path = tmp;
	}

	if ((tmp = getenv ("SCGRAPH_PLUGIN_PATH")))
	{
		_plugin_path = tmp;
	}

	if ((tmp = getenv ("SCGRAPH_SC_PLUGIN_PATH")))
	{
		_sc_plugin_path = tmp;
	}

	if ((tmp = getenv ("SCGRAPH_RECORDING_PATH")))
	{
		_recording_path = tmp;
	}


	if (_verbose)
	{
		std::cout << "[Options]:"<< std::endl 
		          << "  verbose:             " << _verbose << std::endl 
		          << "  port:                " << _port << std::endl
		          << "  control_rate:        " << _control_rate << std::endl 
		          << "  control_busses:      " << _control_busses << std::endl 
		          << "  graphics_rate:       " << _graphics_rate << std::endl;
		std::cout << "  synthdef_path:       \"" << _synthdef_path << "\"" << std::endl
		          << "  sc_plugin_path:      \"" << _sc_plugin_path << "\"" << std::endl
		          << "  plugin_path:         \"" << _plugin_path << "\"" << std::endl
		          << "  texture_path:        \"" << _texture_path << "\"" << std::endl
		          << "  shader_path:        \"" << _shader_path << "\"" << std::endl;
	}
}

void Options::print_usage ()
{
	std::cout << "Usage:" << std::endl
	          << "  scgraph [options]" << std::endl
	          << "  where [options] is any of the following: " << std::endl
	          << "    -u <port>   the UDP port to listen on (default: " << _port << ")" << std::endl
	          << "    -f <rate>   set the control rate (Hz)" << std::endl
	          << "    -c <num>    set the number of control busses" << std::endl
	          << "    -g <num>    set the number of graphics busses" << std::endl
	          << "    -p <num>    set the realtime prio of the control loop (default: 0 (no RT))" << std::endl
	          << "    -r <rate>   set the graphic rate (Hz)" << std::endl
	          << "    -s <file>   create (persistent) synth from SynthDef file" << std::endl
	          << "    -v <level>  output verbosity (min: 0, max: 5)" << std::endl
	          << "    -h          show this help text" << std::endl;
	std::cout << "  ScGraph also honours these environment variables:" << std::endl
	          << "    SCGRAPH_SYNTHDEF_PATH" << std::endl
	          << "    SCGRAPH_TEXTURE_PATH" << std::endl
	          << "    SCGRAPH_SHADER_PATH (not supported yet)" << std::endl
	          << "    SCGRAPH_PLUGIN_PATH" << std::endl
	          << "    SCGRAPH_SC_PLUGIN_PATH" << std::endl
	          << "    SCGRAPH_RECORDING_PATH" << std::endl
	          << "  Specifying either overwrites the respective default path (see -v)" << std::endl;
}
