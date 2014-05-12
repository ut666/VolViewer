/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "Matrix.h"

void Matrix4fSetZero(Matrix4fT* NewObj)
{
    NewObj->s.XX = NewObj->s.XY = NewObj->s.XZ = NewObj->s.XW =
    NewObj->s.YX = NewObj->s.YY = NewObj->s.YZ = NewObj->s.YW =
    NewObj->s.ZX = NewObj->s.ZY = NewObj->s.ZZ = NewObj->s.ZW =
	NewObj->s.TX = NewObj->s.TY = NewObj->s.TZ = NewObj->s.TW =0.0f;
}

void Matrix4fSetIdentity(Matrix4fT* NewObj)
{
    Matrix4fSetZero(NewObj);

    //then set diagonal as 1
    NewObj->s.XX = 
    NewObj->s.YY = 
    NewObj->s.ZZ =
	NewObj->s.TW = 1.0f;
}

double Matrix4fDet(Matrix4fT* NewObj)
{
	double det;

	det = NewObj->s.XX * ( NewObj->s.YY*NewObj->s.ZZ - NewObj->s.ZY*NewObj->s.YZ )
      	- NewObj->s.XY * ( NewObj->s.YX*NewObj->s.ZZ - NewObj->s.ZX*NewObj->s.YZ )
      	+ NewObj->s.XZ * ( NewObj->s.YX*NewObj->s.ZY - NewObj->s.ZX*NewObj->s.YY );

  	return det;
}

Matrix4fT Matrix4fInverse(Matrix4fT* NewObj)
{
	double determinant = Matrix4fDet(NewObj);
	
	Matrix4fT T;

    T.s.XX =  (NewObj->s.YY*NewObj->s.ZZ - NewObj->s.YZ*NewObj->s.ZY) / determinant;
    T.s.XY = -(NewObj->s.XY*NewObj->s.ZZ - NewObj->s.ZY*NewObj->s.XZ) / determinant;
    T.s.XZ =  (NewObj->s.XY*NewObj->s.YZ - NewObj->s.YY*NewObj->s.XZ) / determinant;

    T.s.YX = -(NewObj->s.YX*NewObj->s.ZZ - NewObj->s.YZ*NewObj->s.ZX) / determinant;
    T.s.YY =  (NewObj->s.XX*NewObj->s.ZZ - NewObj->s.ZX*NewObj->s.XZ) / determinant;
    T.s.YZ = -(NewObj->s.XX*NewObj->s.YZ - NewObj->s.YX*NewObj->s.XZ) / determinant;

    T.s.ZX =  (NewObj->s.YX*NewObj->s.ZY - NewObj->s.ZX*NewObj->s.YY) / determinant;
    T.s.ZY = -(NewObj->s.XX*NewObj->s.ZY - NewObj->s.ZX*NewObj->s.XY) / determinant;
    T.s.ZZ =  (NewObj->s.XX*NewObj->s.YY - NewObj->s.XY*NewObj->s.YX) / determinant;

	return T;
}

Matrix4fT Matrix4fTexture(Matrix4fT* NewObj)
{
	Matrix4fT T;

    T.s.XX = NewObj->s.XX;
    T.s.XY = NewObj->s.YX;
    T.s.XZ = NewObj->s.ZX;
    T.s.XW = NewObj->s.TX;

    T.s.YX = NewObj->s.XY;
    T.s.YY = NewObj->s.YY;
    T.s.YZ = NewObj->s.ZY;
    T.s.YW = NewObj->s.TY;

    T.s.ZX = NewObj->s.XZ;
    T.s.ZY = NewObj->s.YZ;
    T.s.ZZ = NewObj->s.ZZ;
    T.s.ZW = NewObj->s.TZ;

	T.s.TX = NewObj->s.XW;
	T.s.TY = NewObj->s.YW;
	T.s.TZ = NewObj->s.ZW;
    T.s.TW = NewObj->s.TW;

	return T;
}

void Matrix3fSetZero(Matrix3fT* NewObj)
{
    NewObj->s.M00 = NewObj->s.M01 = NewObj->s.M02 = 
    NewObj->s.M10 = NewObj->s.M11 = NewObj->s.M12 = 
    NewObj->s.M20 = NewObj->s.M21 = NewObj->s.M22 = 0.0f;
}

