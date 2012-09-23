#ifndef SCGRAPH_TEXTURE_POOL_HH
#define SCGRAPH_TEXTURE_POOL_HH

#include "boost/shared_ptr.hpp"
#include <vector>
#include <string>

#include <QtCore/QObject>
#include <QtGui/QImage>
#include <QtGui/QImageReader>
#include <QtGui/QColor>
#include <QtOpenGL/QGLWidget>

/** a texture image in RGBA format with unsigned chars as data */
struct Texture
{
	Texture (int width, int height, int channels, bool zero);
	~Texture ();

	int _width, _height, _channels;
	unsigned char *_data;
	QImage _img;

	void set_pixel(unsigned int pindex, int32_t color);
	void fill(uint32_t starting_index, uint32_t num_samples, int32_t color);

	void zero();
};

class TexturePool : public QObject
{
	Q_OBJECT

	static TexturePool *_instance;
	std::vector <boost::shared_ptr<Texture> > _textures;

	boost::shared_ptr<Texture> load_image (const std::string &filename);

	void add_image (const std::string &filename);

	void tokenize_path (const std::string &path, void (TexturePool::*function)(const std::string &directory));

	void traverse_directory (const std::string &directory, void (TexturePool::*function)(const std::string &filename));

	void scan_directory (const std::string &directory);


	public:
		TexturePool ();
		virtual ~TexturePool ();
		static TexturePool *get_instance ();

		unsigned int get_number_of_textures ();

		boost::shared_ptr<Texture> get_texture (unsigned int index);

		/** if the index is -1, we simply add it to the end of the list */
		unsigned int add_image (const std::string &path, unsigned int index);
		unsigned int change_image (const std::string &path, unsigned int index);

		void update();

	signals:
		void texture_added (unsigned int index);
		void textures_changed ();
};

#endif
