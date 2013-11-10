#ifndef SCGRAPH_TEXTURE_HH
#define SCGRAPH_TEXTURE_HH

#include "boost/shared_ptr.hpp"
#include "boost/optional.hpp"

#include "plugins/quad.h"

#include <QtCore/QObject>
#include <QtCore/QHash>
#include <QtCore/QQueue>
#include <QtGui/QImage>
#include <QtGui/QImageReader>
#include <QtGui/QColor>


#include <QtCore/QFuture>
#include <QtCore/QtConcurrentRun>		

extern "C"
{
#define __STDC_CONSTANT_MACROS // for UINT64_C
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}


class Texture
{
 public: 
	Texture (int width, int height, int channels, bool zero);
	~Texture ();

	int _width, _height, _channels;

	void set_pixel(unsigned int pindex, int32_t color);
	void fill(uint32_t starting_index, uint32_t num_samples, int32_t color);


	unsigned char *_data;

	void zero();
};


class AbstractTexture
{

 public:
	AbstractTexture();
	virtual ~AbstractTexture();

	boost::shared_ptr<Texture> _texture;


	virtual void get_frame(uint32_t tex_id, uint32_t frame);
	//virtual void really_get_frame(uint32_t tex_id, uint32_t frame);
	virtual int get_width();
	virtual int get_height();
	virtual int get_channels();

	virtual double get_framerate();
	virtual uint32_t get_num_frames();


	virtual unsigned char * get_data();
	
	virtual bool isVideo();

	virtual void zero();
	virtual void set_pixel(unsigned int pindex, int32_t color);
	virtual void fill(uint32_t starting_index, uint32_t num_samples, int32_t color);
	virtual int load(const std::string &filename);
};


class ImageTexture : public AbstractTexture {

	std::string _filename;

 public:
	ImageTexture();
	~ImageTexture();

	int get_width();
	int get_height();
	int get_channels();

	void fill(uint32_t starting_index, uint32_t num_samples, int32_t color);
	void zero();
	void set_pixel(unsigned int pindex, int32_t color);
	unsigned char * get_data();
	int load(const std::string &filename);
};

class VideoTexture : public QObject, public AbstractTexture {

	Q_OBJECT

	std::string _filename;

	AVFormatContext *_pFormatCtx;
	int             _videoStream;
	AVCodecContext  *_pCodecCtx;
	AVCodec         *_pCodec;
	int _tex_width;
	int _tex_height;

	uint32_t _last_frame;

	QFuture<int> _future;
	QQueue<std::pair <uint32_t, uint32_t> >  _decode_queue;

	SwsContext *_ctxt;

	void processing_done();
 public:
	VideoTexture();
	~VideoTexture();
	int get_width();
	int get_height();
	int get_channels();
	double get_framerate();
 
	QMutex _queue_mutex;
	uint32_t get_num_frames();

	int load(const std::string &filename);
	void get_frame(uint32_t tex_id, uint32_t frame);
	int really_get_frame();

	bool isVideo();

 signals:
	void update_tmp_texture(uint32_t id, boost::shared_ptr<Texture> texture);
};

#endif
