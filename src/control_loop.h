/* Copyright 2006 Florian Paul Schmidt */

#ifndef SCGRAPH_CONTROLLOOP_HH
#define SCGRAPH_CONTROLLOOP_HH

#include <sys/time.h>
#include <time.h>
#include <mutex>
#include "ofTimer.h"
#include "ofThread.h"


class ControlLoop : public ofThread
{
	bool           _first_time;

	struct timeval _tv;

	double         _sum_delta_t;
	double         _stats_sum_delta_t;
	int            _iteration_count;
    
    ofTimer *_timer;

	//pthread_t _thread;

	/** a mutex to protect the quit toggle */
    // std::mutex _mutex;
	bool _quit;

	float _freq;

	public:
		/** Preconditions: Global and Options  exist */
		ControlLoop ();
		~ControlLoop ();

		/** Must only be called once after construction */
		void start ();
		void stop ();

		void set_rate (int rate);
    
        void threadedFunction();
};
#endif
