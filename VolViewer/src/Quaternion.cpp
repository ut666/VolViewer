/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "Quaternion.h"

//#define PI 3.141592654

Quaternion::Quaternion()
{
	quat = Vector(0,0,0,1);
}

Quaternion::~Quaternion()
{

}

//Contruct rotation Quaternion given axis about which to rotate and angle 
//of rotation.
//Vector v representing axis of rotation.
//rho distance of rotation, in degrees.
void Quaternion::from_angle(Vector pos, float degrees)
{
	// First we want to convert the degrees to radians 
	// since the angle is assumed to be in radians
	float angle = float((degrees / 180.0f) * PI);

	// Here we calculate the sin( theta / 2) once for optimization
	float result = (float)sin( angle / 2.0f );
		
	// Calculate the x, y and z of the quaternion
	quat = pos * result;

	// Calcualte the w value by cos( theta / 2 )
	quat.w = (float)cos( angle / 2.0f );
}

//retrieve our euler angles from our quaternion
void Quaternion::to_euler(Vector* euler_angles)
{
	//x, yaw, y, pitch, z, roll
/*	euler_angles->x = asin(2*((quat.y*quat.z)-(quat.x*quat.w))); 
	euler_angles->y = atan((-2*((quat.y*quat.w)-(quat.x*quat.z)))/((quat.x*quat.x)+(quat.y*quat.y)-(quat.z*quat.z)-(quat.w*quat.w)));
	euler_angles->z = atan((-2*((quat.x*quat.y)-(quat.z*quat.w)))/((quat.x*quat.x)-(quat.y*quat.y)+(quat.z*quat.z)-(quat.w*quat.w)));
*/
/*	double sqw = quat.w*quat.w;
	double sqx = quat.x*quat.x;
	double sqy = quat.y*quat.y;
	double sqz = quat.z*quat.z;
	euler_angles->x = atan2(2.0 * (quat.x*quat.y + quat.z*quat.w),(sqx - sqy - sqz + sqw));
	euler_angles->y = atan2(2.0 * (quat.y*quat.z + quat.x*quat.w),(-sqx - sqy + sqz + sqw));
	euler_angles->z = asin(-2.0 * (quat.x*quat.z - quat.y*quat.w));*/

	//tan(yaw)   =  2(q1q2+q4q3) / (q42  + q12 - q22- q32)
	euler_angles->z = (float)(atan2( (2*((quat.x*quat.y)+(quat.w*quat.z))) ,((quat.w*quat.w)+(quat.x*quat.x)-(quat.y*quat.y)-(quat.z*quat.z)) ));

	//sin(pitch) = -2(q1q3-q4q2)
	euler_angles->y = (float)(asin( -2*((quat.x*quat.z)-(quat.w*quat.y)) )); 

	//tan(roll)  =  2(q4q1+q2q3) / (q42 - q12 - q22+ q32)
	euler_angles->x = (float)(atan2( (2*((quat.w*quat.x)+(quat.y*quat.z))) ,((quat.w*quat.w)-(quat.x*quat.x)-(quat.y*quat.y)+(quat.z*quat.z)) ));

}
//Build our quaternion from a rotation matrix
void Quaternion::from_matrix(Matrix3f_t *src)
{
	const float trace = 1.0f + src->s.M00 + src->s.M11 + src->s.M22;	
	if (trace > 0.00001f)	
	{		
		const float s = sqrt(trace) * 2;		
		quat.x = (src->s.M21 - src->s.M12) / s;			
		quat.y = (src->s.M02 - src->s.M20) / s;			
		quat.z = (src->s.M10 - src->s.M01) / s;			
		quat.w = s / 4;	
	}
	else if (src->s.M00 > src->s.M11 && src->s.M00 > src->s.M22)	
	{		
		const float s = sqrt(1.0f + src->s.M00 - src->s.M11 - src->s.M22) * 2;		
		quat.x = s / 4;			
		quat.y = (src->s.M10 + src->s.M01) / s;			
		quat.z = (src->s.M02 + src->s.M20) / s;			
		quat.w = (src->s.M21 - src->s.M12) / s;	
	}	
	else if (src->s.M11 > src->s.M22)	
	{		
		const float s = sqrt(1.0f + src->s.M11 - src->s.M00 - src->s.M22) * 2;		
		quat.x = (src->s.M10 + src->s.M01) / s;			
		quat.y = s / 4;			
		quat.z = (src->s.M21 + src->s.M12) / s;			
		quat.w = (src->s.M02 - src->s.M20) / s;	
	}	
	else	
	{		
		const float s = sqrt(1.0f + src->s.M22 - src->s.M00 - src->s.M11) * 2;
		quat.x = (src->s.M02 + src->s.M20) / s;			
		quat.y = (src->s.M21 + src->s.M12) / s;			
		quat.z = s / 4;			
		quat.w = (src->s.M10 - src->s.M01) / s;	
	}
}
void Quaternion::to_matrix(Matrix3f_t *dst)
{
	// Make sure the matrix has allocated memory to store the rotation data
	if(!dst) return;

	// First row
	dst->s.M00 = 1.0f - 2.0f * ( quat.y * quat.y + quat.z * quat.z ); 
	dst->s.M10 = 2.0f * (quat.x * quat.y + quat.z * quat.w);
	dst->s.M20 = 2.0f * (quat.x * quat.z - quat.y * quat.w);

	// Second row
	dst->s.M01 = 2.0f * ( quat.x * quat.y - quat.z * quat.w );  
	dst->s.M11 = 1.0f - 2.0f * ( quat.x * quat.x + quat.z * quat.z ); 
	dst->s.M21 = 2.0f * (quat.z * quat.y + quat.x * quat.w );  

	// Third row
	dst->s.M02 = 2.0f * ( quat.x * quat.z + quat.y * quat.w );
	dst->s.M12 = 2.0f * ( quat.y * quat.z - quat.x * quat.w );
	dst->s.M22 = 1.0f - 2.0f * ( quat.x * quat.x + quat.y * quat.y );  

/*
	// First row
	pMatrix[ 0] = 1.0f - 2.0f * ( quat.y * quat.y + quat.z * quat.z ); 
	pMatrix[ 1] = 2.0f * (quat.x * quat.y + quat.z * quat.w);
	pMatrix[ 2] = 2.0f * (quat.x * quat.z - quat.y * quat.w);
	pMatrix[ 3] = 0.0f;  

	// Second row
	pMatrix[ 4] = 2.0f * ( quat.x * quat.y - quat.z * quat.w );  
	pMatrix[ 5] = 1.0f - 2.0f * ( quat.x * quat.x + quat.z * quat.z ); 
	pMatrix[ 6] = 2.0f * (quat.z * quat.y + quat.x * quat.w );  
	pMatrix[ 7] = 0.0f;  

	// Third row
	pMatrix[ 8] = 2.0f * ( quat.x * quat.z + quat.y * quat.w );
	pMatrix[ 9] = 2.0f * ( quat.y * quat.z - quat.x * quat.w );
	pMatrix[10] = 1.0f - 2.0f * ( quat.x * quat.x + quat.y * quat.y );  
	pMatrix[11] = 0.0f;  

	// Fourth row
	pMatrix[12] = 0;  
	pMatrix[13] = 0;  
	pMatrix[14] = 0;  
	pMatrix[15] = 1.0f;
*/
	// Now pMatrix[] is a 4x4 homogeneous matrix that can be applied to an OpenGL Matrix
}

