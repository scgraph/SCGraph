#ifndef SCGRAPH_BORDERED_RECTANGLE_HH
#define SCGRAPH_BORDERED_RECTANGLE_HH

#include "../unit.h"

class BorderedRectangle : public GUnit
{
	cow_ptr<Geometry> _g;

	public:
		BorderedRectangle ();
		~BorderedRectangle ();

		virtual void process_g (double delta_t);
};

#endif
