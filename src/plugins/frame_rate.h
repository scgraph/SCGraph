#ifndef SCGRAOH_FRAME_RATE_HH
#define SCGRAOH_FRAME_RATE_HH

#include "../unit.h"

class FrameRate : public GUnit
{
	public:
		FrameRate ();
		~FrameRate ();

		virtual void process_c (double delta_t);
};

#endif
