#pragma once

#include "ofMain.h"

#include "ofxOsc.h"

class ofApp : public ofBaseApp {

    ofxOscReceiver _receiver;
    
	public:
		void setup();
		void update();
		void draw();
};
