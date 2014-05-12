/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "Vector.h"

Vector::Vector()
{
	x=y=z=0.0f;
	w=1.0f;
}
Vector::~Vector()
{
}

Vector::Vector(int v)
{
	x = (float) v;
	y = (float) v;
	z = (float) v;
	w = (float) v;
}

Vector::Vector(const Vector& rhs)
{
	x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	w = rhs.w;
}

Vector::Vector(float x0, float y0)
{
	x=x0;	y=y0;	z=0;	w=1;
}

Vector::Vector(float x0, float y0, float z0)
{
	x=x0;	y=y0;	z=z0;	w=1;
}

Vector::Vector(float x0, float y0, float z0, float w0)
{
	x=x0;	y=y0;	z=z0;	w=w0;
}


void Vector::set_values(float x0, float y0, float z0)
{
	x=x0;	y=y0;	z=z0;	w=0;
}

void Vector::get_length(void)
{
	// Finds the length of the vector and stores it in vector.length

	l = (float)sqrt((x * x) + (y * y) + (z * z));
}
float Vector::length(void)
{
	// returns the length of the vector and stores it in vector.length
	
	this->get_length();

	return l;

}
int Vector::is_zero(void)
{
	// Checks if length of current Vertex is zero (ie Vertex is (0,0,0,0) )
	
	this->get_length();

	return(0); // FLOAT_EQ(0.0f, l ) );	// 0 = FALSE, 1 = TRUE
}	
int Vector::is_unit(void)
{
	// Checks if length of current vector is normalised (ie length==1)
	
	this->get_length();

	return(0); // FLOAT_EQ(1.0f, l ) );	// 0 = FALSE, 1 = TRUE
}	

void Vector::normalize(void)
{
	// Make sure vector is not zero
	if(this->is_zero()) return;

	this->get_length();
	
	// Divide x, y, and z by length to normalize the vector
	x /= l;
	y /= l;
	z /= l;
	w /= l;
}

void Vector::cross(const Vector& v1, const Vector& v2)
{
	#ifdef P3SIMD
	__asm
	{
		mov esi, v1
		mov edi, v2
		
		movups xmm0, [esi]
		movups xmm1, [edi]
		movups xmm2, xmm0
		movups xmm3, xmm1
		
		shufps xmm0, xmm0, 0xc9
		shufps xmm1, xmm1, 0xd2
		mulps xmm0, xmm1
		
		shufps xmm2, xmm2, 0xd2
		shufps xmm3, xmm3, 0xc9
		mulps xmm2, xmm3
		
		subps xmm0, xmm2
		
		mov esi, this
		movups [esi],xmm0
	}
	#else
		x = (v1.y * v2.z) - (v1.z * v2.y);
		y = (v1.z * v2.x) - (v1.x * v2.z);
		z = (v1.x * v2.y) - (v1.y * v2.x);
	#endif
}

float Vector::dot(const Vector& rhs)
{
	return (x*rhs.x + y*rhs.y + z*rhs.z);
}

Vector Vector::mult_matrix(const float mat[16])
{
	Vector v0;
	v0.x = x*mat[0] + y*mat[4] + z*mat[8]  + w*mat[12];
	v0.y = x*mat[1] + y*mat[5] + z*mat[9]  + w*mat[13];
	v0.z = x*mat[2] + y*mat[6] + z*mat[10] + w*mat[14];
	v0.w = w;
	//v0.w = x*mat[3] + y*mat[7] + z*mat[11] + w*mat[15];

	return v0;
}
Vector Vector::mult_matrix9(const float mat[9])
{
	Vector v0;
	v0.x = x*mat[0] + y*mat[3] + z*mat[6];
	v0.y = x*mat[1] + y*mat[4] + z*mat[7];
	v0.z = x*mat[2] + y*mat[5] + z*mat[8];
	v0.w = w;
	//v0.w = x*mat[3] + y*mat[7] + z*mat[11] + w*mat[15];

	return v0;
}/*
#define dot2(u,v)   ((u).x * (v).x + (u).y * (v).y + (u).z * (v).z)
#define norm(v)    sqrt(dot2(v,v))  // norm = length of vector
#define d(u,v)     norm(u-v)       // distance = norm of difference
*/

