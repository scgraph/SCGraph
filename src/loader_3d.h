#ifndef SCGRAPH_LOADER_3D_HH
#define SCGRAPH_LOADER_3D_HH

/** This class defines an interface, which different 3d geometry loaders
    have to implement */

#include <string>
#include <boost/shared_ptr.hpp>

#include "graphics.h"

class Loader3D
{
	boost::shared_ptr<Geometry> _graphics;

	public:
		virtual ~Loader3D ();

		/** might throw a std::string Exception */
		Loader3D (const std::string &file_name);

		/** implement this function to read the file */
		virtual void read_file (const std::string &file_name);

		/** returns a reference to the graphics object read from the file */
		virtual const Graphics &get_graphics ();
};

#endif
