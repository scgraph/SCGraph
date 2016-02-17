#include "shader_program.h"

#include <iostream>

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		if (index == 0) return (GUnit*) new GShaderProgram (); 
		if (index == 1) return (GUnit*) new GShaderUniform (); 
		else std::cout << "[GShaderProgram]: wrong type!" << std::endl;
	}

	size_t get_num_of_units ()
	{
		return 2;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "GShaderProgram";
		if (index == 1)
			return "GShaderUniform";
		else
			return 0;
	}
}


GShaderProgram::GShaderProgram () : 
	_l(new ShaderProgram)
{

}

GShaderProgram::~GShaderProgram ()
{
	// std::cout << "[GShaderProgram]: Destructor" << std::endl;
}



void GShaderProgram::process_g (double delta_t)
{
	_graphics_outs[0]._graphics.clear();

	_l.touch()->_index = (size_t)(*_control_ins[0]);

	if (*_control_ins[1] > 0.5)
		_l.touch()->_on = true;
	else
		_l.touch()->_on = false;


	_graphics_outs[0]._graphics.push_back(_l);
}

GShaderUniform::GShaderUniform () : 
	_l(new ShaderUniform),
	_first_time(true)
{

}

GShaderUniform::~GShaderUniform ()
{
	// std::cout << "[GShaderUniform]: Destructor" << std::endl;
}



void GShaderUniform::process_g (double delta_t)
{
	_graphics_outs[0]._graphics.clear();

	_l.touch()->_uniform_index = (size_t)(*_control_ins[0]);

	if (_first_time)
	{
		_first_time = false;
		_values.resize((size_t)(*_control_ins[1]));
	}

	for (size_t i = 0; i < _values.size(); ++i) {
		_values[i] = *_control_ins[2+i];
		//std::cout << "shader uniform " << _l->_uniform_index << ": " << _values[i] << std::endl;
	}
	_l.touch()->_values = _values;

	_graphics_outs[0]._graphics.push_back(_l);
}



