#include "freq_amp.h"

#include <iostream>
#include <cmath>

#include "../scgraph.h"

extern "C"
{
	GUnit *create (size_t index, int special_index)
	{
		return (GUnit*) new FreqAmp (); 
	}

	size_t get_num_of_units ()
	{
		return 1;
	}

	const char *get_unit_name (size_t index)
	{
		if (index == 0)
			return "GFreqAmp";
		else
			return 0;
	}
}


FreqAmp::FreqAmp ()
{

}

FreqAmp::~FreqAmp ()
{
	// std::cout << "[FreqAmp]: Destructor" << std::endl;
}

void FreqAmp::process_c (double delta_t)
{
	ScGraph *scgraph = ScGraph::get_instance ();
    /* TODO
	if (scgraph->_jack_client.get() != 0) {
		_control_outs[0] = scgraph->_jack_client->get_frequency(
																(int)*_control_ins[0], 
																*_control_ins[1]);
	} else {
		_control_outs[0] = 0.0;
	}*/
     _control_outs[0] = 0.0;
}
