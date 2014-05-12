/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "ArcBall.h" 
                                           // ArcBall header
ArcBall::ArcBall(void)
{
	Matrix4fSetIdentity(&Transform);
	Matrix3fSetIdentity(&LastRot);
	Matrix3fSetIdentity(&ThisRot);

    //Clear initial values
    this->StVec.x     =
    this->StVec.y     = 
    this->StVec.z     = 

    this->EnVec.x     =
    this->EnVec.y     = 
    this->EnVec.z     = 0.0f;

	oldpos=0.0;
	AdjustHeight = AdjustWidth = 0.0;
	isDragging=false;
	isZooming=false;
	isTranslate=false;
	rot_type = 0;
	orien_type=0;
}

ArcBall::ArcBall(GLfloat NewWidth, GLfloat NewHeight)
{
	Matrix4fSetIdentity(&Transform);
	Matrix3fSetIdentity(&LastRot);
	Matrix3fSetIdentity(&ThisRot);

    //Clear initial values
    this->StVec.x     =
    this->StVec.y     = 
    this->StVec.z     = 

    this->EnVec.x     =
    this->EnVec.y     = 
    this->EnVec.z     = 0.0f;

    //Set initial bounds
    this->setBounds(NewWidth, NewHeight);
	
	oldpos=0.0;
	AdjustHeight = AdjustWidth = 0.0;
	isDragging=false;
	isZooming=false;
	isTranslate=false;
	rot_type = 0;
	orien_type=0;
}

ArcBall::~ArcBall(void)
{
	//do nothing
}

//Arcball sphere constants:
//Diameter is       2.0f
//Radius is         1.0f
//Radius squared is 1.0f
void ArcBall::_mapToSphere(const Vector* NewPt, Vector* NewVec) const
{
    Vector TempPt;
    GLfloat length;

    //Copy paramter into temp point
    TempPt = *NewPt;

    //Adjust point coords and scale down to range of [-1 ... 1]
    if(rot_type==0)
	{
		TempPt.x  =        (TempPt.x * this->AdjustWidth)  - 1.0f;
		TempPt.y  = 1.0f - (TempPt.y * this->AdjustHeight);
	}
	else if(rot_type==1)
	{
	//	TempPt.x  =        (TempPt.x * this->AdjustWidth)  - 1.0f;
		TempPt.y  = 1.0f - (TempPt.y * this->AdjustHeight);
	}
	else if(rot_type==2)
	{
		TempPt.x  =        (TempPt.x * this->AdjustWidth)  - 1.0f;
	//	TempPt.y  = 1.0f - (TempPt.y * this->AdjustHeight);
	}

    //Compute the square of the length of the vector to the point from the center
    length      = (TempPt.x * TempPt.x) + (TempPt.y * TempPt.y);

    //If the point is mapped outside of the sphere... (length > radius squared)
    if (length > 1.0f)
    {
        GLfloat norm;

        //Compute a normalizing factor (radius / sqrt(length))
        norm    = 1.0f / sqrt(length);

        //Return the "normalized" vector, a point on the sphere
		NewVec->x = TempPt.x * norm;
		NewVec->y = TempPt.y * norm;
		NewVec->z = 0.0f;
    }
    else    //Else it's on the inside
    {
        //Return a vector to a point mapped inside the sphere sqrt(radius squared - length)
		NewVec->x = TempPt.x;
		NewVec->y = TempPt.y;
	    NewVec->z = sqrt(1.0f - length);
	}
}
void ArcBall::setIdentity(void)
{
	Matrix4fSetIdentity(&Transform);
	Matrix3fSetIdentity(&LastRot);
	Matrix3fSetIdentity(&ThisRot);
}
void  ArcBall::setBounds(GLfloat NewWidth, GLfloat NewHeight)
{
	//Set adjustment factor for width/height
	this->AdjustWidth  = 1.0f / ((NewWidth  - 1.0f) * 0.5f);
	this->AdjustHeight = 1.0f / ((NewHeight - 1.0f) * 0.5f);
}
void ArcBall::mousemotion(const Vector* mouspos)
{
	if(isDragging){
		drag(mouspos);
	}
	/*else if(isZooming && mouspos->y!=oldpos.y){
		Vector z;
		Vector t;
		
		_mapToSphere(mouspos, &z);
		z.z +=1;

		z.z *= (mouspos->y-oldpos.y)*0.005;
		translate(0, 0, z.z);
	}*/
	/*else if(isTranslate){
		Vector trans;
		trans.x = (mouspos->x-oldpos.x)*0.125;
		trans.y = (mouspos->y-oldpos.y)*0.125;

		oldpos.x = mouspos->x;
		oldpos.y = mouspos->y;
		
		translate(trans.x, -trans.y, 0);
	}*/
}

