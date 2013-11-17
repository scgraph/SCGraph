#include "texture.h"

#include "options.h"

#include "texture_pool.h"


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

AbstractTexture::AbstractTexture() 
{ }

AbstractTexture::~AbstractTexture() 
{ }

unsigned char * AbstractTexture::get_data() {
	return NULL;
}

int AbstractTexture::get_width() {
	return 0;
}

int AbstractTexture::get_height() {
	return 0;
}

int AbstractTexture::get_channels() {
	return 0;
}


double AbstractTexture::get_framerate() {
	return 0;
}


uint32_t AbstractTexture::get_num_frames() {
	return 1;
}

void AbstractTexture::fill (uint32_t starting_index, uint32_t num_samples, int32_t color)
{

}

int AbstractTexture::load(const std::string &filename) {
	return -1;
}

void AbstractTexture::zero () {

}

bool AbstractTexture::isVideo() {
	return false;
}

void AbstractTexture::get_frame(uint32_t tex_id, uint32_t frame) {
}

void AbstractTexture::set_pixel(unsigned int pindex, int32_t color) {
}

ImageTexture::ImageTexture () {
}

ImageTexture::~ImageTexture() {
}

unsigned char * ImageTexture::get_data() {
	return _texture->_data;
}

int ImageTexture::get_width() {
	return _texture->_width;
}

int ImageTexture::get_height() {
	return _texture->_height;
}

int ImageTexture::get_channels() {
	return _texture->_channels;
}

void ImageTexture::fill (uint32_t starting_index, uint32_t num_samples, int32_t color) {
	_texture->fill( starting_index,  num_samples,  color);
}

void ImageTexture::zero () {
	_texture->zero();
}

void ImageTexture::set_pixel(unsigned int pindex, int32_t color) {
	_texture->set_pixel( pindex,  color);
}

int ImageTexture::load(const std::string &filename) {
	_filename = filename;
	Options *options = Options::get_instance ();

	try {

		const char * tmp = _filename.c_str();
		QImage image(tmp);

		if(image.isNull()) {
			// TODO put this somewhere else
			std::cout << "  [TexturePool]: Unrecognized Image Format. No texture loaded." 
					  << std::endl;
			std::cout << "                 Supported Formats:";

			for (int i = 0; i < QImageReader::supportedImageFormats().size(); ++i)
				std::cout << " " << QImageReader::supportedImageFormats().at(i).data();

			std::cout << "." << std::endl;
			return -1;
		}
		else {
			if(image.isNull()) {
				std::cout 
					<< "  [TexturePool]: Unable to convert image to GL format."
					<< "No texture loaded." 
					<< std::endl;
				return -1;
			}
			else {		
				int im_width, im_height, tex_width, tex_height;

				im_width = image.width();
				im_height = image.height();

				tex_width =  (int)pow(2,(int)ceil(log2(im_width)));
				tex_height = (int)pow(2,(int)ceil(log2(im_height)));

				boost::shared_ptr<Texture> t(new Texture (tex_width, tex_height, 4));

				if (options->_verbose >= 2)	{
					std::cout << "  [TexturePool]: Texture Width/Height: " << tex_width 
							  << "/" << tex_height << std::endl;
					std::cout << "  [TexturePool]: Image source Width/Height: " << im_width 
							  << "/" << im_height << std::endl;
				}

				// std::cout << width << " " << height << std::endl;
	
				for (int i = 0; i < im_width; ++i)	{
					for (int j = 0; j < im_height; ++j)	{
						// swap image
						QRgb color = image.pixel(i,im_height - j - 1);

						int tmpIndex = 4 * (tex_width * j + i);
						t->_data[tmpIndex]     = (unsigned char) qRed(color);
						t->_data[tmpIndex + 1] = (unsigned char) qGreen(color);
						t->_data[tmpIndex + 2] = (unsigned char) qBlue(color);
						t->_data[tmpIndex + 3] = (unsigned char) qAlpha(color);
					}
				}
				_texture = t;
				return 0;
			}
		}
	}
	catch (const char* error)
		{
			std::cout << "[TexturePool]: Problem loading texture: " 
					  << error  << std::endl;
		}
	return -1;
}
