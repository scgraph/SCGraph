#ifndef SCGRAOH_COLOR_HH
#define SCGRAOH_COLOR_HH

#include "../unit.h"
#include "../graphics_visitor.h"
#include "../scgcolor.h"

class GColor : public GUnit, public GraphicsVisitor
{
	public:
		GColor ();
		~GColor ();

		virtual void process_g (double delta_t);
		virtual void visitGeometry (Geometry *g);
		virtual void visitText (Text *t);
};

#endif
