#ifndef SCGRAPH_HVECTOR_3D_HH
#define SCGRAPH_HVECTOR_3D_HH

/** a homogenous 3D vector */
struct HVector3D
{
	HVector3D (const HVector3D& v)
	{
		for (unsigned int i = 0; i < 4; ++i)
			_c[i] = v._c[i];
	}

	HVector3D (float x = 0, float y = 0, float z = 0, float w = 1)
	{
		_c[0] = x;
		_c[1] = y;
		_c[2] = z;
		_c[3] = w;
	}

	float _c[4];
};

#endif