void slerp(Quaternion* from, Quaternion * to, float t, Quaternion* res)
{
	float to1[4];
	double omega, cosom, sinom, scale0, scale1;

	// calc cosine
	cosom = from->quat.x * to->quat.x + from->quat.y * to->quat.y + from->quat.z * to->quat.z
		   + from->quat.w * to->quat.w;

	// adjust signs (if necessary)
	if ( cosom <0.0 ){ cosom = -cosom; to1[0] = - to->quat.x;
	to1[1] = - to->quat.y;
	to1[2] = - to->quat.z;
	to1[3] = - to->quat.w;
	} else  {
	to1[0] = to->quat.x;
	to1[1] = to->quat.y;
	to1[2] = to->quat.z;
	to1[3] = to->quat.w;
	}


	// calculate coefficients
	if ( (1.0 - cosom) > 0.001 ) {
		// standard case (slerp)
		omega = acos(cosom);
		sinom = sin(omega);
		scale0 = sin((1.0 - t) * omega) / sinom;
		scale1 = sin(t * omega) / sinom;


	} else {        
	// "from" and "to" quaternions are very close 
	//  ... so we can do a linear interpolation
		scale0 = 1.0 - t;
		scale1 = t;
	}
	// calculate final values
	res->quat.x = (float)(scale0 * from->quat.x + scale1 * to1[0]);
	res->quat.y = (float)(scale0 * from->quat.y + scale1 * to1[1]);
	res->quat.z = (float)(scale0 * from->quat.z + scale1 * to1[2]);
	res->quat.w = (float)(scale0 * from->quat.w + scale1 * to1[3]);
}

Vector Quaternion::apply(const Vector & p1) const
{
	Vector p2;

	p2.x = quat.w*quat.w*p1.x + 2*quat.y*quat.w*p1.z - 2*quat.z*quat.w*p1.y + quat.x*quat.x*p1.x + 2*quat.y*quat.x*p1.y + 2*quat.z*quat.x*p1.z - quat.z*quat.z*p1.x - quat.y*quat.y*p1.x;
	p2.y = 2*quat.x*quat.y*p1.x + quat.y*quat.y*p1.y + 2*quat.z*quat.y*p1.z + 2*quat.w*quat.z*p1.x - quat.z*quat.z*p1.y + quat.w*quat.w*p1.y - 2*quat.x*quat.w*p1.z - quat.x*quat.x*p1.y;
	p2.z = 2*quat.x*quat.z*p1.x + 2*quat.y*quat.z*p1.y + quat.z*quat.z*p1.z - 2*quat.w*quat.y*p1.x - quat.y*quat.y*p1.z + 2*quat.w*quat.x*p1.y - quat.x*quat.x*p1.z + quat.w*quat.w*p1.z;

	return p2;
}
Quaternion Quaternion::operator *(Quaternion q)
{
	Quaternion r;

	r.quat.w = quat.w*q.quat.w - quat.x*q.quat.x - quat.y*q.quat.y - quat.z*q.quat.z;
	r.quat.x = quat.w*q.quat.x + quat.x*q.quat.w + quat.y*q.quat.z - quat.z*q.quat.y;
	r.quat.y = quat.w*q.quat.y + quat.y*q.quat.w + quat.z*q.quat.x - quat.x*q.quat.z;
	r.quat.z = quat.w*q.quat.z + quat.z*q.quat.w + quat.x*q.quat.y - quat.y*q.quat.x;
	
	return(r);
}
