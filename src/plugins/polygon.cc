#include "polygon.h"

#include <iostream>

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		return (GUnit*) new GPolygon (); 
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


GPolygon::GPolygon () :
	_g (new Geometry),
	_f (new Face),
	_first_time(true)
{
}

GPolygon::~GPolygon ()
{
	// std::cout << "[GPolygon]: Destructor" << std::endl;
}

void GPolygon::process_g (double delta_t)
{
	_graphics_outs[0]._graphics.clear();

	if (_first_time) {
		_first_time = false;

		_f.touch()->_geometry_type = Face::POLYGON;


		for (size_t i = 0; i < (int)*_control_ins[0]; ++i)
		{
			_f.touch()->_vertices.push_back (ofVec3f ());
			_f.touch()->_normals.push_back (ofVec3f ());
		}
	}

	for (size_t i = 0; i < (int)*_control_ins[0]; ++i)
	{
		_f.touch()->_vertices[i][0] =  *_control_ins[1 + 3*i];
		_f.touch()->_vertices[i][1] =  *_control_ins[2 + 3*i];
		_f.touch()->_vertices[i][2] =  *_control_ins[3 + 3*i];

		_f.touch()->_normals[i][0] =  *_control_ins[1 + (int)*_control_ins[0]*3];
		_f.touch()->_normals[i][1] =  *_control_ins[2 + (int)*_control_ins[0]*3];
		_f.touch()->_normals[i][2] =  *_control_ins[3 + (int)*_control_ins[0]*3];
	}

	_g.touch()->_faces.clear();
	_g.touch()->_faces.push_back(_f);
	_graphics_outs[0]._graphics.push_back (_g);
}
