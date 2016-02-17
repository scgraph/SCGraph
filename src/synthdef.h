/* Copyright 2006 Florian Paul Schmidt */

#ifndef SCGRAPH_SYNTHDEF
#define SCGRAPH_SYNTHDEF

#include "util.h"

#include <string>
#include <vector>

struct ParamName
{
	ParamName (unsigned char **data, unsigned long int size);

	std::string  _name;
	int16_t      _index;
};

struct InputSpec
{
	InputSpec (unsigned char **data, unsigned long int size);

	/** if _index == -1 then a constant is indexed
	    otherwise the ugen's output index */
	int16_t _index_of_ugen;
	int16_t _index_of_constant;
	int16_t _index_of_ugen_output;
};

struct OutputSpec
{
	OutputSpec (unsigned char **data, unsigned long int size);

	int8_t _calculation_rate;
};

struct UgenSpec 
{
	UgenSpec (unsigned char **data, unsigned long int size);

	std::string              _name;
	int8_t                   _calculation_rate;
	int16_t                  _special_index;
	std::vector <InputSpec>  _input_specs;
	std::vector <OutputSpec> _output_specs;
};

struct GSynthDef 
{
	std::string             _name;
	std::vector <float>     _constants;
	std::vector <float>     _parameters;
	std::vector <ParamName> _param_names;
	std::vector <UgenSpec>  _ugen_specs;

	/** this constructor builds the instance from the 
	    synthdef data passed to it. 
	    it will advance the data pointer by the appropriate 
	    number of bytes.
	    To do this it has to kow the address of the pointer
	    itself to be able to modify it
	 */
	GSynthDef (unsigned char **data, unsigned long int size);
};

#endif
