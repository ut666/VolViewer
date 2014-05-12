/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <iostream>

#include <math.h>

#include "MyMath.h"

class Vector
{
	public:
		
		float x, y, z, w;
		float l;
		
		//constructors
		Vector();
		~Vector();
		Vector(int v);
		Vector(const Vector& rhs);
		Vector(float x0, float y0);
		Vector(float x0, float y0, float z0);
		Vector(float x0, float y0, float z0, float w0);
		
		void		set_values(float x0, float y0, float z0);
		
		//utility functions
		int			is_zero(void);
		int			is_unit(void);
		void		get_length(void);
		float		length(void);
		void		normalize(void);
		void		cross(const Vector& v1, const Vector& v2);
		float		dot(const Vector& rhs);
		Vector		mult_matrix(const float mat[16]);
		Vector		mult_matrix9(const float mat[9]);
		Vector		unit(void) const;
		float		&operator[](int i);	
		void		linear_interpolation(const Vector& v1, const Vector& v2, float step, float duration );
		int			is_parallel(const Vector& rhs);		
		Vector		get_tangent(const Vector& v);
		void		get_normal(const Vector& v0, const Vector& v1, const Vector& v2);
		void		round(int dp);
		void		build_rotatemat(Vector axis, float angle, float* rot_mat);
		void		rotate3D(Vector axis, float angle);
		float		signed_angle(Vector v2, Vector reference);

		float		distance_to_line(Vector v0, Vector v1);

		//Overloaded Operators
		//ostream&			operator<<(ostream& Vector);
		int					operator==( Vector);
		int					operator!=( Vector);
		Vector				operator-();                // unary minus
		Vector				operator~();                // unary 2D perp operator
		int					operator<(const Vector& b);
		int					operator>(const Vector& b);
		Vector&				operator= (const Vector& rhs);
		Vector&				operator= (const float& rhs);
		Vector				operator+( Vector);        // vector add
		Vector				operator-( Vector);        // vector subtract
		Vector				operator^( Vector);        // 3D exterior cross product
		Vector&				operator*=( float);      // vector scalar mult
		Vector&				operator/=( float);      // vector scalar div
		Vector&				operator+=( Vector);      // vector increment
		Vector&				operator-=( Vector);      // vector decrement
		Vector&				operator^=( Vector);      // 3D exterior cross product
		float				operator|( Vector);        // 2D exterior perp product
		//double			operator*( Vector);        // inner dot product
};
		void				RotateVector (float Data[16], Vector &V, Vector &D);
		Vector				operator* (const Vector& v1, const Vector& v2 );
		Vector				operator/ (Vector& v1, Vector& v2 );
		Vector				operator+ (const Vector& v1, const Vector& v2 );
		Vector				operator- (const Vector& v1, const Vector& v2 );
		Vector				operator*( int, Vector);
		Vector				operator*( float, Vector);
		Vector				operator*( Vector, int);
		Vector				operator*( Vector, float);
		Vector				operator/( Vector, int);
		Vector				operator/( Vector, float);
		int					operator<(const Vector& a, const Vector& b);
		int					operator>(const Vector& a, const Vector& b);

#endif // VECTOR_H