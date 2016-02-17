#include "loader_3d.h"

Loader3D::Loader3D (const std::string &file_name)
{
	_graphics = boost::shared_ptr<Geometry> (new Geometry ());
}

void Loader3D::read_file (const std::string &file_name)
{

}

Loader3D::~Loader3D ()
{

}

const Graphics &Loader3D::get_graphics ()
{
	return *_graphics;
}
