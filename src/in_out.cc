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


void InFeedback::process_g (double delta_t)
{
	ScGraph *scgraph = ScGraph::get_instance ();

	if(!((size_t)(*_control_ins[0]) >= Options::get_instance ()->_graphics_busses)) 
		_bus = (size_t)(*_control_ins[0]);
																				   
	_graphics_outs[0]._graphics.clear();

	for (size_t i = 0; i < scgraph->_past_graphics_busses[_bus]._graphics.size (); ++i)
		{
			_graphics_outs[0]._graphics.push_back(scgraph->_past_graphics_busses[_bus]._graphics[i]);
		}
}

void InFeedback::process_c (double delta_t)
{
	// TODO feedback
	if(!((size_t)(*_control_ins[0]) >= Options::get_instance ()->_control_busses)) 
		_bus = (size_t)(*_control_ins[0]);
	
	_control_outs [0] = ScGraph::get_instance ()->_control_busses[_bus];
}

void XFade2::visitGeometry (Geometry *g) {
	for (size_t i = 0; i < g->_faces.size (); ++i) {
		if (g->_faces[i]->_colors.empty()) {
			g->_faces[i].touch()->_face_color.scale_alpha(_factor);
		}
		else {
			for (size_t j = 0; j < g->_faces[i]->_colors.size (); ++j) {
				g->_faces[i].touch()->_colors[j].scale_alpha(_factor);
			}
		}
	}
}

void XFade2::visitText (Text *t) {
	t->_color.scale_alpha(_factor);
}

void XFade2::process_g (double delta_t)
{
	_graphics_outs[0]._graphics.clear();

	// TODO equal power crossfade

	for (size_t side = 0; side < 2; ++side) {
		if(side == 0)
			_factor = (*_control_ins[2] * -0.5f + 0.5) * *_control_ins[3];
		else
			_factor = (*_control_ins[2] * 0.5f + 0.5) * *_control_ins[3];

		for (size_t i = 0; i < _graphics_ins[side]->_graphics.size(); ++i)
			{
				_graphics_outs[0]._graphics.push_back(_graphics_ins[side]->_graphics[i]);
				_graphics_outs[0]._graphics[i + (side * _graphics_ins[0]->_graphics.size())].touch()->accept(this);
			}
	}
}

void XFade2::process_c (double delta_t)
{
	// TODO equal power crossfade
	_control_outs[0] = ((*_control_ins [0] 
						 * (*_control_ins[2] * -0.5f + 0.5))
						+ (*_control_ins[1] 
						   * (*_control_ins[2] * 0.5 + 0.5))) * *_control_ins[3];
}

void XOut::visitGeometry (Geometry *g) {
	for (size_t i = 0; i < g->_faces.size (); ++i) {
		if (g->_faces[i]->_colors.empty()) {
			g->_faces[i].touch()->_face_color.scale_alpha(_factor);
		}
		else {
			for (size_t j = 0; j < g->_faces[i]->_colors.size (); ++j) {
				g->_faces[i].touch()->_colors[j].scale_alpha(_factor);
			}
		}
	}
}

void XOut::visitText (Text *t) {
	t->_color.scale_alpha(_factor);
}

void XOut::process_g (double delta_t)
{
	ScGraph *scgraph = ScGraph::get_instance ();

	if(!((size_t)(*_control_ins[0]) >= Options::get_instance ()->_graphics_busses)) 
		_bus = (size_t)(*_control_ins[0]);

	// bus_signal = (input_signal * xfade) + (bus_signal * (1 - xfade));

	_factor = (1 - *_control_ins[1]);
	for (size_t i = 0; i < scgraph->_graphics_busses[_bus]._graphics.size(); ++i)
		{
			scgraph->_graphics_busses[_bus]._graphics[i].touch()->accept(this);
		}
	_factor = *_control_ins[1];
	uint32_t former_size = scgraph->_graphics_busses[_bus]._graphics.size();
	for (size_t i = 0; i < _graphics_ins[2]->_graphics.size(); ++i)
		{
			scgraph->_graphics_busses[_bus]._graphics
				.push_back(_graphics_ins[2]->_graphics[i]);
			scgraph->_graphics_busses[_bus]
				._graphics[former_size + i].touch()->accept(this);
		}
}


void XOut::process_c (double delta_t)
{
	if(!((size_t)(*_control_ins[0]) >= Options::get_instance ()->_control_busses)) 
		_bus = (size_t)(*_control_ins[0]);
	// bus_signal = (input_signal * xfade) + (bus_signal * (1 - xfade));
	ScGraph::get_instance ()->_control_busses[_bus] = (*_control_ins [1] * *_control_ins[2]) 
		+ (ScGraph::get_instance ()->_control_busses[_bus] * (1 - *_control_ins[2]));
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


