#include "material.h"

/** a material */
Material::Material () :
	_shinyness (0.0)
{
	for (int i = 0; i < 4; ++i)
	{	
		_ambient_reflection[i] = 1.0;
		_diffuse_reflection[i] = 1.0;
		_specular_reflection[i] = 1.0;
		_emissive_color[i] = 0.0;
	}
}

void Material::scale_alpha(float factor) {
    _ambient_reflection[3] *= factor;
    _diffuse_reflection[3] *= factor;
    _specular_reflection[3] *= factor;
    _emissive_color[3] *= factor;
}

