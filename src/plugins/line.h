#ifndef SCGRAOH_LINE_HH
#define SCGRAOH_LINE_HH

#include "../unit.h"

class Line : public GUnit
{
	cow_ptr<Geometry> _g;

	public:
		Line ();
		~Line ();

		virtual void process_g (double delta_t);
};

#endif
