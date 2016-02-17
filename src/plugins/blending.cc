#include "blending.h"

#include <iostream>


extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		return (GUnit*) new GBlending (); 
	}

	size_t get_num_of_units ()
	{
		return 1;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "GBlending";
		else
			return 0;
	}
}


GBlending::GBlending () :
	_b(new Blending)
{
}


GBlending::~GBlending ()
{
	// std::cout << "[GBlending]: Destructor" << std::endl;
}


void GBlending::process_g (double delta_t)
{
	_graphics_outs[0].clear ();

	if (*_control_ins[0] > 0.5)
		_b.touch()->_on = true;
	else
		_b.touch()->_on = false;

	_graphics_outs[0]._graphics.push_back (_b);
}
