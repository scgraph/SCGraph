#include "face.h"


Face::Face (GeometryType geoType) :
	//_per_vertex_colors (false),
	_render_mode (NORMAL),
	_texture_index (0),
	_frame_id (0),
	_alpha_mul (1.0),
	_thickness (1.0),
	_culling (0)
{
    _geometry_type = geoType;
}

Face::Face (const Face &f) :
	_geometry_type(f._geometry_type),
	_render_mode(f._render_mode),
	_vertices(f._vertices),
	//_per_vertex_colors(f._per_vertex_colors),
	_normals(f._normals),
	_texture_coordinates(f._texture_coordinates),
	_texture_index(f._texture_index),
	_frame_id(f._frame_id),
	_colors(f._colors),
	_alpha_mul(f._alpha_mul),
	_face_color(f._face_color),
	_material(f._material),
	_thickness(f._thickness),
	_culling(f._culling)
{

}


