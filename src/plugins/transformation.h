#ifndef SCGRAOH_TRANSFORMATION_HH
#define SCGRAOH_TRANSFORMATION_HH

#include "../unit.h"
#include "../graphics_visitor.h"

class GTransformation : public GUnit, public GraphicsVisitor
{
	cow_ptr<Linear> _l;

	public:
		GTransformation ();
		~GTransformation ();

		virtual void process_g (double delta_t);
		virtual void visitTransformation (Transformation *t);
};

#endif
