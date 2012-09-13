#ifndef SCGRAPH_TEXTFIELD_HH
#define SCGRAPH_TEXTFIELD_HH

#include "../unit.h"
#include "../string_pool.h"

#include <QtCore/QString.h>
#include <QtGui/QFont.h>


class Textfield : public GUnit
{
	cow_ptr<Text> _t;

	unsigned int _index;

	public:
		Textfield ();
		~Textfield ();

		virtual void process_g (double delta_t);
};

#endif
