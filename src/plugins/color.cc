#include "color.h"

#include <iostream>

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		return (GUnit*) new GColor (); 
	}

	size_t get_num_of_units ()
	{
		return 1;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "GColor";
		else
			return 0;
	}
}


GColor::GColor ()
{

}

GColor::~GColor ()
{
	// std::cout << "[GColor]: Destructor" << std::endl;
}


void GColor::visitGeometry (Geometry *g)
{
    scgColor color(*_control_ins[1], *_control_ins[2], *_control_ins[3], *_control_ins[4]);

	//std::cout << "[GColor]: Destructor" << std::endl;
	for (size_t i = 0; i < g->_faces.size (); ++i)
	{
		if (g->_faces[i]->_colors.size () > 0)
		{
			for (size_t j = 0; j < g->_faces[i]->_colors.size (); ++j)
			{
				g->_faces[i].touch()->_colors[j] = color;
			}
		}
		else
		{
			g->_faces[i].touch()->_face_color = color;
		}
	}
}


void GColor::visitText (Text *t)
{
    scgColor color(*_control_ins[1], *_control_ins[2], *_control_ins[3], *_control_ins[4]);
    
	t->_color = color;
}

void GColor::process_g (double delta_t)
{
	_graphics_outs[0]._graphics.clear();

	std::copy(
		_graphics_ins[0]->_graphics.begin(), 
		_graphics_ins[0]->_graphics.end(), 
		std::back_inserter(_graphics_outs[0]._graphics)
	);

  for (size_t i = 0; i < _graphics_outs[0]._graphics.size (); ++i)
  {
    _graphics_outs[0]._graphics[i].touch()->accept(this);
  }
}
