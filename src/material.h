#ifndef SCGRAPH_MATERIAL_HH
#define SCGRAPH_MATERIAL_HH

//#include "color_rgba.h"

#include "ofColor.h"

/** a material */
struct Material
{
	float     _shinyness;
	ofColor _ambient_reflection;
	ofColor _diffuse_reflection;
	ofColor _specular_reflection;
	ofColor _emissive_color;

	Material ();

	void scale_alpha(float factor);
};

#endif
