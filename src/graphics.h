#ifndef SCGRAPH_GRAPHICS_HH
#define SCGRAPH_GRAPHICS_HH

#include <vector>
#include <list>
#include <iostream>

#include "hvector_3d.h"
#include "ofVec3f.h"

#include "material.h"
#include "scgcolor.h"
#include "face.h"

#include "cow_ptr.h"

#include "transformation_command.h"

//#include "graphics_visitor.h"
struct GraphicsVisitor;

#include <boost/shared_ptr.hpp>


// the superclass for all graphics objects. Every subclass has to reimplement the
//  accept method to call the appropriate method in the visitor 
// (see graphics_visitor.h)
struct Graphics : public DeepCopyable
{
	virtual void accept(GraphicsVisitor* v) = 0;
	virtual void acceptConst(GraphicsVisitor* v) const = 0;

	virtual ~Graphics ();
	Graphics ();

	virtual Graphics* deepCopy() = 0;
};

struct Transformation : public Graphics
{
	std::list <cow_ptr<TransformationCommand> > _commands;

	Transformation ();

	virtual void accept(GraphicsVisitor* v);
	virtual void acceptConst(GraphicsVisitor* v) const;

	virtual Transformation *deepCopy() {
		return new Transformation(*this);
	}
};

struct Geometry : public Transformation
{
	std::vector <cow_ptr<Face> > _faces;

	virtual void accept(GraphicsVisitor* v);
	virtual void acceptConst(GraphicsVisitor* v) const;
	virtual Geometry *deepCopy() {
		return new Geometry(*this);
	}
};



// a light source
struct Light : public Transformation
{
	// bool _dirty;

	virtual void accept(GraphicsVisitor* v);
	virtual void acceptConst(GraphicsVisitor* v) const;
#if 0
	virtual Light *clone ();
#endif

	/* the open gl light index */
	int       _index;

	bool      _on;

	HVector3D _position;
	
	ofVec3f  _spot_direction;
	float     _spot_exponent;
	float     _spot_cutoff;

	scgColor _ambient_color;
	scgColor _diffuse_color;
	scgColor _specular_color;

	float     _constant_attenuation;
	float     _linear_attenuation;
	float     _quadratic_attenuation;

	virtual Light *deepCopy() {
		return new Light(*this);
	}
};

struct Command : public Graphics
{
	virtual void accept(GraphicsVisitor* v);
	virtual void acceptConst(GraphicsVisitor* v) const;

	virtual Command *deepCopy() {
		return new Command(*this);
	}
};

// a command to control transparency details
struct Blending : public Command
{
	bool _on;

	// bool _dirty;

	virtual void accept(GraphicsVisitor* v);
	virtual void acceptConst(GraphicsVisitor* v) const;

	virtual Blending *deepCopy() {
		return new Blending(*this);
	}
};

// a command to control Lighting details
struct Lighting : public Command
{
	bool _on;

	virtual void accept(GraphicsVisitor* v);
	virtual void acceptConst(GraphicsVisitor* v) const;

	virtual Lighting *deepCopy() {
		return new Lighting(*this);
	}
};

struct ShaderProgram : public Command
{
	bool _on;
	unsigned int _index;

	virtual void accept(GraphicsVisitor* v);
	virtual void acceptConst(GraphicsVisitor* v) const;

	virtual ShaderProgram *deepCopy() {
		return new ShaderProgram(*this);
	}
};


// a command to control uniforms of a shader
struct ShaderUniform : public Command
{
	//unsigned int _program_index;
	unsigned int _uniform_index;
	std::vector<float> _values;

	virtual void accept(GraphicsVisitor* v);
	virtual void acceptConst(GraphicsVisitor* v) const;

	virtual ShaderUniform *deepCopy() {
		return new ShaderUniform(*this);
	}
};


// a command to control Culling details
struct Culling : public Command
{
	enum CullingMode {Off = 0, Front = 1, Back = 2, Both = 3};
	CullingMode _mode;

	virtual void accept(GraphicsVisitor* v);
	virtual void acceptConst(GraphicsVisitor* v) const;

	virtual Culling *deepCopy() {
		return new Culling(*this);
	}
};

struct Text : Transformation
{

	std::string _text;

	unsigned int _fontsize;

	scgColor _color;

	Material  _material;

	RenderMode _render_mode;
	float _thickness;

	virtual void accept(GraphicsVisitor* v);
	virtual void acceptConst(GraphicsVisitor* v) const;

	virtual Text *deepCopy() {
		return new Text(*this);
	}

};

#endif
