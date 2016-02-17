#include "tube.h"

#include <iostream>

#define TAU 6.28318531

extern "C"
{
    GUnit *create (size_t index, int special_index)
    {
	return (GUnit*) new Tube (); 
    }

    size_t get_num_of_units ()
    {
	return 1;
    }

    const char *get_unit_name (size_t index)
    {
	if (index == 0)
	    return "GTube";
	else
	    return 0;
    }
}


Tube::Tube () :
    _g(new Geometry),
    _segments(32)
{
    cow_ptr<Face> face (new Face);
    face.touch()->_geometry_type = Face::QUAD_STRIP;

    // disable culling
    face.touch()->_culling = 0;

    // face.touch()->_material._emissive_color._c[0] = 1.0;
    float phi = 0;

    for(int i=0; i <= _segments; i++) {
	phi = TAU * ((float) i/(float) _segments);
	face.touch()->_vertices.push_back (Vector3D (sin(phi), 
						     cos(phi), -1));
	face.touch()->_vertices.push_back (Vector3D (sin(phi), 
						     cos(phi), 1));
	phi = TAU * ((0.5 + (float) i)/(float) _segments);
	face.touch()->_normals.push_back  (Vector3D (sin(phi), 
						     cos(phi), 0));
	face.touch()->_normals.push_back  (Vector3D (sin(phi), 
						     cos(phi), 0));
    }

    face.touch()->_face_color = ColorRGBA (1,1,1,1);

    _g.touch()->_faces.push_back (face);
}

Tube::~Tube ()
{
    // std::cout << "[Tube]: Destructor" << std::endl;
}

void Tube::process_g (double delta_t)
{
    _graphics_outs[0]._graphics.clear();

    _g.touch()->_commands.clear();

    if (_segments != (int) *_control_ins[1]) {
	if(((int) *_control_ins[1]) > 2) {
	    _segments = (int) *_control_ins[1];

	    _g.touch()->_faces[0].touch()->_vertices.clear();
	    _g.touch()->_faces[0].touch()->_normals.clear();

	    // face.touch()->_material._emissive_color._c[0] = 1.0;
	    float phi = 0;
	    for(int i=0; i <= _segments; i++) {
		phi = TAU * ((float) i/(float) _segments);
		_g.touch()->_faces[0].touch()->_vertices.push_back (Vector3D (sin(phi), 
									      cos(phi), -1));
		_g.touch()->_faces[0].touch()->_vertices.push_back (Vector3D (sin(phi), 
									      cos(phi), 1));
		phi = TAU * ((0.5 + (float) i)/(float) _segments);
		_g.touch()->_faces[0].touch()->_normals.push_back  (Vector3D (sin(phi), 
									      cos(phi), 0));
		_g.touch()->_faces[0].touch()->_normals.push_back  (Vector3D (sin(phi), 
									      cos(phi), 0));
	    }
	}
    }

    cow_ptr<Scale> cmd (new Scale);
    cmd.touch()->_scaling_vector = Vector3D (*_control_ins[0],
					     *_control_ins[0], 
					     *_control_ins[0]);
    _g.touch()->_commands.push_back(cmd);

    _graphics_outs[0]._graphics.push_back(cow_ptr<Geometry>(_g));
}
