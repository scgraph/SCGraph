#include "material.h"

#include <iostream>

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		return (GUnit*) new GMaterial (); 
	}

	size_t get_num_of_units ()
	{
		return 1;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "GMaterial";
		else
			return 0;
	}
}


GMaterial::GMaterial ()
{

}

GMaterial::~GMaterial ()
{
	// std::cout << "[GMaterial]: Destructor" << std::endl;
}


void GMaterial::visitGeometry (Geometry *g)
{
	for (size_t i = 0; i < g->_faces.size (); ++i)
	{
		g->_faces[i].touch()->_material = _material;
	}
}

void GMaterial::visitText (Text *t)
{
	t->_material = _material;
}

void GMaterial::process_g (double delta_t)
{	_graphics_outs[0]._graphics.clear();

	std::copy(
		_graphics_ins[0]->_graphics.begin(), 
		_graphics_ins[0]->_graphics.end(), 
		std::back_inserter(_graphics_outs[0]._graphics)
	);

	_material._shinyness = *_control_ins[1];
	for (int i = 0; i < 4; ++i)
	{
		_material._ambient_reflection._c[i] = *_control_ins[2 + i];
		_material._diffuse_reflection._c[i] = *_control_ins[6 + i];
		_material._specular_reflection._c[i] = *_control_ins[10 + i];
		_material._emissive_color._c[i] = *_control_ins[14 + i];
	}

  for (size_t i = 0; i < _graphics_outs[0]._graphics.size (); ++i)
  {
    _graphics_outs[0]._graphics[i].touch()->accept(this);
  }
}