/**
 * Sets this Matrix3 to identity.
 */
void Matrix3fSetIdentity(Matrix3fT* NewObj)
{
    Matrix3fSetZero(NewObj);

    //then set diagonal as 1
    NewObj->s.M00 = 
    NewObj->s.M11 = 
    NewObj->s.M22 = 1.0f;
}

/**
  * Sets the value of this matrix to the matrix conversion of the
  * quaternion argument. 
  * @param q1 the quaternion to be converted 
  */
//$hack this can be optimized some(if s == 0)
void Matrix3fSetRotationFromQuat4f(Matrix3fT* NewObj, const Vector* q1)
{
    GLfloat n, s;
    GLfloat xs, ys, zs;
    GLfloat wx, wy, wz;
    GLfloat xx, xy, xz;
    GLfloat yy, yz, zz;

    n = (q1->x * q1->x) + (q1->y * q1->y) + (q1->z * q1->z) + (q1->w * q1->w);
    s = (n > 0.0f) ? (2.0f / n) : 0.0f;

    xs = q1->x * s;  ys = q1->y * s;  zs = q1->z * s;
    wx = q1->w * xs; wy = q1->w * ys; wz = q1->w * zs;
    xx = q1->x * xs; xy = q1->x * ys; xz = q1->x * zs;
    yy = q1->y * ys; yz = q1->y * zs; zz = q1->z * zs;

    NewObj->s.XX = 1.0f - (yy + zz); NewObj->s.YX =         xy - wz;  NewObj->s.ZX =         xz + wy;
    NewObj->s.XY =         xy + wz;  NewObj->s.YY = 1.0f - (xx + zz); NewObj->s.ZY =         yz - wx;
    NewObj->s.XZ =         xz - wy;  NewObj->s.YZ =         yz + wx;  NewObj->s.ZZ = 1.0f - (xx + yy);
}

/**
 * Sets the value of this matrix to the result of multiplying itself
 * with matrix m1. 
 * @param m1 the other matrix 
 */
void Matrix3fMulMatrix3f(Matrix3fT* NewObj, const Matrix3fT* m1)
{
    Matrix3fT Result; //safe not to initialize

    // alias-safe way.
    Result.s.M00 = (NewObj->s.M00 * m1->s.M00) + (NewObj->s.M01 * m1->s.M10) + (NewObj->s.M02 * m1->s.M20);
    Result.s.M01 = (NewObj->s.M00 * m1->s.M01) + (NewObj->s.M01 * m1->s.M11) + (NewObj->s.M02 * m1->s.M21);
    Result.s.M02 = (NewObj->s.M00 * m1->s.M02) + (NewObj->s.M01 * m1->s.M12) + (NewObj->s.M02 * m1->s.M22);

    Result.s.M10 = (NewObj->s.M10 * m1->s.M00) + (NewObj->s.M11 * m1->s.M10) + (NewObj->s.M12 * m1->s.M20);
    Result.s.M11 = (NewObj->s.M10 * m1->s.M01) + (NewObj->s.M11 * m1->s.M11) + (NewObj->s.M12 * m1->s.M21);
    Result.s.M12 = (NewObj->s.M10 * m1->s.M02) + (NewObj->s.M11 * m1->s.M12) + (NewObj->s.M12 * m1->s.M22);

    Result.s.M20 = (NewObj->s.M20 * m1->s.M00) + (NewObj->s.M21 * m1->s.M10) + (NewObj->s.M22 * m1->s.M20);
    Result.s.M21 = (NewObj->s.M20 * m1->s.M01) + (NewObj->s.M21 * m1->s.M11) + (NewObj->s.M22 * m1->s.M21);
    Result.s.M22 = (NewObj->s.M20 * m1->s.M02) + (NewObj->s.M21 * m1->s.M12) + (NewObj->s.M22 * m1->s.M22);

    //copy result back to this
    *NewObj = Result;
}

