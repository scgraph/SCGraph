#include <iostream>
/* Copyright 2006 Florian Paul Schmidt */

#include "unistd.h"

#include "graphic_loop.h"
#include "scgraph.h"

GraphicLoop::GraphicLoop () :
	_first_time (true),
	_sum_delta_t (0),
	_iteration_count (0)
{
	_timer  = new ofTimer ();
	// TODO connect (_timer, SIGNAL (timeout()), this, SLOT (timer_callback()));
}

GraphicLoop::~GraphicLoop ()
{
	delete _timer;
}

void GraphicLoop::threadedFunction ()
{
	Options *options = Options::get_instance ();

	//_timer->setPeriodicEvent((int)(1000 * 1.0/(float)options->_graphics_rate));
    //_timer->reset();
    
    //while(isThreadRunning()) {
        timer_callback();
    //}
/*
	if (options->_verbose >= 2)
		std::cout << "[GraphicLoop]: Running!" << std::endl;
 */
}

void GraphicLoop::set_rate (int rate)
{
	_timer->setPeriodicEvent ((int)(1000 * (double)1/(double)rate));
    _timer->reset();
}

void GraphicLoop::stop ()
{
	Options *options = Options::get_instance ();

	// TODO _timer->stop ();

	if (options->_verbose >= 2)
		std::cout << "[GraphicLoop]: Stopped!" << std::endl;
}

void GraphicLoop::timer_callback ()
{
	Options *options = Options::get_instance ();
	ScGraph *scgraph = ScGraph::get_instance ();

	if (_first_time)
	{
		_first_time = false;
		int ret = gettimeofday (&_tv, 0);
		if (ret == -1)	
		{
			std::cout << "[GraphicLoop]: Error: gettimeofday () failed. Exiting... " << std::endl;
			exit (EXIT_FAILURE);
		}
		return;
	}

	if (options->_verbose >= 5)
		std::cout << "[GraphicLoop]: iterate!" << std::endl;

	struct timeval tv;
	gettimeofday (&tv, 0);

	double delta_t = (double)(tv.tv_sec - _tv.tv_sec) + (double)(1E-6 * (tv.tv_usec - _tv.tv_usec));
	_sum_delta_t += delta_t;

	// std::cout << delta_t << " " << _sum_delta_t << std::endl;

	if (_sum_delta_t >= 5.0)
	{
		if (options->_verbose >= 4)
			std::cout << "[GraphicLoop]: avg fps: " << _iteration_count/_sum_delta_t << std::endl;

		_iteration_count = 0;
		_sum_delta_t -= 5.0;
	}
	scgraph->run_one_graphics_cycle (delta_t);

	++_iteration_count;
	_tv = tv;
    //sleep((int)(1000 * 1.0/(float)options->_graphics_rate));
    //_timer->waitNext();
}

