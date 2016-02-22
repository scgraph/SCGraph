#include "quad.h"
#include "../texture_pool.h"
#include <iostream>

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		if (index == 0)
			return (GUnit*) new Quad (); 

		if (index == 1)
			return (GUnit*) new TexQuad (); 

		if (index == 2)
			return (GUnit*) new FBTexQuad (); 

		return 0;
	}

	size_t get_num_of_units ()
	{
		return 3;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "GQuad";

		if (index == 1)
			return "GTexQuad";

		if (index == 2)
			return "GFBTexQuad";
 
		return 0;
	}
}


Quad::Quad () :
	_g(new Geometry)
{
	/* create face data - only one face now */
	cow_ptr<Face> face (new Face);
	face.touch()->_geometry_type = Face::QUADS;

	// face.touch()->_material._emissive_color[0] = 1.0;

	for (size_t i = 0; i < 4; ++i)
		{
			face.touch()->_vertices.push_back (ofVec3f ());
			face.touch()->_normals.push_back (ofVec3f (0,0,1));
		}
	
	face.touch()->_face_color = scgColor (1,1,1,1);

	_g.touch()->_faces.push_back (face);
}

Quad::~Quad ()
{
	// std::cout << "[Quad]: Destructor" << std::endl;
}

void Quad::process_g (double delta_t)
{
	_graphics_outs[0]._graphics.clear();

	// std::cout << "Quad" << std::endl;

	for (size_t i = 0; i < 3; ++i)
		{
		for (size_t j = 0; j < 4; ++j) 
			{
				_g.touch()->_faces[0].touch()->_vertices[j][i] = 
					*_control_ins[i + (j * 3)];

				_g.touch()->_faces[0].touch()->_normals[j][i] = 
					*_control_ins[i+12];
			}
		}

	_graphics_outs[0]._graphics.push_back (_g);
}


TexQuad::TexQuad () :
	_g(new Geometry)
{
    //std::cout << "[TexQuad] constructor" << std::endl;
    TexturePool *texture_pool = TexturePool::get_instance ();
    
	_texquad_id_a = texture_pool->get_id();
	_texquad_id_b = texture_pool->get_id();
	_tex_index_a = 0;
	_tex_index_b = 0;
	_frame_index_a = -1;
	_frame_index_b = -1;
	_last_frame_index = -1;
	_last_tex_index = -1;
	_flip = false;

	_processing = false;

	// create face data - only one face now 
	cow_ptr<Face> face (new Face);
	face.touch()->_geometry_type = Face::QUADS;

    // set up double buffers
    texture_pool->texture_loaded.add(this, &TexQuad::flip, 0);
    
	// face.touch()->_material._emissive_color[0] = 1.0;

	for (int i = 0; i < 4; ++i)
	{
		face.touch()->_vertices.push_back (ofVec3f ());
		face.touch()->_normals.push_back (ofVec3f ());
		face.touch()->_texture_coordinates.push_back (ofVec2f());
	}

	face.touch()->_face_color = scgColor (1,1,1,1);
	_g.touch()->_faces.push_back (face);

}

TexQuad::~TexQuad () {
	//std::cout << "[TexQuad] destructor" << std::endl;
	TexturePool *texture_pool = TexturePool::get_instance ();
	texture_pool->delete_textures_at_id(_texquad_id_a);
	texture_pool->delete_textures_at_id(_texquad_id_b);
    
    texture_pool->texture_loaded.remove(this, &TexQuad::flip, 0);
}

void TexQuad::flip(uint32_t & tex_id) {
	// look if broadcast was for us
	//std::cout << "[TexQuad] flip" << std::endl;
	if(tex_id == _texquad_id_a)
		_flip = true;
	else if(tex_id == _texquad_id_b)
		_flip = false;
}


