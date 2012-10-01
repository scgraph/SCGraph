#ifndef SCGRAOH_BUF_INFO_UGENS_HH
#define SCGRAOH_BUF_INFO_UGENS_HH

#include "../unit.h"

class BufFrameRate : public GUnit
{
	float _value;

	public:
		BufFrameRate ();
		~BufFrameRate ();

		virtual void process_c (double delta_t);
};


class BufFrames : public GUnit
{
	float _value;

	public:
		BufFrames ();
		~BufFrames ();

		virtual void process_c (double delta_t);
};

#endif
