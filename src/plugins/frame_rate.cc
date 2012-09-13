#include "frame_rate.h"

#include <iostream>
#include <cmath>

#include "../options.h"

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		return (GUnit*) new FrameRate (); 
	}

	size_t get_num_of_units ()
	{
		return 1;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "FrameRate";
		else
			return 0;
	}
}


FrameRate::FrameRate () :
	_value (1.0)
{

}

FrameRate::~FrameRate ()
{
	// std::cout << "[FrameRate]: Destructor" << std::endl;
}

void FrameRate::process_c (double delta_t)
{
	Options *options = Options::get_instance ();
	_control_outs[0] = options->_graphics_rate;
}
