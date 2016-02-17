#ifndef SCGRAPH_CONTROL_HH
#define SCGRAPH_CONTROL_HH

#include "unit.h"

class Control : public GUnit
{
	public:
		Control (int special_index);

		void process_g (double delta_t);
		void process_c (double delta_t);

		int _special_index;
};


class TrigControl : public GUnit
{
	public:
		TrigControl (int special_index);

		void process_g (double delta_t);
		void process_c (double delta_t);

		int _special_index;
};

#endif

