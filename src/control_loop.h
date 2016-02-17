/* Copyright 2006 Florian Paul Schmidt */

#ifndef SCGRAPH_CONTROLLOOP_HH
#define SCGRAPH_CONTROLLOOP_HH

#include <pthread.h>
#include <sys/time.h>
#include <time.h>



class ControlLoop
{
	bool           _first_time;

	struct timeval _tv;

	double         _sum_delta_t;
	double         _stats_sum_delta_t;
	int            _iteration_count;

	pthread_t _thread;

	/** a mutex to protect the quit toggle */
	pthread_mutex_t _mutex;
	bool _quit;

	float _freq;

	public:
		/** Preconditions: Global and Options  exist */
		ControlLoop ();
		~ControlLoop ();

		void *thread_func ();

		/** Must only be called once after construction */
		void start ();
		void stop ();

		void set_rate (int rate);
};

extern "C"
{
	void *c_control_thread_func (void *arg);
}

#endif
