#ifndef SCGRAOH_CUBE_HH
#define SCGRAOH_CUBE_HH

#include "../unit.h"
#include "../cow_ptr.h"

class Cube : public GUnit
{
	cow_ptr<Geometry> _g;

	public:
		Cube ();
		~Cube ();

		virtual void process_g (double delta_t);
};

#endif
