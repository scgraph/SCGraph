#include "model.h"

#include <iostream>

extern "C"
{
    GUnit *create (size_t index, int special_index)
    {
	return (GUnit*) new Model (); 
    }

    size_t get_num_of_units ()
    {
	return 1;
    }

    const char *get_unit_name (size_t index)
    {
	if (index == 0)
	    return "GModel";
	else
	    return 0;
    }
}



Model::Model ()
{
	//ObjLoader obj = ObjLoader("/Users/hb/src/scgraph/src/fo_frigate.obj");
	ObjLoader obj = ObjLoader("/Users/hb/src/scgraph/src/test.obj");

	_g = obj._geometry;
}

Model::~Model ()
{
    // std::cout << "[Model]: Destructor" << std::endl;
}

void Model::process_g (double delta_t)
{

	_graphics_outs[0]._graphics.clear();

	_g.touch()->_commands.clear();

	cow_ptr<Scale> cmd (new Scale);
	cmd.touch()->_scaling_vector = Vector3D (*_control_ins[0], *_control_ins[0], *_control_ins[0]);
	_g.touch()->_commands.push_back(cmd);

	_graphics_outs[0]._graphics.push_back(cow_ptr<Geometry>(_g));
}
