#include "string_pool.h"

#include "options.h"

#include <sstream>
#include <iostream>
#include <cmath>

#include <sys/types.h>
#include <dirent.h>

#include <cstring>

StringPool *StringPool::_instance = 0;

StringPool *StringPool::get_instance ()
{
	if (!_instance)
		_instance = new StringPool;

	return _instance;
}

StringPool::StringPool () :
	font("/Users/hb/src/scgraph/Bitstream-Vera-Sans-Mono.ttf")
{
	// Create a pixmap font from a TrueType file.

	font.UseDisplayList(true);
	// If something went wrong, bail out.
	if(font.Error())
		std::cout << "[StringPool]: Font loading not working!" << std::endl;
}


unsigned int StringPool::add_string (const std::string &str, unsigned int index)
{
	add_string(str);
	return _strings.size() - 1;
}

unsigned int StringPool::change_string (const std::string &str, unsigned int index)
{
	if(index < _strings.size())
		_strings[index] = str;
	return 0;
}

void StringPool::add_string (const std::string &str)
{
	_strings.push_back(str);

	std::cout << "[StringPool]: Adding string: " << str << std::endl;	
	std::cout << "              New string has index: " 
			  << _strings.size() - 1 << std::endl;

}

StringPool::~StringPool ()
{
	// TODO: clean up
}

unsigned int StringPool::get_number_of_strings ()
{
	return _strings.size ();
}

std::string StringPool::get_string (unsigned int index)
{
	return _strings[index];
}



