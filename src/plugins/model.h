#ifndef SCGRAPH_MODEL_HH
#define SCGRAPH_MODEL_HH

#include "../unit.h"

#include "../obj_loader.h"

class Model : public GUnit
{
	cow_ptr<Geometry> _g;

	public:
		Model ();
		~Model ();

		virtual void process_g (double delta_t);
};

#endif
