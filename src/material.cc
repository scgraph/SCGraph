#include "material.h"

/** a material */
Material::Material () :
	_shinyness (0.0)
{
	for (int i = 0; i < 4; ++i)
	{	
		_ambient_reflection._c[i] = 1.0;
		_diffuse_reflection._c[i] = 1.0;
		_specular_reflection._c[i] = 1.0;
		_emissive_color._c[i] = 0.0;
	}
}

void Material::scale_alpha(float factor) {
	_ambient_reflection.scale_alpha(factor);
	_diffuse_reflection.scale_alpha(factor);
	_specular_reflection.scale_alpha(factor);
	_emissive_color.scale_alpha(factor);
}

