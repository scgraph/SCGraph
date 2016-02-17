#ifndef SCGRAPH_GRAPHICS_BUS_HH
#define SCGRAPH_GRAPHICS_BUS_HH

#include <list>
#include <boost/shared_ptr.hpp>

#include "graphics.h"


struct GraphicsBus 
{
	std::vector <cow_ptr<Graphics> > _graphics;

	GraphicsBus () { }

	/** removes all graphics objects and resets the state */
	void clear ()
	{
		_graphics.clear ();
	}
};

#endif