void TexQuad::process_g (double delta_t) {

    
    
	_graphics_outs[0]._graphics.clear();

	// std::cout << "[TexQuad] process_g" << std::endl;

	for (size_t i = 0; i < 3; ++i) {
		for (size_t j = 0; j < 4; ++j) {
			_g.touch()->_faces[0].touch()->_vertices[j][i] =
				*_control_ins[i + (j * 3)];

			_g.touch()->_faces[0].touch()->_normals[j][i] =
				*_control_ins[i+22];
		}
	}

	for (size_t i = 0; i < 2; ++i) {
		for( size_t j = 0; j < 4; ++j) {
			_g.touch()->_faces[0].touch()->_texture_coordinates[j][i] =
				*_control_ins[i+12+(j*2)];
		}
	}

	if((_last_frame_index == (int)*_control_ins[21])
	   && (_last_tex_index == (int)*_control_ins[20])) {
		_g.touch()->_faces[0].touch()->_texture_index = _last_tex_index;
		if(_flip) {
			_g.touch()->_faces[0].touch()->_frame_id = _texquad_id_a; }
		else {
			_g.touch()->_faces[0].touch()->_frame_id = _texquad_id_b; }
	}
	else {
		_last_frame_index = (int)*_control_ins[21];
		_last_tex_index = (int)*_control_ins[20];

		TexturePool *texture_pool = TexturePool::get_instance ();
		boost::optional<boost::shared_ptr<AbstractTexture > > t = 
			texture_pool->get_texture((int)*_control_ins[20]);

		if(t) {
			if((*(t))->isVideo()) {
				if(_flip) {
					_tex_index_b = (int)*_control_ins[20];
					_frame_index_b = std::max((int)*_control_ins[21], 0);
					//std::cout << "[TexQuad] " << _frame_index_b << std::endl;
					(*t)->get_frame(_texquad_id_b, _frame_index_b);
					_g.touch()->_faces[0].touch()->_texture_index = _tex_index_a;
					_g.touch()->_faces[0].touch()->_frame_id = _texquad_id_a;
				}
				else {
					_tex_index_a = (int)*_control_ins[20];
					_frame_index_a = std::max((int)*_control_ins[21], 0);

					(*t)->get_frame(_texquad_id_a, _frame_index_a);
					_g.touch()->_faces[0].touch()->_texture_index = _tex_index_b;
					_g.touch()->_faces[0].touch()->_frame_id = _texquad_id_b;
				}
			}
			else {
				_g.touch()->_faces[0].touch()->_texture_index = (int)*_control_ins[20];
			}
		}
	}
	_graphics_outs[0]._graphics.push_back (_g);
}


FBTexQuad::FBTexQuad () :
	_g(new Geometry)
{
	/* create face data - only one face now */
	cow_ptr<Face> face (new Face);
	face.touch()->_geometry_type = Face::QUADS;

	// face.touch()->_material._emissive_color[0] = 1.0;

	for (int i = 0; i < 4; ++i)
	{
		face.touch()->_vertices.push_back (ofVec3f ());
		face.touch()->_normals.push_back (ofVec3f ());
		face.touch()->_texture_coordinates.push_back (ofVec2f());
	}

	face.touch()->_face_color = scgColor (1,1,1,1);

	_g.touch()->_faces.push_back (face);
}

FBTexQuad::~FBTexQuad ()
{
	// std::cout << "[FBTexQuad]: Destructor" << std::endl;
}

void FBTexQuad::process_g (double delta_t)
{
	// std::cout << "fbtexquad" << std::endl;
	_graphics_outs[0]._graphics.clear();

	for (size_t i = 0; i < 3; ++i)
		{
		for (size_t j = 0; j < 4; ++j)
			{
				_g.touch()->_faces[0].touch()->_vertices[j][i] = 
					*_control_ins[i + (j * 3)];

				_g.touch()->_faces[0].touch()->_normals[j][i] = 
					*_control_ins[i+21];
			}
		}

	for (size_t i = 0; i < 2; ++i)
		{
		for( size_t j = 0; j < 4; ++j)
			{
				_g.touch()->_faces[0].touch()->_texture_coordinates[j][i] =
					*_control_ins[i+12+(j*2)];
			}
		}

	//std::cout << (int)(*_control_ins[20] * -1) << std::endl;
	_g.touch()->_faces[0].touch()->_texture_index = (int)(*_control_ins[20] * -1);

	// std::cout << (unsigned int)*_control_ins[20] << std::endl;

	_graphics_outs[0]._graphics.push_back (_g);
}
