#include "shader_pool.h"

#include "options.h"

#include <sstream>
#include <iostream>
#include <cmath>
#include <fstream>
#include <stdexcept>

#include <sys/types.h>
#include <dirent.h>

#include <cstring>

ShaderPool *ShaderPool::_instance = 0;

ShaderPool *ShaderPool::get_instance ()
{
	if (!_instance)
		_instance = new ShaderPool;

	return _instance;
}

ShaderPool::ShaderPool ()
{
	//Options *options = Options::get_instance ();

	//tokenize_path (options->_shader_path, &ShaderPool::scan_directory);
}

#if 0
void ShaderPool::tokenize_path (const std::string &path, void (ShaderPool::*function)(const std::string &directory))
{
	std::stringstream path_stream (path);

	// FIXME: dirty hack
	char buffer [PATH_MAX];
	
	while (path_stream.getline (buffer, PATH_MAX, ':'))
	{
		if (std::string (buffer) == "")
			continue;

		(*this.*function) (buffer);
	}
}

void ShaderPool::scan_directory (const std::string &directory)
{
	try
	{
		traverse_directory (directory, &ShaderPool::add_shader);
	}
	catch (const char *e)
	{
		std::cout << "[ShaderPool]: Failed to traverse directory. Reason: " << e << std::endl;
	}
}

void ShaderPool::traverse_directory (const std::string &directory, void (ShaderPool::*function)(const std::string &filename))
{
	Options *options = Options::get_instance ();

	if (options->_verbose >= 2)
		std::cout << "[ShaderPool]: Adding shader directory: " << directory << std::endl;


	DIR *dir = opendir (directory.c_str ());
	if (!dir)
		throw ("[ShaderPool]: Couldn't read directory");


	std::vector<std::string> filenames;
	std::vector<std::string> dirnames;

	struct dirent *dirent;
	while ((dirent = readdir (dir)))
	{
		if ((std::string (dirent->d_name) == std::string (".")) || (std::string (dirent->d_name) == std::string ("..")))
			continue;
 
		if (dirent->d_type == DT_DIR)
		{
			dirnames.push_back (directory + "/" + std::string (dirent->d_name));
		}
		else
		{
			filenames.push_back(directory + "/" + std::string (dirent->d_name));

		}
	}

	std::sort(filenames.begin(), filenames.end());
	std::sort(dirnames.begin(), dirnames.end());

	for (size_t i = 0; i < filenames.size(); ++i)
		(*this.*function) (filenames[i]);

	for (size_t i = 0; i < dirnames.size(); ++i)
		traverse_directory (dirnames[i], function);


	closedir (dir);
}

void ShaderPool::add_shader (const std::string &filename)
{
	Options *options = Options::get_instance ();

	if (options->_verbose >= 2)
		std::cout << "[ShaderPool]: Adding shader program: " << filename << std::endl;

	try
	{
		std::string program;
		std::ifstream is(filename.c_str());

		while(is.good()) {
			std::string line;
			std::getline(is, line);
			program += line;
		}

		Shader::eType type;
		// starts with "vertex_"?
		if (filename.find("vertex_") == 0)
			type = Shader::VERTEX;
		else
			type = Shader::FRAGMENT;

		boost::shared_ptr<Shader> t(new Shader(program, type));
		_shaders.push_back (t);
	
		if (options->_verbose >= 2)
			std::cout << "  [ShaderPool]: New shader has index: " << _shaders.size() - 1 << std::endl;
	
		emit (shaders_changed());
	}
	catch (std::runtime_error &e)
	{
		std::cout << "[ShaderPool]: Problem loading shader: " << e.what () << std::endl;
	}
	// emit(shader_added(_shaders.size () - 1));
}
#endif

ShaderPool::~ShaderPool ()
{
	// TODO: clean up
}

#if 0
unsigned int ShaderPool::get_number_of_shader_programs ()
{
	return _shader_programs.size ();
}
#endif

boost::shared_ptr<ShaderPool::ShaderProgram> ShaderPool::get_shader_program (unsigned int index)
{
	return _shader_programs[index];
}

void ShaderPool::add_shader_program (unsigned int index, boost::shared_ptr<ShaderPool::ShaderProgram> program) {
	std::cout << "[ShaderPool]: add_shader_program()" << std::endl;
	_shader_programs[index] = program;
	emit(shader_programs_changed());
}

void ShaderPool::clear_shader_programs() {
	std::cout << "[ShaderPool]: clear_shader_program()" << std::endl;
	_shader_programs.clear();
	emit(shader_programs_changed());
}