float Vector::distance_to_line(Vector v0, Vector v1)
{
    Vector v = v1-v0;
    Vector w = *this - v0;

	float c1 = w.dot(v);
    if ( c1 <= 0 )
	{
		Vector v2 = *this - v0;
		return sqrt(v2.dot(v2));
	}
    float c2 = v.dot(v);
    if ( c2 <= c1 )
	{
		Vector v2 = *this - v1;
		return sqrt(v2.dot(v2));
	}

	float b = c1 / c2;
    Vector p = v0 + (b * v);
	Vector v2 = *this - p;
	return sqrt(v2.dot(v2));
    
	/*Vector v = v1 - v0;
    Vector w = *this - v0;

    float c1 = dot2(w,v);
    if ( c1 <= 0 )
        return d(*this, v0);

    float c2 = dot2(v,v);
    if ( c2 <= c1 )
        return d(*this, v1);

    float b = c1 / c2;
    Vector Pb = v0 + (b * v);
    return d(*this, Pb);*/
}

Vector Vector::unit(void) const
{
	float l = (float)sqrt((x * x) + (y * y) + (z * z) + (w * w));
	if(l==0) return Vector(0,0,0,0);
	else return Vector(x/l, y/l, z/l);
}

// Return a reference to a component of a vector.
float & Vector::operator[](int i) 
{
   switch (i)
   {
   default:
      // return x;
   case 0:
      return x;
   case 1:
      return y;
   case 2:
      return z;
   }
}
void Vector::linear_interpolation( const Vector& v1, const Vector& v2, float step, float duration )	
{
	//Avoid divisions by zero, cpu doesnt like that much...
	if(duration==0) duration=1;
	
	x = v1.x + (( v2.x - v1.x) * (step/duration));
	y = v1.y + (( v2.y - v1.y) * (step/duration));
	z = v1.z + (( v2.z - v1.z) * (step/duration));
	w = v1.w + (( v2.w - v1.w) * (step/duration));

}

int Vector::is_parallel(const Vector& rhs)
{
	Vector v1(*this), v2(rhs);			// temp vectors
	float angle;						// temp angle

	// Make sure both vectors are not zero 
	if(v1.is_zero() || v2.is_zero()) return 0;

	// normalize both of them
	v1.normalize();				
	v2.normalize();

	angle = v1.dot(v2);	// get value between them
	
	// if cos angle is -1 or 1, the vectors are parallel
	return(0); //FLOAT_EQ(-1.0f, angle) || FLOAT_EQ(1.0f, angle));		
}
void Vector::get_normal(const Vector& v0, const Vector& v1, const Vector& v2)
{
	Vector n0, n1, normal;

	n0.x = v0.x - v1.x;
	n0.y = v0.y - v1.y;
	n0.z = v0.z - v1.z;

	n1.x = v1.x - v2.x;
	n1.y = v1.y - v2.y;
	n1.z = v1.z - v2.z;

	this->cross(n0, n1);
	this->normalize();
}

