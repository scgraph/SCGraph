#include "plugin.h"

#include <dlfcn.h>
#include <assert.h>

GPlugin::GPlugin (const std::string &filename)
{
	_dl = dlopen (filename.c_str (), RTLD_NOW | RTLD_GLOBAL );
	const char *error = dlerror ();
    //std::cout << "error: " << error << std::endl;
	if (error)
	{
       throw ("error!");
        //throw ("[GPlugin]: Error: dlopen failed! \n    Reason: " + std::string(error));
	}

	void *ptr;
	assert (sizeof (ptr) == sizeof (_create));
	assert (sizeof (ptr) == sizeof (_get_num_of_units));
	assert (sizeof (ptr) == sizeof (_get_unit_name));

	// _create = (Unit* (*)(size_t, int))dlsym (_dl, "create");
//error = dlerror ();
	ptr = dlsym (_dl, "create");
	error = dlerror ();
	if (error)
	{
		dlclose (_dl);
		throw (std::string("[GPlugin]: Error: dlsym failed! \n    Reason: ") + std::string (error));
	}
	memcpy (&_create, &ptr, sizeof (ptr));

    error = dlerror ();
	ptr = dlsym (_dl, "get_num_of_units");
	error = dlerror ();
	if (error)
	{
		dlclose (_dl);
		throw (std::string("[GPlugin]: Error: dlsym failed! \n    Reason: ") + std::string (error));
	}
	memcpy (&_get_num_of_units, &ptr, sizeof (ptr));

    error = dlerror ();
	ptr = dlsym (_dl, "get_unit_name");
	error = dlerror ();
	if (error)
	{
		dlclose (_dl);
		throw (std::string("[GPlugin]: Error: dlsym failed! \n    Reason: ") + std::string (error));
	}
	memcpy (&_get_unit_name, &ptr, sizeof (ptr));
}

GPlugin::~GPlugin ()
{
	dlclose (_dl);
}

boost::shared_ptr<GUnit> GPlugin::create_unit (size_t index, int special_index)
{
	boost::shared_ptr<GUnit> ret (_create (index, special_index));
	return ret;
}

size_t GPlugin::get_num_of_units ()
{
	return _get_num_of_units ();
}

std::string GPlugin::get_unit_name (size_t index)
{
	return _get_unit_name (index);
}





