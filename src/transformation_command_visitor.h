#ifndef SCGRAPH_TRANSFORMATION_COMMAND_VISITOR_HH
#define SCGRAPH_TRANSFORMATION_COMMAND_VISITOR_HH

#include "transformation_command.h"

class TransformationCommandVisitor
{
	public:
		virtual ~TransformationCommandVisitor ();

		virtual void visitTranslation (Translation *t);
		virtual void visitRotation (Rotation *r);
		virtual void visitScale (Scale *s);
		virtual void visitLinear (Linear *l);

		virtual void visitTranslationConst (const Translation *t);
		virtual void visitRotationConst (const Rotation *r);
		virtual void visitScaleConst (const Scale *s);
		virtual void visitLinearConst (const Linear *l);
};

#endif
