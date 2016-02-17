#ifndef SCGRAOH_SUM3_HH
#define SCGRAOH_SUM3_HH

#include "../unit.h"

class Sum3 : public GUnit
{
	public:
		Sum3 ();
		~Sum3 ();

		virtual void process_g (double delta_t);
		virtual void process_c (double delta_t);
};

#endif
