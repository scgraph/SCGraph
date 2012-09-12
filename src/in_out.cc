#include "in_out.h"

#include "scgraph.h"
#include "options.h"

void In::process_g (double delta_t)
{
	ScGraph *scgraph = ScGraph::get_instance ();

	if(!((size_t)(*_control_ins[0]) >= Options::get_instance ()->_graphics_busses)) 
		_bus = (size_t)(*_control_ins[0]);
																				   
	_graphics_outs[0]._graphics.clear();

	for (size_t i = 0; i < scgraph->_graphics_busses[_bus]._graphics.size (); ++i)
		{
			_graphics_outs[0]._graphics.push_back(scgraph->_graphics_busses[_bus]._graphics[i]);
		}
}

void In::process_c (double delta_t)
{
	if(!((size_t)(*_control_ins[0]) >= Options::get_instance ()->_control_busses)) 
		_bus = (size_t)(*_control_ins[0]);
	
	_control_outs [0] = ScGraph::get_instance ()->_control_busses[_bus];
}

void Out::process_g (double delta_t)
{
	ScGraph *scgraph = ScGraph::get_instance ();

	if(!((size_t)(*_control_ins[0]) >= Options::get_instance ()->_graphics_busses)) 
		_bus = (size_t)(*_control_ins[0]);

	for (size_t i = 0; i < _graphics_ins[1]->_graphics.size (); ++i)
		{
			scgraph->_graphics_busses[_bus]._graphics.push_back (_graphics_ins[1]->_graphics[i]);
		}
}

void Out::process_c (double delta_t)
{
	if(!((size_t)(*_control_ins[0]) >= Options::get_instance ()->_control_busses)) 
		_bus = (size_t)(*_control_ins[0]);
	
	ScGraph::get_instance ()->_control_busses[_bus] += *_control_ins [1];
}


void ReplaceOut::process_g (double delta_t)
{
	ScGraph *scgraph = ScGraph::get_instance ();

	if(!((size_t)(*_control_ins[0]) >= Options::get_instance ()->_graphics_busses)) 
		_bus = (size_t)(*_control_ins[0]);

	scgraph->_graphics_busses[_bus]._graphics.clear ();

	for (size_t i = 0; i < _graphics_ins[1]->_graphics.size (); ++i)
		{
			scgraph->_graphics_busses[_bus]._graphics.push_back (_graphics_ins[1]->_graphics[i]);
		}
}

void ReplaceOut::process_c (double delta_t)
{

	if(!((size_t)(*_control_ins[0]) >= Options::get_instance ()->_control_busses)) 
		_bus = (size_t)(*_control_ins[0]);
	
	ScGraph::get_instance ()->_control_busses[_bus] = *_control_ins [1];
}


