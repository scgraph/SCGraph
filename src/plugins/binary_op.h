#ifndef SCGRAOH_BINARY_OP_HH
#define SCGRAOH_BINARY_OP_HH

#include "../unit.h"
#include "../graphics_visitor.h"

class BinaryOp : public GUnit, public GraphicsVisitor
{
	int _special_index;

	public:
		BinaryOp (int special_index);
		~BinaryOp ();

		virtual void visitGeometry (Geometry *g);
		virtual void visitText (Text *t);

		virtual void process_g (double delta_t);
		virtual void process_c (double delta_t);
};

#endif


class MulAdd : public GUnit, public GraphicsVisitor
{
	public:
		MulAdd ();
		~MulAdd ();

		virtual void visitGeometry (Geometry *g);
		virtual void visitText (Text *t);

		virtual void process_g (double delta_t);
		virtual void process_c (double delta_t);
};
