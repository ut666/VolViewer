/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef MATRIX4X4_H
#define MATRIX4X4_H

#include <iostream>
using namespace std;

#include "Vector.h"

class Matrix4x4
{
public:

	float m[16];

	Matrix4x4();
	~Matrix4x4();

	void identity(void);
	void inverse(void);
	void transpose(void);
	void find_rotation(Vector v1, Vector v2);
	void find_rotationfromaxis(Vector fromx, Vector fromy, Vector fromz, Vector tox, Vector toy, Vector toz);
	void find_rotation_from_euler(float anglex, float angley, float anglez);

	Matrix4x4& operator= (const Matrix4x4& m2);
	Matrix4x4& operator= (float m2[16]);
};

Matrix4x4  operator* (const Matrix4x4& m1, const Matrix4x4& m2 );
Vector	   operator* (const Matrix4x4& m1, const Vector& v );
Matrix4x4  operator+ (const Matrix4x4& m1, const Matrix4x4& m2 );
ostream&			operator<<(ostream& output, const Matrix4x4& m1);

#endif //MATRIX4X4_H