void Vector::build_rotatemat(Vector axis, float angle,float* rot_mat)
{
	//axis.normalize();

	float c = cos(angle);
	float s = sin(angle);
	float t = 1.0 - c;

	for(int i=0; i<16; i++)
		rot_mat[i]=0.0;

	rot_mat[0] = (t * (axis.x*axis.x)) + c;
	rot_mat[1] = (t * (axis.x*axis.y)) + (s*axis.z);
	rot_mat[2] = (t * (axis.x*axis.z)) - (s*axis.y);
	rot_mat[3] = 0;

	rot_mat[4] = (t * (axis.x*axis.y)) - (s*axis.z);
	rot_mat[5] = (t * (axis.y*axis.y)) + c;
	rot_mat[6] = (t * (axis.y*axis.z)) + (s*axis.x);
	rot_mat[7] = 0;

	rot_mat[8] = (t * (axis.x*axis.z)) + (s*axis.y);
	rot_mat[9] = (t * (axis.y*axis.z)) - (s*axis.x);
	rot_mat[10] = (t * (axis.z*axis.z)) + c;
	rot_mat[11] = 0;

	rot_mat[12] = 0;
	rot_mat[13] = 0;
	rot_mat[14] = 0;
	rot_mat[15] = 1;
}

void Vector::rotate3D(Vector axis, float angle)
{
	//axis.normalize();

	float c = cos(angle);
	float s = sin(angle);
	float t = 1.0 - cos(angle);

	float rot_mat[16];

	for(int i=0; i<16; i++)
		rot_mat[i]=0.0;

	rot_mat[0] = (t * (axis.x*axis.x)) + c;
	rot_mat[1] = (t * (axis.x*axis.y)) + (s*axis.z);
	rot_mat[2] = (t * (axis.x*axis.z)) - (s*axis.y);
	rot_mat[3] = 0;

	rot_mat[4] = (t * (axis.x*axis.y)) - (s*axis.z);
	rot_mat[5] = (t * (axis.y*axis.y)) + c;
	rot_mat[6] = (t * (axis.y*axis.z)) + (s*axis.x);
	rot_mat[7] = 0;

	rot_mat[8] = (t * (axis.x*axis.z)) + (s*axis.y);
	rot_mat[9] = (t * (axis.y*axis.z)) - (s*axis.x);
	rot_mat[10] = (t * (axis.z*axis.z)) + c;
	rot_mat[11] = 0;

	rot_mat[12] = 0;
	rot_mat[13] = 0;
	rot_mat[14] = 0;
	rot_mat[15] = 1;

	Vector temp1(x,y,z,w);
	Vector temp2 = temp1.mult_matrix(rot_mat);

	x = temp2.x;
	y = temp2.y;
	z = temp2.z;
	w = temp2.w;
}

float Vector::signed_angle(Vector v2, Vector reference)
{
	/*float signed_angle;
	
	Vector v1 = *this;

	v1.normalize();
	v2.normalize();
	reference.normalize();

	Vector v1v2cross;
	float v1v2dot;
	float nv1v2dot;

	v1v2cross.cross(v1, v2);
	v1v2dot = v1.dot(v2);
	nv1v2dot = reference.dot(v1v2cross);

	signed_angle = std::atan2(nv1v2dot, v1v2dot);
	return v1v2cross.dot(reference) < 0.f ? -signed_angle : signed_angle;*/

	//return signed_angle;

	//signed_angle = atan2(  N * ( V1 x V2 ), V1 * V2  ); 
	// where * is dot product and x is cross product
	// N is the normal to the polygon
	// ALL vectors: N, V1, V2 must be normalized

	Vector v1 = *this;
    Vector c;
	c.cross(v1, v2);
    float angle = std::atan2((float)c.length(), (float)v1.dot(v2));
	
//	printf("angleXX: %f\n",angle);
//	printf("c.dot: %f\n",c.dot(reference));
	return c.dot(reference) < 0.f ? -angle : angle;
}

void Vector::round(int dp)
{
	x = roundup(x, dp);
	y = roundup(y, dp);
	z = roundup(z, dp);
}
/*
// Write output Vector in format: "(%f, %f, %f)"
ostream& operator<<( ostream& output, Vector P) 
{

	output << "(" << P.x << ", " << P.y << ", " << P.z << ")";
	return output;
}
*/
//----------------------------------------------------------
// Comparison
//----------------------------------------------------------

int Vector::operator==( Vector Q)
{
	return (x==Q.x && y==Q.y && z==Q.z);
}
int Vector::operator!=( Vector Q)
{
	return (x!=Q.x || y!=Q.y || z!=Q.z);
}

