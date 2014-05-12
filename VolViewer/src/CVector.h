/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef CVector_H
#define CVector_H

#define USE_GLVERTEX

#ifdef USE_GLVERTEX
	#ifdef win32
		#include <windows.h>
	#endif
#include "gl.h"
#endif

class CVector
{
public:
private:
	double value_[4];
	double texcoord_[3];
	double color_[4];

public:
	CVector(
		);

	CVector(
		const CVector& vector_in
		);

	CVector(
		const double& a_in,
		const double& b_in,
		const double& c_in,
		const double& d_in = 1.0
		);
	CVector(
		const double& a_in,
		const double& b_in,
		const double& c_in,
		const double& d_in,
		const double& s_in,
		const double& t_in,
		const double& r_in
		);
	CVector(
		const double& a_in,
		const double& b_in,
		const double& c_in,
		const double& d_in,
		const double& s_in,
		const double& t_in,
		const double& r_in,
		const double& R_in,
		const double& G_in,
		const double& B_in,
		const double& A_in
		);
	CVector(
		const double& a_in,
		const double& b_in,
		const double& c_in,
		const double& d_in,
		const double& R_in,
		const double& G_in,
		const double& B_in,
		const double& A_in
		);

	CVector&
	operator=(
		const CVector& vector_in
		);

	double&
	operator[](
		const unsigned short index_in
		);

	double
	operator[](
		const unsigned short index_in
		) const;

	double&
	operator()(
		const unsigned short index_in
		);

	double
	operator()(
		const unsigned short index_in
		) const;

	void
	set(
		const double& a_in, 
		const double& b_in,
		const double& c_in,
		const double& d_in
		);

	void
	get(
		double& a_out, 
		double& b_out,
		double& c_out,
		double& d_out
		) const;

	bool
	operator==(
		const CVector& vector_in
		) const;

	bool
	operator!=(
		const CVector& vector_in
		) const;

	CVector
	operator-(
		) const;

	friend CVector
	operator-(
		const CVector& vector0_in,
		const CVector& vector1_in
		);

	friend CVector
	operator+(
		const CVector& vector0_in,
		const CVector& vector1_in
		);

	friend double
	operator*(
		const CVector& vector0_in,
		const CVector& vector1_in
		);

	double
	getInnerProduct(
		const CVector& vector_in
		) const;
   
	friend CVector
	operator*(
		const double& scalar_in,
		const CVector& vector_in
		);

	friend CVector
	operator*(
		const CVector& vector_in,
		const double& scalar_in
		);

	friend CVector
	operator/(
		const CVector& vector_in,
		const double& scalar_in
		);

	CVector&
	operator+=(
		const CVector& vector_in
		);

	CVector&
	operator-=(
		const CVector& vector_in
		);

	CVector&
	operator*=(
		const double& scalar_in
		);

	CVector&
	operator/=(
		const double& scalar_in
		);

	double
	getLength(
		) const;

	void
	normalize(
		);

	bool
	isNormalized(
		) const;

	bool
	isNil(
		) const;

#ifdef USE_GLVERTEX
	inline void
	glVertex(bool bColor = true, bool bTexCoord = true) const{
	if (bColor) glColor4d(color_[0], color_[1], color_[2], color_[3]);
	if (bTexCoord) glTexCoord3d(texcoord_[0], texcoord_[1], texcoord_[2]);
	glVertex4d(value_[0],value_[1],value_[2],value_[3]);
}
#endif
};


#endif // CVector_H
