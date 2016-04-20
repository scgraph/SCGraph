#include "ofApp.h"
#include "scgraph.h"
#include "options.h"
#include "osc_handler.h"
#include "texture_pool.h"

//--------------------------------------------------------------
void ofApp::setup(){
    Options *options = Options::get_instance ();
    ofSetEscapeQuitsApp(false);
    
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

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
 
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
