#include "node_tree.h"
#include "options.h"
#include "scgraph.h"

#include <iostream>
#include <boost/scoped_ptr.hpp>
#include <stdexcept>

#if 0
NodeTree::NodeTree () :
	_root (0, SCGRAPH_DEFAULT_GROUP_ID)
{
	_node_map [SCGRAPH_DEFAULT_GROUP_ID] = &_root;
}
#endif

NodeTree::NodeTree () :
	_last_id (-1)
{
	Tree::Node<NodePtr> *node = new Tree::Node<NodePtr>(NodePtr(new GGroup(SCGRAPH_DEFAULT_GROUP_ID)));
	_tree.set_root (node);
	_node_map[SCGRAPH_DEFAULT_GROUP_ID] = node;

	g_new(SCGRAPH_DEFAULT_GROUP_ID + 1, 0, SCGRAPH_DEFAULT_GROUP_ID);

}

void NodeTree::dump (Tree::Node<NodePtr> *node)
{
#if 0
	std::cout << "g_new modified _node_map:" << std::endl;
	for (std::map<int, Tree::Node<NodePtr> *>::iterator it = _node_map.begin (); it != _node_map.end (); ++it)
	{
		std::cout << (*it).first << std::endl;
	}
#endif
	if (!node)
		return;

	if (dynamic_cast<GGroup*> (node->_payload.get()))
	{
		std::cout << "[ G";
		std::cout << node->_payload.get()->_id << " ";
		dump (node->_start);
		std::cout << "] ";
		dump (node->_next);
	}
	else
	{
		std::cout << "S";
		std::cout << node->_payload.get()->_id << " ";
		dump (node->_next);
	}
}

boost::shared_ptr<GSynth> NodeTree::s_new (const std::string &synth_name, int id, int add_action, int add_target_id)
{
	ScGraph *scgraph = ScGraph::get_instance ();

	Tree::Node<NodePtr> *node = find_node_by_id (id);
	if (node)
        throw(std::runtime_error("[NodeTree]: Warning: Node already exists! NOT adding synth!"));

	int new_id = id;

	if (new_id == -1)
		new_id = next_free_id ();

	_last_id = new_id;

	Tree::Node<NodePtr> *target_node;
	if((add_action == 0) || (add_action == 1)) {
		target_node = find_group_by_id (add_target_id);
		if (!target_node)
			throw(std::runtime_error("[NodeTree]: Warning: Target group doesn't exist! NOT adding synth!"));
	}
	else if((add_action > 1) && (add_action < 5)) {
		target_node = find_node_by_id (add_target_id);
		if (!target_node)
			throw(std::runtime_error("[NodeTree]: Warning: Target node doesn't exist! NOT adding synth!"));
	}
	else {
		throw (std::runtime_error("[NodeTree]: Bad add_action"));
	}

	boost::shared_ptr<GSynth> synth 
		(new GSynth (scgraph->_synthdef_pool.get_synthdef (synth_name), new_id));

	Tree::Node<NodePtr> *new_node = new Tree::Node<NodePtr> (synth);

	_node_map[new_id] = new_node;

	switch(add_action)
		{
		case 0:
			{
				target_node->push_front (new_node);
			}
			break;

		case 1:
			{
				target_node->push_back (new_node);
			}
			break;

		case 2:
			{
				target_node->insert (new_node, true);
			}
			break;

		case 3:
			{
				target_node->insert (new_node, false);
			}
			break;

		case 4:
			{
				target_node->insert (new_node, true);
				delete target_node;
			}
			break;
		}

	Options *options = Options::get_instance ();
	if (options->_verbose >= 2)
		{
			dump (_tree.get_root ());
			std::cout << std::endl;
		}

	return synth;
}


