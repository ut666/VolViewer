/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef MATRIX_H
#define MATRIX_H

#include <qgl.h>
#include "Vector.h"

//Should write this up in a proper class, someday...

//A single precision floating point 3 by 3 matrix. 
typedef union Matrix3f_t
{
        struct
        {
            //column major
            union { GLfloat M00; GLfloat XX; GLfloat SX; };  //XAxis.X and Scale X
            union { GLfloat M10; GLfloat XY;             };  //XAxis.Y
            union { GLfloat M20; GLfloat XZ;             };  //XAxis.Z
            union { GLfloat M01; GLfloat YX;             };  //YAxis.X
            union { GLfloat M11; GLfloat YY; GLfloat SY; };  //YAxis.Y and Scale Y
            union { GLfloat M21; GLfloat YZ;             };  //YAxis.Z
            union { GLfloat M02; GLfloat ZX;             };  //ZAxis.X
            union { GLfloat M12; GLfloat ZY;             };  //ZAxis.Y
            union { GLfloat M22; GLfloat ZZ; GLfloat SZ; };  //ZAxis.Z and Scale Z
        } s;
        GLfloat M[9];
} Matrix3fT;    

 //A single precision floating point 4 by 4 matrix. 
typedef union Matrix4f_t
{
        struct
        {
            //column major
            union { GLfloat M00; GLfloat XX; GLfloat SX; };  //XAxis.X and Scale X
            union { GLfloat M10; GLfloat XY;             };  //XAxis.Y
            union { GLfloat M20; GLfloat XZ;             };  //XAxis.Z
            union { GLfloat M30; GLfloat XW;             };  //XAxis.W
            union { GLfloat M01; GLfloat YX;             };  //YAxis.X
            union { GLfloat M11; GLfloat YY; GLfloat SY; };  //YAxis.Y and Scale Y
            union { GLfloat M21; GLfloat YZ;             };  //YAxis.Z
            union { GLfloat M31; GLfloat YW;             };  //YAxis.W
            union { GLfloat M02; GLfloat ZX;             };  //ZAxis.X
            union { GLfloat M12; GLfloat ZY;             };  //ZAxis.Y
            union { GLfloat M22; GLfloat ZZ; GLfloat SZ; };  //ZAxis.Z and Scale Z
            union { GLfloat M32; GLfloat ZW;             };  //ZAxis.W
            union { GLfloat M03; GLfloat TX;             };  //Trans.X
            union { GLfloat M13; GLfloat TY;             };  //Trans.Y
            union { GLfloat M23; GLfloat TZ;             };  //Trans.Z
            union { GLfloat M33; GLfloat TW; GLfloat SW; };  //Trans.W and Scale W
        } s;
        GLfloat M[16];
} Matrix4fT;    

void Matrix4fSetZero(Matrix4fT* NewObj);
void Matrix4fSetIdentity(Matrix4fT* NewObj);
double Matrix4fDet(Matrix4fT* NewObj);
Matrix4fT Matrix4fInverse(Matrix4fT* NewObj);
Matrix4fT Matrix4fTexture(Matrix4fT* NewObj);

void Matrix3fSetZero(Matrix3fT* NewObj);
void Matrix3fSetIdentity(Matrix3fT* NewObj);
void Matrix3fSetRotationFromQuat4f(Matrix3fT* NewObj, const Vector* q1);
void Matrix3fMulMatrix3f(Matrix3fT* NewObj, const Matrix3fT* m1);
void Matrix4fSetRotationScaleFromMatrix4f(Matrix4fT* NewObj, const Matrix4fT* m1);
GLfloat Matrix4fSVD(const Matrix4fT* NewObj, Matrix3fT* rot3, Matrix4fT* rot4);
void Matrix4fSetRotationScaleFromMatrix3f(Matrix4fT* NewObj, const Matrix3fT* m1);
void Matrix4fMulRotationScale(Matrix4fT* NewObj, GLfloat scale);
void Matrix4fSetRotationFromMatrix3f(Matrix4fT* NewObj, const Matrix3fT* m1);
void Matrix3fMulVector3f(const Matrix3fT& NewObj, Vector* a);
void Matrix4fMulVector4f(const Matrix4fT& NewObj, Vector* a);
void Matrix4fFromLookAt(Vector eye, Vector at, Vector up, Matrix4fT* NewObj);
void Matrix3fRotationFromLookAt(Vector eye, Vector at, Vector up, Matrix3fT* NewObj);

#endif // MATRIX_H