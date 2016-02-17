#include "rotate.h"

#include <iostream>

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		return (GUnit*) new Rotate (); 
	}

	size_t get_num_of_units ()
	{
		return 1;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "GRotate";
		else
			return 0;
	}
}


Rotate::Rotate () :
  _r(new Rotation)
{

}

Rotate::~Rotate ()
{
}

void Rotate::visitTransformation (Transformation *t)
{
	boost::shared_ptr<Rotation> r (new Rotation);
	_r.touch()->_rotation_vector = Vector3D (*_control_ins[1], *_control_ins[2], *_control_ins[3]);
	_r.touch()->_rotation_angle = *_control_ins[4];
	t->_commands.push_back (_r);
}


void Rotate::process_g (double delta_t)
{
  _graphics_outs[0]._graphics.clear();
  std::copy(_graphics_ins[0]->_graphics.begin(), _graphics_ins[0]->_graphics.end(), std::back_inserter(_graphics_outs[0]._graphics));

  for (size_t i = 0; i < _graphics_outs[0]._graphics.size (); ++i)
  {
    _graphics_outs[0]._graphics[i].touch()->accept(this);
  }
}
