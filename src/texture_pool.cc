#include "texture_pool.h"

#include "options.h"

#include <sstream>
#include <iostream>
#include <cmath>

#include <sys/types.h>
#include <dirent.h>

#include <cstring>

Texture::Texture (int width, int height, int channels, bool zero = true)
{
	_width = width; _height = height; _channels = channels;

	_data = new unsigned char [width * height * channels];

	if(zero)
		this->zero();
}


Texture::~Texture ()
{
	delete[] _data;
}

void Texture::fill (uint32_t starting_index, uint32_t num_samples, int32_t color)
{
	for(int i = 0; i < num_samples; i++) {
		set_pixel(starting_index + i, 
				  color);
	}
}

void Texture::zero ()
{
	memset (_data, 0, _width * _height * _channels);
}

void Texture::set_pixel(unsigned int pindex, int32_t color)
{
	if(pindex < (_width * _height)) {
		int tmpIndex = 4 * pindex;
		_data[tmpIndex]     = (unsigned char) (color & 0xFF);
		_data[tmpIndex + 1] = (unsigned char) ((color >> 8) & 0xFF);
		_data[tmpIndex + 2] = (unsigned char) ((color >> 16) & 0xFF);
		_data[tmpIndex + 3] = (unsigned char) ((color >> 25) & 0xFF);
	}
	else {
		std::cout << "[TexturePool]: coordinate out of range!"  << std::endl;
	}
}

TexturePool *TexturePool::_instance = 0;

TexturePool *TexturePool::get_instance ()
{
	if (!_instance)
		_instance = new TexturePool;

	return _instance;
}

TexturePool::TexturePool ()
{
	Options *options = Options::get_instance ();

	tokenize_path (options->_texture_path, &TexturePool::scan_directory);
}

void TexturePool::tokenize_path (const std::string &path, void (TexturePool::*function)(const std::string &directory))
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

void TexturePool::scan_directory (const std::string &directory)
{
	try
	{
		traverse_directory (directory, &TexturePool::add_image);
	}
	catch (const char *e)
	{
		std::cout << "[TexturePool]: Failed to traverse directory. Reason: " << e << std::endl;
	}
}

void TexturePool::traverse_directory (const std::string &directory, void (TexturePool::*function)(const std::string &filename))
{
	Options *options = Options::get_instance ();

	if (options->_verbose >= 2)
		std::cout << "[TexturePool]: Adding texture directory: " << directory << std::endl;


	DIR *dir = opendir (directory.c_str ());
	if (!dir)
		throw ("[TexturePool]: Couldn't read directory");


	std::vector<std::string> filenames;
	std::vector<std::string> dirnames;

	struct dirent *dirent;
	while ((dirent = readdir (dir)))
	{
		if ((std::string (dirent->d_name) == std::string (".")) 
			|| (std::string (dirent->d_name) == std::string ("..")))
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

unsigned int TexturePool::add_image (const std::string &filename, unsigned int index)
{
	std::cout << "[TexturePool]: Adding texture: " << filename << std::endl;
	boost::shared_ptr<Texture> tmp = load_image(filename);

	if(tmp) {
		_textures.push_back (tmp);
			
		//if (options->_verbose >= 2)
		std::cout << "  [TexturePool]: New texture has index: " 
				  << _textures.size() - 1 << std::endl;
	
		emit (textures_changed());
	}
	return _textures.size() - 1;
}

void TexturePool::add_image (const std::string &filename)
{
	add_image(filename, 0);
}

unsigned int TexturePool::change_image (const std::string &filename, unsigned int index)
{
	change_image(filename, index);
	boost::shared_ptr<Texture> tmp = load_image(filename);

	if(tmp) {
		if(index < _textures.size()) {
			_textures[index] = tmp;
			
			std::cout << "  [TexturePool]: Changed texture at index " 
					  << index << std::endl;
	
		emit (textures_changed());
		}
	}

	return _textures.size() - 1;
}

boost::shared_ptr<Texture> TexturePool::load_image (const std::string &filename)
{
	Options *options = Options::get_instance ();

	try
	{

		const char * tmp = filename.c_str();
		QImage image(tmp);

		if(image.isNull()) {
			std::cout << "  [TexturePool]: Unrecognized Image Format. No texture loaded." 
					  << std::endl;
			std::cout << "                 Supported Formats:";
			for (int i = 0; i < QImageReader::supportedImageFormats().size(); ++i)
			{
				std::cout << " " << QImageReader::supportedImageFormats().at(i).data();
			}
			std::cout << "." << std::endl;
		}
		else
		{
			int im_width, im_height, tex_width, tex_height;

			if(image.isNull()) 
				{
				std::cout 
					<< 	"  [TexturePool]: Unable to convert image to GL format. No texture loaded." 
					<< std::endl;
				}
			else
				{		
					im_width = image.width();
					im_height = image.height();

					tex_width =  (int)pow(2,(int)ceil(log2(im_width)));
					tex_height = (int)pow(2,(int)ceil(log2(im_height)));

					boost::shared_ptr<Texture> t(new Texture (tex_width, tex_height, 4));

					if (options->_verbose >= 2)
						{
							std::cout << "  [TexturePool]: Texture Width/Height: " << tex_width 
									  << "/" << tex_height << std::endl;
							std::cout << "  [TexturePool]: Image source Width/Height: " << im_width 
									  << "/" << im_height << std::endl;
						}

					// std::cout << width << " " << height << std::endl;
	
					for (int i = 0; i < im_width; ++i)
						{
							for (int j = 0; j < im_height; ++j)
								{
									// swap image
									QRgb color = image.pixel(i,im_height - j - 1);

									int tmpIndex = 4 * (tex_width * j + i);
									t->_data[tmpIndex]     = (unsigned char) qRed(color);
									t->_data[tmpIndex + 1] = (unsigned char) qGreen(color);
									t->_data[tmpIndex + 2] = (unsigned char) qBlue(color);
									t->_data[tmpIndex + 3] = (unsigned char) qAlpha(color);
								}
						}

					return t;
				}
		}
	}
	catch (const char* error)
	{
		std::cout << "[TexturePool]: Problem loading texture: " 
				  << error  << std::endl;
	}
	// emit(texture_added(_textures.size () - 1));
}

TexturePool::~TexturePool ()
{
	// TODO: clean up
}



unsigned int TexturePool::get_number_of_textures ()
{
	return _textures.size ();
}

boost::shared_ptr<Texture> TexturePool::get_texture (unsigned int index)
{
	if(index < _textures.size())
		return _textures[index];
	else
		{
			std::cout << "[TexturePool]: texture doesn't exist!"  << std::endl;
		}
}


void TexturePool::update ()
{
	emit (textures_changed());
}
