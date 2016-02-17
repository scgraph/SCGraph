#ifndef SCGRAOH_POINTS_HH
#define SCGRAOH_POINTS_HH

#include "../unit.h"

class Points : public GUnit
{
	cow_ptr<Geometry> _g;
	cow_ptr<Face> _f;

	bool _first_time;

	public:
		Points ();
		~Points ();

		virtual void process_g (double delta_t);
};

#endif
