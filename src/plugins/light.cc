#include "light.h"

#include <iostream>

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		return (GUnit*) new GLight (); 
	}

	size_t get_num_of_units ()
	{
		return 1;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "GLight";
		else
			return 0;
	}
}


GLight::GLight () :
	_l (new Light)
{

}

GLight::~GLight ()
{
	// std::cout << "[GLight]: Destructor" << std::endl;
}

void GLight::process_g (double delta_t)
{
	_l.touch()->_index = (int)(*_control_ins[0]);

	if (*_control_ins[1] > 0.5)
		_l.touch()->_on = true;
	else 
		_l.touch()->_on = false;

	_l.touch()->_position._c[0] = *_control_ins[2];
	_l.touch()->_position._c[1] = *_control_ins[3];
	_l.touch()->_position._c[2] = *_control_ins[4];
	_l.touch()->_position._c[3] = *_control_ins[5];

	_l.touch()->_spot_direction._c[0] = *_control_ins[6];
	_l.touch()->_spot_direction._c[1] = *_control_ins[7];
	_l.touch()->_spot_direction._c[2] = *_control_ins[8];

	_l.touch()->_spot_exponent = *_control_ins[9];
	_l.touch()->_spot_cutoff = *_control_ins[10];

	_l.touch()->_ambient_color._c[0] = *_control_ins[11];
	_l.touch()->_ambient_color._c[1] = *_control_ins[12];
	_l.touch()->_ambient_color._c[2] = *_control_ins[13];
	_l.touch()->_ambient_color._c[3] = *_control_ins[14];

	_l.touch()->_diffuse_color._c[0] = *_control_ins[15];
	_l.touch()->_diffuse_color._c[1] = *_control_ins[16];
	_l.touch()->_diffuse_color._c[2] = *_control_ins[17];
	_l.touch()->_diffuse_color._c[3] = *_control_ins[18];

	_l.touch()->_specular_color._c[0] = *_control_ins[19];
	_l.touch()->_specular_color._c[1] = *_control_ins[20];
	_l.touch()->_specular_color._c[2] = *_control_ins[21];
	_l.touch()->_specular_color._c[3] = *_control_ins[22];

	_l.touch()->_constant_attenuation = *_control_ins[23];
	_l.touch()->_linear_attenuation = *_control_ins[24];
	_l.touch()->_quadratic_attenuation = *_control_ins[25];

	_graphics_outs[0]._graphics.clear();
	_graphics_outs[0]._graphics.push_back(_l);
}
