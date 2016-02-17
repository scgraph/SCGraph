#ifndef SCGRAOH_CULLING_HH
#define SCGRAOH_CULLING_HH

#include "../unit.h"
#include "../graphics_visitor.h"


class GCulling : public GUnit, public GraphicsVisitor
{
	cow_ptr <Culling> _c;

	public:
		GCulling ();
		~GCulling ();

		virtual void process_g (double delta_t);
};

#endif
