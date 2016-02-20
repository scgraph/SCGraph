/* Copyright 2006 Florian Paul Schmidt */

#include "scgraph.h"

// TODO: remove this test code
#include "synthdef_pool.h"

#include <iostream>
#include <stdexcept>

#include "ofThread.h"

ScGraph *ScGraph::_instance = 0;


ScGraph::ScGraph (int argc, char *argv[])
{
#ifdef HAVE_JACK
	try {
		_jack_client = boost::shared_ptr<JackClient> (new JackClient (Options::get_instance()->_jack_ports, "scgraph"));
	} catch (...) {
		_jack_client = boost::shared_ptr<JackClient>();
	}
#endif

	Options *options = Options::get_instance ();

	// _jack_client  = new JackClient (2, "scgraph");

	_control_busses.resize (options->_control_busses);

	for (size_t i = 0; i < options->_graphics_busses; ++i)
	{
		_past_graphics_busses.push_back (GraphicsBus());
		_graphics_busses.push_back (GraphicsBus());
	}

	// Create persistent synths
	for (size_t i = 0; i < options->_persistent_synthdef_filenames.size(); ++i)
	{
		try {
			std::cout << "[ScGraph]: Creating persistent synth from synthdef file: " << options->_persistent_synthdef_filenames[i] << std::endl;
			GSynthDefFile file(options->_persistent_synthdef_filenames[i]);
			boost::shared_ptr<GSynth> synth(new GSynth(file._synthdefs[0], -2 - i));
			_persistent_synths.push_back(synth);
		} catch (...) {
			std::cout << "[ScGraph]: Error creating persistent synth from synthdef file: " << options->_persistent_synthdef_filenames[i] << std::endl;
		}
	}

	_delta_t = 1.0/options->_graphics_rate;
}

ScGraph* ScGraph::get_instance ()
{
	if (!_instance)
	{
		std::cout << "[ScGraph]: Error: Must be constructed with get_instance (argc, argv). Exiting..." << std::endl;
		exit (EXIT_FAILURE);
	}
	else
	{
		return _instance;
	}
}

ScGraph* ScGraph::get_instance (int argc, char *argv[])
{
	if (!_instance)
	{
		try
		{
			_instance = new ScGraph (argc, argv);
			return _instance;
		}
                catch (const std::exception& ex) {
                    std::cout << "[ScGraph]: Error: Failed to create instance of ScGraph. Reason: " << ex.what() << std::endl;
                    exit (EXIT_FAILURE);
		}
	}
	else
	{
		return _instance;
	}
}

ScGraph::~ScGraph ()
{
	// TODO delete _jack_client;
    
	_instance = 0;
}

void ScGraph::stop ()
{
    lock_for_write();


	_osc_handler.stop ();
	_control_loop.stop ();
	_graphic_loop.stop ();

    unlock();
}

void ScGraph::start ()
{
    lock_for_write();
    // start the threads
	//_graphic_loop.startThread(true); // blocking
	//_control_loop.startThread(true);
    _osc_handler.startThread(true);

    unlock();
}

void ScGraph::run_one_control_cycle (double delta_t)
{
	lock_for_read();

#if 0
	// FIXME: dirty hack to test jack stuff
#ifdef HAVE_JACK
	_control_busses [0] = _jack_client->get_frequency (0, 30);
	_control_busses [1] = _jack_client->get_frequency (0, 1000);
	_control_busses [2] = _jack_client->get_frequency (0, 7000);
#endif
#endif
	

	for (Tree::Tree<NodePtr>::Iterator it = _node_tree.begin (); it != _node_tree.end (); ++it)
	{
		GSynth *synth = dynamic_cast<GSynth*>((*it).get ());
		if (synth == 0)
			continue;

		synth->process_c (delta_t);

		if (synth->get_done_action () != 0)
		{
			_done_actions.push_back(std::pair<int, int>(synth->_id, synth->get_done_action()));
		}
	}
	unlock();

	lock_for_read();

	for (size_t i = 0; i < _persistent_synths.size(); ++i)
		_persistent_synths[i]->process_c(delta_t);

	unlock();

}


void ScGraph::run_one_graphics_cycle (double delta_t)
{
	_delta_t = delta_t;

	lock_for_read();
    std::cout << "graphics cycle" << std::endl;

	for (size_t i = 0; i < _past_graphics_busses.size (); ++i)
	{
		_past_graphics_busses[i].clear ();
	}
	_past_graphics_busses.swap(_graphics_busses);

	for (Tree::Tree<NodePtr>::Iterator it = _node_tree.begin (); it != _node_tree.end (); ++it)
	{
		(*it)->process_g (delta_t);
	}
    std::cout << "graphics cycle done" << std::endl;

	unlock();

	/* do the doneActions */
	lock_for_write();
    std::cout << "scgraph: done_action" << std::endl;
	for 
		(
		 std::vector<std::pair<int, int> >::iterator it = _done_actions.begin();
		 it != _done_actions.end (); 
		 ++it
		 ) {
		
		_node_tree.done_action ((*it).first, (*it).second);
	}
	
	_done_actions.clear ();
    std::cout << "scgraph: done_action done" << std::endl;
	unlock();

	lock_for_read();

	for (size_t i = 0; i < _persistent_synths.size(); ++i)
		_persistent_synths[i]->process_g(delta_t);

	unlock();
}


void ScGraph::c_set (size_t bus, float value)
{
	// TODO: implement in NodeTree
	_control_busses[bus] = value;
}


void  ScGraph::lock_for_read ()
{
    lock(); // TODO _read_write_lock.lockForRead ();
}

void  ScGraph::lock_for_write ()
{
    lock();// TODO _read_write_lock.lockForWrite ();
}

void  ScGraph::lock ()
{
    _main_mutex.lock();
}

void  ScGraph::unlock ()
{
    _main_mutex.unlock();
    // TODO _read_write_lock.unlock ();
}




