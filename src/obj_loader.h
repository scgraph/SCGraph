#ifndef SCGRAPH_OBJ_LOADER_HH
#define SCGRAPH_OBJ_LOADER_HH

#include <vector>

#include "loader_3d.h"
#include "graphics.h"

class ObjLoader : public Loader3D
{
	void push_back (double n);

	public:
		void read_file (const std::string &file_name);
		cow_ptr <Geometry> _geometry;


		ObjLoader (const std::string &file_name) : 
			  Loader3D (file_name),
			  _geometry(new Geometry)
		{
			read_file (file_name); 
		}
};


#endif
