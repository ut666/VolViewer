/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QUATERNION_H
#define QUATERNION_H

#include <math.h>

#include "Vector.h"
#include "Matrix.h"

class Quaternion  
{
public:

	Vector quat;

	Quaternion();
	Quaternion(Vector v, double rho);
	~Quaternion();

	void from_matrix(Matrix3f_t *src);
	void from_angle(Vector pos, float degrees);

	void to_matrix(Matrix3f_t *dst);
	void to_euler(Vector* v);

	Vector apply(const Vector& v) const;
	Quaternion operator *(Quaternion q);
};

void slerp(Quaternion* from, Quaternion * to, float t, Quaternion* res);

#endif	//QUATERNION_H