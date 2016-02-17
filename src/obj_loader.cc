#include "obj_loader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

void ObjLoader::read_file (const std::string &file_name)
{
	std::ifstream istream (file_name.c_str ());

	char *buffer = new char [1024];

	std::vector <Vector3D> _vertices;
	std::vector <Vector3D> _normals;
	std::vector <Vector2D> _texture_coordinates;

	std::cout << "[ObjLoader] Loading model in " << file_name << std::endl;


	while (istream.good ())
	{
		istream.getline (buffer, 1023);

		//std::cout << buffer << std::endl;
		/* comments */
		if (buffer[0] == '#')
			continue;

		std::stringstream stream (buffer);

		std::string linetype;

		stream >> linetype;

	    //std::cout << linetype << std::endl;

		if (linetype == "v")
		{
			// std::cout << "vertex" << std::endl;
			Vector3D v;
			stream >> v._c[0] >> v._c[1] >> v._c[2];
			_vertices.push_back (v);
		}
		else if (linetype == "vt")
		{
			// std::cout << "texturecoordinate" << std::endl;
			Vector2D v;
			stream >> v._c[0] >> v._c[1];
			_texture_coordinates.push_back (v);
		}
		else if (linetype == "vn")
		{
		   std::cout << "normal" << std::endl;
			Vector3D v;
			stream >> v._c[0] >> v._c[1] >> v._c[2];
			_normals.push_back (v);
		}
		else if (linetype == "f" || linetype == "fo")
		{
			cow_ptr<Face> face (new Face(Face::POLYGON));
			while (stream.good ())
			{
				/* check for type of face definition v or v/t or v//n or v/t/n */
				std::string tmp;
				stream >> tmp;

				if (tmp.length() == 0)
					break;

				size_t pos1 = tmp.find('/');
				size_t pos2 = tmp.find('/', pos1 + 1);

				// std::cout << (int)pos1 << " " << (int)pos2 << " " << (int)pos3 << std::endl;

				// bad bad parsing, but what the hell :)
				if (pos1 == std::string::npos)
				{
					// ok this must be a single vector
					std::stringstream tmpstream(tmp);
					size_t index;
					tmpstream >> index;
					face.touch()->_vertices.push_back (_vertices[index]);
				}
				else
				{
					// find the second /
					if (pos2 == std::string::npos)
					{
						// no second / found. in this case the format is v/t
						//std::cout << tmp.substr(0, pos1) << std::endl;
						std::stringstream tmpstream (tmp.substr(0, pos1));
						size_t index1;
						tmpstream >> index1;
						face.touch()->_vertices.push_back (_vertices[index1]);

						// std::cout << tmp.substr(pos1 + 1, tmp.length() - (pos1 + 1)) << std::endl;
						std::stringstream tmpstream2 (tmp.substr(pos1 + 1, tmp.length() - (pos1 - 1)));
						size_t index2;
						tmpstream2 >> index2;
						face.touch()->_texture_coordinates.push_back(_texture_coordinates[index2]);
					}
					else
					{
						// okay we found the second /. let's check whether it's v/t/n or v//n
						if (pos2 - pos1 == 1)
						{
							// it's v//n
							std::stringstream tmpstream (tmp.substr(0, pos1));
							size_t index1;
							tmpstream >> index1;
							face.touch()->_vertices.push_back (_vertices[index1]);
	
							// std::cout << tmp.substr(pos1 + 2, tmp.length() - (pos1 + 2)) << std::endl;
							std::stringstream tmpstream2 (tmp.substr(pos1 + 2, tmp.length() - (pos1 - 2)));
							size_t index2;
							tmpstream2 >> index2;
							face.touch()->_normals.push_back(_normals[index2]);
						}
						else if (pos2 - pos1 > 1)
						{
							// it's v/t/n
							std::stringstream tmpstream (tmp.substr(0, pos1));
							size_t index1;
							tmpstream >> index1;
							face.touch()->_vertices.push_back (_vertices[index1]);
	
							// std::cout << tmp.substr(pos1 + 1, (pos2) - (pos1 + 1)) << std::endl;
							std::stringstream tmpstream2 (tmp.substr(pos1 + 1, (pos2) - (pos1 + 1)));
							size_t index2;
							tmpstream2 >> index2;
							face.touch()->_texture_coordinates.push_back(_texture_coordinates[index2]);

							// std::cout << tmp.substr(pos2 + 1, tmp.length() - (pos2 + 1)) << std::endl;
							std::stringstream tmpstream3 (tmp.substr(pos2 + 1, tmp.length() - (pos2 + 1)));
							size_t index3;
							tmpstream3 >> index3;
							face.touch()->_normals.push_back(_normals[index3]);
						}
					}
				}
			}

			_geometry.touch()->_faces.push_back (face);
		}
	}
}
