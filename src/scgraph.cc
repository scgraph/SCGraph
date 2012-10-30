/* Copyright 2006 Florian Paul Schmidt */

#include "scgraph.h"

// TODO: remove this test code
#include "synthdef_pool.h"

#include <iostream>

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

	// _instance = this;
	Options *options = Options::get_instance ();

	// _jack_client  = new JackClient (2, "scgraph");

	_control_busses.resize (options->_control_busses);

	for (size_t i = 0; i < options->_graphics_busses; ++i)
	{
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
		catch (const char *e)
		{
			std::cout << "[ScGraph]: Error: Failed to create instance of ScGraph. Reason: " << e << std::endl;
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
	// delete _jack_client;

	// pthread_mutex_destroy (&_main_mutex);
	_instance = 0;
}

void ScGraph::stop ()
{
	// pthread_mutex_lock (&_main_mutex);

	// QWriteLocker locker (&_read_write_lock);


	_osc_handler.stop ();
	_control_loop.stop ();
	_graphic_loop.stop ();

	// pthread_mutex_unlock (&_main_mutex);
}

void ScGraph::start ()
{
	// pthread_mutex_lock (&_main_mutex);
	QWriteLocker locker (&_read_write_lock);

	_graphic_loop.start ();
	_control_loop.start ();
	_osc_handler.start ();

	// pthread_mutex_unlock (&_main_mutex);
}

void ScGraph::run_one_control_cycle (double delta_t)
{
	lock_for_read();
	// QReadLocker locker (&_read_write_lock);

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
	// pthread_mutex_lock (&_main_mutex);
	// QReadLocker locker (&_read_write_lock);
	// _read_write_lock.lockForRead();
	_delta_t = delta_t;

	lock_for_read();

	for (size_t i = 0; i < _graphics_busses.size (); ++i)
	{
		_graphics_busses[i].clear ();
	}

	for (Tree::Tree<NodePtr>::Iterator it = _node_tree.begin (); it != _node_tree.end (); ++it)
	{
		(*it)->process_g (delta_t);
	}

	//locker.unlock ();
	unlock();
	//_read_write_lock.unlock();

	/* do the doneActions */
	lock_for_write();
	if (_done_actions.size () != 0)
	{
		// _read_write_lock.lockForWrite();
		//QWriteLocker locker (&_read_write_lock);

		for 
		(
			std::vector<std::pair<int, int> >::iterator it = _done_actions.begin();
			it != _done_actions.end (); 
			++it
		) {
			// std::cout << "scgraph: done_action" << std::endl;
			_node_tree.done_action ((*it).first, (*it).second);
		}

		_done_actions.clear ();
		// _read_write_lock.unlock();
	}
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
	_read_write_lock.lockForRead ();
}

void  ScGraph::lock_for_write ()
{
	_read_write_lock.lockForWrite ();
}

#if 0
void  ScGraph::lock ()
{
	_main_mutex.lock ();
}
#endif

void  ScGraph::unlock ()
{
	_read_write_lock.unlock ();
}





