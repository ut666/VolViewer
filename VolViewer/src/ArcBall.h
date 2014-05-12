/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef ARCBALL_H
#define ARCBALL_H

#include <iostream>
#include <qgl.h> 

//assuming IEEE-754(GLfloat), which i believe has max precision of 7 bits
#define Epsilon 1.0e-5

#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"

class ArcBall
{

	public:

		ArcBall();
		ArcBall(GLfloat NewWidth, GLfloat NewHeight);
		~ArcBall();
		
		int rot_type;
		int orien_type;

		bool isDragging;
		bool isZooming;
		bool isTranslate;

		Vector oldpos;

		//Final Transform
		Matrix4fT   Transform;
		//Last Rotation
		Matrix3fT   LastRot;
		//This Rotation
		Matrix3fT   ThisRot;

		Vector		StVec;				//Saved click vector
		Vector		EnVec;				//Saved drag vector
		GLfloat     AdjustWidth;    //Mouse bounds width
		GLfloat     AdjustHeight;   //Mouse bounds height

		void	setBounds(GLfloat NewWidth, GLfloat NewHeight);
		void	clickDrag(const Vector* mouspos);
		void	clickZoom(const Vector* mouspos);
		void	clickTranslate(const Vector* mouspos);
		void	mousemotion(const Vector* mouspos);
		void    drag(const Vector* NewPt);
		void	translate(float x, float y, float z);
		inline void _mapToSphere(const Vector* NewPt, Vector* NewVec) const;
		void	setIdentity(void);

		void	addRotation(Matrix3fT mat);

		void	rotateX(double angle);
		void	rotateY(double angle);
		void	rotateZ(double angle);

		const ArcBall& operator= (const ArcBall& right);
};

#endif	//ARCBALL_H

