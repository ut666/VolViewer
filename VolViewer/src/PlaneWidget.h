/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef PLANEWIDGET_H
#define PLANEWIDGET_H
#include <qgl.h>
#include <vector>
#include <fstream>

using namespace std;

#include "Vector.h"
#include "ArcBall.h"
#include "Matrix4x4.h"

class PlaneWidget
{
public:

	//normal of the plane
	Vector normal;

	//centre of the plane
	Vector centre;

	//equation of the plane
	//ax + by + cz + d = 0
	GLdouble plane_equation[4];

	//camera manip helpers
	ArcBall arcball;
	Vector glob_mouse;

	float angleX;
	float angleY;
	float translate;

	Vector perpX;
	Vector perpY;

	Vector axisX;
	Vector axisY;
	Vector axisZ;

	Matrix4x4 rot_mat;

	Vector v1;
	Vector v2;
	Vector v3;
	Vector v4;

	Vector _v1;
	Vector _v2;
	Vector _v3;
	Vector _v4;
	
	PlaneWidget();
	PlaneWidget(Vector _v1, Vector _v2, Vector _v3, Vector _v4, int mode);

	~PlaneWidget();

	void mouseMove(Vector mousepos);
	void mousePress(Vector mousepos, int type);
	void mouseRelease();

	void display(int surface);
	void display_points();
	void display3d(int surface);
	void eval_planeequation(void);
	void translate_by_vec(Vector t);

	const PlaneWidget& operator= (const PlaneWidget& right);
};

#endif // PLANEWIDGET_H