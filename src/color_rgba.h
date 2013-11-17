#ifndef SCGRAPH_COLORRGBA_HH
#define SCGRAPH_COLORRGBA_HH

/** RED GREEN BLUE ALPHA */
struct ColorRGBA
{
	ColorRGBA (float r = 1, float g = 1, float b = 1, float a = 1);

	float _c[4];

	void scale_alpha(float factor);
};

#endif
