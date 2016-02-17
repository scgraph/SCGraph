#ifndef SCGRAOH_MATERIAL_HH
#define SCGRAOH_MATERIAL_HH

#include "../unit.h"
#include "../graphics_visitor.h"

class GMaterial : public GUnit, public GraphicsVisitor
{
	Material _material;

	public:
		GMaterial ();
		~GMaterial ();

		virtual void process_g (double delta_t);
		virtual void visitGeometry (Geometry *g);
		virtual void visitText (Text *t);
};

#endif
