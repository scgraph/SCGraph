#ifndef SCGRAOH_TUBE_HH
#define SCGRAOH_TUBE_HH

#include "../unit.h"
#include "../cow_ptr.h"

class Tube : public GUnit
{
	cow_ptr<Geometry> _g;
	int _segments;

	public:
		Tube ();
		~Tube ();

		virtual void process_g (double delta_t);
};

#endif
