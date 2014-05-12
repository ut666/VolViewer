/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "PlaneWidget.h"

PlaneWidget::PlaneWidget()
{
	arcball.setBounds(800, 800);
/*	v1 = Vector(-128.0, -128.0, 0.0);
	v2 = Vector( 128.0, -128.0, 0.0);
	v3 = Vector( 128.0,  128.0, 0.0);
	v4 = Vector(-128.0,  128.0, 0.0);*/

	plane_equation[0] = plane_equation[1] = plane_equation[2] = plane_equation[3] = 0.0;
	mouseMove(Vector(0.0, 0.0, 0.0));
	translate = angleX = angleY = 0.0;

    perpX = Vector(1,0,0);
    perpY = Vector(0,1,0);

    axisX = Vector(1,0,0);
    axisY = Vector(0,1,0);
    axisZ = Vector(0,0,1);
}
PlaneWidget::~PlaneWidget()
{
}
PlaneWidget::PlaneWidget(Vector _V1, Vector _V2, Vector _V3, Vector _V4, int mode)
{
	arcball.setBounds(800, 800);
	arcball.orien_type = mode;

	v1 = _V1;
	v2 = _V2;
	v3 = _V3;
	v4 = _V4;

	plane_equation[0] = plane_equation[1] = plane_equation[2] = plane_equation[3] = 0.0;
	normal.get_normal(_v1, _v2, _v3);
	mouseMove(Vector(0.0, 0.0, 0.0));

	translate = angleX = angleY = 0.0;

	perpX = Vector(1,0,0);
	perpY = Vector(0,1,0);
}

void PlaneWidget::mouseMove(Vector mousepos)
{
	float dy=0;

	if(arcball.rot_type==1)
	{
		mousepos.x = 0;
		arcball.mousemotion(&mousepos);	
	}
	if(arcball.rot_type==2)
	{
//		mousepos.x = 0;
//		mousepos.x = mousepos.y;
		mousepos.y=0;
		arcball.mousemotion(&mousepos);
	}
	if(arcball.rot_type==3)
	{
		dy = (glob_mouse.y - mousepos.y);
		glob_mouse = mousepos;

	    arcball.Transform.s.TX += dy*normal.x;
        arcball.Transform.s.TY += dy*normal.y; 
        arcball.Transform.s.TZ += dy*normal.z;		
	}

	_v1 = v1; _v2 = v2; _v3 = v3; _v4 = v4;

	if( (arcball.orien_type==1 || arcball.orien_type == 2)) _v1.z = _v2.z = _v3.z = _v4.z = 0.0;
	if( (arcball.orien_type==3 || arcball.orien_type == 4)) _v1.x = _v2.x = _v3.x = _v4.x = 0.0;
	if( (arcball.orien_type==5 || arcball.orien_type == 6)) _v1.y = _v2.y = _v3.y = _v4.y = 0.0;

	Matrix4fMulVector4f(arcball.Transform, &_v1);
	Matrix4fMulVector4f(arcball.Transform, &_v2);
	Matrix4fMulVector4f(arcball.Transform, &_v3);
	Matrix4fMulVector4f(arcball.Transform, &_v4);

	if( (arcball.orien_type==1 || arcball.orien_type == 2))
	{
		_v1.z += v1.z;
		_v2.z += v2.z;
		_v3.z += v3.z;
		_v4.z += v4.z;
	}
	if( (arcball.orien_type==3 || arcball.orien_type == 4))
	{
		_v1.x += v1.x;
		_v2.x += v2.x;
		_v3.x += v3.x;
		_v4.x += v4.x;
	}
	if( (arcball.orien_type==5 || arcball.orien_type == 6))
	{
		_v1.y += v1.y;
		_v2.y += v2.y;
		_v3.y += v3.y;
		_v4.y += v4.y;
	}

	eval_planeequation();
}

void PlaneWidget::mousePress(Vector mousepos, int type)
{
	arcball.rot_type = type;
		
	if(arcball.rot_type==1) mousepos.x = 0;
	if(arcball.rot_type==2) mousepos.y = 0;

	arcball.clickDrag(&mousepos);
	glob_mouse = mousepos;
}

void PlaneWidget::eval_planeequation(void)
{
	centre = (_v1+_v2+_v3+_v4)/4.0f;

	normal.get_normal(_v1, _v2, _v3);

	//A = y1 (z2 - z3)      + y2 (z3 - z1)        + y3 (z1 - z2)
	plane_equation[0] = _v1.y*(_v2.z-_v3.z) + _v2.y*(_v3.z-_v1.z) + _v3.y*(_v1.z-_v2.z);

					//B = z1 (x2 - x3)      + z2 (x3 - x1)        + z3 (x1 - x2) 
	plane_equation[1] = _v1.z*(_v2.x-_v3.x) + _v2.z*(_v3.x-_v1.x) + _v3.z*(_v1.x-_v2.x);

					//C = x1 (y2 - y3)      + x2 (y3 - y1)        + x3 (y1 - y2) 
	plane_equation[2] = _v1.x*(_v2.y-_v3.y)+ _v2.x*(_v3.y-_v1.y) + _v3.x*(_v1.y-_v2.y);

					//- D = x1 (y2 z3 - y3 z2)            + x2 (y3 z1 - y1 z3)              + x3 (y1 z2 - y2 z1)
	float d = _v1.x*(_v2.y*_v3.z - _v3.y*_v2.z) + _v2.x*(_v3.y*_v1.z-_v1.y*_v3.z) + _v3.x*(_v1.y*_v2.z - _v2.y*_v1.z);
	plane_equation[3] = -d;
}

