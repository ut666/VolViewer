/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */

#include "Matrix4x4.h"

Matrix4x4::Matrix4x4()
{
	for(int i=0; i<16; i++)
		m[i] = 0;
}
Matrix4x4::~Matrix4x4()
{

}
void Matrix4x4::inverse(void)
{

}
void Matrix4x4::identity(void)
{
	for(int i=0; i<16; i++)
		m[i] = 0;

	m[0] = 1;
	m[5] = 1;
	m[10] = 1;
	m[15] = 1;
}
void Matrix4x4::transpose(void)
{
	Matrix4x4 m1;
	m1=*this;

	this->m[0] = m1.m[0];
	this->m[1] = m1.m[4];
	this->m[2] = m1.m[8];
	this->m[3] = m1.m[12];
	this->m[4] = m1.m[1];
	this->m[5] = m1.m[5];
	this->m[6] = m1.m[9];
	this->m[7] = m1.m[13];
	this->m[8] = m1.m[2];
	this->m[9] = m1.m[6];
	this->m[10] = m1.m[10];
	this->m[11] = m1.m[14];
	this->m[12] = m1.m[3];
	this->m[13] = m1.m[7];
	this->m[14] = m1.m[11];
	this->m[15] = m1.m[15];
}
void Matrix4x4::find_rotationfromaxis(Vector fromx, Vector fromy, Vector fromz, Vector tox, Vector toy, Vector toz)
{
	Matrix4x4 to, from, rot;

	//TO
	to.m[0] = tox.x;
	to.m[1] = toy.x; 
	to.m[2] = toz.x;
	to.m[3] = 0;
	to.m[4] = tox.y;
	to.m[5] = toy.y;
	to.m[6] = toz.y;
	to.m[7] = 0;
	to.m[8] = tox.z;
	to.m[9] = toy.z;
	to.m[10] = toz.z;
	to.m[11] = 0;
	to.m[12] = 0;
	to.m[13] = 0;
	to.m[14] = 0;
	to.m[15] = 1;

	//FROM
	from.m[0] = fromx.x;
	from.m[1] = fromy.x; 
	from.m[2] =	fromz.x;
	from.m[3] = 0;
	from.m[4] = fromx.y;
	from.m[5] = fromy.y;
	from.m[6] = fromz.y;
	from.m[7] = 0;
	from.m[8] = fromx.z;
	from.m[9] = fromy.z;
	from.m[10] = fromz.z;
	from.m[11] = 0;
	from.m[12] = 0;
	from.m[13] = 0;
	from.m[14] = 0;
	from.m[15] = 1;

	from.transpose();

	rot = to*from;

	*this = rot;
}

void Matrix4x4::find_rotation(Vector u, Vector v)
{
	if(u==v) return;

	u.normalize();
	v.normalize();

	Vector n,vxn,uxn;

	n.cross(v, u);
	vxn.cross(v, n);
	uxn.cross(u, n);

	Matrix4x4 to, from, rot;

	//TO
	to.m[0] = v.x;
	to.m[1] = n.x; 
	to.m[2] = vxn.x;
	to.m[3] = 0;

	to.m[4] = v.y;
	to.m[5] = n.y;
	to.m[6] = vxn.y;
	to.m[7] = 0;
	
	to.m[8] = v.z;
	to.m[9] = n.z;
	to.m[10] = vxn.z;
	to.m[11] = 0;
	
	to.m[12] = 0;
	to.m[13] = 0;
	to.m[14] = 0;
	to.m[15] = 1;

	//FROM
	from.m[0] = u.x;
	from.m[1] = n.x; 
	from.m[2] = uxn.x;
	from.m[3] = 0;
	from.m[4] = u.y;
	from.m[5] = n.y;
	from.m[6] = uxn.y;
	from.m[7] = 0;
	from.m[8] = u.z;
	from.m[9] = n.z;
	from.m[10] = uxn.z;
	from.m[11] = 0;
	from.m[12] = 0;
	from.m[13] = 0;
	from.m[14] = 0;
	from.m[15] = 1;

	from.transpose();

//	cout<<to<<endl;
//	cout<<from<<endl;

	rot = to*from;


	*this = rot;
}

void Matrix4x4::find_rotation_from_euler(float anglex, float angley, float anglez)
{

	anglex *= PI / 180.0;
	angley *= PI / 180.0;
	anglez *= PI / 180.0;

	float cd = cos(anglex);
	float co = cos(angley);
	float cw = cos(anglez);

	float sd = sin(anglex);
	float so = sin(angley);
	float sw = sin(anglez);

	m[0] = co*cw;
	m[1] = sd*so*cw+cd*sw;
	m[2] = -cd*so*cw+sd*sw;
	m[3] = 0;

	m[4] = -co*sw;
	m[5] = -sd*so*sw+cd*cw;
	m[6] = cd*so*sw+sd*cw;
	m[7] = 0;
	
	m[8]  = so;
	m[9]  = -sd*co;
	m[10] = cd*co;
	m[11] = 0 ;

	m[12] = 0;
	m[13] = 0;
	m[14] = 0;
	m[15] = 1;
}

