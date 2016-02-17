#include "render_mode.h"

#include <iostream>

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		return (GUnit*) new GRenderMode (); 
	}

	size_t get_num_of_units ()
	{
		return 1;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "GRenderMode";
		else
			return 0;
	}
}


GRenderMode::GRenderMode ()
{

}

GRenderMode::~GRenderMode ()
{
	// std::cout << "[GRenderMode]: Destructor" << std::endl;
}


void GRenderMode::visitGeometry (Geometry *g)
{
	if (*_control_ins[1] > 0.5) {
		for (size_t i = 0; i < g->_faces.size (); ++i)
			{
				g->_faces[i].touch()->_render_mode = WIREFRAME;
				g->_faces[i].touch()->_thickness = *_control_ins[2];
			}
	}
	else {
		for (size_t i = 0; i < g->_faces.size (); ++i)
			{
				g->_faces[i].touch()->_render_mode = NORMAL;
			}
	}
}

void GRenderMode::visitText (Text *t)
{
	if (*_control_ins[1] > 0.5) {
		t->_thickness = *_control_ins[2];
		t->_render_mode = WIREFRAME;
	}
	else {
		t->_render_mode = NORMAL;
	}
}

void GRenderMode::process_g (double delta_t)
{	

	_graphics_outs[0]._graphics.clear();

	std::copy(
		_graphics_ins[0]->_graphics.begin(), 
		_graphics_ins[0]->_graphics.end(), 
		std::back_inserter(_graphics_outs[0]._graphics)
	);

	for (size_t i = 0; i < _graphics_outs[0]._graphics.size (); ++i)
		{
			_graphics_outs[0]._graphics[i].touch()->accept(this);
		}
}
