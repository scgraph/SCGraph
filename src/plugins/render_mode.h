#ifndef SCGRAOH_RENDER_MODE_HH
#define SCGRAOH_RENDER_MODE_HH

#include "../unit.h"
#include "../graphics_visitor.h"

class GRenderMode : public GUnit, public GraphicsVisitor
{
	public:
		GRenderMode ();
		~GRenderMode ();

		virtual void process_g (double delta_t);
		virtual void visitGeometry (Geometry *g);
		virtual void visitText (Text *t);
};

#endif
