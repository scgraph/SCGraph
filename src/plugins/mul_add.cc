#include "mul_add.h"

#include <iostream>

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		return (GUnit*) new MulAdd (); 
	}

	size_t get_num_of_units ()
	{
		return 1;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "MulAdd";
		else
			return 0;
	}
}


MulAdd::MulAdd ()
{

}

MulAdd::~MulAdd ()
{
	// std::cout << "[MulAdd]: Destructor" << std::endl;
}

void MulAdd::process_c (double delta_t)
{
	_control_outs[0] = *_control_ins[0] * *_control_ins[1] + *_control_ins[2];
}
