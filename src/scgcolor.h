#ifndef SCGRAPH_COLORRGBA_HH
#define SCGRAPH_COLORRGBA_HH

#include "ofColor.h"

class scgColor : public ofColor
{
    using ofColor::ofColor;
    
    public:
        void scale_alpha(float factor);
};

#endif
