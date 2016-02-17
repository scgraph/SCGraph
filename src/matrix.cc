#include "matrix.h"
#include <cmath>

Matrix::Matrix ()
{

}

Matrix::Matrix (const Matrix &m)
{
	for (int i = 0; i < 16; ++i)
		_coefficients[i] = m._coefficients[i];
}

float& Matrix::at (int i, int j)
{
	return _coefficients [i + 4*j];
}

float Matrix::at_c (int i, int j) const
{
	return _coefficients [i + j*4];
}

void Matrix::set_coefficient (int i, int j, float value)
{
	_coefficients [i + j*4] = value;
}

const float* Matrix::get_coefficients () const
{
	return _coefficients;
}

float* Matrix::get_coefficients ()
{
	return _coefficients;
}


void Matrix::set_axis_rotation (const Vector3D &axis, float angle)
{
	float cos_angle = cos (angle);
	float one_minus_cos_angle = (1.0 - cos_angle);
	float sin_angle = sin (angle);

	float x = axis._c[0];
	float x_squared = x * x;
	float y = axis._c[1];
	float y_squared = y * y;
	float z = axis._c[2];
	float z_squared = z * z;
	
	at(0,0) = cos_angle + one_minus_cos_angle * x_squared;
	at(0,1) = one_minus_cos_angle * x * y - sin_angle * z;
	at(0,2) = one_minus_cos_angle * x * z + sin_angle * y;
	at(0,3) = 0;

	at(1,0) = one_minus_cos_angle * y * x + sin_angle * z;
	at(1,1) = cos_angle + one_minus_cos_angle * y_squared;
	at(1,2) = one_minus_cos_angle * y * z - sin_angle * x;
	at(1,3) = 0;

	at(2,0) = one_minus_cos_angle * z * x - sin_angle * y;
	at(2,1) = one_minus_cos_angle * z * y + sin_angle * x;
	at(2,2) = cos_angle + one_minus_cos_angle * z_squared;
	at(2,3) = 0;

	at(3,0) = 0;
	at(3,1) = 0;
	at(3,2) = 0;
	at(3,3) = 1;
}

void Matrix::normalize ()
{
	for (int row = 0; row < 4; ++row)
	{
		float sum = 0;
		for (int col = 0; col < 4; ++col)
		{
			sum += at_c(row,col) * at_c(row,col);
		}
		sum = sqrt (sum);
		for (int col = 0; col < 4; ++col)
		{
			at(row,col) = at_c(row,col) / sum;
		}
	}
}

void Matrix::set_null ()
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			at (i,j) = 0;
		}
	}
}

void Matrix::set_identity ()
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			if (i != j)
				at (i,j) = 0;
			else
				at (i,j) = 1;
		}
	}
}

void Matrix::dump ()
{
	for (int i = 0; i < 4; ++i)
	{
		std::cout << "| ";
		for (int j = 0; j < 4; ++j)
		{
			std::cout << at (i,j) << " ";
		}
		std::cout << "|" << std::endl;
	}
	std::cout << std::endl;
}

void Matrix::add_translation (const Vector3D& vec)
{
	for (int i = 0; i < 3; ++i)
		at(i, 3) += vec._c[i];
}

void Matrix::add_translation (float x, float y, float z)
{
	at (0, 3) += x;
	at (1, 3) += y;
	at (2, 3) += z;
}

void Matrix::add_stretch (float x, float y, float z)
{
	at (0, 0) += x;
	at (1, 1) += y;
	at (2, 2) += z;
}

float Matrix::element_product (int i, int j, const Matrix &m)
{
	float sum = 0;

	for (int k = 0; k < 4; ++k)
		sum = sum + (at_c (i,k) * m.at_c (k,j));

	return sum;
}

/** multiply with argument and returns result */
Matrix Matrix::mul (const Matrix &m)
{
	Matrix tmp;
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			tmp.at (i,j) = element_product (i,j,m);
		}
	}
	return tmp;
}

void Matrix::mul_me (const Matrix &m)
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			at (i,j) = element_product (i,j,m);
		}
	}
}


