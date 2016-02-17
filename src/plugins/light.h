#ifndef SCGRAOH_LIGHT_HH
#define SCGRAOH_LIGHT_HH

#include "../unit.h"

class GLight : public GUnit
{
	cow_ptr<Light> _l;

	public:
		GLight ();
		~GLight ();

		virtual void process_g (double delta_t);
};

#endif
