#include "translate.h"

#include <iostream>
#include <cmath>
#include <algorithm>

#include "../transformation_command.h"

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		return (GUnit*) new Translate (); 
	}

	size_t get_num_of_units ()
	{
		return 1;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "GTranslate";
		else
			return 0;
	}
}

Translate::Translate () :
	_t(new Translation)
{

}

Translate::~Translate ()
{

}

void Translate::visitTransformation (Transformation *t)
{
	_t.touch()->_translation_vector = Vector3D (*_control_ins[1], *_control_ins[2], *_control_ins[3]);
	t->_commands.push_back (_t);
}


void Translate::process_g (double delta_t)
{
	_graphics_outs[0]._graphics.clear();
	std::copy(_graphics_ins[0]->_graphics.begin(), _graphics_ins[0]->_graphics.end(), std::back_inserter(_graphics_outs[0]._graphics));

  for (size_t i = 0; i < _graphics_outs[0]._graphics.size (); ++i)
	{
		_graphics_outs[0]._graphics[i].touch()->accept(this);
	}
}
