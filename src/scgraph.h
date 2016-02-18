/* Copyright 2006 Florian Paul Schmidt */

#ifndef SCGRAPH_HH
#define SCGRAPH_HH

#include "options.h"
#include "control_loop.h"
#include "graphic_loop.h"
#include "osc_handler.h"
//#include "plugin_pool.h"
#include "synthdef_pool.h"
#include "synthdef_file.h"
#include "synthdef.h"
#include "synth.h"
#include "node_tree.h"
#include "graphics_bus.h"
#include "buffer_pool.h"

#include "cow_ptr.h"

#ifdef HAVE_JACK
#include "jack_client.h"
#endif
/*
#include <QtGui/QApplication>
#include <QtCore/QTimer>
#include <QtCore/QReadWriteLock>
#include <QtCore/QDebug>

#include <QtGui/QWidget>
#include <QtGui/QIcon>
*/
// #include <SuperCollider/plugin_interface/SC_InterfaceTable.h>

#include "ofThread.h"
#include <mutex>

/** this class implements the singleton pattern and is the global
 * access point for pretty much everything */

class ScGraph
{
	
    std::mutex                        _main_mutex;

	static ScGraph*                   _instance;

	/** first is id, second is action */
	std::vector<std::pair<int, int> > _done_actions;
    
   

	protected:
		ScGraph (int argc, char *argv[]);
	
	public:
		~ScGraph ();
		static ScGraph* get_instance ();
		static ScGraph* get_instance (int argc, char *argv[]);

		/*
		// reimplemented from QApplication so we can throw exceptions in slots
		virtual bool notify(QObject * receiver, QEvent * event) {
			try {
				return QApplication::notify(receiver, event);
			} catch(std::exception& e) {
				qCritical() << "Exception thrown:" << e.what();
			}
			return false;
		}
		*/

	public:
#ifdef HAVE_JACK
		boost::shared_ptr<JackClient>
		                           _jack_client;
#endif
		BufferPool                 _buffer_pool;

		// the plugin pool has to be destroyed after the nodetree, because it holds the
		// code of all the unit generators (including the destructors - tadaaa!
		// PluginPool                 _plugin_pool;
		SynthDefPool               _synthdef_pool;

		NodeTree                   _node_tree;

		ControlLoop                _control_loop;
		GraphicLoop                _graphic_loop;

		/* the osc handler must be initialized after the other stuff */
		OscHandler                 _osc_handler;

		std::vector<float>         _control_busses;

		std::vector <GraphicsBus>
		                           _graphics_busses;

		std::vector <GraphicsBus>
		                           _past_graphics_busses;

		std::vector<boost::shared_ptr<GSynth> >
		                           _persistent_synths;

		//QReadWriteLock             _read_write_lock;


//		SC_InterfaceTable          _sc_interface_table;
#if 0
		pthread_mutex_t            _control_mutex;
		std::vector<float>         _control_busses_copy;
	
		pthread_mutex_t            _graphic_mutex;
#endif

		double _delta_t;

		void start ();
		void stop ();

		void run_one_control_cycle (double delta_t);
		void run_one_graphics_cycle (double delta_t);

		void c_set (size_t bus, float value);

		/** convenience functions */
		void lock_for_read ();
		void lock_for_write ();
		void lock ();
		void unlock ();
};


#endif
