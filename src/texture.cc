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
			int im_width, im_height, tex_width, tex_height;

			if(image.isNull()) {
				std::cout 
					<< "  [TexturePool]: Unable to convert image to GL format."
					<< "No texture loaded." 
					<< std::endl;
				return -1;
			}
			else {		
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

VideoTexture::VideoTexture () {
}

VideoTexture::~VideoTexture() {
	// Close the codec
	if(_pCodecCtx != NULL)
		avcodec_close(_pCodecCtx);

	// Close the video file
	if(_pFormatCtx != NULL)
		avformat_close_input(&_pFormatCtx);
}

bool VideoTexture::isVideo() {
	return true;
}

int VideoTexture::get_width() {
	if(_pCodecCtx != NULL)
		return _pCodecCtx->width;
	else
		return -1;
}

int VideoTexture::get_height() {
	if(_pCodecCtx != NULL)
		return _pCodecCtx->height;
	else
		return -1;
}


double VideoTexture::get_framerate() {
	if(_pCodecCtx != NULL)
		return (double) (((double) _pCodecCtx->time_base.num)
						/ ((double) _pCodecCtx->time_base.den));
	else
		return -1;
}

uint32_t VideoTexture::get_num_frames() {
	if(_pFormatCtx != NULL)
		// this is probably very rough
		return (uint32_t) _pFormatCtx->duration;
	else
		return 0;
}

int VideoTexture::get_channels() {
	return 3;
}

void VideoTexture::get_frame(uint32_t tex_id, uint32_t frame) {
	QFuture<void> future = QtConcurrent::run(this,
											 &VideoTexture::really_get_frame,
											 tex_id, frame);
}


int VideoTexture::load(const std::string &filename) {
	_filename = filename;
	Options *options = Options::get_instance ();

	_pFormatCtx = NULL;
	int             i;
    _pCodecCtx = NULL;
    _pCodec = NULL;
	_ctxt = NULL;
    AVFrame         *pFrame = NULL; 
    AVPacket        packet;
    int             frameFinished;
    int             numBytes;

	int tex_width, tex_height;

	int err = 0;

    // Open video file
	if((err = avformat_open_input(&_pFormatCtx, _filename.c_str(), NULL, NULL)) != 0)	{
		std::cout << "[VideoTexture] Couldn't load video file " << _filename 
				  << ". Error code " << err << std::endl;
		return err;
	}
	else {
		// Retrieve stream information
		if((err = avformat_find_stream_info(_pFormatCtx, NULL))<0) 
			return err; // Couldn't find stream information

		// Dump information about file onto standard error
		// TODO make this verbose
		av_dump_format(_pFormatCtx, 0, _filename.c_str(), false);

		// Find the first video stream
		_videoStream=-1;
		for(i=0; i<_pFormatCtx->nb_streams; i++)
			if(_pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
				_videoStream=i;
				break;
			}

		if(_videoStream==-1)
			return -1; // Didn't find a video stream
		// Get a pointer to the codec context for the video stream
		_pCodecCtx=_pFormatCtx->streams[_videoStream]->codec;

		// Find the decoder for the video stream
		_pCodec=avcodec_find_decoder(_pCodecCtx->codec_id);
		if(_pCodec==NULL)
			return -1; // Codec not found

		// Open codec
		if((err = avcodec_open2(_pCodecCtx, _pCodec, NULL))<0)
			return err;

		_tex_width =  (int)pow(2,(int)ceil(log2(_pCodecCtx->width)));
		_tex_height = (int)pow(2,(int)ceil(log2(_pCodecCtx->height)));
			
		return 0;
	}
}

int VideoTexture::really_get_frame(uint32_t texquad_id, uint32_t frame)
{
	AVPacket        packet;
    int             frameFinished;
    int             numBytes;
    AVFrame         *pFrame = NULL; 

	int err = 0;

	if((err = avformat_seek_file(_pFormatCtx, _videoStream, 
								 frame, frame, frame + 10,
								 AVSEEK_FLAG_FRAME)) < 0) {
		std::cout << "[VideoTexture] Error while seeking " 
				  << frame << " error: " << err <<  "." << std::endl;
		return -1;
	} else {

		avcodec_flush_buffers(_pCodecCtx);

		// Allocate video frame
		pFrame=avcodec_alloc_frame();

  		// Read frame and save it as Texture
		int frames = 0;
		while((av_read_frame(_pFormatCtx, &packet)==0) && (frames < 1)) {
			// Is this a packet from the video stream?
			if(packet.stream_index==_videoStream) {
				// Decode video frame
				err = avcodec_decode_video2(_pCodecCtx, pFrame, &frameFinished, 
											&packet);

				// Did we get a video frame?
				if((err > 0) && frameFinished) {
					frames++;

					AVPicture pic;
					err = avpicture_alloc(&pic, PIX_FMT_RGB24, pFrame->width, pFrame->height);
					if(err < 0) {
						std::cout << "[VideoTexture] allocation failed" << std::endl;
						return -1;
					}
				
					_ctxt = 
						sws_getCachedContext(_ctxt,
											 pFrame->width, pFrame->height, 
											 static_cast<enum PixelFormat>(pFrame->format), 
											 pFrame->width, pFrame->height,
											 PIX_FMT_RGB24, 
											 SWS_BILINEAR,
											 NULL, NULL, NULL);

					if (_ctxt == NULL) {
						std::cout << "[VideoTexture] Error while calling sws_getContext" << std::endl;
					}

					// Convert the image from its native
					// format to RGB

					if((err = sws_scale(_ctxt, pFrame->data, pFrame->linesize, 
										0, pFrame->height, pic.data, pic.linesize)) != 0) {
						boost::shared_ptr<Texture> t(new Texture (_tex_width, _tex_height,
																  3, true));

						for (int i = 0; i < pFrame->width; ++i) {
							for(int j = 0; j < pFrame->height; ++j) {
								int tmpIndex = 3 * (_tex_width * j + i);
								int picIndex = 3 * (pFrame->width * (pFrame->height - j - 1) + i);
								t->_data[tmpIndex] = *(pic.data[0] + picIndex);
								t->_data[tmpIndex+1] = *(pic.data[0] + picIndex + 1);
								t->_data[tmpIndex+2] = *(pic.data[0] + picIndex + 2);
							}
						}
						//std::cout << "[Texture] processed frame " << frame << std::endl;

						TexturePool::get_instance ()->update_tmp_texture(texquad_id, t);
					}
					else {
						std::cout << "[VideoTexture] Couldn't process frame " 
								  << frames << " error: " << err <<  "." << std::endl;
					}
					avpicture_free(&pic);
				}
			}
			// Free the packet that was allocated by av_read_frame
			av_free_packet(&packet);
		}
		// Free the YUV frame
		if(pFrame != NULL)
			av_free(pFrame);
	}
	return 0;
}


