#include "triangle.h"

#include <iostream>

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		return (GUnit*) new Triangle (); 
	}

	size_t get_num_of_units ()
	{
		return 1;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "GTriangle";
		else
			return 0;
	}
}


Triangle::Triangle () :
	_g(new Geometry)
{
	/* create face data - only one face now */
	cow_ptr<Face> face (new Face);
	face.touch()->_geometry_type = Face::TRIANGLES;

	// face.touch()->_material._emissive_color._c[0] = 1.0;

	face.touch()->_vertices.push_back (Vector3D ());
	face.touch()->_vertices.push_back (Vector3D ());
	face.touch()->_vertices.push_back (Vector3D ());

	face.touch()->_normals.push_back (Vector3D ());
	face.touch()->_normals.push_back (Vector3D ());
	face.touch()->_normals.push_back (Vector3D ());

	face.touch()->_face_color = ColorRGBA (1,1,1,1);

	_g.touch()->_faces.push_back (face);
}

Triangle::~Triangle ()
{
	// std::cout << "[Triangle]: Destructor" << std::endl;
}

void Triangle::process_g (double delta_t)
{
	_graphics_outs[0]._graphics.clear();

	for (size_t i = 0; i < 3; ++i)
	{
		_g.touch()->_faces[0].touch()->_vertices[0]._c[i] = *_control_ins[i];
		_g.touch()->_faces[0].touch()->_vertices[1]._c[i] = *_control_ins[i+3];
		_g.touch()->_faces[0].touch()->_vertices[2]._c[i] = *_control_ins[i+6];
	}

	_graphics_outs[0]._graphics.push_back (_g);
}
