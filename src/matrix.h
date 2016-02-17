#ifndef SCGRAPH_MATRIX_HH
#define SCGRAPH_MATRIX_HH

#include "vector_3d.h"

#include <vector>
#include <iostream>


/** a homogenous 4 x 4 Matrix */
class Matrix
{
	float _coefficients[4*4];

	public:
		Matrix ();

		Matrix (const Matrix &m);

		float& at (int i, int j);

		float at_c (int i, int j) const;

		void set_coefficient (int i, int j, float value);

		const float* get_coefficients () const;

		float* get_coefficients ();

		void set_null ();

		void set_identity ();

		void set_axis_rotation (const Vector3D &axis, float angle);

		void dump ();

		void add_translation (const Vector3D& vec);

		void add_translation (float x, float y, float z);

		void add_stretch (float x, float y, float z);

		float element_product (int i, int j, const Matrix &m);

		void normalize ();

		/** multiply with argument and returns result */
		Matrix mul (const Matrix &m);

		void mul_me (const Matrix &m);

};

#endif
