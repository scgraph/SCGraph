#include "ofApp.h"
#include "scgraph.h"
//#include "graphic_loop.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetEscapeQuitsApp(false);
}

//--------------------------------------------------------------
void ofApp::update(){
    //std::cout << "update" << std::endl;
    ScGraph::get_instance()->_control_loop.threadedFunction();
    ScGraph::get_instance()->_graphic_loop.threadedFunction();
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
