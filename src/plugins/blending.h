#ifndef SCGRAOH_BLENDING_HH
#define SCGRAOH_BLENDING_HH

#include "../unit.h"
#include "../graphics_visitor.h"

class GBlending : public GUnit
{
	public:
		GBlending ();
		~GBlending ();

		cow_ptr<Blending> _b;

		virtual void process_g (double delta_t);
};

#endif