void Matrix4fSetRotationScaleFromMatrix4f(Matrix4fT* NewObj, const Matrix4fT* m1)
{
    NewObj->s.XX = m1->s.XX; NewObj->s.YX = m1->s.YX; NewObj->s.ZX = m1->s.ZX;
    NewObj->s.XY = m1->s.XY; NewObj->s.YY = m1->s.YY; NewObj->s.ZY = m1->s.ZY;
    NewObj->s.XZ = m1->s.XZ; NewObj->s.YZ = m1->s.YZ; NewObj->s.ZZ = m1->s.ZZ;
}

/**
  * Performs SVD on this matrix and gets scale and rotation.
  * Rotation is placed into rot3, and rot4.
  * @param rot3 the rotation factor(Matrix3d). if null, ignored
  * @param rot4 the rotation factor(Matrix4) only upper 3x3 elements are changed. if null, ignored
  * @return scale factor
  */
GLfloat Matrix4fSVD(const Matrix4fT* NewObj, Matrix3fT* rot3, Matrix4fT* rot4)
{
    GLfloat s, n;

    // this is a simple svd.
    // Not complete but fast and reasonable.
    // See comment in Matrix3d.

    s = sqrt(
            ( (NewObj->s.XX * NewObj->s.XX) + (NewObj->s.XY * NewObj->s.XY) + (NewObj->s.XZ * NewObj->s.XZ) + 
              (NewObj->s.YX * NewObj->s.YX) + (NewObj->s.YY * NewObj->s.YY) + (NewObj->s.YZ * NewObj->s.YZ) +
              (NewObj->s.ZX * NewObj->s.ZX) + (NewObj->s.ZY * NewObj->s.ZY) + (NewObj->s.ZZ * NewObj->s.ZZ) ) / 3.0f );

    if (rot3)   //if pointer not null
    {
        //this->getRotationScale(rot3);
        rot3->s.XX = NewObj->s.XX; rot3->s.XY = NewObj->s.XY; rot3->s.XZ = NewObj->s.XZ;
        rot3->s.YX = NewObj->s.YX; rot3->s.YY = NewObj->s.YY; rot3->s.YZ = NewObj->s.YZ;
        rot3->s.ZX = NewObj->s.ZX; rot3->s.ZY = NewObj->s.ZY; rot3->s.ZZ = NewObj->s.ZZ;

        // zero-div may occur.

        n = 1.0f / sqrt( (NewObj->s.XX * NewObj->s.XX) +
                                  (NewObj->s.XY * NewObj->s.XY) +
                                  (NewObj->s.XZ * NewObj->s.XZ) );
        rot3->s.XX *= n;
        rot3->s.XY *= n;
        rot3->s.XZ *= n;

        n = 1.0f / sqrt( (NewObj->s.YX * NewObj->s.YX) +
                                  (NewObj->s.YY * NewObj->s.YY) +
                                  (NewObj->s.YZ * NewObj->s.YZ) );
        rot3->s.YX *= n;
        rot3->s.YY *= n;
        rot3->s.YZ *= n;

        n = 1.0f / sqrt( (NewObj->s.ZX * NewObj->s.ZX) +
                                  (NewObj->s.ZY * NewObj->s.ZY) +
                                  (NewObj->s.ZZ * NewObj->s.ZZ) );
        rot3->s.ZX *= n;
        rot3->s.ZY *= n;
        rot3->s.ZZ *= n;
    }

    if (rot4)   //if pointer not null
    {
        if (rot4 != NewObj)
        {
            Matrix4fSetRotationScaleFromMatrix4f(rot4, NewObj);  // private method
        }

        // zero-div may occur.

        n = 1.0f / sqrt( (NewObj->s.XX * NewObj->s.XX) +
                                  (NewObj->s.XY * NewObj->s.XY) +
                                  (NewObj->s.XZ * NewObj->s.XZ) );
        rot4->s.XX *= n;
        rot4->s.XY *= n;
        rot4->s.XZ *= n;

        n = 1.0f / sqrt( (NewObj->s.YX * NewObj->s.YX) +
                                  (NewObj->s.YY * NewObj->s.YY) +
                                  (NewObj->s.YZ * NewObj->s.YZ) );
        rot4->s.YX *= n;
        rot4->s.YY *= n;
        rot4->s.YZ *= n;

        n = 1.0f / sqrt( (NewObj->s.ZX * NewObj->s.ZX) +
                                  (NewObj->s.ZY * NewObj->s.ZY) +
                                  (NewObj->s.ZZ * NewObj->s.ZZ) );
        rot4->s.ZX *= n;
        rot4->s.ZY *= n;
        rot4->s.ZZ *= n;
    }

    return s;
}

