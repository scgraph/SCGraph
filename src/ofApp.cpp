#include "ofApp.h"
#include "scgraph.h"
#include "options.h"
#include "osc_handler.h"
#include "texture_pool.h"

//--------------------------------------------------------------
void ofApp::setup(){
    Options *options = Options::get_instance ();
    ofSetEscapeQuitsApp(false);
    
    // need to set frame rate in windowless setup
    // otherwise it loops as fast as possible
    ofSetFrameRate(30);
    
#ifdef OFXOSC
    _receiver.setup(options->_port);
#endif
}

//--------------------------------------------------------------
void ofApp::update(){
    
    ScGraph* scgraph = ScGraph::get_instance();
    //std::cout << "update" << std::endl;
#ifdef OFXOSC
    
    while(_receiver.hasWaitingMessages()) {
        ofxOscMessage msg;
        if(_receiver.getNextMessage(msg)) {
            scgraph->_osc_handler.handle_message_of(&msg);
        };
    };
#endif

#ifndef OFXOSC
    OscMessage *msg;
    while(scgraph->_osc_handler._channel.tryReceive(msg)) {
        scgraph->_osc_handler.handle_message(msg);
    }
#endif
    
    scgraph->_control_loop.threadedFunction();
    scgraph->_graphic_loop.threadedFunction();
    //std::cout << "update done" << std::endl;
}


//--------------------------------------------------------------
void ofApp::draw(){
}

