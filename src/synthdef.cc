/* Copyright 2006 Florian Paul Schmidt */

#include "synthdef.h"
#include "scgraph.h"

#include <iostream>

GSynthDef::GSynthDef (unsigned char **data, unsigned long int size)
{
	Options *options = Options::get_instance ();

	if (options->_verbose >= 3)
		std::cout << "[GSynthDef]:" << std::endl;


	if (options->_verbose >= 3)
	{
		std::cout << " [Hexdump]:" << std::hex << std::endl;
		int col_count = 0;
		for (unsigned char *i = *data; i != *data + size; ++i)
		{
			if (col_count == 0)
				std::cout << "  ";
			// std::cout << (unsigned int)*i << " ";
			if ((*i) < 16)
				std::cout << "0";
	
			std::cout << (unsigned int)(*i);
	
			col_count += 1;
			col_count %= 16;
	
			std::cout << " ";
	
			if (col_count == 0)
				std::cout << std::endl;
		}
		std::cout << std::dec << std::endl;
		// std::cout << " [HexDump end]" << std::endl;
	}

	unsigned char *orig_data = *data;

	if (options->_verbose >= 3)
		std::cout << " size: " << size << std::endl;
	
	// the name
	_name = read_string (data);

	if (options->_verbose >= 3)
		std::cout << " name: " << _name << std::endl;

	// the constants
	uint32_t num = read_int32_t (data);

	if (options->_verbose >= 3)
		std::cout << " constants [" << num << "]: ";

	for (uint32_t i = 0; i < num; ++i)
	{
		float tmp = read_float (data);
		_constants.push_back (tmp);

		if (options->_verbose >= 3)
			std::cout << tmp << " ";
	}

	if (options->_verbose >= 3)
		std::cout << std::endl;

	// the parameters
	num = read_int32_t (data);

	if (options->_verbose >= 3)
		std::cout << " parameters [" << num << "]: ";

	for (uint32_t i = 0; i < num; ++i)
	{
		float tmp = read_float (data);
		_parameters.push_back (tmp);

		if (options->_verbose >= 3)
			std::cout << tmp << " ";
	}

	if (options->_verbose >= 3)
		std::cout << std::endl;

	// the parameter names
	num = read_int32_t (data);

	if (options->_verbose >= 3)
		std::cout << " parameter names [" << num << "]:" << std::endl;

	for (uint32_t i = 0; i < num; ++i)
	{
		_param_names.push_back (ParamName (data, size - (*data - orig_data)));
	}

	// the ugen-specs
	num = read_int32_t (data);

	if (options->_verbose >= 3)
		std::cout << " ugen specs [" << num << "]:" <<  std::endl;

	for (uint32_t i = 0; i < num; ++i)
	{
		
		if (options->_verbose >= 3)
			std::cout << "  [" << i << "]:";

		_ugen_specs.push_back (UgenSpec (data, size - (*data - orig_data)));
	}
}

ParamName::ParamName (unsigned char **data, unsigned long int size)
{
	Options *options = Options::get_instance ();

	_name = read_string (data);
	_index = read_int32_t (data);

	if (options->_verbose >= 3)
	{
		std::cout << " [ParamName]:" << std::endl;
		std::cout << "  name: " << _name << std::endl
		          << "  index: " << _index << std::endl;
	}
}

UgenSpec::UgenSpec (unsigned char **data, unsigned long int size)
{
	Options *options = Options::get_instance ();

	unsigned char *orig_data = *data;

	if (options->_verbose >= 3)
		std::cout << "[UgenSpec]:" << std::endl;

	_name = read_string (data);

	if (options->_verbose >= 3)
		std::cout << "   name: " << _name << std::endl;

	_calculation_rate = read_int8_t (data);

	if(_name.compare("Control") == 0)
		_calculation_rate = 1;

	if (options->_verbose >= 3)
		std::cout << "    calculation_rate: " << (int)_calculation_rate << std::endl;

		uint32_t num_ins = read_int32_t (data);
		uint32_t num_outs =read_int32_t (data);

	_special_index = read_int16_t (data);

	if (options->_verbose >= 3)
		std::cout << "    num_ins: " << num_ins << " nums_outs: " << num_outs << " special_index: " << _special_index << std::endl;

	for (uint32_t i = 0; i < num_ins; ++i)
		_input_specs.push_back (InputSpec (data, size - (*data - orig_data)));
	
	for (uint32_t i = 0; i < num_outs; ++i)
		_output_specs.push_back (OutputSpec (data, size - (*data - orig_data)));
}

InputSpec::InputSpec (unsigned char **data, unsigned long int size)
{
	Options *options = Options::get_instance ();

	if (options->_verbose >= 3)
		std::cout << "   [InputSpec]:" << std::endl;

	_index_of_ugen = read_int32_t (data);

	if (options->_verbose >= 3)
		std::cout << "     index_of_ugen: " << _index_of_ugen;

	if (_index_of_ugen == -1)
	{
		_index_of_constant = read_int32_t (data);
		if (options->_verbose >= 3)
			std::cout << "  index_of_constant: " << _index_of_constant;
	}
	else
	{
		_index_of_ugen_output = read_int32_t (data);
		if (options->_verbose >= 3)
			std::cout << " index_of_ugen_output: " << _index_of_ugen_output;
	}
	
	if (options->_verbose >= 3)
		std::cout << std::endl;
}

OutputSpec::OutputSpec (unsigned char **data, unsigned long int size)
{
	Options *options = Options::get_instance ();

	if (options->_verbose >= 3)
		std::cout << "   [OutputSpec]:" << std::endl;

	_calculation_rate = read_int8_t (data);

	if (options->_verbose >= 3)
		std::cout << "     calculation_rate: " << (int)_calculation_rate << std::endl;
}
