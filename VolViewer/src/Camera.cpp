/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "Camera.h"
//Added by qt3to4:
#include <QEvent>
#include <QMouseEvent>
#include <stdlib.h>
#include <glut.h>

Vector J(0, 1, 0);

// Constants for class Camera
Vector EYE(0, 0, 1);
Vector AT(0, 0, 0);
Vector UP(0, 1, 0);


// Constructors for class Camera
Camera::Camera():
   eye(EYE), 
   eyeOld(EYE), 
   eyeNew(EYE), 
   at(AT), 
   atOld(AT), 
   atNew(AT), 
   up(UP),
   steps(0),
   maxSteps(50),
   ratio(1.0),
   smooth(true)
   { }

Camera::Camera(const Vector & eye, const Vector & at, const Vector & up) : 
   eye(eye), 
   eyeOld(eye), 
   eyeNew(eye), 
   at(at), 
   atOld(at), 
   atNew(at), 
   up(up.unit()),
   steps(0),
   maxSteps(50),
   ratio(1.0),
   smooth(true)
{ }

Camera::Camera(const Vector & eye, const Vector & at) : 
   eye(eye), 
   eyeOld(eye), 
   eyeNew(eye), 
   at(at), 
   atOld(at), 
   atNew(at), 
   up(J),
   steps(0),
   maxSteps(50),
   ratio(1.0),
   smooth(true)
{ }

Camera::Camera(const Vector & eye) : 
   eye(eye), 
   eyeOld(eye), 
   eyeNew(eye), 
   at(AT), 
   atOld(AT), 
   atNew(AT), 
   up(J),
   steps(0),
   maxSteps(50),
    ratio(1.0),
  smooth(true)
{ }


// Member functions for class Camera
void Camera::set(const Vector & e, const Vector & m, const Vector & u)
{
   eye = e;
   eyeOld = e;
   eyeNew = e;
   at = m;
   atOld = m;
   atNew = m;
   up = u.unit();

   fov = 45;
   width = 800;
   height = 600; 
   znear = 1.0f;
   zfar = 10000.0f;
   maxSteps = 50;
   zscreen = 10.0f;
   smooth = false;
}

void Camera::set(const Vector & e, const Vector & m)
{
   eye = e;
   eyeOld = e;
   eyeNew = e;
   at = m;
   atOld = m;
   atNew = m;
   up = J;

   fov = 90;
   width = 800;
   height = 600; 
   znear = 1.0f;
   zfar = 10000.0f;
   maxSteps = 50;
   zscreen = 10.0f;
   ratio = 1.0;
   smooth = false;
}

void Camera::set(const Vector & e)
{
	// Constants for class Camera
	/*Vector EYE(0, 0, 1);
	Vector AT(0, 0, 0);
	Vector UP(0, 1, 0);
	Vector J(0, 1, 0);*/

   eye = e;
   eyeOld = e;
   eyeNew = e;
   at = AT;
   atOld = AT;
   atNew = AT;
   up = UP;

   fov = 90;
   width = 800;
   height = 600; 
   znear = 1.0f;
   zfar = 10000.0f;
   maxSteps = 50;
   zscreen = 10.0f;
   ratio = 1.0;
   smooth = false;
}

