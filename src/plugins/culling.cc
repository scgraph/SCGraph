#include "culling.h"

#include <iostream>

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		return (GUnit*) new GCulling (); 
	}

	size_t get_num_of_units ()
	{
		return 1;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "GCulling";
		else
			return 0;
	}
}


GCulling::GCulling () :
	_c(new Culling)
{
}

GCulling::~GCulling ()
{
	// std::cout << "[GCulling]: Destructor" << std::endl;
}


void GCulling::process_g (double delta_t)
{
	_graphics_outs[0]._graphics.clear();

	int mode = (int)(*_control_ins[0]);
	switch (mode)
	{
		case 0:
			_c.touch()->_mode = Culling::Off;
		break;
		case 1:
			_c.touch()->_mode = Culling::Front;
		break;
		case 2:
			_c.touch()->_mode = Culling::Back;
		break;
		case 3:
			_c.touch()->_mode = Culling::Both;
		break;
	}

	_graphics_outs[0]._graphics.push_back(_c);
}
