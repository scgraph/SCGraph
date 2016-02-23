#include "texture.h"
#include "ofThread.h"
#include "ofEvent.h"
#include "ofVideoPlayer.h"
#include "ofThreadChannel.h"

class VideoTextureQueue;

class VideoTexture : public AbstractTexture {

	std::string _filename;
	uint32_t _last_frame;

 public:
	VideoTexture();
	~VideoTexture();
	int get_width();
	int get_height();
	int get_channels();
	double get_framerate();
 
    int _tex_width;
    int _tex_height;
    
    ofVideoPlayer _player;
    
    VideoTextureQueue* _queue;
    
	uint32_t get_num_frames();

	int load(const std::string &filename);
	void get_frame(uint32_t tex_id, uint32_t frame);

	bool isVideo();
    
    ofEvent<std::pair<uint32_t, boost::shared_ptr<Texture> > > update_tmp_texture;
};

class VideoTextureQueue : public ofThread {
    
    public:
        void threadedFunction();
    
        VideoTexture* _texture;
    
        ofThreadChannel<std::pair<uint32_t, uint32_t> > _frame_queue;
};
