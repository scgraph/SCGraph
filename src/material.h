#ifndef SCGRAPH_MATERIAL_HH
#define SCGRAPH_MATERIAL_HH

#include "color_rgba.h"

/** a material */
struct Material
{
	float     _shinyness;
	ColorRGBA _ambient_reflection;
	ColorRGBA _diffuse_reflection;
	ColorRGBA _specular_reflection;
	ColorRGBA _emissive_color;

	Material ();

	void scale_alpha(float factor);
};

#endif