void Matrix4fSetRotationScaleFromMatrix3f(Matrix4fT* NewObj, const Matrix3fT* m1)
{
    NewObj->s.XX = m1->s.XX; NewObj->s.YX = m1->s.YX; NewObj->s.ZX = m1->s.ZX;
    NewObj->s.XY = m1->s.XY; NewObj->s.YY = m1->s.YY; NewObj->s.ZY = m1->s.ZY;
    NewObj->s.XZ = m1->s.XZ; NewObj->s.YZ = m1->s.YZ; NewObj->s.ZZ = m1->s.ZZ;
}

void Matrix4fMulRotationScale(Matrix4fT* NewObj, GLfloat scale)
{
    NewObj->s.XX *= scale; NewObj->s.YX *= scale; NewObj->s.ZX *= scale;
    NewObj->s.XY *= scale; NewObj->s.YY *= scale; NewObj->s.ZY *= scale;
    NewObj->s.XZ *= scale; NewObj->s.YZ *= scale; NewObj->s.ZZ *= scale;
}

/**
  * Sets the rotational component (upper 3x3) of this matrix to the matrix
  * values in the T precision Matrix3d argument; the other elements of
  * this matrix are unchanged; a singular value decomposition is performed
  * on this object's upper 3x3 matrix to factor out the scale, then this
  * object's upper 3x3 matrix components are replaced by the passed rotation
  * components, and then the scale is reapplied to the rotational
  * components.
  * @param m1 T precision 3x3 matrix
  */
void Matrix4fSetRotationFromMatrix3f(Matrix4fT* NewObj, const Matrix3fT* m1)
{
    GLfloat scale;
    scale = Matrix4fSVD(NewObj, 0, 0);

    Matrix4fSetRotationScaleFromMatrix3f(NewObj, m1);
    Matrix4fMulRotationScale(NewObj, scale);
}
//multiplies Matric M by Vector A and stores result into Vector A
void Matrix3fMulVector3f(const Matrix3fT& NewObj, Vector* a)
{
//        a->x = a->x*NewObj.s.XX + a->y*NewObj.s.XY + a->z*NewObj.s.XZ; //+ a->w*NewObj.s.XW;
//        a->y = a->x*NewObj.s.YX + a->y*NewObj.s.YY + a->z*NewObj.s.YZ; //+ a->w*NewObj.s.YW;
//       a->z = a->x*NewObj.s.ZX + a->y*NewObj.s.ZY + a->z*NewObj.s.ZZ; //+ a->w*NewObj.s.ZW;

	Vector b;
	b.x = a->x*NewObj.M[0] + a->y*NewObj.M[3] + a->z*NewObj.M[6]; //+ a->w*NewObj.s.XW;
    b.y = a->x*NewObj.M[1] + a->y*NewObj.M[4] + a->z*NewObj.M[7]; //+ a->w*NewObj.s.YW;
    b.z = a->x*NewObj.M[2] + a->y*NewObj.M[5] + a->z*NewObj.M[8]; //+ a->w*NewObj.s.ZW;
	
	*a = b;
}