Matrix4x4  operator* (const Matrix4x4& m1, const Matrix4x4& m2 )
{
	Matrix4x4 m3;

	m3.m[0]		= m1.m[0]	* m2.m[0] + m1.m[1]	* m2.m[4] + m1.m[2]	* m2.m[8]  + m1.m[3] * m2.m[12];
	m3.m[4]		= m1.m[4]	* m2.m[0] + m1.m[5]	* m2.m[4] + m1.m[6]	* m2.m[8]  + m1.m[7] * m2.m[12];
	m3.m[8]		= m1.m[8]	* m2.m[0] + m1.m[9]	* m2.m[4] + m1.m[10]* m2.m[8]  + m1.m[11]* m2.m[12];
	m3.m[12]	= m1.m[12]  * m2.m[0] + m1.m[13]* m2.m[4] + m1.m[14]* m2.m[8]  + m1.m[15]* m2.m[12];
	m3.m[1]		= m1.m[0]	* m2.m[1] + m1.m[1]	* m2.m[5] + m1.m[2]	* m2.m[9]  + m1.m[3] * m2.m[13];
	m3.m[5]		= m1.m[4]	* m2.m[1] + m1.m[5]	* m2.m[5] + m1.m[6]	* m2.m[9]  + m1.m[7] * m2.m[13];
	m3.m[9]		= m1.m[8]	* m2.m[1] + m1.m[9]	* m2.m[5] + m1.m[10]* m2.m[9]  + m1.m[11]* m2.m[13];
	m3.m[13]	= m1.m[12]  * m2.m[1] + m1.m[13]* m2.m[5] + m1.m[14]* m2.m[9]  + m1.m[15]* m2.m[13];
	m3.m[2]		= m1.m[0]	* m2.m[2] + m1.m[1]	* m2.m[6] + m1.m[2]	* m2.m[10] + m1.m[3] * m2.m[14];
	m3.m[6]		= m1.m[4]	* m2.m[2] + m1.m[5]	* m2.m[6] + m1.m[6]	* m2.m[10] + m1.m[7] * m2.m[14];
	m3.m[10]	= m1.m[8]	* m2.m[2] + m1.m[9]	* m2.m[6] + m1.m[10]* m2.m[10] + m1.m[11]* m2.m[14];
	m3.m[14]	= m1.m[12]  * m2.m[2] + m1.m[13]* m2.m[6] + m1.m[14]* m2.m[10] + m1.m[15]* m2.m[14];
	m3.m[3]		= m1.m[0]	* m2.m[3] + m1.m[1]	* m2.m[7] + m1.m[2]	* m2.m[11] + m1.m[3] * m2.m[15];
	m3.m[7]		= m1.m[4]	* m2.m[3] + m1.m[5]	* m2.m[7] + m1.m[6]	* m2.m[11] + m1.m[7] * m2.m[15];
	m3.m[11]	= m1.m[8]	* m2.m[3] + m1.m[9]	* m2.m[7] + m1.m[10]* m2.m[11] + m1.m[11]* m2.m[15];
	m3.m[15]	= m1.m[12]  * m2.m[3] + m1.m[13]* m2.m[7] + m1.m[14]* m2.m[11] + m1.m[15]* m2.m[15];

	return m3;
}
Vector	   operator* (const Matrix4x4& m1, const Vector& v )
{
	Vector v0;

	v0.x = v.x*m1.m[0] + v.y*m1.m[4] + v.z*m1.m[8]  + v.w*m1.m[12];
	v0.y = v.x*m1.m[1] + v.y*m1.m[5] + v.z*m1.m[9]  + v.w*m1.m[13];
	v0.z = v.x*m1.m[2] + v.y*m1.m[6] + v.z*m1.m[10] + v.w*m1.m[14];
	v0.w = v.x*m1.m[3] + v.y*m1.m[7] + v.z*m1.m[11] + v.w*m1.m[15];

	return v0;
}
Matrix4x4  operator+ (const Matrix4x4& m1, const Matrix4x4& m2 )
{
	Matrix4x4 m3;
	for(int i=0; i<16; i++)
		m3.m[i] = m1.m[i]+m2.m[i];

	return m3;
}
Matrix4x4& Matrix4x4::operator= (const Matrix4x4& m2)
{
	for(int i=0; i<16; i++)
		m[i] = m2.m[i];

	return *this;
}
Matrix4x4& Matrix4x4::operator= (float m2[16])
{
	for(int i=0; i<16; i++)
		m[i] = m2[i];

	return *this;
}
ostream& operator<<(ostream& output, const Matrix4x4& m1)
{
	output <<"m:"<<endl;
	for(int i=0; i<16; i+=4)
	{
		output << "(" << m1.m[i] << "\t" <<  m1.m[i+1] << "\t" << m1.m[i+2] << "\t" << m1.m[i+3] <<")"<<endl;
	}

	return output;
}
