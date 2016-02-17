#include "lighting.h"

#include <iostream>

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		return (GUnit*) new GLighting (); 
	}

	size_t get_num_of_units ()
	{
		return 1;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "GLighting";
		else
			return 0;
	}
}


GLighting::GLighting () :
	_l(new Lighting)
{

}

GLighting::~GLighting ()
{
	// std::cout << "[GLighting]: Destructor" << std::endl;
}



void GLighting::process_g (double delta_t)
{
	_graphics_outs[0]._graphics.clear();

	if (*_control_ins[0] > 0.5)
		_l.touch()->_on = true;
	else
		_l.touch()->_on = false;


	_graphics_outs[0]._graphics.push_back(_l);
}
