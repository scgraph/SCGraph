#ifndef SCGRAPH_TEXTURE_POOL_HH
#define SCGRAPH_TEXTURE_POOL_HH

#include "boost/shared_ptr.hpp"
#include "boost/make_shared.hpp"
#include "boost/optional.hpp"

#include "texture.h"

#ifdef HAVE_FFMPEG
#include "video_texture.h"
#endif

#include <vector>
#include <string>

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtGui/QImage>
#include <QtGui/QImageReader>
#include <QtGui/QColor>
#include <QtOpenGL/QGLWidget>

/** a texture image in RGBA format with unsigned chars as data */


class TexturePool : public QObject {
	Q_OBJECT

	static TexturePool *_instance;
	std::vector <boost::shared_ptr<AbstractTexture> > _textures;



	uint32_t nextID;

	void add_image (const std::string &filename);

	void tokenize_path (const std::string &path, void (TexturePool::*function)(const std::string &directory));

	void traverse_directory (const std::string &directory, void (TexturePool::*function)(const std::string &filename));

	void scan_directory (const std::string &directory);


	public:
		TexturePool ();
		virtual ~TexturePool ();
		static TexturePool *get_instance ();

		unsigned int get_number_of_textures ();

		QHash<uint32_t, boost::shared_ptr<Texture> > _tmp_textures;

		boost::optional<boost::shared_ptr<AbstractTexture> > get_texture (unsigned int index);

		/** if the index is -1, we simply add it to the end of the list */
		unsigned int add_image (const std::string &path, unsigned int index);
		unsigned int change_image (const std::string &path, unsigned int index);

		void delete_textures_at_id(uint32_t id);

		void loaded_tmp_texture(uint32_t id);


		uint32_t get_id();

		void update_texture(unsigned int index);

		void update_tmp_texture(uint32_t id, bool samep);

	signals:
		void texture_loaded (uint32_t id);
		void texture_changed (unsigned int index);

		void delete_texture (uint32_t id);
		void change_tmp_texture (uint32_t id, bool samep);
};

#endif
