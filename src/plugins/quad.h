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

	uint32_t _texquad_id_a, _texquad_id_b;
	int _tex_index_a, _tex_index_b, _frame_index_a, _frame_index_b;
	int _last_frame_index, _last_tex_index;
	bool _flip;
	bool _processing;

 public:
	TexQuad ();
	~TexQuad ();

	virtual void process_g (double delta_t);
    void flip(uint32_t & tex_id);
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
