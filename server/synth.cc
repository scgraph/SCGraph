/* Copyright 2006 Florian Paul Schmidt */

#include "synth.h"
#include "plugin_pool.h"


#include <iostream>

GSynth::GSynth (boost::shared_ptr<GSynthDef> synthdef, int id) :
	GNode (id),
	_synthdef (synthdef),
	_running (true),
	_done_action (0)
{
	_constants = _synthdef->_constants;
	_parameters = _synthdef->_parameters;

	size_t control = 0;

	for (size_t i = 0; i < _synthdef->_ugen_specs.size (); ++i)
	{
		std::string name = _synthdef->_ugen_specs[i]._name;

		PluginPool *PluginPool = PluginPool::get_instance ();

		boost::shared_ptr<GUnit> unit = PluginPool->create_unit (name, _synthdef->_ugen_specs[i]._special_index, this);

		if (name == "Control")
		{
			for (size_t j = 0; j < _parameters.size (); ++j)
				unit->_control_ins.push_back (&_parameters[control++]);

			/* an empty graphics bus reference, since there's no constants nor
		       controls at graphics rate */
			unit->_graphics_ins.push_back (0);
		}

		for (size_t j = 0; j < _synthdef->_ugen_specs[i]._input_specs.size (); ++j)
		{
			/* is it a constant? */
			if (_synthdef->_ugen_specs[i]._input_specs[j]._index_of_ugen == -1)
			{
				/* yes it is */
				unit->_control_ins.push_back (&_constants[_synthdef->_ugen_specs[i]._input_specs[j]._index_of_constant]);

				unit->_graphics_ins.push_back (0);
			}
			else
			{
				/* no, it isn't */
				unit->_control_ins.push_back (&_units[_synthdef->_ugen_specs[i]._input_specs[j]._index_of_ugen]->_control_outs[_synthdef->_ugen_specs[i]._input_specs[j]._index_of_ugen_output]);

				GraphicsBus *b = &(_units[_synthdef->_ugen_specs[i]._input_specs[j]._index_of_ugen]->_graphics_outs[_synthdef->_ugen_specs[i]._input_specs[j]._index_of_ugen_output]);

				unit->_graphics_ins.push_back (b);
			}
		}

		for (size_t j = 0; j < _synthdef->_ugen_specs[i]._output_specs.size (); ++j)
		{
			unit->_control_outs.push_back (0);
			GraphicsBus b;
			unit->_graphics_outs.push_back (b);
		}

		_units.push_back(unit);
	}
}

GSynth::~GSynth ()
{
	//std::cout << "[GSynth]: Destructor" << std::endl;
}

void GSynth::set_running (bool on_off)
{
	_running = on_off;
}

void GSynth::c_set (int control_index, float value)
{
	_parameters[control_index] = value;
}

void GSynth::c_set (const char *control_name, float value)
{
	for (size_t i = 0; i < _synthdef->_param_names.size(); ++i)
	{
		if (std::string(_synthdef->_param_names[i]._name) == std::string (control_name))
		{
			// std::cout << "match: " << _synthdef->_param_names[i]._name << std::endl;
			c_set (_synthdef->_param_names[i]._index, value);
		}
	}
}

void GSynth::process_g (double delta_t)
{
	if (!_running)
		return;

	for (size_t i = 0; i < _units.size (); ++i)
	{
		if (_synthdef->_ugen_specs[i]._calculation_rate == 2)
		{
			try
			{
				_units[i]->process_g (delta_t);

				int done_action;
				if ((done_action = _units[i]->get_done_action ()) != 0)
					_done_action = done_action;
			}
			catch (const char *e)
			{
				std::cout << "[GSynth]: Error while running process_g () of unit " << _synthdef->_ugen_specs[i]._name <<": Reason: " << e << std::endl;
			}
		}
	}
}

void GSynth::process_c (double delta_t)
{
	if (!_running)
		return;

	PluginPool *plugin_pool = PluginPool::get_instance ();

	// plugin pool needs to know who's currently processing
	plugin_pool->set_current_synth (this);

	for (size_t i = 0; i < _units.size (); ++i)
	{
		if (_synthdef->_ugen_specs[i]._calculation_rate == 1)
		{
			try
			{
				_units[i]->process_c (delta_t);

				int done_action;
				if ((done_action = _units[i]->get_done_action ()) != 0)
					_done_action = done_action;
			}
			catch (const char *e)
			{
				std::cout << "[GSynth]: Error while running process_c () of unit " << _synthdef->_ugen_specs[i]._name <<": Reason: " << e << std::endl;
			}
		}
	}
}

int GSynth::get_done_action ()
{
	return _done_action;
}

void GSynth::set_done_action (int done_action)
{
	_done_action = done_action;
}

