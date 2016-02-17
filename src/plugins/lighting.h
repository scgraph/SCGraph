#ifndef SCGRAOH_LIGHTING_HH
#define SCGRAOH_LIGHTING_HH

#include "../unit.h"
#include "../graphics_visitor.h"

class GLighting : public GUnit
{
	cow_ptr<Lighting> _l;
	public:
		GLighting ();
		~GLighting ();

		virtual void process_g (double delta_t);
};

#endif