void ArcBall::clickDrag(const Vector* mouspos)
{
	isDragging = true;
	
	// Set Last Static Rotation To Last Dynamic One
	LastRot = ThisRot;

    //Map the point to the sphere
    this->_mapToSphere(mouspos, &this->StVec);
}
void ArcBall::clickZoom(const Vector* mouspos)
{
	 oldpos.x = mouspos->x;
	 oldpos.y = mouspos->y;

	 isZooming = true;
}
void ArcBall::clickTranslate(const Vector* mouspos)
{
	oldpos.x = mouspos->x;
	oldpos.y = mouspos->y;

	isTranslate = true;
}
void ArcBall::addRotation(Matrix3fT mat)
{
	LastRot = ThisRot;

	Matrix3fMulMatrix3f(&mat, &LastRot);				// Accumulate Last Rotation Into This One
	Matrix4fSetRotationFromMatrix3f(&Transform, &mat);	// Set Our Final Transform's Rotation From This One
}

void ArcBall::rotateY(double angle)
{
	LastRot = ThisRot;

	Vector up = Vector(0,1,0);
	Quaternion newrot;
	newrot.from_angle(up, angle);

	Matrix3fSetRotationFromQuat4f(&ThisRot, &newrot.quat);	// Convert Quaternion Into Matrix3fT
	Matrix3fMulMatrix3f(&ThisRot, &LastRot);				// Accumulate Last Rotation Into This One
	Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);	// Set Our Final Transform's Rotation From This One
}
void ArcBall::rotateZ(double angle)
{
	LastRot = ThisRot;

	Vector up = Vector(0,0,1);
	Quaternion newrot;
	newrot.from_angle(up, angle);
	
	Matrix3fSetRotationFromQuat4f(&ThisRot, &newrot.quat);	// Convert Quaternion Into Matrix3fT
	Matrix3fMulMatrix3f(&ThisRot, &LastRot);				// Accumulate Last Rotation Into This One
	Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);	// Set Our Final Transform's Rotation From This One
}
void ArcBall::rotateX(double angle)
{
	LastRot = ThisRot;

	Vector up = Vector(1,0,0);
	Quaternion newrot;
	newrot.from_angle(up, angle);
	
	Matrix3fSetRotationFromQuat4f(&ThisRot, &newrot.quat);	// Convert Quaternion Into Matrix3fT
	Matrix3fMulMatrix3f(&ThisRot, &LastRot);				// Accumulate Last Rotation Into This One
	Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);	// Set Our Final Transform's Rotation From This One
}

//Mouse drag, calculate rotation
void ArcBall::drag(const Vector* NewPt)
{
	Vector NewRot;
    
	//Map the point to the sphere
    this->_mapToSphere(NewPt, &this->EnVec);

    //Return the quaternion equivalent to the rotation
    Vector  Perp;

    //Compute the vector perpendicular to the begin and end vectors
    Perp.cross(this->StVec, this->EnVec);

    //Compute the length of the perpendicular vector
    if (Perp.length() > Epsilon)    //if its non-zero
    {
        //We're ok, so return the perpendicular vector as the transform after all
        if(orien_type==0 || orien_type==1 || orien_type==2)
		{
			NewRot.x = Perp.x;
			NewRot.y = Perp.y;
			NewRot.z = Perp.z;
		}
		else if(orien_type==3 || orien_type==4)
		{
			NewRot.x = Perp.z;
			NewRot.y = Perp.y;
			NewRot.z = Perp.x;

		}
		else if(orien_type==5 || orien_type==6)
		{
			NewRot.x = Perp.x;
			NewRot.y = Perp.z;
			NewRot.z = Perp.y;
		}

        //In the quaternion values, w is cosine (theta / 2), where theta is rotation angle
        NewRot.w = this->StVec.dot(this->EnVec);
    }
    else                                    //if its zero
    {
        //The begin and end vectors coincide, so return an identity transform
        NewRot.x = 
        NewRot.y = 
        NewRot.z = 
        NewRot.w = 0.0f;
    }

	Matrix3fSetRotationFromQuat4f(&ThisRot, &NewRot);		// Convert Quaternion Into Matrix3fT
	Matrix3fMulMatrix3f(&ThisRot, &LastRot);				// Accumulate Last Rotation Into This One
	Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);	// Set Our Final Transform's Rotation From This One
}

void ArcBall::translate(float x, float y, float z)
{
    Transform.s.TX += x;
    Transform.s.TY += y; 
    Transform.s.TZ += z;
}
const ArcBall& ArcBall::operator= (const ArcBall& right)
{
	rot_type = right.rot_type;
	orien_type = right.orien_type;
	isDragging = right.isDragging;
	isZooming = right.isZooming;
	isTranslate = right.isTranslate;

	oldpos = right.oldpos;
	Transform = right.Transform;
	LastRot = right.LastRot;
	ThisRot = right.ThisRot;

	StVec = right.StVec;
	EnVec = right.EnVec;
	//AdjustWidth = right.AdjustWidth;
	//AdjustHeight = right.AdjustHeight;

	return *this;
}
