#ifndef SCGRAPH_MATERIAL_HH
#define SCGRAPH_MATERIAL_HH

#include "scgcolor.h"

/** a material */
struct Material
{
	float     _shinyness;
	scgColor _ambient_reflection;
	scgColor _diffuse_reflection;
	scgColor _specular_reflection;
	scgColor _emissive_color;

	Material ();

	void scale_alpha(float factor);
};

#endif
