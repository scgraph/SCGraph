#ifndef SCGRAPH_GRAPHICS_VISITOR_HH
#define SCGRAPH_GRAPHICS_VISITOR_HH

#include "graphics.h"

/** a graphics visitor to implement the visitor pattern on the graphics
    bus objects (which are derived from Graphics) */
struct GraphicsVisitor
{
	// CONST

	/** the default implementation does nothing */
	virtual void visitGraphicsConst (const Graphics *g);

	/** the default implementation of visitTransformation calls visitGraphics */
	virtual void visitTransformationConst (const Transformation *t);

	/** the default implementation of visitGeometry calls visitTransformation */
	virtual void visitGeometryConst (const Geometry *g);

	/** the default implementation of visitLight calls visitTransformation */
	virtual void visitLightConst (const Light *l);

	/** the default implementation of visitCommand calls visitGraphics */
	virtual void visitCommandConst (const Command *c);

	/** the default implementation of visitTransparency calls visitCommand */
	virtual void visitBlendingConst (const Blending *t);

	/** the default implementation of visitTransparency calls visitCommand */
	virtual void visitLightingConst (const Lighting *l);

	/** the default implementation of visitTransparency calls visitCommand */
	virtual void visitShaderProgramConst (const ShaderProgram *l);

	/** the default implementation of visitTransparency calls visitCommand */
	virtual void visitShaderUniformConst (const ShaderUniform *l);

	/** the default implementation of visitTransparency calls visitCommand */
	virtual void visitCullingConst (const Culling *c);

	virtual void visitTextConst (const Text *g);


	// NON CONST
	/** the default implementation does nothing */
	virtual void visitGraphics (Graphics *g);

	/** the default implementation of visitTransformation calls visitGraphics */
	virtual void visitTransformation (Transformation *t);

	/** the default implementation of visitGeometry calls visitTransformation */
	virtual void visitGeometry (Geometry *g);

	/** the default implementation of visitLight calls visitTransformation */
	virtual void visitLight (Light *l);

	/** the default implementation of visitCommand calls visitGraphics */
	virtual void visitCommand (Command *c);

	/** the default implementation of visitTransparency calls visitCommand */
	virtual void visitBlending (Blending *t);

	/** the default implementation of visitTransparency calls visitCommand */
	virtual void visitLighting (Lighting *l);

	/** the default implementation of visitTransparency calls visitCommand */
	virtual void visitShaderProgram (ShaderProgram *l);

	/** the default implementation of visitTransparency calls visitCommand */
	virtual void visitShaderUniform (ShaderUniform *l);

	/** the default implementation of visitTransparency calls visitCommand */
	virtual void visitCulling (Culling *c);

	virtual void visitText (Text *g);

	GraphicsVisitor ();
	virtual ~GraphicsVisitor ();
};

#endif
