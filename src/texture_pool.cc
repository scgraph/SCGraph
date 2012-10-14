#include "texture_pool.h"

#include "options.h"


#include <sstream>
#include <iostream>
#include <cmath>

#include <sys/types.h>
#include <dirent.h>

#include <cstring>

extern "C"
{
#define __STDC_CONSTANT_MACROS // for UINT64_C
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

extern "C"
{
int ffmpeg_lock_callback(void **mutex, enum AVLockOp op);
}

int ffmpeg_lock_callback(void **mutex, enum AVLockOp op)
{
	static QMutex m;

	switch(op)
		{
		case AV_LOCK_CREATE:
			{
				*mutex = &m;
				break;
			}
		case AV_LOCK_OBTAIN:
			{
				((QMutex*)(*mutex))->lock();
				break;
			}
		case AV_LOCK_RELEASE:
			{
				((QMutex*)(*mutex))->unlock();
				break;
			}
		case AV_LOCK_DESTROY:
			{
				*mutex = 0;
				break;
			}
		}

	return 0;
} 

typedef boost::shared_ptr< AbstractTexture > AbstractTexturePtr;

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

	// Register all formats and codecs
	av_register_all();
	
	av_lockmgr_register(&ffmpeg_lock_callback); 

	qRegisterMetaType<uint32_t>("uint32_t");
	qRegisterMetaType<boost::shared_ptr<Texture> >("boost::shared_ptr<Texture>");

	nextID = 0;

	tokenize_path (options->_texture_path, &TexturePool::scan_directory);
}


uint32_t TexturePool::get_id() {
	return nextID++;
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
	AbstractTexturePtr tmp = boost::make_shared<ImageTexture>();

	if(tmp->load(filename) == 0) {
		_textures.push_back (tmp);
			
		//if (options->_verbose >= 2)
		std::cout << "  [TexturePool]: New texture has index: " 
				  << _textures.size() - 1 << std::endl;
	
		emit (texture_changed(_textures.size() - 1));
	}
	else {
		AbstractTexturePtr tmp = boost::make_shared<VideoTexture>();
		if(tmp->load(filename) == 0) {
			_textures.push_back (tmp);

			std::cout << "  [TexturePool]: New video texture has index: " 
					  << _textures.size() - 1 << std::endl;
	
			emit (texture_changed(_textures.size() - 1));
		}
	}
	return _textures.size() - 1;
}


void TexturePool::add_image (const std::string &filename)
{
	add_image(filename, 0);
}

unsigned int TexturePool::change_image (const std::string &filename, unsigned int index)
{
	AbstractTexturePtr tmp = boost::make_shared<ImageTexture>();
	if(tmp->load(filename) == 0) {
		if(index < _textures.size()) {

			// TODO TODO TODO
			_textures[index] = tmp;

			std::cout << "  [TexturePool]: Changed texture at index " 
					  << index << std::endl;
	
		emit (texture_changed(index));
		}
	}

	return _textures.size() - 1;
}


TexturePool::~TexturePool ()
{
	// TODO: clean up
}


void TexturePool::delete_textures_at_id(uint32_t id) {
	_tmp_textures.remove(id);
	emit(delete_texture(id));
}

void TexturePool::update_tmp_texture(uint32_t id, bool samep)
{
	//std::cout << "[TexturePool] updating tmp texture" << std::endl;
	emit(change_tmp_texture(id, samep));
}

void TexturePool::loaded_tmp_texture(uint32_t id) {
	emit(texture_loaded(id));
}

unsigned int TexturePool::get_number_of_textures ()
{
	return _textures.size ();
}

boost::optional<boost::shared_ptr<AbstractTexture> > TexturePool::get_texture (unsigned int index)
{
	if(index < _textures.size())
		return boost::optional<boost::shared_ptr<AbstractTexture> >(_textures[index]);
	else
		{
			//std::cout << "[TexturePool]: texture doesn't exist!"  << std::endl;
			return boost::optional<boost::shared_ptr<AbstractTexture> >();
		}
}


void TexturePool::update_texture (unsigned int index)
{
	emit (texture_changed(index));
}
