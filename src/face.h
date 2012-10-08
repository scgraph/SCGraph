#ifndef SCGRAPH_FACE_HH
#define SCGRAPH_FACE_HH

#include "color_rgba.h"
#include "material.h"
#include "vector_3d.h"
#include "vector_2d.h"

#include <vector>
#include "cow_ptr.h"

enum RenderMode { NORMAL, WIREFRAME } ;

/** This class represents a face of a graphics object */
struct Face : public DeepCopyable
{
	/** these types correspond to OpenGL types for vertices */
  enum GeometryType {POINTS, LINES, LINE_STRIP, LINE_LOOP, TRIANGLES, 
					 QUADS, TRIANGLE_STRIP, TRIANGLE_FAN, QUAD_STRIP,
					 POLYGON};
  
	GeometryType _geometry_type;

	RenderMode _render_mode;

	/** the vertices of the face. For every vertex you need a normal, too */ 
	std::vector<Vector3D>      _vertices;

	std::vector<Vector3D>      _normals;

	/** if this vector is nonempty it has to have the same size
	    as _vertices. if it's empty then no texture will be applied */
	std::vector<Vector2D>      _texture_coordinates;

	/** this indicates which texture to use */
	int               _texture_index;
	uint32_t               _frame_id;

	/** use this color vector if you use per vertex color.
	    in the other case use the per face color below.
	     if this is nonempty it has to have the same size as _vertices */
	std::vector<ColorRGBA>     _colors;
	float _alpha_mul;

	/** set to true if you want to specify colors per vertex */
	//bool                       _per_vertex_colors;

	/** this is the single color used to draw the face when lighting
	    is disabled and _per_vertex_colors is false */
	ColorRGBA                  _face_color;


	Material                   _material;

	float                      _thickness;

	/** 0 - no culling, 1 - back_culling, 2 - front culling */
	int                        _culling;

	Face (GeometryType = POINTS);

	Face (const Face &f);

	virtual Face *deepCopy() {
		return new Face(*this);
	}

	virtual ~Face() { }
};


#endif