//multiplies Matric M by Vector A and stores result into Vector A
void Matrix4fMulVector4f(const Matrix4fT& NewObj, Vector* a)
{
	Vector b;
	b.x = a->x*NewObj.s.XX + a->y*NewObj.s.XY + a->z*NewObj.s.XZ + a->w*NewObj.s.XW;
    b.y = a->x*NewObj.s.YX + a->y*NewObj.s.YY + a->z*NewObj.s.YZ + a->w*NewObj.s.YW;
    b.z = a->x*NewObj.s.ZX + a->y*NewObj.s.ZY + a->z*NewObj.s.ZZ + a->w*NewObj.s.ZW;		
	b.x +=NewObj.s.TX;
	b.y +=NewObj.s.TY;
	b.z +=NewObj.s.TZ;

	*a = b;
}
void Matrix3fRotationFromLookAt(Vector eye, Vector at, Vector up, Matrix3fT* NewObj)
{
	Vector a,r,u;

	a = eye - at;
	r.cross(up, a);
	u.cross(a, r);

	a.normalize();
	r.normalize();
	u.normalize();

	NewObj->M[0] = r.x;
	NewObj->M[1] = r.y;
	NewObj->M[2] = r.z;

	NewObj->M[3] = u.x;
	NewObj->M[4] = u.y;
	NewObj->M[5] = u.z;

	NewObj->M[6] = a.x;
	NewObj->M[7] = a.y;
	NewObj->M[8] = a.z;
}
void Matrix4fFromLookAt(Vector eye, Vector at, Vector up, Matrix4fT* NewObj)
{
	Vector zaxis = eye - at;
	zaxis.normalize();
	
	Vector yaxis;
	yaxis = up;

	Vector xaxis;

	xaxis.x = yaxis.y*zaxis.z - yaxis.z*zaxis.y;
	xaxis.y = -yaxis.x*zaxis.z + yaxis.z*zaxis.x;
	xaxis.z = yaxis.x*zaxis.y - yaxis.y*zaxis.x;

	yaxis.x = zaxis.y*xaxis.z - zaxis.z*xaxis.y;
	yaxis.y = -zaxis.x*xaxis.z + zaxis.z*xaxis.x;
	yaxis.z = zaxis.x*xaxis.y - zaxis.y*xaxis.x;

	xaxis.normalize();
	yaxis.normalize();

	NewObj->s.XX = xaxis.x;
	NewObj->s.YX = xaxis.y;
	NewObj->s.ZX = xaxis.z;
	NewObj->s.TX = -xaxis.x*eye.x + -xaxis.y*eye.y + -xaxis.z*eye.z;

	NewObj->s.XY = yaxis.x;
	NewObj->s.YY = yaxis.y;
	NewObj->s.ZY = yaxis.z;
	NewObj->s.TY = -yaxis.x*eye.x + -yaxis.y*eye.y + -yaxis.z*eye.z;

	NewObj->s.XZ = zaxis.x;
	NewObj->s.YZ = zaxis.y;
	NewObj->s.ZZ = zaxis.z;
	NewObj->s.TZ = -zaxis.x*eye.x + -zaxis.y*eye.y + -zaxis.z*eye.z;

	NewObj->s.XW = 0.0;
	NewObj->s.YW = 0.0;
	NewObj->s.ZW = 0.0;
	NewObj->s.TW = 1.0;
	/*
	Vector zaxis = at - eye;
	zaxis.normalize();

	Vector xaxis;
	xaxis.cross(up, zaxis);
	xaxis.normalize();

	Vector yaxis;
	yaxis.cross(zaxis, xaxis);

	NewObj->s.XX = xaxis.x;
	NewObj->s.XY = yaxis.x;
	NewObj->s.XZ = zaxis.x;
	NewObj->s.XW = 0.0;

	NewObj->s.YX = xaxis.y;
	NewObj->s.YY = yaxis.y;
	NewObj->s.YZ = zaxis.y;
	NewObj->s.YW = 0.0;

	NewObj->s.ZX = xaxis.z;
	NewObj->s.ZY = yaxis.z;
	NewObj->s.ZZ = zaxis.z;
	NewObj->s.ZW = 0.0;

	NewObj->s.TX = -xaxis.dot(eye);
	NewObj->s.TY = -yaxis.dot(eye);
	NewObj->s.TZ = -zaxis.dot(eye);
	NewObj->s.TW = 1.0;*/

/*
	 xaxis.x           yaxis.x           zaxis.x          0
	 xaxis.y           yaxis.y           zaxis.y          0
	 xaxis.z           yaxis.z           zaxis.z          0
	-dot(xaxis, eye)  -dot(yaxis, eye)  -dot(zaxis, eye)  l 
*/
}