#include "circle.h"

#include <iostream>

#define TAU 6.28318531

extern "C"
{
    GUnit *create (size_t index, int special_index)
    {
	return (GUnit*) new Circle (); 
    }

    size_t get_num_of_units ()
    {
	return 1;
    }

    const char *get_unit_name (size_t index)
    {
	if (index == 0)
	    return "GCircle";
	else
	    return 0;
    }
}



Circle::Circle () :
    _g(new Geometry),
    _segments(32)
{
    cow_ptr<Face> face (new Face);
    face.touch()->_geometry_type = Face::TRIANGLE_FAN;

    // face.touch()->_material._emissive_color._c[0] = 1.0;
    float phi = 0;
    face.touch()->_vertices.push_back(Vector3D (0, 0, 0));
    face.touch()->_normals.push_back(Vector3D (0, 0, 1));
    for(int i=0; i <= _segments; i++) {
		phi = TAU * ((float) i/(float) _segments);
		face.touch()->_vertices.push_back(Vector3D (sin(phi), cos(phi), 0));
		face.touch()->_normals.push_back(Vector3D (0, 0, 1));
    }

    face.touch()->_face_color = ColorRGBA(1,1,1,1);

    _g.touch()->_faces.push_back(face);
}

Circle::~Circle ()
{
    // std::cout << "[Circle]: Destructor" << std::endl;
}

void Circle::process_g (double delta_t)
{
    _graphics_outs[0]._graphics.clear();

    if (_segments != (int) *_control_ins[0] && (int) *_control_ins[0] > 2) {
		_segments = (int) *_control_ins[0];

		_g.touch()->_faces[0].touch()->_vertices.clear();
		_g.touch()->_faces[0].touch()->_normals.clear();

		// face.touch()->_material._emissive_color._c[0] = 1.0;
		float phi = 0;

		_g.touch()->_faces[0].touch()->_vertices.push_back(Vector3D (0, 0, 0));
		_g.touch()->_faces[0].touch()->_normals.push_back(Vector3D (0, 0, 1));
		for(int i=0; i <= _segments; i++) {
			phi = TAU * ((float) i/(float) _segments);
			_g.touch()->_faces[0].touch()->_vertices.push_back(Vector3D (sin(phi), cos(phi), 0));
			_g.touch()->_faces[0].touch()->_normals.push_back(Vector3D (0, 0, 1));
		}
	}

    _graphics_outs[0]._graphics.push_back(_g);
}
