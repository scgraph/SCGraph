#include "tree.h"
#include <iostream>
#include <map>

#include <boost/shared_ptr.hpp>

class Foo
{
	public:
		friend std::ostream &operator<< (std::ostream& out, const Foo& foo)
		{
			out << "Foo";
			return out;
		}
		Foo()
		{
			std::cout << "foo" << std::endl;
		}
		~Foo()
		{
			std::cout << "~foo" << std::endl;
		}

};

void walk_tree (Tree::Tree<int> &tree)
{
	std::cout << "PreOrder" << std::endl;
	for (Tree::Tree<int>::Iterator it = tree.begin (); it != tree.end (); ++it)
	{
		std::cout << (*it) << " ";
	}
	std::cout << std::endl;

	std::cout << "PostOrder" << std::endl;
	for (Tree::Tree<int>::Iterator it = tree.pbegin (); it != tree.pend (); ++it)
	{
		std::cout << (*it) << " ";
	}
	std::cout << std::endl;

	std::cout << "PostOrder, leafs only" << std::endl;
	for (Tree::Tree<int>::Iterator it = tree.pbegin (); it != tree.pend (); ++it)
	{
		if (!(it.current_node())->is_leaf ())
			continue;

		std::cout << (*it) << " ";
	}
	std::cout << std::endl;
	std::cout << std::endl;
}


int main()
{

	try
	{
		Tree::Tree<Foo> tree_foo;
		tree_foo.dump();

		Tree::Tree<int> tree;
	
		// Tree::Tree<int&> tree2;
	
		// Tree::Tree<std::pair<int, int> > tree2;
		Tree::Tree<void *> tree3;
	
		// NodeList<int> *node_list = new NodeList<int>(1000);
	
	
		Tree::Node<int> *node = new Tree::Node<int>(1);
		tree.set_root (node);
		tree.dump ();
		walk_tree (tree);
	
		node = new Tree::Node<int>(2);
		tree.get_root()->push_front (node);
		tree.dump ();
		walk_tree (tree);
	
		
		node = new Tree::Node<int>(3);
		tree.get_root()->push_front (node);
		tree.dump ();
		walk_tree (tree);
		
	
		tree.get_root()->push_front (7);
		tree.dump ();
		walk_tree (tree);
	
		node->push_back (new Tree::Node<int>(4));
		tree.dump ();
		walk_tree (tree);
	
		node->insert (new Tree::Node<int>(1024), true);
		tree.dump ();
		walk_tree (tree);
	
		node->insert (new Tree::Node<int>(1025), false);
		tree.dump ();
		walk_tree (tree);
	
		node->insert (1026, true);
		tree.dump ();
		walk_tree (tree);
	
		node->insert (1027, false);
		tree.dump ();
		walk_tree (tree);
	
		Tree::Node<int>::Iterator it = node->get_iterator ();
		while (it.valid())
		{
			std::cout << (*it) << std::endl;
			--it;
		}

		it = node->get_iterator ();
		while (it.valid())
		{
			std::cout << (*it) << std::endl;
			it = it.up ();
		}
	
		node = new Tree::Node<int>(5);
		tree.get_root()->push_back (node);
		tree.dump ();
		walk_tree (tree);
	
		delete node;
		tree.dump ();
		walk_tree (tree);
	}
	catch (char * e)
	{
		std::cout << e << std::endl;
	}

	Tree::Tree<boost::shared_ptr<Foo> > tree4;
	boost::shared_ptr<Foo> p(new Foo);
	Tree::Node<boost::shared_ptr<Foo> > *node = new Tree::Node<boost::shared_ptr<Foo> >(boost::shared_ptr<Foo> (new Foo));
	tree4.set_root(node);

}
