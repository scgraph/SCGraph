#include "cube.h"

#include <iostream>

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		return (GUnit*) new Cube (); 
	}

	size_t get_num_of_units ()
	{
		return 1;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "GCube";
		else
			return 0;
	}
}


Cube::Cube () :
	_g(new Geometry)
{
	// std::cout << "aaaah" << std::endl;
	/* create face data - only one face now */
	cow_ptr<Face> face (new Face);
	face.touch()->_geometry_type = Face::QUADS;

	//std::cout << "aaaargh" << std::endl;
	// face.touch()->_material._emissive_color._c[0] = 1.0;

	face.touch()->_vertices.push_back (Vector3D (-1, 1, -1));
	face.touch()->_vertices.push_back (Vector3D (1, 1, -1));
	face.touch()->_vertices.push_back (Vector3D (1, -1, -1));
	face.touch()->_vertices.push_back (Vector3D (-1, -1, -1));

	face.touch()->_normals.push_back (Vector3D (-1, 1, -1));
	face.touch()->_normals.push_back (Vector3D (1, 1, -1));
	face.touch()->_normals.push_back (Vector3D (1, -1, -1));
	face.touch()->_normals.push_back (Vector3D (-1, -1, -1));

// 	face.touch()->_normals.push_back (Vector3D (0, 0, -1));
// 	face.touch()->_normals.push_back (Vector3D (0, 0, -1));
// 	face.touch()->_normals.push_back (Vector3D (0, 0, -1));
// 	face.touch()->_normals.push_back (Vector3D (0, 0, -1));



	face.touch()->_vertices.push_back (Vector3D (-1, -1, 1));
	face.touch()->_vertices.push_back (Vector3D (1, -1, 1));
	face.touch()->_vertices.push_back (Vector3D (1, 1, 1));
	face.touch()->_vertices.push_back (Vector3D (-1, 1, 1));

	face.touch()->_normals.push_back (Vector3D (-1, -1, 1));
	face.touch()->_normals.push_back (Vector3D (1, -1, 1));
	face.touch()->_normals.push_back (Vector3D (1, 1, 1));
	face.touch()->_normals.push_back (Vector3D (-1, 1, 1));
// 	face.touch()->_normals.push_back (Vector3D (0, 0, 1));
// 	face.touch()->_normals.push_back (Vector3D (0, 0, 1));
// 	face.touch()->_normals.push_back (Vector3D (0, 0, 1));
// 	face.touch()->_normals.push_back (Vector3D (0, 0, 1));


	face.touch()->_vertices.push_back (Vector3D (1, -1, -1));
	face.touch()->_vertices.push_back (Vector3D (1, -1, 1));
	face.touch()->_vertices.push_back (Vector3D (-1, -1, 1));
	face.touch()->_vertices.push_back (Vector3D (-1, -1, -1));

	face.touch()->_normals.push_back (Vector3D (1, -1, -1));
	face.touch()->_normals.push_back (Vector3D (1, -1, 1));
	face.touch()->_normals.push_back (Vector3D (-1, -1, 1));
	face.touch()->_normals.push_back (Vector3D (-1, -1, -1));
// 	face.touch()->_normals.push_back (Vector3D (0, -1, 0));
// 	face.touch()->_normals.push_back (Vector3D (0, -1, 0));
// 	face.touch()->_normals.push_back (Vector3D (0, -1, 0));
// 	face.touch()->_normals.push_back (Vector3D (0, -1, 0));


	face.touch()->_vertices.push_back (Vector3D (-1, 1, -1));
	face.touch()->_vertices.push_back (Vector3D (-1, 1, 1));
	face.touch()->_vertices.push_back (Vector3D (1, 1, 1));
	face.touch()->_vertices.push_back (Vector3D (1, 1, -1));

	face.touch()->_normals.push_back (Vector3D (-1, 1, -1));
	face.touch()->_normals.push_back (Vector3D (-1, 1, 1));
	face.touch()->_normals.push_back (Vector3D (1, 1, 1));
	face.touch()->_normals.push_back (Vector3D (1, 1, -1));
// 	face.touch()->_normals.push_back (Vector3D (0, 1, 0));
// 	face.touch()->_normals.push_back (Vector3D (0, 1, 0));
// 	face.touch()->_normals.push_back (Vector3D (0, 1, 0));
// 	face.touch()->_normals.push_back (Vector3D (0, 1, 0));


	face.touch()->_vertices.push_back (Vector3D (1, 1, -1));
	face.touch()->_vertices.push_back (Vector3D (1, 1, 1));
	face.touch()->_vertices.push_back (Vector3D (1, -1, 1));
	face.touch()->_vertices.push_back (Vector3D (1, -1, -1));

	face.touch()->_normals.push_back (Vector3D (1, 1, -1));
	face.touch()->_normals.push_back (Vector3D (1, 1, 1));
	face.touch()->_normals.push_back (Vector3D (1, -1, 1));
	face.touch()->_normals.push_back (Vector3D (1, -1, -1));
// 	face.touch()->_normals.push_back (Vector3D (1, 0, 0));
// 	face.touch()->_normals.push_back (Vector3D (1, 0, 0));
// 	face.touch()->_normals.push_back (Vector3D (1, 0, 0));
// 	face.touch()->_normals.push_back (Vector3D (1, 0, 0));



	face.touch()->_vertices.push_back (Vector3D (-1, -1, -1));
	face.touch()->_vertices.push_back (Vector3D (-1, -1, 1));
	face.touch()->_vertices.push_back (Vector3D (-1, 1, 1));
	face.touch()->_vertices.push_back (Vector3D (-1, 1, -1));

	face.touch()->_normals.push_back (Vector3D (-1, -1, -1));
	face.touch()->_normals.push_back (Vector3D (-1, -1, 1));
	face.touch()->_normals.push_back (Vector3D (-1, 1, 1));
	face.touch()->_normals.push_back (Vector3D (-1, 1, -1));
/*	face.touch()->_normals.push_back (Vector3D (-1, 0, 0));
	face.touch()->_normals.push_back (Vector3D (-1, 0, 0));
	face.touch()->_normals.push_back (Vector3D (-1, 0, 0));
	face.touch()->_normals.push_back (Vector3D (-1, 0, 0));
*/
	face.touch()->_face_color = ColorRGBA (1,1,1,1);

	// std::cout << "ugh" << std::endl;
	_g.touch()->_faces.push_back (face);

	//_graphics_outs[0].touch()->_graphics.push_back(_g);

	// std::cout << "agh" << std::endl;
	//_graphics_outs[0].touch()->_graphics.push_back(_g);
}

Cube::~Cube ()
{
	// std::cout << "[Cube]: Destructor" << std::endl;
}

void Cube::process_g (double delta_t)
{
	_graphics_outs[0]._graphics.clear();

	_g.touch()->_commands.clear();

	cow_ptr<Scale> cmd (new Scale);
	cmd.touch()->_scaling_vector = Vector3D (*_control_ins[0], *_control_ins[0], *_control_ins[0]);
	_g.touch()->_commands.push_back(cmd);

	_graphics_outs[0]._graphics.push_back(cow_ptr<Geometry>(_g));
}
