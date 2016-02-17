#ifndef SCGRAPH_CIRCLE_HH
#define SCGRAPH_CIRCLE_HH

#include "../unit.h"

class Circle : public GUnit
{
	cow_ptr<Geometry> _g;

	int _segments;

	public:
		Circle ();
		~Circle ();

		virtual void process_g (double delta_t);
};

#endif