//------------------------------------------------------------------
//  Unary Ops
//------------------------------------------------------------------
// unary minus
Vector Vector::operator-() {
	Vector v;
	v.x = -x; v.y = -y; v.z = -z;
	v.w = w;
	return v;
}
// unary 2D perp operator
Vector Vector::operator~() {
	Vector v;
	v.x = -y; v.y = x; v.z = z;
	v.w = w;
	return v;
}
//------------------------------------------------------------------
//  Scalar Ops
//------------------------------------------------------------------
// Scalar Multiplies
Vector operator*( int c, Vector w ) {
	Vector v;
	v.x = c * w.x;
	v.y = c * w.y;
	v.z = c * w.z;
	v.w = c * w.w;
	return v;
}
Vector operator*( float c, Vector w ) {
	Vector v;
	v.x = c * w.x;
	v.y = c * w.y;
	v.z = c * w.z;
	v.w = c * w.w;
	return v;
}
Vector operator*( Vector w, int c ) {
	Vector v;
	v.x = c * w.x;
	v.y = c * w.y;
	v.z = c * w.z;
	v.w = c * w.w;
	return v;
}
Vector operator*( Vector w, float c ) {
	Vector v;
	v.x = c * w.x;
	v.y = c * w.y;
	v.z = c * w.z;
	v.w = c * w.w;
	return v;
}
// Scalar Divides
Vector operator/( Vector w, int c ) {
	Vector v;
	v.x = w.x / c;
	v.y = w.y / c;
	v.z = w.z / c;
	v.w = w.w / c;
	return v;
}
Vector operator/( Vector w, float c ) {
	Vector v;
	v.x = w.x / c;
	v.y = w.y / c;
	v.z = w.z / c;
	v.w = w.w / c;
	return v;
}

int Vector::operator<(const Vector& b) {
    
	if(this->x<b.x) return true;
	if(b.x<this->x) return false;

	if(this->x==b.x)
	{
		if(this->y<b.y) return true;
		if(b.y<this->y) return false;

		if(this->y==b.y)
		{
			if(this->z>b.z) return true;
			if(b.z>this->z) return false;
			
			return false;
		}
	}

	return false;
}

int Vector::operator>(const Vector& b) {
    
	if(this->x>b.x) return true;
	if(b.x>this->x) return false;

	if(this->x==b.x)
	{
		if(this->y>b.y) return true;
		if(b.y>this->y) return false;

		if(this->y==b.y)
		{
			if(this->z>b.z) return true;
			if(b.z>this->z) return false;
			
			return false;
		}
	}

	return false;
}

//------------------------------------------------------------------
//  Arithmetic Ops
//------------------------------------------------------------------
Vector Vector::operator+( Vector v2 ) {
	Vector v;
	v.x = x + v2.x;
	v.y = y + v2.y;
	v.z = z + v2.z;
	v.w = w + v2.w;
	return v;
}
Vector Vector::operator-( Vector v2 ) {
	Vector v;
	v.x = x - v2.x;
	v.y = y - v2.y;
	v.z = z - v2.z;
	v.w = w - v2.w;
	return v;
}
//------------------------------------------------------------------
//  Products
//------------------------------------------------------------------
// Inner Dot Product
//float Vector::operator*( Vector w ) {
//	return (x * w.x + y * w.y + z * w.z);
//}
// 2D Exterior Perp Product
float Vector::operator|( Vector w ) {
	return (x * w.y - y * w.x);
}
// 3D Exterior Cross Product
Vector Vector::operator^( Vector w ) {
	Vector v;
	v.x = y * w.z - z * w.y;
	v.y = z * w.x - x * w.z;
	v.z = x * w.y - y * w.x;
	return v;
}
//------------------------------------------------------------------
//  Shorthand Ops
//------------------------------------------------------------------
Vector& Vector::operator*=( float c ) {        // vector scalar mult
	x *= c;
	y *= c;
	z *= c;
	return *this;
}
Vector& Vector::operator/=( float c ) {        // vector scalar div
	x /= c;
	y /= c;
	z /= c;
	return *this;
}
Vector& Vector::operator+=( Vector v2 ) {        // vector increment
	x += v2.x;
	y += v2.y;
	z += v2.z;
	w += v2.w;
	return *this;
}
Vector& Vector::operator-=( Vector v2 ) {        // vector decrement
	x -= v2.x;
	y -= v2.y;
	z -= v2.z;
	w -= v2.w;
	return *this;
}
Vector& Vector::operator^=(Vector w ) {        // 3D exterior cross product
	float ox=x, oy=y, oz=z;
	x = oy * w.z - oz * w.y;
	y = oz * w.x - ox * w.z;
	z = ox * w.y - oy * w.x;
	return *this;
}

