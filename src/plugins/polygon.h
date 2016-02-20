#ifndef SCGRAPH_POLYGON_HH
#define SCGRAPH_POLYGON_HH

#include "../unit.h"
#include "../face.h"

class GPolygon : public GUnit
{
	cow_ptr<Geometry> _g;
	cow_ptr<Face> _f;

	bool _first_time;

	public:
		GPolygon ();
		~GPolygon ();

		virtual void process_g (double delta_t);
};

#endif
