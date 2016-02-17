/* Copyright 2006 Florian Paul Schmidt */

#ifndef SCGRAPHNODE_HH
#define SCGRAPHNODE_HH

#include <list>

struct Group;

struct GNode
{
	int _id;

	GNode (int id);
	virtual ~GNode ();

	virtual void process_g (double delta_t) = 0;
	virtual void process_c (double delta_t) = 0;
};

#endif

