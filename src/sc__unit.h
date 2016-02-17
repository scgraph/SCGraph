#ifndef SCGRAPH_SC_UNIT_HHHH
#define SCGRAPH_SC_UNIT_HHHH

#include <string>

struct Unit;
struct sc_msg_iter;

typedef void (*UnitCtorFunc)(struct Unit* inUnit);
typedef void (*UnitDtorFunc)(struct Unit* inUnit);
typedef void (*UnitCmdFunc)(struct Unit *unit, struct sc_msg_iter *args);

struct ScUnit
{
	ScUnit ();

	std::string  _name;
	UnitCtorFunc _ctor;
	UnitDtorFunc _dtor;
	UnitCmdFunc  _cmd;
	unsigned int _size;
};

#endif

