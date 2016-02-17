#ifndef NARY_TREE_HH
#define NARY_TREE_HH

#include <iostream>
#include <stack>

/** Somebody think of the children!!! Doesn't anyone think of the children?!? */
namespace Tree
{

/** All member variables are strictly for internal use. Use the methods instead */
template <class T> struct Node
{
	T        _payload;

	Node<T> *_parent;
	Node<T> *_next;
	Node<T> *_previous;

	// = 0 if no children
	Node<T> *_start;
	Node<T> *_end;

	/** You usually don't want to set any of the default parameters, unless
	    you really know what you're doing
	 */
	Node (const T& payload, Node<T>* parent = 0, Node<T>* next = 0, Node<T>* previous = 0, Node<T> *start = 0, Node<T> *end = 0) :
		_payload  (payload),
		_parent (parent),
		_next (next),
		_previous (previous),
		_start (start),
		_end (end)
	{

	}

	/** This destructor does take care of maintaining the tree structure
		[i.e. updating parent/neighbouring nodes]. 
	*/
	virtual ~Node ()
	{
		//std::cout << "~Node()" << std::endl;
		// if we have children, take care of them first
		if (_start != 0)
		{
			Node<T>* current = _start;
			while (current != 0)
			{
				Node<T> *tmp = current->_next;
				delete current;
				current = tmp;
			}
		}

		// ok, all children gone. now remove ourself cleanly
		if (_parent == 0)
		{
			// we are done if no parent exists
			return;
		}

		if (_previous == 0 && _next == 0)
		{
			_parent->_start = 0;
			_parent->_end   = 0;
			return;
		}

		if (_previous == 0)
			_parent->_start = _next;
		else
			_previous->_next = _next;

		if (_next == 0)
			_parent->_end = _previous;
		else
			_next->_previous = _previous;
	}

	bool has_children ()
	{
		return !(_start == 0 && _end == 0);
	}

	bool is_leaf ()
	{
		return !(has_children ());
	}

#if 0
	virtual void dump ()
	{
		std::cout << Node<T>::_payload << " ";

		Node<T> *current = _start;

		if (_start == 0)
			return;

		std::cout << "[ ";
		while (current != 0)
		{
			current->dump ();
			// std::cout << " ";
			current = current->_next;
		}
		std::cout << "] ";
	}
#endif

	void push_front (const T &payload)
	{
		Node<T> *node = new Node<T>(payload);
		insert (_start, node, true);
	}

	void push_back (const T &payload)
	{
		Node<T> *node = new Node<T>(payload);
		insert (_end, node, false);
	}

	void push_front (Node<T> *what)
	{
		insert (_start, what, true);
	}

	void push_back (Node<T> *what)
	{
		insert (_end, what, false);
	}

	/** Insert a node containing what before or after this node. */
	void insert (const T& what, bool before)
	{
		Node<T> *node = new Node<T>(what);
		insert (node, before);
	}

	void insert (Node<T> *what, bool before)
	{
		if (_parent == 0)
			throw ("Node::insert () - Error: No parent");

		what->_parent = _parent;

		if (before)
		{
			what->_previous = _previous;
			_previous = what;
			what->_next = this;
		}
		else
		{
			what->_next = _next;
			_next = what;
			what->_previous = this;
		}

		if (what->_previous == 0)
			_parent->_start = what;
		else
			what->_previous->_next = what;

		if (what->_next == 0)
			_parent->_end = what;
		else
			what->_next->_previous = what;
	}

	/** Inserts what before or after where into the list of children of this node.. */
	void insert (Node<T> *where, Node<T> *what, bool before)
	{
		if (where != 0 && where->_parent != this)
			throw ("Node::insert () - Error, target is not child of this node");

		if (_start == 0 && _end == 0)
		{
			if (where != 0)
				throw ("Node::insert () - Error: Target does not exist");

			what->_parent = this;

			_start = what;
			_end = what;

			return;
		}

		if (before)
		{
			what->_previous = where->_previous;
			where->_previous = what;
			what->_next = where;

		}
		else
		{
			what->_next = where->_next;
			where->_next = what;
			what->_previous = where;

		}

		if (what->_previous == 0)
			_start = what;
		else
			what->_previous->_next = what;


		if (what->_next == 0)
			_end = what;
		else
			what->_next->_previous = what;

		what->_parent = this;

#if 0
		if (what->_previous)
			std::cout << "pn " << what->_previous->_next->_payload <<  std::endl;
		if (what->_next)
			std::cout << "np " << what->_next->_previous->_payload <<  std::endl;
#endif
	};

	/** this iterator class allows "direct" traversel via the provided members */
	class Iterator;

	Iterator get_iterator ()
	{
		return Iterator(this);
	}
};


template <class T> class Node<T>::Iterator
{
	Node<T> *_current;

	public:
		/** an iterator that starts with the specified node */
		Iterator (Node<T> *node) :
			_current (node)
		{
		}

		/** this constructor produces an iterator == end (); */
		Iterator () :
			_current (0)
		{
		}

		bool valid ()
		{
			return (_current != 0);
		}

		bool invalid()
		{
			return (_current == 0);
		}

		Node<T> *current_node ()
		{
			return (_current);
		}

		T *operator->()
		{
			return &_current->_payload;
		}

		T &operator* ()
		{
			return (_current->_payload);
		}

		/** Move one level up to the parent */
		Iterator& up ()
		{
			_current = _current->_parent;
			return *this;
		}

		/** Move one level down to the first child of the current node */
		Iterator& down_first ()
		{
			_current = _current->_start;
			return *this;
		}

		/** Move one level down to the last child of the current node */
		Iterator& down_last ()
		{
			_current = _current->_end;
			return *this;
		}

		/** The ++ and -- operators are equivalent to the next() and previous() methods */
		Iterator& operator++()
		{
			_current = _current->_next;
			return *this;
		}

		Iterator& operator--()
		{
			_current = _current->_previous;
			return *this;
		}

		Iterator& next ()
		{
			_current = _current->_next;
			return *this;
		}

		Iterator &previous ()
		{
			_current = _current->_previous;
			return *this;
		}

		bool operator== (const Iterator &it)
		{
			return (_current == it._current);
		}

		bool operator!= (const Iterator &it)
		{
			return !(_current == it._current);
		}
};


enum TreeIteratorType {PreOrder, PostOrder};

/** this class implements an n-ary tree which is optimized to allow quick traversal 
	of childs of one node (i.e. finding the previous node given a pointer to some node.
*/
template <class T> class Tree
{
	Node<T> *_root;

	public:
		/** as every node carries payload, so does the root node */
		Tree (Node<T>  *root = 0) :
			_root (root)
		{

		}

		~Tree ()
		{
			if (_root)
				delete _root;
		}

		class Iterator;

		Node<T> *get_root ()
		{
			return _root;
		}

		/** Inserts node inte the tree as child of target, before or after target.
			If the target is 0, the node will become the new root of the tree.
		*/
		void insert (Node<T>* target, Node<T>* node, bool before)
		{

		}

		void set_root (Node<T> *node)
		{
			if (_root)
				delete _root;

			_root = node;
		}

		/** Returns a default order (PreOrder) iterator */
		Iterator begin ()
		{
			return Iterator (_root);
		}

		/** Returns a default order (PreOrder) iterator */
		const Iterator end ()
		{
			return Iterator ();
		}

		/** Returns PostOrder iterator */
		Iterator pbegin ()
		{
			return Iterator (_root, PostOrder);
		}

		/** Returns a PostOrder iterator */
		const Iterator pend ()
		{
			return Iterator (PostOrder);
		}

		/** Generate a rudimentary ascii representation of the tree.
			Keep in mind that the Type T needs to have a suitable operator<<
			to be able to dump the content of the instances, or you might 
			get gibberish on the screen */
		void dump ()
		{
			std::cout << "[Tree]: Dump:" << std::endl;

#if 0
			if (_root)
				_root->dump ();
#endif
			std::cout << std::endl;
		}
};

template <class T> class Tree<T>::Iterator
{
	std::stack<Node<T>* > _stack;

	TreeIteratorType      _type;

	void init (Node<T>* node)
	{
		_stack.push (node);

		switch (_type)
		{
			case PostOrder:
				while (_stack.top()->_start != 0)
				{
					// descend until there's no more children
					_stack.push (_stack.top()->_start);
				}
			break;

			default:
			break;
		}
	}

	public:
		Iterator (Node<T> *node, TreeIteratorType type = PreOrder) :
			_type (type)
		{
			init (node);
		}

		Iterator (TreeIteratorType type = PreOrder) :
			_type (type)
		{
		}

		Node<T> *current_node ()
		{
			if (_stack.size () == 0)
				return 0;

			return (_stack.top());
		}

		T *operator->()
		{
			return &(_stack.top ()->_payload);
		}

		T &operator* ()
		{
			return (_stack.top ()->_payload);
		}

		Iterator& operator++ ()
		{
			if (_stack.size () == 0)
				return (*this);

			switch (_type)
			{
				case (PostOrder):
				{
					// Node<T> *top = _stack.top ();

					if (_stack.top()->_next == 0)
					{
						_stack.pop ();
						return (*this);
					}

					_stack.top () = _stack.top()->_next;

					while (_stack.top()->_start != 0)
					{
						// descend until there's no more children
						_stack.push (_stack.top()->_start);
					}
					return (*this);
				}
				break;

				default:
				{
					Node<T> *top = _stack.top ();

					if (top->_start != 0)
					{
						// non empty children list
						_stack.push (top->_start);
						return (*this);
					}

					// ok, so either it's a node or an empty list.
					// is there a next element then?
					if (top->_next != 0)
					{
						_stack.top () = top->_next;
						return (*this);
					}

					// no? ok.. so we go up until we either find a next node
					// or we come to the end.
					while (_stack.size () != 0 && _stack.top ()-> _next == 0)
					{
						_stack.pop ();
					}

					if (_stack.size () != 0)
					{
						_stack.top () = _stack.top ()->_next;
					}

					return (*this);
				}
				break;
			}
		}

		bool operator== (const Iterator &it)
		{
			if (_stack.size () == 0 && it._stack.size () == 0)
				return true;

			if (_stack.size () != it._stack.size ())
				return false;

			return (_stack.top () == it._stack.top ());
		}

		bool operator!= (const Iterator &it)
		{
			if (_stack.size () == 0 && it._stack.size () == 0)
				return false;

			if (_stack.size () != it._stack.size ())
				return true;

			return !(_stack.top () == it._stack.top ());
		}
};

}

#endif

