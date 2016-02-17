#include "stretch.h"

#include <iostream>

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		return (GUnit*) new Stretch (); 
	}

	size_t get_num_of_units ()
	{
		return 1;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "GStretch";
		else
			return 0;
	}
}


Stretch::Stretch () :
	_s(new Scale)
{

}

Stretch::~Stretch ()
{
	// std::cout << "[Stretch]: Destructor" << std::endl;
}

void Stretch::visitTransformation (Transformation *t)
{
#if 0
	// std::cout << "[Stretch]: visitTransformation" << std::endl;
	Matrix m = t->_transformation_matrix;

	Matrix stretch;
	// stretch.set_null ();
	stretch.set_null ();
	stretch.add_stretch (*_control_ins[1], *_control_ins[2], *_control_ins[3]);
	stretch.at(3,3) = 1;
	
	t->_transformation_matrix = m.mul (stretch);
#endif

	_s.touch()->_scaling_vector = Vector3D (*_control_ins[1], *_control_ins[2], *_control_ins[3]);
	t->_commands.push_back (_s);

}

void Stretch::process_g (double delta_t)
{
  _graphics_outs[0]._graphics.clear();
  std::copy(_graphics_ins[0]->_graphics.begin(), _graphics_ins[0]->_graphics.end(), std::back_inserter(_graphics_outs[0]._graphics));

  for (size_t i = 0; i < _graphics_outs[0]._graphics.size (); ++i)
  {
    _graphics_outs[0]._graphics[i].touch()->accept(this);
  }
}
