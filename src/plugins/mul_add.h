#ifndef SCGRAOH_MUL_ADD_HH
#define SCGRAOH_MUL_ADD_HH

#include "../unit.h"

class MulAdd : public GUnit
{
	public:
		MulAdd ();
		~MulAdd ();

		virtual void process_c (double delta_t);
};

#endif
