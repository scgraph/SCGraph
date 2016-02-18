#ifndef SCGRAPH_STRING_POOL_HH
#define SCGRAPH_STRING_POOL_HH

#include "boost/shared_ptr.hpp"
#include <vector>
#include <string>
/*
#include <QtCore/QObject>
#include <QtCore/QString>

 */
#include "ofTrueTypeFont.h"

class StringPool //: public QObject
{
	//Q_OBJECT

	static StringPool *_instance;
	std::vector <std::string> _strings;

	

	void add_string (const std::string &filename);

	public:
		StringPool ();
		virtual ~StringPool ();
		static StringPool *get_instance ();

		unsigned int get_number_of_strings ();
		std::string get_string (unsigned int index);
		unsigned int change_string (const std::string &str, unsigned int index);

		ofTrueTypeFont* _font;
                //FTBufferFont* _font;
                //FTTextureFont* _font;
		void set_font(const std::string &filename);

		/** if the index is -1, we simply add it to the end of the list */
		unsigned int add_string (const std::string &str, unsigned int index);

};

#endif
