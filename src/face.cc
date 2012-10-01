#include "face.h"

Face::Face (GeometryType) :
	//_per_vertex_colors (false),
	_texture_index (0),
	_frame_id (0),
	_thickness (1.0),
	_culling (0)
{

}

Face::Face (const Face &f)
{
	_geometry_type = f._geometry_type;

	_vertices = f._vertices;

	//_per_vertex_colors = f._per_vertex_colors;
	_colors = f._colors;

	_face_color = f._face_color;

	_texture_coordinates = f._texture_coordinates;
	_texture_index = f._texture_index;
	_frame_id = f._frame_id;

	_normals = f._normals;

	_material = f._material;

	_thickness = f._thickness;

	_culling = f._culling;
}


