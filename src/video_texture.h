#ifndef SCGRAPH_VIDEO_TEXTURE_HH
#define SCGRAPH_VIDEO_TEXTURE_HH

#include "texture.h"

extern "C"
{
#define __STDC_CONSTANT_MACROS // for UINT64_C
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

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
