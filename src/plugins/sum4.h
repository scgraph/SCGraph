#ifndef SCGRAOH_SUM4_HH
#define SCGRAOH_SUM4_HH

#include "../unit.h"

class Sum4 : public GUnit
{
	public:
		Sum4 ();
		~Sum4 ();

		virtual void process_g (double delta_t);
		virtual void process_c (double delta_t);
};

#endif
