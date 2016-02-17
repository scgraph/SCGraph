#ifndef SCGRAPH_TRIANGLE_HH
#define SCGRAPH_TRIANGLE_HH

#include "../unit.h"

class Triangle : public GUnit
{
	cow_ptr<Geometry> _g;

	public:
		Triangle ();
		~Triangle ();

		virtual void process_g (double delta_t);
};

#endif
