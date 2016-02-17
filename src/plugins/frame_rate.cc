#include "frame_rate.h"

#include <iostream>
#include <cmath>

#include "../scgraph.h"

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


FrameRate::FrameRate ()
{

}

FrameRate::~FrameRate ()
{
	// std::cout << "[FrameRate]: Destructor" << std::endl;
}

void FrameRate::process_c (double delta_t)
{
	_control_outs[0] = 1.0/ScGraph::get_instance()->_delta_t;
}
