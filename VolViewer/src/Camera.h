/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef CAMERA_H
#define CAMERA_H

#include <qgl.h>
//Added by qt3to4:
#include <QMouseEvent>

#include "Vector.h"
#include "Quaternion.h"


class Camera
{
public:

   Camera();
   Camera(const Vector & eye, const Vector & at, const Vector & up);
   Camera(const Vector & eye, const Vector & at);
   Camera(const Vector & eye);

   void set(const Vector & eye, const Vector & at, const Vector & up);
   void set(const Vector & eye, const Vector & at);
   void set(const Vector & eye);

   void update(const Vector & e, const Vector & m);
   void display_cursor(void);

   void moveUp(float dist);
   void moveForward(float dist);
   void moveLeft(float dist);
   void pitchUp(double angle);
   void headingLeft(double angle);
   void rotateLeft(double angle);
   void rotateUp(double angle);
   void zoomIn(float dist);

   void aim();
   void idle();
   void reshape(int w, int h);

   void mouse_move(QMouseEvent * e );
   void mouse_press(QMouseEvent * e );
   void mouse_release(void);

   //third person style mouse look
   void mouseLook(double heading, double pitch);
   void mouseRotate(double x_angle, double y_angle);

	// Eye, At, Up Vectors for gluLookAt()
	Vector eye;
	Vector eyeOld;
	Vector eyeNew;
	Vector at;
	Vector atOld;
	Vector atNew;
	Vector up;
	
	// Current value of step counter
	int steps;
	// Maximum number of steps for a smooth movement
	int maxSteps;
	// Mode for motion
	bool smooth;

	float fov, znear, zfar, zscreen, ratio;
	int width, height;

	int mouse_active;
	int cursor_id;
	int mode;
	Vector mouse_centre, mouse_current;

	const Camera& operator= (const Camera& right);

};

#endif //CAMERA_H