void NodeTree::g_freeAll (int id)
{
	// TODO: update node map
	Tree::Node<NodePtr> *node = find_group_by_id (id);

	if (!node)
		throw (std::runtime_error("[NodeTree]: g_freeAll(): Unknown group!"));

	node = node->_start;

	if (!node)
	{
		return;
	}

	while (node)
	{
		if (dynamic_cast<GSynth*> (node->_payload.get()))
		{
			Tree::Node<NodePtr> *tmp = node->_next;

			// the node destructor should take care of updating all
			// pointers..
			_node_map.erase ((*node->_payload)._id);

			delete node;

			node = tmp;
		}
		else
		{
			Tree::Node<NodePtr> *tmp = node->_next;

			// call freeAll recursively
			g_freeAll (node->_payload.get()->_id);
			node = tmp;
		}
	}
	Options *options = Options::get_instance ();
	if (options->_verbose >= 2)
	{
		dump (_tree.get_root ());
		std::cout << std::endl;
	}
}

int NodeTree::g_new (int id, int add_action, int add_target_id)
{
	Tree::Node<NodePtr> *node = find_node_by_id (id);

	if (node)
		throw (std::runtime_error("[NodeTree]: Warning: Node ID already exists!"));
	
	int new_id = id;

	if (new_id == -1)
		new_id = next_free_id ();

	switch (add_action)
	{
		case 0:
		{
			Tree::Node<NodePtr> *target_group = find_group_by_id (add_target_id);

			if (!target_group)
				throw (std::runtime_error("[NodeTree]: Warning: Target group doesn't exist!"));

			boost::shared_ptr<GGroup> group (new GGroup (new_id));

			Tree::Node<NodePtr> *new_node = new Tree::Node<NodePtr> (group);
			_node_map[new_id] = new_node;
			target_group->push_front (new_node);
		}
		break;

		case 1:
		{
			Tree::Node<NodePtr> *target_group = find_group_by_id (add_target_id);

			if (!target_group)
				throw (std::runtime_error("[NodeTree]: Warning: Target group doesn't exist!"));

			boost::shared_ptr<GGroup> group (new GGroup (new_id));

			Tree::Node<NodePtr> *new_node = new Tree::Node<NodePtr> (group);
			_node_map[new_id] = new_node;
			target_group->push_back (new_node);
		}
		break;

		case 2:
		{
			Tree::Node<NodePtr> *target_node = find_node_by_id (add_target_id);

			if (!target_node)
				throw (std::runtime_error("[NodeTree]: Warning: Target node doesn't exist!"));

			boost::shared_ptr<GGroup> group (new GGroup (new_id));

			Tree::Node<NodePtr> *new_node = new Tree::Node<NodePtr> (group);
			_node_map[new_id] = new_node;
			target_node->insert (new_node, true);
		}
		break;
	
		case 3:
		{
			Tree::Node<NodePtr> *target_node = find_node_by_id (add_target_id);

			if (!target_node)
				throw (std::runtime_error("[NodeTree]: Warning: Target node doesn't exist!"));

			boost::shared_ptr<GGroup> group (new GGroup (new_id));

			Tree::Node<NodePtr> *new_node = new Tree::Node<NodePtr> (group);
			_node_map[new_id] = new_node;
			target_node->insert (new_node, false);
		}
		break;

		case 4:
		{
			Tree::Node<NodePtr> *target_node = find_node_by_id (add_target_id);

			if (!target_node)
				throw (std::runtime_error("[NodeTree]: Warning: Target node doesn't exist!"));

			boost::shared_ptr<GGroup> group (new GGroup (new_id));

			Tree::Node<NodePtr> *new_node = new Tree::Node<NodePtr> (group);
			_node_map[new_id] = new_node;
			target_node->insert (new_node, true);
			delete (target_node);
		}
		break;

		default:
		break;
	}

	Options *options = Options::get_instance ();
	if (options->_verbose >= 2)
	{
		dump (_tree.get_root ());
		std::cout << std::endl;
	}

	return new_id;
}

Tree::Node<NodePtr> *NodeTree::find_node_by_id (int id)
{
	if (_node_map.find(id) == _node_map.end ())
		return 0;

	return _node_map[id];
}

