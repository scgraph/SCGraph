#include "text.h"

#include <iostream>

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		return (GUnit*) new Textfield (); 
	}

	size_t get_num_of_units ()
	{
		return 1;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "GText";
		else
			return 0;
	}
}


Textfield::Textfield () :
	_t(new Text)
{
	_t.touch()->_color = ColorRGBA (1,1,1,1);

	_t.touch()->_fontsize = 24;
	
	_t.touch()->_text = std::string("Blub");
}

Textfield::~Textfield ()
{
	// std::cout << "[Textfield]: Destructor" << std::endl;
}

void Textfield::process_g (double delta_t)
{

	StringPool *sp = StringPool::get_instance();

	_t.touch()->_fontsize = (unsigned int) *_control_ins[0];

	unsigned int index = (unsigned int) *_control_ins[1];

	if(index < sp->get_number_of_strings()) 
		_t.touch()->_text = sp->get_string(index);

	_graphics_outs[0]._graphics.clear();

	_graphics_outs[0]._graphics.push_back(_t);
}
