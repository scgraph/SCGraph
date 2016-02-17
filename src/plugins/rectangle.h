#ifndef SCGRAPH_RECTANGLE_HH
#define SCGRAPH_RECTANGLE_HH

#include "../unit.h"

class Rectangle : public GUnit
{
	cow_ptr<Geometry> _g;

	public:
		Rectangle ();
		~Rectangle ();

		virtual void process_g (double delta_t);
};

#endif
