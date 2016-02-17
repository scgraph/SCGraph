#ifndef SCGRAOH_STRETCH_HH
#define SCGRAOH_STRETCH_HH

#include "../unit.h"
#include "../graphics_visitor.h"

class Stretch : public GUnit, public GraphicsVisitor
{
	cow_ptr<Scale> _s;
	public:
		Stretch ();
		~Stretch ();

		virtual void process_g (double delta_t);
		virtual void visitTransformation (Transformation *t);
};

#endif
