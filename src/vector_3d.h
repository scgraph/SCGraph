#ifndef SCGRAPH_VECTOR_3D_HH
#define SCGRAPH_VECTOR_3D_HH

#include <cmath>

struct Vector3D
{
	Vector3D (const Vector3D& v)
	{
		for (unsigned int i = 0; i < 3; ++i)
			_c[i] = v._c[i];
	}

	Vector3D (float x = 0, float y = 0, float z = 0)
	{
		_c[0] = x;
		_c[1] = y;
		_c[2] = z;
	}

	float _c[3];

	void normalize ()
	{
		float length = sqrt (_c[0] * _c[0] + _c[1] * _c[1] + _c[2] * _c[2]);

		_c[0] = _c[0] / length;
		_c[1] = _c[1] / length;
		_c[2] = _c[2] / length;
	}

	void mul (float m)
	{
		_c[0] = _c[0] * m;
		_c[1] = _c[1] * m;
		_c[2] = _c[2] * m;
		
	}

	/** cross multiply this vector with v and return result */
	Vector3D cross_product (const Vector3D &v)
	{
		Vector3D ret 
		(
			_c[1] * v._c[2] - _c[2] * v._c[1],
			_c[2] * v._c[0] - _c[0] * v._c[2],
			_c[0] * v._c[1] - _c[1] * v._c[0]
		);
		return ret;
	}
};

#endif
