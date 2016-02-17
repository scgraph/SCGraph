#include "transformation.h"

#include <iostream>

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		return (GUnit*) new GTransformation (); 
	}

	size_t get_num_of_units ()
	{
		return 1;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "GTransform";
		else
			return 0;
	}
}


GTransformation::GTransformation() :
	_l(new Linear)
{

}

GTransformation::~GTransformation ()
{
	// std::cout << "[GTransformation]: Destructor" << std::endl;
}

void GTransformation::visitTransformation (Transformation *t)
{
	for (int i = 0; i < 16; ++i)
	{
		(_l.touch()->_transformation_matrix.get_coefficients())[i] = *_control_ins[i+1];
	}
	t->_commands.push_back (_l);
}

void GTransformation::process_g (double delta_t)
{
	_graphics_outs[0]._graphics.clear();
	std::copy(_graphics_ins[0]->_graphics.begin(), _graphics_ins[0]->_graphics.end(), std::back_inserter(_graphics_outs[0]._graphics));

  for (size_t i = 0; i < _graphics_outs[0]._graphics.size (); ++i)
	{
		_graphics_outs[0]._graphics[i].touch()->accept(this);
	}
}
