#ifndef SCGRAPH_UNIT_HH
#define SCGRAPH_UNIT_HH

#include "graphics_bus.h"
#include "cow_ptr.h"

#include <vector>
#include <boost/shared_ptr.hpp>

/** This class defines a ScGraph UGen */
class GUnit
{
	protected:
		int _special_index;

		/** the unit can set this to signal a done action, 
			which will be executed after the process cycle.
			it is initialized to 0 which means "no doneAction".
		*/
		int _done_action;

	public:
		GUnit (int special_index = 0);
		virtual ~GUnit ();

		virtual void process_c (double delta_t);

		virtual void process_g (double delta_t);

		const int get_done_action ();

		/** the in's are set to point to the connected
			outs */
		std::vector <float*>        _control_ins;
		std::vector <GraphicsBus *> _graphics_ins;

		/** the outs themselves carry the value */
		std::vector <float>        _control_outs;
		std::vector <GraphicsBus>  _graphics_outs;
};

#endif