void Camera::aim()
{
   gluLookAt
   (
      eye[0], eye[1], eye[2], 
      at[0], at[1], at[2], 
      up[0], up[1], up[2]
   );

   display_cursor();
}
void Camera::display_cursor(void)
{
	glPushMatrix();
	 	glTranslatef(at.x, at.y, at.z);
		
		Vector v1, v2, v3, v4, v5, v6;
		v3  = Vector(0.5, 0, 0);
		v4  = Vector(-0.5, 0, 0);
		v1  = Vector(0, 0.5, 0);
		v2  = Vector(0, -0.5, 0);
		v5  = Vector(0, 0, 0.5);
		v6  = Vector(0, 0, -0.5);

		glBegin(GL_LINES);
			
			glColor3f(1,1,1);
			glVertex3fv(&v1.x);
			glVertex3fv(&v2.x);
			
			glColor3f(1,1,1);
			glVertex3fv(&v3.x);
			glVertex3fv(&v4.x);	
			
			glColor3f(1,1,1);
			glVertex3fv(&v5.x);
			glVertex3fv(&v6.x);
		glEnd();
		
	glPopMatrix();
}
void Camera::reshape(int w, int h)
{
	if(w == 0) w = 1; if(h == 0) h = 1;  
	width = w;	height = h;

	mouse_centre.x = width/2; mouse_centre.y = height/2;

	glViewport (0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, width/height, znear, zfar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();
}

void Camera::update(const Vector & e,const Vector & m)
{
   eyeNew = e;
   atNew = m;
   if (smooth)
   {
      if (steps > 0)
      {
         eyeOld = eye;
         atOld = at;
      }
      steps = maxSteps;
      
   }
   else
   {
      eye = eyeNew;
      at = atNew;
   }
}

void Camera::moveUp(float dist)
{
   Vector disp = dist * up;
   update(eyeOld + disp, atOld + disp);
}

void Camera::moveForward(float dist)
{
   Vector disp = dist * (at - eye).unit();
   update(eyeOld + disp, atOld + disp);
}

void Camera::moveLeft(float dist)
{
   Vector temp;
   temp.cross(up, at - eye);
   Vector disp = dist * temp.unit();
   update(eyeOld + disp, atOld + disp);
}
void Camera::pitchUp(double angle)
{
	Vector temp;
	temp.cross(eyeOld - atOld, up);
	Vector axis = temp.unit();
	Quaternion rot;
	rot.from_angle(axis, angle);
	update(eyeOld, eyeOld + rot.apply(atOld - eyeOld));
}

void Camera::headingLeft(double angle)
{
	Quaternion rot;
	rot.from_angle(up, angle);
	update(eyeOld, eyeOld + rot.apply(atOld - eyeOld));
}
void Camera::rotateLeft(double angle)
{
	Quaternion rot;
	rot.from_angle(up, angle);
	update(atOld+rot.apply(eyeOld-atOld), atOld );
}
void Camera::rotateUp(double angle)
{
	Vector temp;
	temp.cross(eyeOld - atOld, up);
	Vector axis = temp.unit();
	Quaternion rot;
	rot.from_angle(axis, angle);
	update(atOld + rot.apply(eyeOld-atOld), atOld);
}

void Camera::zoomIn(float dist)
{
   Vector disp = dist * (at - eye).unit();
   update(eyeOld + disp, atOld);
}

void Camera::idle()
{
   if (steps > 0)
   {
      float t = float(steps) / float(maxSteps);
      eye = eyeNew + t * (eyeOld - eyeNew);
      at = atNew + t * (atOld - atNew);
      steps--;
   }
   else
   {
      eyeOld = eyeNew;
      atOld = atNew;
      steps = 0;
   }
}

void Camera::mouse_move(QMouseEvent* e)
{	
	int x = e->x();
	int y = e->y();
	
	if(mouse_active==0) return;

	if(mouse_active==1)
	{
		int deltaX( x - mouse_centre.x );
		int deltaY( y - mouse_centre.y );
		if( deltaX == 0 && deltaY == 0 ) return;

		mouseLook(-deltaX*0.125, deltaY*0.125);
		//glutWarpPointer( mouse_centre.x, mouse_centre.y );
	}
	if(mouse_active==2)
	{
		int deltaX( x - mouse_current.x );
		int deltaY( y - mouse_current.y );
		if( deltaX == 0 && deltaY == 0 ) return;
		
		mouse_current.x = x;
		mouse_current.y = y;

		mouseRotate(-deltaY*0.125, deltaX*0.125);
	}
}

void Camera::mouse_press(QMouseEvent* e)
{
 	int x = e->x();
	int y = e->y();

	if ((e->button()&Qt::RightButton)) 
	{
		mouse_active=1;
		//cursor_id = glutGet( GLUT_WINDOW_CURSOR );
		//glutSetCursor( GLUT_CURSOR_NONE );
    }
	if((e->button()&Qt::LeftButton))
	{
		mouse_current.x=x;
		mouse_current.y=y;
		mouse_active=2;
	 }
}

void Camera::mouse_release(void)
{
	mouse_active=0;
	//glutSetCursor(cursor_id);
}

void Camera::mouseRotate(double x_angle, double y_angle)
{
	Vector temp;
	temp.cross(eyeOld - atOld, up);
	Vector axis = temp.unit();

	Quaternion x_rot, y_rot, rot;
	
	x_rot.from_angle(up, y_angle);
	y_rot.from_angle(axis, x_angle);

	rot = x_rot * y_rot;
	
	update(atOld + rot.apply(eyeOld-atOld), atOld);
}

void Camera::mouseLook( double heading_angle, double pitch_angle)
{
	Vector temp;
	temp.cross(eyeOld - atOld, up);
	Vector axis = temp.unit();

	Quaternion pitch_rot, heading_rot, rot;
	
	pitch_rot.from_angle(axis, pitch_angle);
	heading_rot.from_angle(up, heading_angle);
	
	rot = pitch_rot * heading_rot;

	update(eyeOld, eyeOld + rot.apply(atOld - eyeOld));
}
const Camera& Camera::operator= (const Camera& right)
{
	// Eye, At, Up Vectors for gluLookAt()
	eye = right.eye;
	eyeOld = right.eyeOld;
	eyeNew = right.eyeNew;
	at = right.at;
	atOld = right.atOld;
	atNew = right.atNew;
	up = right.up;
	
	// Current value of step counter
	steps = right.steps;
	// Maximum number of steps for a smooth movement
	maxSteps = right.maxSteps;
	// Mode for motion
	smooth = right.smooth;

	fov = right.fov;
	znear = right.znear;
	zfar = right.zfar;
	zscreen = right.zscreen;
	ratio = right.ratio;
	
	//width = right.width;
	//height = right.height;

	mouse_active = right.mouse_active;
	cursor_id = right.cursor_id;
	mode = right.mode;
	mouse_centre = right.mouse_centre;
	mouse_current = right.mouse_current;

	return *this;
}