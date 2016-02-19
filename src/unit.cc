#include "unit.h"

#include <iostream>

#include "scgraph.h"


GUnit::GUnit (int special_index) :
	_special_index (special_index),
	_done_action (0)
{
}

GUnit::~GUnit ()
{
	// std::cout << "[GUnit]: Destructor" << std::endl;
}

void GUnit::process_g (double delta_t)
{
	_control_outs[0] = 0;
	throw (std::runtime_error("[GUnit]: Error: This unit does not have a graphics rate processing function"));
}

void GUnit::process_c (double delta_t)
{
	_control_outs[0] = 0;
	throw (std::runtime_error("[GUnit]: Error: This unit does not have a control rate processing function"));
}

const int GUnit::get_done_action ()
{
	// std::cout << "unit: _done_action: " << _done_action << std::endl;
	return _done_action;
}