void PlaneWidget::mouseRelease()
{
	arcball.isDragging = false;
	arcball.rot_type = 0;
}

void PlaneWidget::display3d(int surface)
{
/*	Vector v_offset = (centre - planes[0].centre);
	
	if(i==0) 	glTranslatef(v_offset.x, v_offset.y, v_offset.z);
	else if(i==1) glTranslatef(-v_offset.x, -v_offset.y, -v_offset.z);

	glColor3f(1.0, 1.0, 1.0);
	glBegin(surface);
		glVertex3fv(&_v1.x);
		glVertex3fv(&_v2.x);
		glVertex3fv(&_v3.x);
		glVertex3fv(&_v4.x);
	glEnd();
*/
}
void PlaneWidget::translate_by_vec(Vector t)
{
	centre += t;

	v1 += t;
	v2 += t;
	v3 += t;
	v4 += t;

	_v1 += t;
	_v2 += t;
	_v3 += t;
	_v4 += t;

	eval_planeequation();
}

void PlaneWidget::display_points()
{
	Vector p1, p2, p3;
	float interp1, interp2;

	int grid_size=25;

	glPointSize(2.0);
	glBegin(GL_POINTS);
	for(int i=0; i<=grid_size; i++)
	{
		interp1 = (float)i/(float)grid_size;

		p1.x = interp1*_v1.x+(1.0-interp1)*_v2.x;
		p1.y = interp1*_v1.y+(1.0-interp1)*_v2.y;
		p1.z = interp1*_v1.z+(1.0-interp1)*_v2.z;

		p2.x = interp1*_v4.x+(1.0-interp1)*_v3.x;
		p2.y = interp1*_v4.y+(1.0-interp1)*_v3.y;
		p2.z = interp1*_v4.z+(1.0-interp1)*_v3.z;

		for(int j=0; j<=grid_size; j++)
		{
			interp2 = (float)j/(float)grid_size;
	
			p3.x = interp2*p1.x+(1.0-interp2)*p2.x;
			p3.y = interp2*p1.y+(1.0-interp2)*p2.y;
			p3.z = interp2*p1.z+(1.0-interp2)*p2.z;

			glVertex3fv(&p3.x);
		}
	}
	glEnd();
}
void PlaneWidget::display(int surface)
{	
	glBegin(surface);
		glVertex3fv(&_v1.x);
		glVertex3fv(&_v2.x);
		glVertex3fv(&_v3.x);
		glVertex3fv(&_v4.x);
	glEnd();

/*
	glBegin(GL_POINTS);
		
		glColor3f(1.0, 1.0, 0.0);
		glVertex3fv(&_v1.x);
		
		glColor3f(1.0, 1.0, 1.0);
		glVertex3fv(&_v2.x);
		
		glColor3f(1.0, 1.0, 1.0);
		glVertex3fv(&_v3.x);
		
		glColor3f(1.0, 1.0, 0.0);
		glVertex3fv(&_v4.x);

	glEnd();
	*/
/*	Vector vn = centre + (abs(v1.z)/3.0f)*normal;

	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_LINES);
		glVertex3fv(&centre.x);
		glVertex3fv(&vn.x);
	glEnd();
*/
/*
	if(arcball.rot_type==1)
	{
		Vector l1, l2;	
		l1 = (_v1 + _v4)*0.5;
		l2 = (_v2 + _v3)*0.5;
		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_LINES);
			glVertex3fv(&l1.x);
			glVertex3fv(&l2.x);
		glEnd();
	}
	else if(arcball.rot_type==2)
	{
		Vector l1, l2;	
		l1 = (_v1 + _v2)*0.5;
		l2 = (_v3 + _v4)*0.5;
		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_LINES);
			glVertex3fv(&l1.x);
			glVertex3fv(&l2.x);
		glEnd();
	}
*/

}

const PlaneWidget& PlaneWidget::operator= (const PlaneWidget& right)
{
	//plane equation
	plane_equation[0] = right.plane_equation[0];
	plane_equation[1] = right.plane_equation[1];
	plane_equation[2] = right.plane_equation[2];
	plane_equation[3] = right.plane_equation[3];

	//arcball
	arcball = right.arcball;

	//mouse, normal, centre
	glob_mouse = right.glob_mouse;
	normal = right.normal;
	centre = right.centre;

	//plane geometry
	v1 = right.v1; v2 = right.v2; v3 = right.v3; v4 = right.v4;
	_v1 = right._v1; _v2 = right._v2; _v3 = right._v3; _v4 = right._v4;

	angleX = right.angleX;
	angleY = right.angleY; 
	translate = right.translate;

/*	perpX = right.perpX;
	perpY = right.perpY;

	axisX = right.axisX;
	axisY = right.axisY;
	axisZ = right.axisZ;
*/
	rot_mat = right.rot_mat;

	return *this;
}
