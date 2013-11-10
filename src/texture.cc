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

VideoTexture::VideoTexture () :
	_pFormatCtx(NULL),
	_videoStream(-1),
    _pCodecCtx(NULL),
	_pCodec(NULL),
	_tex_width(0),
	_tex_height(0),
	_last_frame(0),
	_ctxt(NULL)
{

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

int VideoTexture::load(const std::string &filename) {
	_filename = filename;

    AVFrame         *pFrame = NULL; 
    AVPacket        packet;
    int             frameFinished;

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

		// try to find the "best" stream
		_videoStream = av_find_best_stream(_pFormatCtx, AVMEDIA_TYPE_VIDEO, 
										   -1, -1, 
										   &_pCodec, NULL);

		if(_videoStream >= 0) {
			_pCodecCtx=_pFormatCtx->streams[_videoStream]->codec;
		}
		else {
			// if that failed, find the first video stream
			std::cout << "[VideoTexture] Find first stream for " 
					  << _filename 
					  << std::endl; 

			for(size_t i=0; i<_pFormatCtx->nb_streams; i++)
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
		}

		// Open codec
		if((err = avcodec_open2(_pCodecCtx, _pCodec, NULL))<0)
			return err;

		_tex_width =  (int)pow(2,(int)ceil(log2(_pCodecCtx->width)));
		_tex_height = (int)pow(2,(int)ceil(log2(_pCodecCtx->height)));
			
		return 0;
	}
}

void VideoTexture::processing_done() {
	_queue_mutex.lock();
	if(!_decode_queue.isEmpty()) {
		_queue_mutex.unlock();
		_future = QtConcurrent::run(this,
									&VideoTexture::really_get_frame);
	}
	else {
		_queue_mutex.unlock();
	}
}

void VideoTexture::get_frame(uint32_t tex_id, uint32_t frame) {
	_queue_mutex.lock();
	_decode_queue.enqueue(std::pair<uint32_t, uint32_t> (tex_id, frame));
	_queue_mutex.unlock();
	if(!_future.isRunning())
		processing_done();
}

int VideoTexture::really_get_frame()
{
	AVPacket        packet;
    int             frameFinished;
    AVFrame         *pFrame = NULL; 
	uint32_t texquad_id, frame;
	int frames = 0;
	int err = 0;

	for(;;) {
		_queue_mutex.lock();
		if(!_decode_queue.isEmpty()) {
			std::pair<uint32_t, uint32_t> tmp = _decode_queue.dequeue();
			_queue_mutex.unlock();
			texquad_id = tmp.first;
			frame = tmp.second;
		}
		else {
			_queue_mutex.unlock();
			break;
		}
		
		bool samep = false;
		TexturePool *tp = TexturePool::get_instance ();
		if(tp->_tmp_textures.contains(texquad_id) 
		   && (tp->_tmp_textures.value(texquad_id)->_width == _tex_width)
		   && (tp->_tmp_textures.value(texquad_id)->_height == _tex_height)) {
			samep = true;
		}
		else {
			boost::shared_ptr<Texture> tmp(new Texture (_tex_width, 
													  _tex_height,
													  3, true));
			tp->_tmp_textures.insert(texquad_id, tmp);
		}

		boost::shared_ptr<Texture> t = tp->_tmp_textures.value(texquad_id);

		if(_last_frame != (frame - 1)) {
			err = avformat_seek_file(_pFormatCtx, _videoStream, 
									 frame, frame, frame + 10,
									 AVSEEK_FLAG_FRAME);
			if(err >= 0) {
				avcodec_flush_buffers(_pCodecCtx); }
			else {
				std::cout << "[VideoTexture] Error while seeking " 
						  << frame << " error: " << err <<  "." << std::endl;
				TexturePool::get_instance()->update_tmp_texture(texquad_id, samep);
			}
		}

		_last_frame = frame;
		
		if(err >= 0) {
			// Allocate video frame
			pFrame=avcodec_alloc_frame();

			// Read frame and save it as Texture
			frames = 0;
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
						err = avpicture_alloc(&pic, PIX_FMT_RGB24, 
											  pFrame->width, pFrame->height);
						if(err < 0) {
							std::cout << "[VideoTexture] allocation failed" 
									  << std::endl;
							TexturePool::get_instance()->update_tmp_texture(texquad_id, samep);
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
											0, pFrame->height, 
											pic.data, pic.linesize)) != 0) {
							if(samep)
								t->zero();
						
							for (int i = 0; i < pFrame->width; ++i) {
								for(int j = 0; j < pFrame->height; ++j) {
									int tmpIndex = 3 * (_tex_width * j + i);
									int picIndex = 3 * (pFrame->width 
														* (pFrame->height - j - 1) + i);
									t->_data[tmpIndex] = *(pic.data[0] + picIndex);
									t->_data[tmpIndex+1] = *(pic.data[0] + picIndex + 1);
									t->_data[tmpIndex+2] = *(pic.data[0] + picIndex + 2);
								}
							}

							//std::cout << "[Texture] processed frame " << frame << std::endl;

							TexturePool::get_instance()->update_tmp_texture(texquad_id, samep);
						}
						else {
							std::cout << "[VideoTexture] Couldn't process frame " 
									  << frames << " error: " << err <<  "." 
									  << std::endl;
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
	}
	processing_done();
	return 0;
}

