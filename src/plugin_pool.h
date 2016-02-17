#ifndef SCGRAPH_PLUGINPOOL_HH
#define SCGRAPH_PLUGINPOOL_HH

#include <vector>
#include <string>

#include <sndfile.h>

#include "plugin.h"

#include "synth.h"

#include <boost/shared_ptr.hpp>


struct InterfaceTable;
struct World;
struct Graph;
struct Unit;
struct ScUnit;

class PluginPool
{
	static PluginPool* _instance;

	// this needs to be updated to a valid synth, so the done actions, etc
	// can be directed to the right synth
	GSynth*             _current_synth;

	InterfaceTable     *_sc_interface_table;

	World              *_sc_world;
	Graph              *_sc_graph;

	std::vector <boost::shared_ptr<GPlugin> > _plugins;
	std::vector <boost::shared_ptr<ScUnit> > _sc_units;

	void add_sc_plugin_dir (const std::string &directory);
	void add_sc_plugin (const std::string &filename);

	void add_plugin_dir (const std::string &directory);
	void add_plugin (const std::string &filename);

	void tokenize_path (const std::string &path, void (PluginPool::*function)(const std::string &directory));

	void traverse_directory (const std::string &directory, void (PluginPool::*function)(const std::string &filename));

	protected:
		PluginPool ();
	public:
		~PluginPool ();

		static PluginPool *get_instance ();

		void init ();

		unsigned int get_number_of_plugins ();

		boost::shared_ptr<GPlugin> get_plugin (unsigned int index);

		boost::shared_ptr<GUnit> create_unit (const std::string &name, int special_index, GSynth *synth);

		Unit *create_sc_unit (const std::string &name, int special_index, int parent_index, int num_inputs, int num_outputs, int calc_rate, double rate, int buf_length);

		void destroy_sc_unit(Unit *u);

		void add_sc_unit (boost::shared_ptr<ScUnit> unit);

		const std::vector<boost::shared_ptr<GPlugin> > &get_plugins ();

		void set_current_synth (GSynth *synth);

		GSynth *get_current_synth ();
};


#endif
