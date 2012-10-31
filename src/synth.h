/* Copyright 2006 Florian Paul Schmidt */

#ifndef SCGRAPH_SYNTH_HH
#define SCGRAPH_SYNTH_HH

#include "node.h"
#include "synthdef.h"
#include "group.h"
#include "unit.h"

#include <string>
#include <boost/shared_ptr.hpp>
#include <vector>


class GSynth : public GNode
{
	/** The synthdef we are made of */
	boost::shared_ptr<GSynthDef>           _synthdef;

	std::vector<boost::shared_ptr<GUnit> > _units;
	std::vector<float>                     _constants;
	std::vector<float>                     _parameters;

	bool                                   _running;

	int                                    _done_action;

	public:
		GSynth (boost::shared_ptr<GSynthDef> synthdef, int id);
		virtual ~GSynth ();

		void process_g (double delta_t);
		void process_c (double delta_t);

		void set_running (bool on_off);

		const int get_done_action ();
		void set_done_action (int done_action);

		void c_set (int control_index, float value);
		void c_set (const char *control_name, float value);
		int get_index(const char *control_name);
};


#endif