Vector& Vector::operator= (const float& rhs)
{
	x = rhs;
	y = rhs;
	z = rhs;
	w = rhs;

	return *this;
}

Vector& Vector::operator=(const Vector& rhs)
{
    x = rhs.x;
	y = rhs.y;
	z = rhs.z;
	w = rhs.w;

    return *this;
}


void RotateVector (float Data[16], Vector &V, Vector &D)				// Rotate A Vector Using The Supplied Matrix ( NEW )
{
	D.x = (Data[0] * V.x) + (Data[4] * V.y) + (Data[8]  * V.z);	// Rotate Around The X Axis ( NEW )
	D.y = (Data[1] * V.x) + (Data[5] * V.y) + (Data[9]  * V.z);	// Rotate Around The Y Axis ( NEW )
	D.z = (Data[2] * V.x) + (Data[6] * V.y) + (Data[10] * V.z);	// Rotate Around The Z Axis ( NEW )
}

Vector operator*( const Vector& v1, const Vector& v2 )
{
	Vector tmp;

	tmp.x = v1.x * v2.x;
	tmp.y = v1.y * v2.y;
	tmp.z = v1.z * v2.z;
	tmp.w = v1.w * v2.w;

	return tmp;
}

Vector operator/(Vector& v1, Vector& v2 )
{
	if(v2.is_zero()) return v2;

	Vector tmp;
	
	tmp.x = v1.x / v2.x;
	tmp.y = v1.y / v2.y;
	tmp.z = v1.z / v2.z;
	tmp.w = v1.w / v2.w;

	return tmp;
}

Vector operator+(const Vector& v1, const Vector& v2 )
{
	Vector tmp;

	tmp.x = v1.x + v2.x;
	tmp.y = v1.y + v2.y;
	tmp.z = v1.z + v2.z;
	tmp.w = v1.w + v2.w;

	return tmp;
}

Vector operator-(const Vector& v1, const Vector& v2 )
{
	Vector tmp;

	tmp.x = v1.x - v2.x;
	tmp.y = v1.y - v2.y;
	tmp.z = v1.z - v2.z;
	tmp.w = v1.w - v2.w;

	return tmp;
}

int operator<(const Vector& a, const Vector& b) {
    
	if(a.x<b.x) return true;
	if(b.x<a.x) return false;

	if(a.x==b.x)
	{
		if(a.y<b.y) return true;
		if(b.y<a.y) return false;

		if(a.y==b.y)
		{
			if(a.z>b.z) return true;
			if(b.z>a.z) return false;
			
			return false;
		}
	}

	return false;
}

int operator>(const Vector& a, const Vector& b) {
    
	if(a.x>b.x) return true;
	if(b.x>a.x) return false;

	if(a.x==b.x)
	{
		if(a.y>b.y) return true;
		if(b.y>a.y) return false;

		if(a.y==b.y)
		{
			if(a.z>b.z) return true;
			if(b.z>a.z) return false;
			
			return false;
		}
	}

	return false;
}
