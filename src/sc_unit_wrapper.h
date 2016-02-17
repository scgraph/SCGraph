#ifndef SCGRAOH_SCUNITWRAPPER_HH
#define SCGRAOH_SCUNITWRAPPER_HH

#include "unit.h"


struct ScUnit;
struct Unit;

class ScUnitWrapper : public GUnit
{
	boost::shared_ptr<ScUnit> _sc_unit;

	Unit                     *_unit;

	bool                      _first_time;
	public:
		ScUnitWrapper (int special_index, boost::shared_ptr<ScUnit> unit);
		~ScUnitWrapper ();

		virtual void process_c (double delta_t);
};

#endif
