#include "points.h"

#include <iostream>

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		return (GUnit*) new Points (); 
	}

	size_t get_num_of_units ()
	{
		return 1;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "GPoints";
		else
			return 0;
	}
}


Points::Points () :
	_g(new Geometry),
	_f(new Face),
	_first_time(true)
{
	/* create face data - only one face now */
	_f.touch()->_geometry_type = Face::POINTS;

	// face->_material._emissive_color._c[0] = 1.0;

	_f.touch()->_face_color = ColorRGBA (1,1,1,1);

	//_g.touch()->_faces.push_back (face);
}

Points::~Points ()
{
	// std::cout << "[Points]: Destructor" << std::endl;
}

void Points::process_g (double delta_t)
{
	_graphics_outs[0]._graphics.clear();

	if (_first_time) {
		_first_time = false;
		for (size_t i = 0; i < (int)*_control_ins[1]; ++i) 
			_f.touch()->_vertices.push_back(Vector3D());

	}

	_f.touch()->_thickness = *_control_ins[0];

	for (size_t i = 0; i < (int)*_control_ins[1]; ++i)
	{
		for (size_t j = 0; j < 3; ++j)
		{
			_f.touch()->_vertices[i]._c[j] = *_control_ins[2 + (3 * i) + j];
		}
	}
	_g.touch()->_faces.clear();
	_g.touch()->_faces.push_back(_f);

	_graphics_outs[0]._graphics.push_back (_g);
}
