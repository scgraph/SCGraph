#include "polygon.h"

#include <iostream>

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		return (GUnit*) new Polygon (); 
	}

	size_t get_num_of_units ()
	{
		return 1;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "GPolygon";
		else
			return 0;
	}
}


Polygon::Polygon () :
	_g (new Geometry),
	_f (new Face),
	_first_time(true)
{
}

Polygon::~Polygon ()
{
	// std::cout << "[Polygon]: Destructor" << std::endl;
}

void Polygon::process_g (double delta_t)
{
	_graphics_outs[0]._graphics.clear();

	if (_first_time) {
		_first_time = false;

		_f.touch()->_geometry_type = Face::POLYGON;


		for (size_t i = 0; i < (int)*_control_ins[0]; ++i)
		{
			_f.touch()->_vertices.push_back (Vector3D ());
			_f.touch()->_normals.push_back (Vector3D ());
		}
	}

	for (size_t i = 0; i < (int)*_control_ins[0]; ++i)
	{
		_f.touch()->_vertices[i]._c[0] =  *_control_ins[1 + 3*i];
		_f.touch()->_vertices[i]._c[1] =  *_control_ins[2 + 3*i];
		_f.touch()->_vertices[i]._c[2] =  *_control_ins[3 + 3*i];

		_f.touch()->_normals[i]._c[0] =  *_control_ins[1 + (int)*_control_ins[0]*3];
		_f.touch()->_normals[i]._c[1] =  *_control_ins[2 + (int)*_control_ins[0]*3];
		_f.touch()->_normals[i]._c[2] =  *_control_ins[3 + (int)*_control_ins[0]*3];
	}

	_g.touch()->_faces.clear();
	_g.touch()->_faces.push_back(_f);
	_graphics_outs[0]._graphics.push_back (_g);
}
