#include "graphics.h"
#include "graphics_visitor.h"


/** the superclass for all graphics objects */
Graphics::~Graphics ()
{
}

Graphics::Graphics () 
{

}


Transformation::Transformation ()
{
}

void Transformation::accept(GraphicsVisitor* v)
{
	v->visitTransformation(this);
}

void Transformation::acceptConst(GraphicsVisitor* v) const
{
	v->visitTransformationConst(this);
}

#if 0
Transformation *Transformation::clone ()
{
	Transformation *t = new Transformation ();

	t->_commands = _commands;

	return t;
}
#endif

void Geometry::accept(GraphicsVisitor* v)
{
	v->visitGeometry(this);
}
void Geometry::acceptConst(GraphicsVisitor* v) const
{
	v->visitGeometryConst(this);
}

#if 0
Geometry *Geometry::clone ()
{
	Geometry *g = new Geometry ();

	g->_commands = _commands;

	for (size_t i = 0; i < _faces.size (); ++i)
	{
		cow_ptr<Face> p (new Face (*(_faces[i])));
		g.touch()->_faces.push_back (p);
	}

	return g;
}
#endif

/** a light source */
void Light::accept(GraphicsVisitor* v)
{
		v->visitLight(this);
}
void Light::acceptConst(GraphicsVisitor* v) const
{
		v->visitLightConst(this);
}

#if 0
Light *Light::clone ()
{
	Light *l = new Light ();

	l->_commands = _commands;

	l->_index = _index;

	l->_on = _on;

	l->_position = _position;
	
	l->_spot_direction = _spot_direction;
	l->_spot_exponent = _spot_exponent;
	l->_spot_cutoff = _spot_cutoff;

	l->_ambient_color = _ambient_color;
	l->_diffuse_color = _diffuse_color;
	l->_specular_color = _specular_color;

	l->_constant_attenuation = _constant_attenuation;
	l->_linear_attenuation = _linear_attenuation;
	l->_quadratic_attenuation = _quadratic_attenuation;

	return l;
}
#endif

void Command::accept(GraphicsVisitor* v)
{
	v->visitGraphics(this);
}

void Command::acceptConst(GraphicsVisitor* v) const
{
	v->visitGraphicsConst(this);
}

#if 0
Command *Command::clone ()
{
	Command *c = new Command ();

	return c;
}
#endif

void Lighting::accept(GraphicsVisitor* v)
{
	v->visitLighting(this);
}


void Lighting::acceptConst(GraphicsVisitor* v) const
{
	v->visitLightingConst(this);
}


void ShaderProgram::accept(GraphicsVisitor* v)
{
	v->visitShaderProgram(this);
}


void ShaderProgram::acceptConst(GraphicsVisitor* v) const
{
	v->visitShaderProgramConst(this);
}


void ShaderUniform::accept(GraphicsVisitor* v)
{
	v->visitShaderUniform(this);
}


void ShaderUniform::acceptConst(GraphicsVisitor* v) const
{
	v->visitShaderUniformConst(this);
}


void Blending::accept(GraphicsVisitor* v)
{
	v->visitBlending(this);
}
void Blending::acceptConst(GraphicsVisitor* v) const
{
	v->visitBlendingConst(this);
}


void Culling::accept(GraphicsVisitor* v)
{
	v->visitCulling(this);
}
void Culling::acceptConst(GraphicsVisitor* v) const
{
	v->visitCullingConst(this);
}

void Text::accept(GraphicsVisitor* v)
{
	v->visitText(this);
}
void Text::acceptConst(GraphicsVisitor* v) const
{
	v->visitTextConst(this);
}
