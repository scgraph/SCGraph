#ifndef SCGRAOH_ROTATE_HH
#define SCGRAOH_ROTATE_HH

#include "../unit.h"
#include "../graphics_visitor.h"

class Rotate : public GUnit, public GraphicsVisitor
{
  cow_ptr<Rotation> _r;

	public:
		Rotate ();
		~Rotate ();

		virtual void process_g (double delta_t);
		virtual void visitTransformation (Transformation *t);
		
};

#endif
