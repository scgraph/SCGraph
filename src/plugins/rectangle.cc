#include "rectangle.h"

#include <iostream>

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		return (GUnit*) new Rectangle (); 
	}

	size_t get_num_of_units ()
	{
		return 1;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "GRectangle";
		else
			return 0;
	}
}


Rectangle::Rectangle () :
	_g(new Geometry)
{
	/* create face data - only one face now */
	cow_ptr<Face> face (new Face);
	face.touch()->_geometry_type = Face::QUADS;

	// face->_material._emissive_color._c[0] = 1.0;

	face.touch()->_vertices.push_back (Vector3D ());
	face.touch()->_normals.push_back (Vector3D (0,0,1));

	face.touch()->_vertices.push_back (Vector3D ());
	face.touch()->_normals.push_back (Vector3D (0,0,1));

	face.touch()->_vertices.push_back (Vector3D ());
	face.touch()->_normals.push_back (Vector3D (0,0,1));

	face.touch()->_vertices.push_back (Vector3D ());
	face.touch()->_normals.push_back (Vector3D (0,0,1));

	face.touch()->_face_color = ColorRGBA (1,1,1,1);

	_g.touch()->_faces.push_back (face);
	//_graphics_outs[0].touch()->_graphics.push_back(_g);
}

Rectangle::~Rectangle ()
{
	// std::cout << "[Rectangle]: Destructor" << std::endl;
}

void Rectangle::process_g (double delta_t)
{
	_graphics_outs[0]._graphics.clear();

	Face *f = _g.touch()->_faces[0].touch();

	f->_vertices[0]._c[0] = - (*_control_ins[0])/2.0;
	f->_vertices[0]._c[1] = - (*_control_ins[1])/2.0;
	f->_vertices[0]._c[2] = 0;

	f->_vertices[1]._c[0] = (*_control_ins[0])/2.0;
	f->_vertices[1]._c[1] = - (*_control_ins[1])/2.0;
	f->_vertices[1]._c[2] = 0;

	f->_vertices[2]._c[0] = (*_control_ins[0])/2.0;
	f->_vertices[2]._c[1] = (*_control_ins[1])/2.0;
	f->_vertices[2]._c[2] = 0;

	f->_vertices[3]._c[0] = - (*_control_ins[0])/2.0;
	f->_vertices[3]._c[1] = (*_control_ins[1])/2.0;
	f->_vertices[3]._c[2] = 0;

	_graphics_outs[0]._graphics.push_back(_g);
}
