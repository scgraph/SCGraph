#ifndef SCGRAPH_PLUGIN_HH
#define SCGRAPH_PLUGIN_HH

#include <string>
#include <cstring>
#include "unit.h"

#include <boost/shared_ptr.hpp>

class GPlugin
{
	void *_dl;

	GUnit*  (*_create) (size_t, int);
	size_t (*_get_num_of_units)(void);
	const char*  (*_get_unit_name)(size_t);
	

	public:
		GPlugin (const std::string &filename);
		~GPlugin ();

		std::string get_unit_name (size_t index);

		size_t get_num_of_units ();

		boost::shared_ptr<GUnit> create_unit (size_t index, int special_index);
};

#endif
