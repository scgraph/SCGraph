/* Copyright 2006 Florian Paul Schmidt */

#ifndef SCGRAPH_GROUP_HH
#define SCGRAPH_GROUP_HH

#include "node.h"
#include "graphics.h"

#include <list>

/** see SuperCollider Server-Command-Reference for info on the basics */
struct GGroup : public GNode
{
	// Graphics _graphics;

	GGroup (int id);
	virtual ~GGroup ();

	void process_g (double delta_t);
	void process_c (double delta_t);
};

#endif

