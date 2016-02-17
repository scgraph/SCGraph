#include "graphics_visitor.h"
#include <iostream>

/** the default implementation of visitCommand calls visitGraphics */
void GraphicsVisitor::visitCommand (Command *c)
{
	visitGraphics (c);
}

/** the default implementation of visitTransparency calls visitCommand */
void GraphicsVisitor::visitBlending (Blending *t)
{
	visitCommand (t);
}

/** the default implementation of visitTransparency calls visitCommand */
void GraphicsVisitor::visitLighting (Lighting *l)
{
	visitCommand (l);
}

/** the default implementation of visitTransparency calls visitCommand */
void GraphicsVisitor::visitShaderProgram (ShaderProgram *l)
{
	visitCommand (l);
}

/** the default implementation of visitTransparency calls visitCommand */
void GraphicsVisitor::visitShaderUniform (ShaderUniform *l)
{
	visitCommand (l);
}

/** the default implementation of visitTransparency calls visitCommand */
void GraphicsVisitor::visitCulling (Culling *c)
{
	visitCommand (c);
}

void GraphicsVisitor::visitTransformation (Transformation *t)
{
	// std::cout << "visitTransformation" << std::endl;
	visitGraphics (t);
}

void GraphicsVisitor::visitGeometry (Geometry *g)
{
	// std::cout << "visitGeometry" << std::endl;
	visitTransformation (g);
}

void GraphicsVisitor::visitLight (Light *l)
{
	// std::cout << "visitLight" << std::endl;
	visitTransformation (l);
}

void GraphicsVisitor::visitGraphics (Graphics *g)
{
	// std::cout << "visitGraphics" << std::endl;
}


void GraphicsVisitor::visitText (Text *g)
{
	//visitGeometry(&(g->_geom));
	// std::cout << "visitText" << std::endl;
	visitTransformation(g);
}













/** the default implementation of visitCommand calls visitGraphics */
void GraphicsVisitor::visitCommandConst (const Command *c)
{
	visitGraphicsConst (c);
}

/** the default implementation of visitTransparency calls visitCommand */
void GraphicsVisitor::visitBlendingConst (const Blending *t)
{
	visitCommandConst (t);
}

/** the default implementation of visitTransparency calls visitCommand */
void GraphicsVisitor::visitLightingConst (const Lighting *l)
{
	visitCommandConst (l);
}

/** the default implementation of visitTransparency calls visitCommand */
void GraphicsVisitor::visitShaderProgramConst (const ShaderProgram *l)
{
	visitCommandConst (l);
}

/** the default implementation of visitTransparency calls visitCommand */
void GraphicsVisitor::visitShaderUniformConst (const ShaderUniform *l)
{
	visitCommandConst (l);
}

/** the default implementation of visitTransparency calls visitCommand */
void GraphicsVisitor::visitCullingConst (const Culling *c)
{
	visitCommandConst (c);
}

void GraphicsVisitor::visitTransformationConst (const Transformation *t)
{
	// std::cout << "visitTransformation" << std::endl;
	visitGraphicsConst (t);
}

void GraphicsVisitor::visitGeometryConst (const Geometry *g)
{
	// std::cout << "visitGeometry" << std::endl;
	visitTransformationConst (g);
}

void GraphicsVisitor::visitLightConst (const Light *l)
{
	// std::cout << "visitLight" << std::endl;
	visitTransformationConst (l);
}

void GraphicsVisitor::visitGraphicsConst (const Graphics *g)
{
	// std::cout << "visitGraphics" << std::endl;
}


void GraphicsVisitor::visitTextConst (const Text *g)
{
	// std::cout << "visitText" << std::endl;
	visitTransformationConst(g);
}















GraphicsVisitor::GraphicsVisitor ()
{

}

GraphicsVisitor::~GraphicsVisitor ()
{

}
