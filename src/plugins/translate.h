#ifndef SCGRAOH_TRANSLATE_HH
#define SCGRAOH_TRANSLATE_HH

#include "../unit.h"
#include "../graphics_visitor.h"

class Translate : public GUnit, public GraphicsVisitor
{
	cow_ptr<Translation> _t;

	public:
		Translate ();
		~Translate ();
		virtual void process_g (double delta_t);

		virtual void visitTransformation (Transformation *t);
};

#endif
