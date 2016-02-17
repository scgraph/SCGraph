#include "sc_unit_wrapper.h"

#include "plugin_pool.h"

#include "options.h"
#include "SC_Unit.h"
#include "sc__unit.h"

ScUnitWrapper::ScUnitWrapper (int special_index, boost::shared_ptr<ScUnit> sc_unit) :
	GUnit (special_index),
	_sc_unit (sc_unit),
	_first_time (true)
{
}

ScUnitWrapper::~ScUnitWrapper ()
{
	// TODO: clean up
	if (_sc_unit->_dtor)
		_sc_unit->_dtor(_unit);

	PluginPool::get_instance ()->destroy_sc_unit(_unit);
}

void ScUnitWrapper::process_c (double delta_t)
{
	// std::cout << "wri wra wrap" << std::endl;
	if (_first_time)
	{
		PluginPool *PluginPool = PluginPool::get_instance ();
		Options *options = Options::get_instance ();
        _unit = PluginPool->create_sc_unit(_sc_unit->_name,
                                           _special_index, 0,
                                           _control_ins.size(),
                                           _control_outs.size(), 0,
                                           options->_control_rate, 1);
	}

	for (size_t i = 0; i < _control_ins.size(); ++i)
	{
		_unit->mInBuf[i][0] = *_control_ins[i];
	}

	if (_first_time)
	{
		_first_time = false;
		_sc_unit->_ctor (_unit);
	}

	if (_unit->mCalcFunc)
		_unit->mCalcFunc (_unit, 1);

	for (size_t i = 0; i < _control_outs.size(); ++i)
	{
		// std::cout << "out" << _unit->mOutBuf[i][0] << std::endl;
		_control_outs[i] = _unit->mOutBuf[i][0];
	}
}
