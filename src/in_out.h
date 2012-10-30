#ifndef SCGRAOH_INOUT_HH
#define SCGRAOH_INOUT_HH

#include "unit.h"
#include "graphics_visitor.h"

class In : public GUnit
{
 public:
	In () :
	_bus (0)
		{
			/* we have a single graphics rate output */
			// cow_ptr<GraphicsBus> b (new GraphicsBus);
			// _graphics_outs.push_back (b);
		}

	size_t _bus;
	void process_g (double delta_t);
	void process_c (double delta_t);
};


class InFeedback : public GUnit
{
 public:
	InFeedback () :
	_bus (0)
		{
			/* we have a single graphics rate output */
			// cow_ptr<GraphicsBus> b (new GraphicsBus);
			// _graphics_outs.push_back (b);
		}

	size_t _bus;
	void process_g (double delta_t);
	void process_c (double delta_t);
};


class XFade2 : public GUnit, public GraphicsVisitor
{
	float _factor;

 public:
	XFade2 () :
	_bus (0)
		{
			/* we have a single graphics rate output */
			// cow_ptr<GraphicsBus> b (new GraphicsBus);
			// _graphics_outs.push_back (b);
		}

		void visitGeometry (Geometry *g);
		void visitText (Text *t);

	size_t _bus;
	void process_g (double delta_t);
	void process_c (double delta_t);
};

// TODO: Multi in and multi out

class Out : public GUnit
{
 public:
	Out () :
	_bus (0)
		{
			/* we have no outputs, thus, nothing to do */
		}

	size_t _bus;
	void process_g (double delta_t);
	void process_c (double delta_t);
};


class XOut : public GUnit, public GraphicsVisitor
{
	float _factor;

 public:
	XOut () :
	_bus (0)
		{
			/* we have no outputs, thus, nothing to do */
		}
		void visitGeometry (Geometry *g);
		void visitText (Text *t);


	size_t _bus;
	void process_g (double delta_t);
	void process_c (double delta_t);
};


class ReplaceOut : public GUnit
{
 public:
	ReplaceOut () :
	_bus (0)
		{
			/* we have no outputs, thus, nothing to do */
		}

	size_t _bus;
	void process_g (double delta_t);
	void process_c (double delta_t);
};

#endif

