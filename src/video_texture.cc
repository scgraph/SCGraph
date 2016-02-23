#include "video_texture.h"

#include "options.h"

#include "texture_pool.h"
#include "scgraph.h"


VideoTexture::VideoTexture () :
_tex_width(0),
_tex_height(0),
_last_frame(0)
{
    _queue = new VideoTextureQueue;
    _queue->_texture = this;
    _queue->startThread(true);
    
}

VideoTexture::~VideoTexture() {
    _player.close();
    _queue->stopThread();
}

bool VideoTexture::isVideo() {
    return true;
}

int VideoTexture::get_width() {
    return _player.getWidth();
}

int VideoTexture::get_height() {
    return _player.getHeight();
}


double VideoTexture::get_framerate() {
    return _player.getTotalNumFrames()/_player.getDuration();
}

uint32_t VideoTexture::get_num_frames() {
    return _player.getTotalNumFrames();
}

int VideoTexture::get_channels() {
    return 3;
}

int VideoTexture::load(const std::string &filename) {
    _filename = filename;
    
    // Open video file
    _player.setUseTexture(false);
    _player.load(filename);
    _player.setVolume(0);
    _player.play();
    _player.stop();
    
    if(_player.isLoaded()) {
        _tex_width =  (int)pow(2,(int)ceil(log2(_player.getWidth())));
        _tex_height = (int)pow(2,(int)ceil(log2(_player.getHeight())));
        std::cout << "[VideoTexture] Loaded video file " << _filename << std::endl;
        return 0;
    }
    else {
        std::cout << "[VideoTexture] Couldn't load video file " << _filename
        << std::endl;
        return 1;
    }
}

void VideoTexture::get_frame(uint32_t tex_id, uint32_t frame) {
    std::pair<uint32_t, uint32_t> i = make_pair(tex_id, frame);
    if(_player.isInitialized()) {
        std::cout << "[VideoTexture] queuing frame " << frame << std::endl;
        _queue->_frame_queue.send(i);
    }
    else {
        _player.update();
        std::cout << "[VideoTexture] player not initialized yet - not queuing frame " << frame << std::endl;
    }
}

void VideoTextureQueue::threadedFunction() {
    std::pair<uint32_t, uint32_t> incoming;
    std::pair<uint32_t, bool> outgoing;
    uint32_t texquad_id;
    uint32_t frame;
    
    TexturePool *tp = TexturePool::get_instance ();
    while(_frame_queue.receive(incoming)) {
        // only process last frame requested, probably other frames are not needed anymore
        if(_frame_queue.empty()) {
            std::cout << "[VideoTexture] started processing frame " << frame << std::endl;
        texquad_id = incoming.first;
        frame = incoming.second;
        
        _texture->_player.setFrame(std::min<unsigned int>(frame, _texture->_player.getTotalNumFrames()));
        _texture->_player.update();
        
        ofPixels pixels = _texture->_player.getPixels();
        lock();
        bool samep = false;
        if(tp->_tmp_textures.count(texquad_id) > 0
           && (tp->_tmp_textures.at(texquad_id)->_width == _texture->_tex_width)
           && (tp->_tmp_textures.at(texquad_id)->_height == _texture->_tex_height)) {
            samep = true;
        }
        else {
            boost::shared_ptr<Texture> tmp(new Texture (_texture->_tex_width,
                                                        _texture->_tex_height,
                                                        3, true));
            tp->_tmp_textures.insert({texquad_id, tmp});
        }
        
        boost::shared_ptr<Texture> t = tp->_tmp_textures.at(texquad_id);
        
        for (int i = 0; i < pixels.getWidth(); ++i)	{
            for (int j = 0; j < pixels.getHeight(); ++j)	{
                // swap image
                ofColor color = pixels.getColor(i,pixels.getHeight() - j - 1);
                
                int tmpIndex = 3 * (_texture->_tex_width * j + i);
                t->_data[tmpIndex]     = (unsigned char) color[0];
                t->_data[tmpIndex + 1] = (unsigned char) color[1];
                t->_data[tmpIndex + 2] = (unsigned char) color[2];
            }
        }
        
        std::cout << "[VideoTexture] processed frame " << frame << std::endl;
        
        outgoing = make_pair(texquad_id, samep);
        tp->change_tmp_texture.notify(this, outgoing);
        unlock();
        }
        else {
            std::cout << "[VideoTexture] skipped frame " << frame << std::endl;
            _texture->_player.update();
        }
    }
}
