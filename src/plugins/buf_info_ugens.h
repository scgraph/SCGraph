#ifndef SCGRAOH_BUF_INFO_UGENS_HH
#define SCGRAOH_BUF_INFO_UGENS_HH

#include "../unit.h"

class BufFrameRate : public GUnit
{
	public:
		BufFrameRate ();
		~BufFrameRate ();

		virtual void process_c (double delta_t);
};


class BufFrames : public GUnit
{
	public:
		BufFrames ();
		~BufFrames ();

		virtual void process_c (double delta_t);
};

#endif
