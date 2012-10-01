#include "buf_info_ugens.h"

#include <iostream>
#include <cmath>

#include "../texture_pool.h"

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		if(index == 0)
			return (GUnit*) new BufFrameRate (); 

		if(index == 1)
			return (GUnit*) new BufFrames (); 

		return 0;
	}

	size_t get_num_of_units ()
	{
		return 2;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "BufFrameRate";
		
		if (index == 1)
			return "BufFrames";
		return 0;
	}
}


BufFrameRate::BufFrameRate () :
	_value (1.0)
{

}

BufFrameRate::~BufFrameRate ()
{
	// std::cout << "[BufFrameRate]: Destructor" << std::endl;
}

void BufFrameRate::process_c (double delta_t)
{
	TexturePool *tp = TexturePool::get_instance ();
	boost::optional<boost::shared_ptr<AbstractTexture> > t = 
		tp->get_texture((int) *_control_ins[0]);
	if(t) {
		_control_outs[0] = (*t)->get_framerate();
			}
	else {
		_control_outs[0] = 0;
	}
}


BufFrames::BufFrames () :
	_value (1.0)
{

}

BufFrames::~BufFrames ()
{
	// std::cout << "[BufFrames]: Destructor" << std::endl;
}

void BufFrames::process_c (double delta_t)
{
	TexturePool *tp = TexturePool::get_instance ();
	boost::optional<boost::shared_ptr<AbstractTexture> > t = 
		tp->get_texture((int) *_control_ins[0]);
	if(t) {
		_control_outs[0] = (*t)->get_num_frames();
			}
	else {
		_control_outs[0] = 0;
	}
}