Tree::Node<NodePtr> *NodeTree::find_synth_by_id (int id)
{
	Tree::Node<NodePtr> *node = find_node_by_id (id);

	if (!node)
		return 0;

	if (dynamic_cast<GSynth*> (node->_payload.get()))
		return node;
	else
		return 0;
}

Tree::Node<NodePtr> *NodeTree::find_group_by_id (int id)
{
	Tree::Node<NodePtr> *node = find_node_by_id (id);

	if (!node)
		return 0;

	if (dynamic_cast<GGroup*> (node->_payload.get()))
		return node;
	else
		return 0;
}


// TODO: check if this is toooo slow
int NodeTree::next_free_id ()
{
	int id = SCGRAPH_DEFAULT_GROUP_ID;

	while (true)
	{
		if (_node_map.find (id) == _node_map.end())
		{
			return id;
		}
		++id;
	}
}

Tree::Tree<NodePtr>::Iterator NodeTree::begin ()
{
	return _tree.pbegin ();
}

Tree::Tree<NodePtr>::Iterator NodeTree::end ()
{
	return _tree.pend ();
}

void NodeTree::done_action (int id, int done_action)
{
	// ScGraph *scgraph = ScGraph::get_instance ();

	/* there's a reader lock held when this is called from the unit */
	//scgraph->unlock ();

	//scgraph->lock_for_write ();

	// here the done action code
	// std::cout << id << " " << done_action << std::endl;

	switch (done_action)
	{
		case 1:
		break;

		case 2:
		{
			n_free (id);
		}
		break;
	}


	//scgraph->unlock ();

	/* reestablich reader lock */
	//scgraph->lock_for_read ();
}


void NodeTree::n_free (int id)
{
	// std::cout << "n_free" << std::endl;

	Tree::Node<NodePtr> *node = find_synth_by_id (id);

	if (node)
		delete node;

	_node_map.erase (id);
}


void NodeTree::n_set (int id, const std::string &param_name, float value)
{

	Tree::Node<NodePtr> *node = find_synth_by_id (id);
	if (node)
	{
		GSynth* synth;
		if ((synth = dynamic_cast<GSynth*> (node->_payload.get())))
		{
			synth->c_set (param_name.c_str(), value);
			return;
		}
	}

	node = find_group_by_id (id);
	if (node)
	{
		GGroup* group;
		if ((group = dynamic_cast<GGroup*> (node->_payload.get())))
		{
			// iterate over group and c_set on each one
			Tree::Node<NodePtr>::Iterator it = node->get_iterator ();

			it = it.down_first ();

			while (it != 0)
			{
				n_set ((*it)->_id, param_name, value);
				++it;
			}
			return;
			//synth.c_set (param_name, value);
		}
	}

	std::cout << "[NodeTree]: Warning: n_set (): node " << id << " doesn't exist." << std::endl;
}

void NodeTree::n_set (int id, int index, float value)
{
	Tree::Node<NodePtr> *node = find_synth_by_id (id);
	if (node)
	{
		GSynth* synth;
		if ((synth = dynamic_cast<GSynth*> (node->_payload.get())))
		{
			synth->c_set (index, value);
			return;
		}
	}

	node = find_group_by_id (id);
	if (node)
	{
		GGroup* group;
		if ((group = dynamic_cast<GGroup*> (node->_payload.get())))
		{
			// iterate over group and c_set on each one
			Tree::Node<NodePtr>::Iterator it = node->get_iterator ();

			it = it.down_first ();

			while (it != 0)
			{
				n_set ((*it)->_id, index, value);
				++it;
			}
			return;
			//synth.c_set (param_name, value);
		}
	}

	std::cout << "[NodeTree]: Warning: n_set (): node " << id << " doesn't exist." << std::endl;

}


int NodeTree::get_index(int id, const std::string &param_name) {
	Tree::Node<NodePtr> *node = find_synth_by_id (id);
	GSynth* synth;
	if ((synth = dynamic_cast<GSynth*> (node->_payload.get())))
	{
		return synth->get_index(param_name.c_str());
	}
	else throw std::runtime_error("parameter name not known... please report this as a bug");
}



