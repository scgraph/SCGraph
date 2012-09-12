#ifndef SCGRAPH_QUAD_HH
#define SCGRAPH_QUAD_HH

#include "../unit.h"

class Quad : public GUnit
{
	cow_ptr<Geometry> _g;

	public:
		Quad ();
		~Quad ();

		virtual void process_g (double delta_t);
};

class TexQuad : public GUnit
{
	cow_ptr<Geometry> _g;

	public:
		TexQuad ();
		~TexQuad ();

		virtual void process_g (double delta_t);
};

class FBTexQuad : public GUnit
{
	cow_ptr<Geometry> _g;

	public:
		FBTexQuad ();
		~FBTexQuad ();

		virtual void process_g (double delta_t);
};


#endif
