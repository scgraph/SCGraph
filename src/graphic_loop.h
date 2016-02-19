// /* Copyright 2006 Florian Paul Schmidt */

#ifndef SCGRAPH_GRAPHIC_LOOP_HH
#define SCGRAPH_GRAPHIC_LOOP_HH

//#include <QtCore/QObject>
//#include <QtCore/QTimer>

#include <sys/time.h>
#include "ofTimer.h"
#include "ofThread.h"

class GraphicLoop //: public ofThread
{
	ofTimer        *_timer;
	bool           _first_time;

	double         _sum_delta_t;
	int            _iteration_count;

	struct timeval _tv;

	public:
		GraphicLoop ();
		~GraphicLoop ();

		/** Must only be called once after construction */
		void start ();
		void stop ();

		void set_rate (int rate);

	//public slots:
		void timer_callback ();
    void threadedFunction();
};

#endif
