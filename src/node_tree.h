#ifndef SCGRAPH_NODE_TREE_HH
#define SCGRAPH_NODE_TREE_HH

#include "group.h"
#include "synth.h"
#include "tree.h"

#include <stack>
#include <map>

#include <iostream>
#include <boost/shared_ptr.hpp>

#define SCGRAPH_DEFAULT_GROUP_ID 0


typedef boost::shared_ptr<GNode> NodePtr;

/** This class models the node tree which defines execution order AND hierarchical ownership. */
class NodeTree
{
	// Group _root;

	Tree::Tree<NodePtr> _tree;

	// TODO: std::map von iteratoren auf id's

	/* these return 0 if none is found */
	/* we have to keep track of ids and the associated nodes */
	std::map<int, Tree::Node<NodePtr>* > _node_map;

	int next_free_id ();

	void dump (Tree::Node<NodePtr> *node);

	/** if s_new is called with id -1, then the autocreated id is saved here */
	int                          _last_id;

	public:
		NodeTree ();

		/** Utility functions */
		Tree::Node<NodePtr> *find_node_by_id (int id);
		Tree::Node<NodePtr> *find_synth_by_id (int id);
		Tree::Node<NodePtr> *find_group_by_id (int id);

		boost::shared_ptr<GSynth> s_new (const std::string &synth_name, int id, int add_action, int target_id);
	
		/*
			add actions:
				0 - add the new group to the the head of the group specified
			by the add target ID.
				1 - add the new group to the the tail of the group specified
			by the add target ID.
				2 - add the new group just before the node specified by the
			add target ID.
				3 - add the new group just after the node specified by the add
			target ID.
				4 - the new node replaces the node specified by the add target
			ID. The target node is freed.
			Multiple groups may be created in one command by adding arguments.
		*/
		int g_new (int id, int add_action, int add_target_id);

		int g_head (int id, int node_id);

		int g_tail (int id, int node_id);

		void g_freeAll (int id);

		void g_deepFree (int id);

		void n_free (int id);

		void n_set (int id, const std::string &param_name, float value);

		int get_index(int id, const std::string &param_name);

		void n_set (int id, int index, float value);

		/** plugins call this to execute their doneAction */
		void done_action (int id, int done_action);

		/** These members return post order iterators for the tree */
		Tree::Tree<NodePtr>::Iterator begin ();

		/** These members return post order iterators for the tree */
		Tree::Tree<NodePtr>::Iterator end ();
};


#endif
