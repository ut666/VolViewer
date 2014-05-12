/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "PlaneWidgetInCube.h"
#define EPSILON 0.0000001

PlaneWidgetInCube::PlaneWidgetInCube()
{
	renderall=0;
	invert_cut=0;
	arbmat.identity();

	Xsize=Ysize=Zsize=0;

	for(int i=0; i<6; i++) selected[i] = -1;

	moviemode=0;
	rotmode=0;
	render_selected=1;
}
PlaneWidgetInCube::~PlaneWidgetInCube()
{
	clear();
}
void PlaneWidgetInCube::clear(void)
{
	points.clear();
	slice.clear();
	cutpoints.clear();
	viewsorted.clear();
	clipvolcentre_points.clear();

	for(int i=0; i<plane_clippoints.size(); i++)
		plane_clippoints[i].clear();
	plane_clippoints.clear();

	for(int i=0; i<planeplanepoints.size(); i++)
		planeplanepoints[i].clear();
	planeplanepoints.clear();

	for(int i=0; i<planeplanepoints2.size(); i++)
		planeplanepoints2[i].clear();
	planeplanepoints2.clear();
	
	planes.clear();
}

void PlaneWidgetInCube::setup(void)
{
	glClipPlane(GL_CLIP_PLANE0, planes[0].plane_equation);
	glClipPlane(GL_CLIP_PLANE1, planes[1].plane_equation);
	glClipPlane(GL_CLIP_PLANE2, planes[2].plane_equation);
	glClipPlane(GL_CLIP_PLANE3, planes[3].plane_equation);
	glClipPlane(GL_CLIP_PLANE4, planes[4].plane_equation);
	glClipPlane(GL_CLIP_PLANE5, planes[5].plane_equation);
}
void PlaneWidgetInCube::enable(void)
{
	glEnable(GL_CLIP_PLANE0);
	glEnable(GL_CLIP_PLANE1);
	glEnable(GL_CLIP_PLANE2);
	glEnable(GL_CLIP_PLANE3);
	glEnable(GL_CLIP_PLANE4);
	glEnable(GL_CLIP_PLANE5);
}
void PlaneWidgetInCube::disable(void)
{
	glDisable(GL_CLIP_PLANE0);
	glDisable(GL_CLIP_PLANE1);
	glDisable(GL_CLIP_PLANE2);
	glDisable(GL_CLIP_PLANE3);
	glDisable(GL_CLIP_PLANE4);
	glDisable(GL_CLIP_PLANE5);
}

void PlaneWidgetInCube::enable(int id)
{
	if(id==0) glEnable(GL_CLIP_PLANE0);
	if(id==1) glEnable(GL_CLIP_PLANE1);
	if(id==2) glEnable(GL_CLIP_PLANE2);
	if(id==3) glEnable(GL_CLIP_PLANE3);
	if(id==4) glEnable(GL_CLIP_PLANE4);
	if(id==5) glEnable(GL_CLIP_PLANE5);
}
void PlaneWidgetInCube::disable(int id)
{
	if(id==0) glDisable(GL_CLIP_PLANE0);
	if(id==1) glDisable(GL_CLIP_PLANE1);
	if(id==2) glDisable(GL_CLIP_PLANE2);
	if(id==3) glDisable(GL_CLIP_PLANE3);
	if(id==4) glDisable(GL_CLIP_PLANE4);
	if(id==5) glDisable(GL_CLIP_PLANE5);
}
void PlaneWidgetInCube::invert(void)
{
	for(int i=0; i<6; i++)
	{
		planes[i].plane_equation[0] = -planes[i].plane_equation[0];
		planes[i].plane_equation[1] = -planes[i].plane_equation[1];
		planes[i].plane_equation[2] = -planes[i].plane_equation[2];
		planes[i].plane_equation[3] = -planes[i].plane_equation[3];
	}

	setup();
}
void PlaneWidgetInCube::init(float cube_size)
{
	Xsize=cube_size;
	Ysize=cube_size;
	Zsize=cube_size;

	//define a cube by 8 vertices
	v1 = Vector(-1.0,  1.0,  1.0);
	v2 = Vector( 1.0,  1.0,  1.0);
	v3 = Vector(-1.0, -1.0,  1.0);
	v4 = Vector( 1.0, -1.0,  1.0);
	v5 = Vector(-1.0,  1.0, -1.0);
	v6 = Vector( 1.0,  1.0, -1.0);
	v7 = Vector(-1.0, -1.0, -1.0);
	v8 = Vector( 1.0, -1.0, -1.0);

	//scale it by size
	cube_size /= 2.0;
	v1 *= cube_size; v2 *= cube_size; v3 *= cube_size; v4 *= cube_size;
	v5 *= cube_size; v6 *= cube_size; v7 *= cube_size; v8 *= cube_size;

	//define its 6 planes
	PlaneWidget p1(v1,v2,v4,v3,1);
	PlaneWidget p2(v7,v8,v6,v5,2);
	PlaneWidget p3(v2,v6,v8,v4,3);
	PlaneWidget p4(v3,v7,v5,v1,4);
	PlaneWidget p5(v5,v6,v2,v1,5);
	PlaneWidget p6(v4,v8,v7,v3,6);

	//save the 6 planes
	planes.clear();
	planes.push_back(p1);
	planes.push_back(p2);
	planes.push_back(p3);
	planes.push_back(p4);
	planes.push_back(p5);
	planes.push_back(p6);

	for(int i=0; i<plane_clippoints.size(); i++)
		plane_clippoints[i].clear();

	plane_clippoints.resize(6);

	for(int i=0; i<6; i++) selected[i] = -1;

	planes[0].perpX = Vector(1,0,0);
	planes[0].perpY = Vector(0,1,0);
	planes[0].axisX = Vector(1,0,0);
	planes[0].axisY = Vector(0,1,0);
	planes[0].axisZ = Vector(0,0,1);
	
	planes[1].perpX = Vector(1,0,0);
	planes[1].perpY = Vector(0,1,0);
	planes[1].axisX = Vector(1,0,0);
	planes[1].axisY = Vector(0,1,0);
	planes[1].axisZ = Vector(0,0,1);

	planes[2].perpX = Vector(0,1,0);
	planes[2].perpY = Vector(0,0,1);
	planes[2].axisX = Vector(0,1,0);
	planes[2].axisY = Vector(0,0,1);
	planes[2].axisZ = Vector(1,0,0);

	planes[3].perpX = Vector(0,1,0);
	planes[3].perpY = Vector(0,0,1);
	planes[3].axisX = Vector(0,1,0);
	planes[3].axisY = Vector(0,0,1);
	planes[3].axisZ = Vector(1,0,0);

	planes[4].perpX = Vector(0,0,1);
	planes[4].perpY = Vector(1,0,0);
	planes[4].axisX = Vector(0,0,1);
	planes[4].axisY = Vector(1,0,0);
	planes[4].axisZ = Vector(0,1,0);
	
	planes[5].perpX = Vector(0,0,1);
	planes[5].perpY = Vector(1,0,0);
	planes[5].axisX = Vector(0,0,1);
	planes[5].axisY = Vector(1,0,0);
	planes[5].axisZ = Vector(0,1,0);
	
	eval_planeintersections();
}
void PlaneWidgetInCube::translate_by_vec(Vector t)
{
	v1 += t;
	v2 += t;
	v3 += t;
	v4 += t;
	v5 += t;
	v6 += t;
	v7 += t;
	v8 += t;

	for(int i=0; i<planes.size(); i++)
		planes[i].translate_by_vec(t);

}

void PlaneWidgetInCube::init(float cube_sizeX, float cube_sizeY, float cube_sizeZ)
{
	Xsize=cube_sizeX;
	Ysize=cube_sizeY;
	Zsize=cube_sizeZ;

	//define a cube by 8 vertices
	v1 = Vector(-1.0,  1.0,  1.0);
	v2 = Vector( 1.0,  1.0,  1.0);
	v3 = Vector(-1.0, -1.0,  1.0);
	v4 = Vector( 1.0, -1.0,  1.0);
	v5 = Vector(-1.0,  1.0, -1.0);
	v6 = Vector( 1.0,  1.0, -1.0);
	v7 = Vector(-1.0, -1.0, -1.0);
	v8 = Vector( 1.0, -1.0, -1.0);

	//scale it by size
	Vector cube_size = Vector(cube_sizeX, cube_sizeY, cube_sizeZ);
	cube_size /= 2.0;

	v1 = v1*cube_size; 
	v2 = v2*cube_size; 
	v3 = v3*cube_size; 
	v4 = v4*cube_size;
	v5 = v5*cube_size; 
	v6 = v6*cube_size; 
	v7 = v7*cube_size; 
	v8 = v8*cube_size;

	//define its 6 planes
	PlaneWidget p1(v1,v2,v4,v3,1);
	PlaneWidget p2(v7,v8,v6,v5,2);
	PlaneWidget p3(v2,v6,v8,v4,3);
	PlaneWidget p4(v3,v7,v5,v1,4);
	PlaneWidget p5(v5,v6,v2,v1,5);
	PlaneWidget p6(v4,v8,v7,v3,6);

	//save the 6 planes
	planes.clear();
	planes.push_back(p1);
	planes.push_back(p2);
	planes.push_back(p3);
	planes.push_back(p4);
	planes.push_back(p5);
	planes.push_back(p6);

	for(int i=0; i<plane_clippoints.size(); i++)
		plane_clippoints[i].clear();

	plane_clippoints.resize(6);

	for(int i=0; i<6; i++) selected[i] = -1;

	planes[0].perpX = Vector(1,0,0);
	planes[0].perpY = Vector(0,1,0);
	planes[0].axisX = Vector(1,0,0);
	planes[0].axisY = Vector(0,1,0);
	planes[0].axisZ = Vector(0,0,1);
	
	planes[1].perpX = Vector(1,0,0);
	planes[1].perpY = Vector(0,1,0);
	planes[1].axisX = Vector(1,0,0);
	planes[1].axisY = Vector(0,1,0);
	planes[1].axisZ = Vector(0,0,1);

	planes[2].perpX = Vector(0,1,0);
	planes[2].perpY = Vector(0,0,1);
	planes[2].axisX = Vector(0,1,0);
	planes[2].axisY = Vector(0,0,1);
	planes[2].axisZ = Vector(1,0,0);

	planes[3].perpX = Vector(0,1,0);
	planes[3].perpY = Vector(0,0,1);
	planes[3].axisX = Vector(0,1,0);
	planes[3].axisY = Vector(0,0,1);
	planes[3].axisZ = Vector(1,0,0);

	planes[4].perpX = Vector(0,0,1);
	planes[4].perpY = Vector(1,0,0);
	planes[4].axisX = Vector(0,0,1);
	planes[4].axisY = Vector(1,0,0);
	planes[4].axisZ = Vector(0,1,0);
	
	planes[5].perpX = Vector(0,0,1);
	planes[5].perpY = Vector(1,0,0);
	planes[5].axisX = Vector(0,0,1);
	planes[5].axisY = Vector(1,0,0);
	planes[5].axisZ = Vector(0,1,0);
	
	eval_planeintersections();
}
void PlaneWidgetInCube::init(Vector c, Vector size)
{
	Xsize=size.x;
	Ysize=size.y;
	Zsize=size.z;

	//define a cube by 8 vertices
	v1 = Vector(-1.0,  1.0,  1.0);
	v2 = Vector( 1.0,  1.0,  1.0);
	v3 = Vector(-1.0, -1.0,  1.0);
	v4 = Vector( 1.0, -1.0,  1.0);
	v5 = Vector(-1.0,  1.0, -1.0);
	v6 = Vector( 1.0,  1.0, -1.0);
	v7 = Vector(-1.0, -1.0, -1.0);
	v8 = Vector( 1.0, -1.0, -1.0);

	//scale it by size
	Vector cube_size = Vector(Xsize, Ysize, Zsize);
	cube_size /= 2.0;

	v1 = v1*cube_size; 
	v2 = v2*cube_size; 
	v3 = v3*cube_size; 
	v4 = v4*cube_size;
	v5 = v5*cube_size; 
	v6 = v6*cube_size; 
	v7 = v7*cube_size; 
	v8 = v8*cube_size;

	v1 += c;
	v2 += c;
	v3 += c;
	v4 += c;
	v5 += c;
	v6 += c;
	v7 += c;
	v8 += c;

	//define its 6 planes
	PlaneWidget p1(v1,v2,v4,v3,1);
	PlaneWidget p2(v7,v8,v6,v5,2);
	PlaneWidget p3(v2,v6,v8,v4,3);
	PlaneWidget p4(v3,v7,v5,v1,4);
	PlaneWidget p5(v5,v6,v2,v1,5);
	PlaneWidget p6(v4,v8,v7,v3,6);

	//save the 6 planes
	planes.clear();
	planes.push_back(p1);
	planes.push_back(p2);
	planes.push_back(p3);
	planes.push_back(p4);
	planes.push_back(p5);
	planes.push_back(p6);

	for(int i=0; i<plane_clippoints.size(); i++)
		plane_clippoints[i].clear();

	plane_clippoints.resize(6);

	for(int i=0; i<6; i++) selected[i] = -1;

	planes[0].perpX = Vector(1,0,0);
	planes[0].perpY = Vector(0,1,0);
	planes[0].axisX = Vector(1,0,0);
	planes[0].axisY = Vector(0,1,0);
	planes[0].axisZ = Vector(0,0,1);
	
	planes[1].perpX = Vector(1,0,0);
	planes[1].perpY = Vector(0,1,0);
	planes[1].axisX = Vector(1,0,0);
	planes[1].axisY = Vector(0,1,0);
	planes[1].axisZ = Vector(0,0,1);

	planes[2].perpX = Vector(0,1,0);
	planes[2].perpY = Vector(0,0,1);
	planes[2].axisX = Vector(0,1,0);
	planes[2].axisY = Vector(0,0,1);
	planes[2].axisZ = Vector(1,0,0);

	planes[3].perpX = Vector(0,1,0);
	planes[3].perpY = Vector(0,0,1);
	planes[3].axisX = Vector(0,1,0);
	planes[3].axisY = Vector(0,0,1);
	planes[3].axisZ = Vector(1,0,0);

	planes[4].perpX = Vector(0,0,1);
	planes[4].perpY = Vector(1,0,0);
	planes[4].axisX = Vector(0,0,1);
	planes[4].axisY = Vector(1,0,0);
	planes[4].axisZ = Vector(0,1,0);
	
	planes[5].perpX = Vector(0,0,1);
	planes[5].perpY = Vector(1,0,0);
	planes[5].axisX = Vector(0,0,1);
	planes[5].axisY = Vector(1,0,0);
	planes[5].axisZ = Vector(0,1,0);
	
	eval_planeintersections();
}
void PlaneWidgetInCube::init2(float cube_size)
{
	Xsize=cube_size;
	Ysize=cube_size;
	Zsize=cube_size;

	//define a cube by 8 vertices
	v1 = Vector(0.0,  1.0,  1.0);
	v2 = Vector( 1.0,  1.0,  1.0);
	v3 = Vector(0.0, 0.0,  1.0);
	v4 = Vector( 1.0, 0.0,  1.0);
	v5 = Vector(0.0,  1.0, 0.0);
	v6 = Vector( 1.0,  1.0, 0.0);
	v7 = Vector(0.0, 0.0, 0.0);
	v8 = Vector( 1.0, 0.0, 0.0);

	//scale it by size
	v1 *= cube_size; v2 *= cube_size; v3 *= cube_size; v4 *= cube_size;
	v5 *= cube_size; v6 *= cube_size; v7 *= cube_size; v8 *= cube_size;

	//define its 6 planes
	PlaneWidget p1(v1,v2,v4,v3,1);
	PlaneWidget p2(v7,v8,v6,v5,2);
	PlaneWidget p3(v2,v6,v8,v4,3);
	PlaneWidget p4(v3,v7,v5,v1,4);
	PlaneWidget p5(v5,v6,v2,v1,5);
	PlaneWidget p6(v4,v8,v7,v3,6);

	//save the 6 planes
	planes.clear();
	planes.push_back(p1);
	planes.push_back(p2);
	planes.push_back(p3);
	planes.push_back(p4);
	planes.push_back(p5);
	planes.push_back(p6);

	planes[0].perpX = Vector(1,0,0);
	planes[0].perpY = Vector(0,1,0);
	planes[0].axisX = Vector(1,0,0);
	planes[0].axisY = Vector(0,1,0);
	planes[0].axisZ = Vector(0,0,1);
	
	planes[1].perpX = Vector(1,0,0);
	planes[1].perpY = Vector(0,1,0);
	planes[1].axisX = Vector(1,0,0);
	planes[1].axisY = Vector(0,1,0);
	planes[1].axisZ = Vector(0,0,1);

	planes[2].perpX = Vector(0,1,0);
	planes[2].perpY = Vector(0,0,1);
	planes[2].axisX = Vector(0,1,0);
	planes[2].axisY = Vector(0,0,1);
	planes[2].axisZ = Vector(1,0,0);

	planes[3].perpX = Vector(0,1,0);
	planes[3].perpY = Vector(0,0,1);
	planes[3].axisX = Vector(0,1,0);
	planes[3].axisY = Vector(0,0,1);
	planes[3].axisZ = Vector(1,0,0);

	planes[4].perpX = Vector(0,0,1);
	planes[4].perpY = Vector(1,0,0);
	planes[4].axisX = Vector(0,0,1);
	planes[4].axisY = Vector(1,0,0);
	planes[4].axisZ = Vector(0,1,0);
	
	planes[5].perpX = Vector(0,0,1);
	planes[5].perpY = Vector(1,0,0);
	planes[5].axisX = Vector(0,0,1);
	planes[5].axisY = Vector(1,0,0);
	planes[5].axisZ = Vector(0,1,0);
	eval_planeintersections();
}
void PlaneWidgetInCube::reset_selected(void)
{
	//define a cube by 8 vertices
	v1 = Vector(-1.0,  1.0,  1.0);
	v2 = Vector( 1.0,  1.0,  1.0);
	v3 = Vector(-1.0, -1.0,  1.0);
	v4 = Vector( 1.0, -1.0,  1.0);
	v5 = Vector(-1.0,  1.0, -1.0);
	v6 = Vector( 1.0,  1.0, -1.0);
	v7 = Vector(-1.0, -1.0, -1.0);
	v8 = Vector( 1.0, -1.0, -1.0);

	//scale it by size
	Vector cube_size = Vector(Xsize, Ysize, Zsize);
	cube_size /= 2.0;

	v1 = v1*cube_size; 
	v2 = v2*cube_size; 
	v3 = v3*cube_size; 
	v4 = v4*cube_size;
	v5 = v5*cube_size; 
	v6 = v6*cube_size; 
	v7 = v7*cube_size; 
	v8 = v8*cube_size;

	//define its 6 planes
	PlaneWidget p1(v1,v2,v4,v3,1);
	PlaneWidget p2(v7,v8,v6,v5,2);
	PlaneWidget p3(v2,v6,v8,v4,3);
	PlaneWidget p4(v3,v7,v5,v1,4);
	PlaneWidget p5(v5,v6,v2,v1,5);
	PlaneWidget p6(v4,v8,v7,v3,6);

	//resetthe current plane


//	for(int i=0; i<6; i++)
//	{

		if(selected[0]==1)
		{
			planes[0] = p1;
			plane_clippoints[0].clear();
			planes[0].perpX = Vector(1,0,0);
			planes[0].perpY = Vector(0,1,0);
			planes[0].axisX = Vector(1,0,0);
			planes[0].axisY = Vector(0,1,0);
			planes[0].axisZ = Vector(0,0,1);
		}
		if(selected[1]==1)
		{
			planes[1] = p2;
			plane_clippoints[1].clear();
			planes[1].perpX = Vector(1,0,0);
			planes[1].perpY = Vector(0,1,0);
			planes[1].axisX = Vector(1,0,0);
			planes[1].axisY = Vector(0,1,0);
			planes[1].axisZ = Vector(0,0,1);
		}
		if(selected[2]==1)
		{
			planes[2] = p3;
			plane_clippoints[2].clear();
			planes[2].perpX = Vector(0,1,0);
			planes[2].perpY = Vector(0,0,1);
			planes[2].axisX = Vector(0,1,0);
			planes[2].axisY = Vector(0,0,1);
			planes[2].axisZ = Vector(1,0,0);
		}
		if(selected[3]==1)
		{
			planes[3] = p4;
			plane_clippoints[3].clear();
			planes[3].perpX = Vector(0,1,0);
			planes[3].perpY = Vector(0,0,1);
			planes[3].axisX = Vector(0,1,0);
			planes[3].axisY = Vector(0,0,1);
			planes[3].axisZ = Vector(1,0,0);
		}
		if(selected[4]==1)
		{
			planes[4] = p5;
			plane_clippoints[4].clear();
			planes[4].perpX = Vector(1,0,0);
			planes[4].perpY = Vector(0,0,1);
			planes[4].axisX = Vector(0,0,1);
			planes[4].axisY = Vector(1,0,0);
			planes[4].axisZ = Vector(0,1,0);
		}
		if(selected[5]==1)
		{
			planes[5] = p6;
			plane_clippoints[5].clear();
			planes[5].perpX = Vector(1,0,0);
			planes[5].perpY = Vector(0,0,1);
			planes[5].axisX = Vector(0,0,1);
			planes[5].axisY = Vector(1,0,0);
			planes[5].axisZ = Vector(0,1,0);
		}
//	}

	eval_planeintersections();
}
void PlaneWidgetInCube::reset_all(void)
{
	//define a cube by 8 vertices
	v1 = Vector(-1.0,  1.0,  1.0);
	v2 = Vector( 1.0,  1.0,  1.0);
	v3 = Vector(-1.0, -1.0,  1.0);
	v4 = Vector( 1.0, -1.0,  1.0);
	v5 = Vector(-1.0,  1.0, -1.0);
	v6 = Vector( 1.0,  1.0, -1.0);
	v7 = Vector(-1.0, -1.0, -1.0);
	v8 = Vector( 1.0, -1.0, -1.0);

	//scale it by size
	Vector cube_size = Vector(Xsize, Ysize, Zsize);
	cube_size /= 2.0;

	v1 = v1*cube_size; 
	v2 = v2*cube_size; 
	v3 = v3*cube_size; 
	v4 = v4*cube_size;
	v5 = v5*cube_size; 
	v6 = v6*cube_size; 
	v7 = v7*cube_size; 
	v8 = v8*cube_size;

	//define its 6 planes
	PlaneWidget p1(v1,v2,v4,v3,1);
	PlaneWidget p2(v7,v8,v6,v5,2);
	PlaneWidget p3(v2,v6,v8,v4,3);
	PlaneWidget p4(v3,v7,v5,v1,4);
	PlaneWidget p5(v5,v6,v2,v1,5);
	PlaneWidget p6(v4,v8,v7,v3,6);

	for(int i=0; i<6; i++)
		selected[i]=-1;

	planes[0] = p1;
	plane_clippoints[0].clear();
	planes[1] = p2;
	plane_clippoints[1].clear();
	planes[2] = p3;
	plane_clippoints[2].clear();
	planes[3] = p4;
	plane_clippoints[3].clear();
	planes[4] = p5;
	plane_clippoints[4].clear();
	planes[5] = p6;
	plane_clippoints[5].clear();

	planes[0].angleX = planes[0].angleY = 
	planes[1].angleX = planes[1].angleY = 
	planes[2].angleX = planes[2].angleY = 
	planes[3].angleX = planes[3].angleY =
	planes[4].angleX = planes[4].angleY =
	planes[5].angleX = planes[5].angleY = 0.0; 

	planes[0].translate = planes[1].translate = planes[2].translate = planes[3].translate = planes[4].translate = planes[5].translate = 0.0;

	planes[0].perpX = Vector(1,0,0);
	planes[0].perpY = Vector(0,1,0);
	planes[0].axisX = Vector(1,0,0);
	planes[0].axisY = Vector(0,1,0);
	planes[0].axisZ = Vector(0,0,1);
	
	planes[1].perpX = Vector(1,0,0);
	planes[1].perpY = Vector(0,1,0);
	planes[1].axisX = Vector(1,0,0);
	planes[1].axisY = Vector(0,1,0);
	planes[1].axisZ = Vector(0,0,1);

	planes[2].perpX = Vector(0,1,0);
	planes[2].perpY = Vector(0,0,1);
	planes[2].axisX = Vector(0,1,0);
	planes[2].axisY = Vector(0,0,1);
	planes[2].axisZ = Vector(1,0,0);

	planes[3].perpX = Vector(0,1,0);
	planes[3].perpY = Vector(0,0,1);
	planes[3].axisX = Vector(0,1,0);
	planes[3].axisY = Vector(0,0,1);
	planes[3].axisZ = Vector(1,0,0);

	planes[4].perpX = Vector(0,0,1);
	planes[4].perpY = Vector(1,0,0);
	planes[4].axisX = Vector(0,0,1);
	planes[4].axisY = Vector(1,0,0);
	planes[4].axisZ = Vector(0,1,0);
	
	planes[5].perpX = Vector(0,0,1);
	planes[5].perpY = Vector(1,0,0);
	planes[5].axisX = Vector(0,0,1);
	planes[5].axisY = Vector(1,0,0);
	planes[5].axisZ = Vector(0,1,0);

	for(int i=0; i<6; i++)
		planes[i].eval_planeequation();

	eval_planeintersections();
}
void PlaneWidgetInCube::select(int i)
{
	if(i>planes.size()) return;
	if(i<0) return;

	//planes[i].translate = planes[i].angleX = planes[i].angleY = 0.0; 
	
	selected[i] = -selected[i];

	eval_planeintersections();
}
void PlaneWidgetInCube::add()
{
	PlaneWidget plane;
	planes.push_back(plane);
}

void PlaneWidgetInCube::remove()
{

}

void PlaneWidgetInCube::display_texmapped(void)
{
	if(moviemode==1) return;
	Vector t1, t2, t3, t4;
		
	t1 = Vector(0.0,0.0, 0.5);
	t2 = Vector(1.0,0.0, 0.5);
	t3 = Vector(1.0,1.0, 0.5);
	t4 = Vector(0.0,1.0, 0.5);

	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_3D);
	glEnable(GL_BLEND);
	for(int i=0; i<planes.size(); i++)
	{
	//	if(i==selected)
	//	{	
			glBegin(GL_QUADS);
				glTexCoord3fv(&t1.x);
				glVertex3fv(&planes[i]._v1.x);
				glTexCoord3fv(&t2.x);
				glVertex3fv(&planes[i]._v2.x);
				glTexCoord3fv(&t3.x);
				glVertex3fv(&planes[i]._v3.x);
				glTexCoord3fv(&t4.x);
				glVertex3fv(&planes[i]._v4.x);
			glEnd();
			
	//	}
	//	else if(renderall)
	//	{
	//		planes[i].display(GL_LINE_LOOP);
	//	}
	}

	glDisable(GL_TEXTURE_3D);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
}
void PlaneWidgetInCube::display(int index, int surface)
{	
	if(moviemode==1) return;

	glBegin(surface);
	for(int j=0; j<plane_clippoints[index].size(); j++)
	{
		glVertex3fv(&plane_clippoints[index][j].x);		
	}
	glEnd();
}
void PlaneWidgetInCube::viewsort(Vector eye)
{
	vector <int> visited;
	visited.resize(6);
	
	viewsorted.clear();
	
	for(int i=0; i<6; i++) visited[i]=0;
	
	float minlength=99999999999999.9;
	Vector v;
	int index_helper = -1;
	
	for(int i=0; i<6; i++)
	{
		minlength=99999999999999.9;
		
		for(int j=0; j<6; j++)
		{			
			if(visited[j]==0)
			{
				v = eye - planes[j].centre;
				
				if(v.length()<minlength)
				{
					index_helper = j;
					minlength = v.length();
				}
			}
		}
		
		if(index_helper!=-1)
		{
			visited[index_helper]=1;
			viewsorted.push_back(index_helper);
		}
	}
}

void PlaneWidgetInCube::display(float linesize,float  pointsize, float bg_oversize, Vector col, Vector eye)
{	
	if(moviemode==1) return;
	
	viewsort(eye);
	
	glDisable(GL_LIGHTING);

	glPointSize(pointsize*0.5);
	glLineWidth(linesize*0.5);
	
	int j;
	
	//if renderall is on show all the clipping planes
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 2.0f);
	if(renderall==1)
	{
		for(int i=0; i<viewsorted.size(); i++)
		{	
			j = viewsorted[i];
			
			if(selected[j]!=1)
			{
				/*if(planes[j].normal.dot(eye)<0.0)
				{
					/*if(col==1.0) glColor3f(0.25, 0.25, 0.25);
					else if(col==0.0) glColor3f(0.75, 0.75, 0.75);

					display(j, GL_TRIANGLE_FAN);	*/
					/*
					if(col==1.0) glColor3f(0.125, 0.125, 0.125);
					else if(col==0.0) glColor3f(0.875, 0.875, 0.875);

					display(j, GL_LINE_LOOP);	
				}
				else*/
				{
					glColor3fv(&col.x);
					display(j, GL_LINE_LOOP);
				}
			}
		}	
		glDisable(GL_POLYGON_OFFSET_LINE);
	}


	/*glColor3f(0.5, 0.5, 0.5);
	for(int j=0; j<plane_clippoints.size(); j++)
	{		
	glBegin(GL_LINE_LOOP);
		for(int i=1; i<plane_clippoints[j].size(); i++)
		{
			glVertex3fv(&plane_clippoints[j][i-1].x);
			glVertex3fv(&plane_clippoints[j][i].x);
			glVertex3fv(&planes[j].centre.x);
		}
		if(plane_clippoints[j].size()>2)
		{
			glVertex3fv(&plane_clippoints[j][0].x);
			glVertex3fv(&plane_clippoints[j][plane_clippoints[j].size()-1].x);
			glVertex3fv(&planes[j].centre.x);
		}
	glEnd();
	}*/

	//for the currently selected plane, we 
	//draw black and yellow lines
	//and the vertices for the plane bounding box intersections.
	for(int i=0; i<planes.size(); i++)
	{
		if(selected[i]==1 && render_selected==1)
		{
			/*glPointSize(pointsize*bg_oversize*4.0);
			glLineWidth(linesize*bg_oversize);
			glColor3fv(&col.x);
			glBegin(GL_LINE_LOOP);
			for(int j=0; j<plane_clippoints[i].size(); j++)
			{
				glVertex3fv(&plane_clippoints[i][j].x);		
			}
			glEnd();*/

			glPointSize(pointsize);
			glLineWidth(linesize);
			glColor3f(1,1,0);
			glBegin(GL_LINE_LOOP);
			for(int j=0; j<plane_clippoints[i].size(); j++)
			{
				glVertex3fv(&plane_clippoints[i][j].x);		
			}
			glEnd();

			/*glColor3f(1,0,0);
			glBegin(GL_POINTS);
			if(planeplanepoints.size())
			{
				for(int j=0; j<planeplanepoints[i].size(); j++)
				{
					glVertex3fv(&planeplanepoints[i][j].x);		
				}
			}
			glEnd();*/

                        /*planes[i].perpX.normalize();
			planes[i].perpY.normalize();

                        Vector v1 = planes[i].centre;
			Vector v2 = planes[i].centre+50.0f*planes[i].perpX;
			glColor3f(1,0,0);
			glBegin(GL_LINES);
				glVertex3fv(&v1.x);
				glVertex3fv(&v2.x);
			glEnd();
			
			v1 = planes[i].centre;
			v2 = planes[i].centre+50.0f*planes[i].perpY;
			glColor3f(0,1,0);
			glBegin(GL_LINES);
				glVertex3fv(&v1.x);
				glVertex3fv(&v2.x);
                        glEnd();

                        glBegin(GL_POINTS);
                                glColor3f(1,0,0);
                                glVertex3fv(&planes[i]._v1.x);
                                glColor3f(1,1,0);
                                glVertex3fv(&planes[i]._v2.x);
                                glColor3f(0,1,0);
                                glVertex3fv(&planes[i]._v3.x);
                                glColor3f(0,0,1);
                                glVertex3fv(&planes[i]._v4.x);
                        glEnd();*/
                        /*if(!plane_clippoints.empty())
			{
				Vector vX = planes[i].centre - plane_clippoints[i][0];
				Vector v = plane_clippoints[i][0] - (0.1f * vX);
		
				glColor3f(1,0,0);
				glBegin(GL_POINTS);
				glVertex3fv(&v.x);
				glEnd();
			}*/
		}
		
		/*
		glColor3f(1,0,0);
		glBegin(GL_POINTS);
		for(int j=0; j<planeplanepoints2[i].size(); j++)
		{
			glVertex3fv(&planeplanepoints2[i][j].x);		
		}
		glEnd();*/
	
	}

	/*
	glPointSize(10.0);
	glBegin(GL_POINTS);

		glColor3f(1,0,0);
		glVertex3fv(&v1.x);
		glVertex3fv(&v2.x);
		glVertex3fv(&v3.x);
		glVertex3fv(&v4.x);
		glVertex3fv(&v5.x);
		glVertex3fv(&v6.x);
		glVertex3fv(&v7.x);
		glVertex3fv(&v8.x);
	glEnd();

	glPointSize(10.0);
	glBegin(GL_POINTS);
		glColor3f(1,1,0);
		glVertex3fv(&bbvmin.x);
		glColor3f(0,1,1);
		glVertex3fv(&bbvmax.x);
	glEnd();
	
	glPointSize(1.0);*/

	glEnable(GL_LIGHTING);
}

void PlaneWidgetInCube::display2()
{	
	if(moviemode==1) return;

	Vector v_offset = (centre - planes[0].centre);
	
	glPushMatrix();
//	for(int i=0; i<planes.size(); i++)
//	{
//		if(i==0) 	glTranslatef(v_offset.x, v_offset.y, v_offset.z);
//		else if(i==1) glTranslatef(-v_offset.x, -v_offset.y, -v_offset.z);

		glTranslatef(v_offset.x, v_offset.y, v_offset.z);

		glColor4f(0.0, 1.0, 0.0, 1.0);
		glPointSize(10.0);

		glBegin(GL_POINTS);
		for(int j=0; j<points.size(); j++)
		{
			glVertex3fv(&points[j].x);		
		}
		glEnd();

		glColor4f(1.0, 1.0, 0.0, 1.0);
		glPointSize(10.0);
		glBegin(GL_POINTS);
			glVertex3fv(&centre.x);	
		glEnd();

		glColor4f(1.0, 1.0, 1.0, 1.0);
		planes[0].display(GL_LINE_LOOP);
	

//	}
	glPopMatrix();


}

void PlaneWidgetInCube::mouseMove(Vector mousepos)
{
	for(int i=0; i<6; i++)
	{
		if(selected[i]==1)
		{
			planes[i].mouseMove(mousepos);
			eval_planeintersections();
		}
	}
}
void PlaneWidgetInCube::mousePress(Vector mousepos, int type)
{
	for(int i=0; i<6; i++)
	{
		if(selected[i]==1)
		{
			planes[i].mousePress(mousepos, type);
		}
	}
}
void PlaneWidgetInCube::mouseRelease()
{
	for(int i=0; i<6; i++)
	{
		if(selected[i]==-1)
		{
			planes[i].mouseRelease();
		}
	}
}

void PlaneWidgetInCube::eval_centre(void)
{		
	centre = Vector(0, 0, 0, 0);

	for(int i=0; i<planes.size(); i++)
	{
		centre += planes[i].centre;
	}

	centre /= planes.size();
}

void PlaneWidgetInCube::eval_planeplaneintersections(void)
{
	//evaluate the plane plane intersetion 
	//======================================================

	//The equation of a plane (points P are on the plane with normal N and point P3 on the plane) 
	//can be written as : N dot (P - P3) = 0

	//The equation of the line (points P on the line passing through points P1 and P2) 
	//can be written as : P = P1 + u (P2 - P1)

	//The intersection of these two occurs when
	// N dot (P1 + u (P2 - P1)) = N dot P3

	//Solving for u gives
	// u = (N dot(P3 - P1)) / (N dot(P2 - P1))
	
	//if 1 > u < 0 then we have a valid intersection.
	
	//note special case
	//if u==0 the line is perpendicular to N

	printf("eval_planeplaneintersections\n");

	for(int i=0; i<planeplanepoints.size(); i++) planeplanepoints[i].clear();
	planeplanepoints.clear();
	planeplanepoints.resize(6);
	
	eval_centre();

	Vector line0_v1, line0_v2;
	Vector line1_v1, line1_v2;
	double mua, mub;
	Vector pa, pb;
	
	for(int i=0; i<plane_clippoints.size(); i++)
	{
		for(int ii=0; ii<plane_clippoints[i].size(); ii++)
		{
			if(ii==plane_clippoints[i].size()-1)
			{
				line0_v1 = plane_clippoints[i][ii];
				line0_v2 = plane_clippoints[i][0];
			}
			else
			{
				line0_v1 = plane_clippoints[i][ii];
				line0_v2 = plane_clippoints[i][ii+1];
			}
			
			for(int j=0; j<plane_clippoints.size(); j++)
			{
				if(j!=i)
				{
					for(int jj=0; jj<plane_clippoints[j].size(); jj++)
					{
						if(jj==plane_clippoints[j].size()-1)
						{
							line1_v1 = plane_clippoints[j][jj];
							line1_v2 = plane_clippoints[j][0];
						}
						else
						{
							line1_v1 = plane_clippoints[j][jj];
							line1_v2 = plane_clippoints[j][jj+1];
						}
						
						if(intersectLineLine(line0_v1, line0_v2, line1_v1, line1_v2, &pa, &pb, &mua, &mub))
						{						 							
								if(pa==pb) planeplanepoints[i].push_back(pa);
						}
					}					
				}		
			}
		}		
	}

	int helper;
	Vector v,c,n,t;	

	for(int i=0; i<planeplanepoints2.size(); i++) planeplanepoints2[i].clear();
	planeplanepoints2.clear();
	planeplanepoints2.resize(6);
	
	for(int i=0; i<planes.size(); i++)
		for(int j=0; j<plane_clippoints[i].size(); j++)
			plane_clippoints[i][j].w=0.0;

	for(int i=0; i<planes.size(); i++)
		for(int j=0; j<planeplanepoints[i].size(); j++)
			planeplanepoints[i][j].w=0.0;

	for(int i=0; i<planes.size(); i++)
	{
		for(int j=0; j<plane_clippoints[i].size(); j++)
		{
			for(int jj=0; jj<plane_clippoints[i].size(); jj++)
			{
				if(plane_clippoints[i][j]==plane_clippoints[i][jj])
				{
					plane_clippoints[i][j].w++;
					//plane_clippoints[i][jj].w++;		
					//planeplanepoints2[i].push_back(plane_clippoints[i][j]);		
				}
			}
		}		
	}

	for(int i=0; i<planes.size(); i++)
	{
		for(int j=0; j<planeplanepoints[i].size(); j++)
		{
			//planeplanepoints[i][j].w = 0.0;
			for(int jj=0; jj<planeplanepoints[i].size(); jj++)
			{
				if(planeplanepoints[i][j]==planeplanepoints[i][jj])
				{
					 planeplanepoints[i][j].w++;
					 //planeplanepoints[i][jj].w++;
					//planeplanepoints2[i].push_back(planeplanepoints[i][j]);		
				}
			}
		}		
	}
	
	for(int i=0; i<planes.size(); i++)
	{
		for(int j=0; j<plane_clippoints[i].size(); j++)
		{
			printf("plane_clippoints[i][j].w: %f\n", plane_clippoints[i][j].w);
			if(plane_clippoints[i][j].w>1.0) planeplanepoints2[i].push_back(plane_clippoints[i][j]);	
		}
		for(int j=0; j<planeplanepoints[i].size(); j++)
		{
			//printf("planeplanepoints[i][j].w: %f\n", planeplanepoints[i][j].w);
			if(planeplanepoints[i][j].w>0.0) planeplanepoints2[i].push_back(planeplanepoints[i][j]);	
		}
	}

	/*for(int i=0; i<planes.size(); i++)
	{
		for(int j=0; j<plane_clippoints[i].size(); j++)
		{
			v = plane_clippoints[i][j];
			helper=0.0;
			for(int ii=0; ii<planes.size(); ii++)
			{
				if(ii!=i)
				{
					c = planes[ii].centre;
					n = planes[ii].normal;
					t = v - planes[i].centre;
					v = v + 2.0f*t;
					if(n.dot(v-c)>0.0)
					{
						helper++;
					}
				}
			}
			//if(helper>=5) 
			planeplanepoints2[i].push_back(plane_clippoints[i][j]);
			
			printf("helper1: %d\n", helper);
		}
	}
	for(int i=0; i<planes.size(); i++)
	{
		for(int j=0; j<planeplanepoints[i].size(); j++)
		{
			v = planeplanepoints[i][j];
			helper=0.0;
			for(int ii=0; ii<planes.size(); ii++)
			{
				if(i!=ii)
				{
					c = planes[ii].centre;
					n = planes[ii].normal;
					t = v - planes[i].centre;
					v = v + 2.0f*t;
					if(n.dot(v-c)>0.0)
					{
						helper++;
					}
					//if(helper>=5) 
					planeplanepoints2[i].push_back(planeplanepoints[i][j]);
					
					printf("helper2: %d\n", helper);
				}
			}
		}
	}*/
}
void PlaneWidgetInCube::plane_translate(float dist, int selected_)
{
/*	int selected_=-1;
	for(int i=0; i<6; i++)
	{
		if(selected[i]==1) selected_ = i;
	}
*/
	if(selected_==-1) return;

//	printf("selected: %d\n", selected);
	
	Vector _v1 = planes[selected_]._v1;
	Vector _v2 = planes[selected_]._v2;
	Vector _v3 = planes[selected_]._v3;
	Vector _v4 = planes[selected_]._v4;
	Vector _normal = planes[selected_].normal;
	
	Vector vv_0 = (_v1+(dist*_normal));		
	Vector vv_1 = (_v2+(dist*_normal));		
	Vector vv_2 = (_v3+(dist*_normal));		
	Vector vv_3 = (_v4+(dist*_normal));		

	planes[selected_]._v1 = (vv_0);
	planes[selected_]._v2 = (vv_1);
	planes[selected_]._v3 = (vv_2);
	planes[selected_]._v4 = (vv_3);

	planes[selected_].eval_planeequation();
	eval_planeintersections();

	Vector c1 = planes[selected_]._v1 + planes[selected_]._v2 + planes[selected_]._v3 + planes[selected_]._v4;
	c1 /= 4.0;
	Vector c2 = planes[selected_].v1 + planes[selected_].v2 + planes[selected_].v3 + planes[selected_].v4;
	c2 /= 4.0;
	Vector l = c1-c2;
	
	planes[selected_].translate = l.length();
}
void PlaneWidgetInCube::plane_rotate1(Vector cen, float angle, int selected_)
{
	//find which plane we have currently active
/*	int selected_=-1;
	for(int i=0; i<6; i++)
	{
		if(selected[i]==1) selected_ = i;
	}*/
	//if none then stop
	if(selected_==-1) return;

	//increment our angle of rotation.
	planes[selected_].angleX += angle;

	//translate our clipping plane to the origin
	Vector cen2 = planes[selected_].v1+planes[selected_].v2+planes[selected_].v3+planes[selected_].v4;
	cen2 *= 0.25;

	cen = cen2;

	Vector v1cen = planes[selected_].v1-cen;
	Vector v2cen = planes[selected_].v2-cen;
	Vector v3cen = planes[selected_].v3-cen;
	Vector v4cen = planes[selected_].v4-cen;

	Matrix4x4 rot_matX, axis_rotmat;
	Vector n;
	
	//rotate our x axis by our new angle
	v1cen.build_rotatemat(planes[selected_].perpX, angle, &rot_matX.m[0]);
	planes[selected_].perpY = planes[selected_].perpY.mult_matrix(rot_matX.m);
	
	//and evaluate our new normal for the new plane rotation
	n.cross(planes[selected_].perpY, planes[selected_].perpX);
	
	//now we can evaluate the rotation matrix to go from our original set of axis to the other
	axis_rotmat.find_rotationfromaxis(planes[selected_].perpX, planes[selected_].perpY, n, planes[selected_].axisX, planes[selected_].axisY, planes[selected_].axisZ);
	planes[selected_].rot_mat = axis_rotmat;
	
	//rotate our plane vertices now
	planes[selected_].rot_mat = axis_rotmat;	
	v1cen = v1cen.mult_matrix(planes[selected_].rot_mat.m);
	v2cen = v2cen.mult_matrix(planes[selected_].rot_mat.m);
	v3cen = v3cen.mult_matrix(planes[selected_].rot_mat.m);
	v4cen = v4cen.mult_matrix(planes[selected_].rot_mat.m);

	//and now retranslate our plane to its original position
	planes[selected_]._v1 = v1cen+cen;
	planes[selected_]._v2 = v2cen+cen;
	planes[selected_]._v3 = v3cen+cen;
	planes[selected_]._v4 = v4cen+cen;
	Vector vcen = planes[selected_].centre-cen;
	planes[selected_]._v1 += vcen;
	planes[selected_]._v2 += vcen;
	planes[selected_]._v3 += vcen;
	planes[selected_]._v4 += vcen;

	//evaluate the equation of the plane, the intersection points with the bounding box, and upload our planes to the GPU
	planes[selected_].eval_planeequation();
	eval_planeintersections();
}
void PlaneWidgetInCube::plane_rotate2(Vector cen, float angle, int selected_)
{
	//find which plane we have currently active
	/*int selected_=-1;
	for(int i=0; i<6; i++)
	{
		if(selected[i]==1) selected_ = i;
	}*/
	//if none then stop
	if(selected_==-1) return;

	//increment our angle of rotation.
	planes[selected_].angleY += angle;

	//translate our clipping plane to the origin
	/*Vector cen = planes[selected_].v1+planes[selected_].v2+planes[selected_].v3+planes[selected_].v4;
	cen *= 0.25;*/
	cen = Vector(0,0,0);

	Vector v1cen = planes[selected_].v1-cen;
	Vector v2cen = planes[selected_].v2-cen;
	Vector v3cen = planes[selected_].v3-cen;
	Vector v4cen = planes[selected_].v4-cen;

	Matrix4x4 rot_matY, axis_rotmat;
	Vector n;
	
	//rotate our y axis by our new angle
	v1cen.build_rotatemat(planes[selected_].perpY, angle, &rot_matY.m[0]);
	planes[selected_].perpX = planes[selected_].perpX.mult_matrix(rot_matY.m);

	//and evaluate our new normal for the new plane rotation
	n.cross(planes[selected_].perpX,planes[selected_].perpY);

	//now we can evaluate the rotation matrix to go from our original set of axis to the other
	axis_rotmat.find_rotationfromaxis(planes[selected_].perpX, planes[selected_].perpY, n, planes[selected_].axisX, planes[selected_].axisY, planes[selected_].axisZ);
	planes[selected_].rot_mat = axis_rotmat;
	
	//rotate our plane vertices now
	v1cen = v1cen.mult_matrix(planes[selected_].rot_mat.m);
	v2cen = v2cen.mult_matrix(planes[selected_].rot_mat.m);
	v3cen = v3cen.mult_matrix(planes[selected_].rot_mat.m);
	v4cen = v4cen.mult_matrix(planes[selected_].rot_mat.m);
	
	//and now retranslate our plane to its original position	
	planes[selected_]._v1 = v1cen+cen;
	planes[selected_]._v2 = v2cen+cen;
	planes[selected_]._v3 = v3cen+cen;
	planes[selected_]._v4 = v4cen+cen;
	Vector vcen = planes[selected_].centre-cen;
	planes[selected_]._v1 += vcen;
	planes[selected_]._v2 += vcen;
	planes[selected_]._v3 += vcen;
	planes[selected_]._v4 += vcen;

	//evaluate the equation of the plane, the intersection points with the bounding box, and upload our planes to the GPU
	planes[selected_].eval_planeequation();
	eval_planeintersections();
}
bool PlaneWidgetInCube::intersectLineLine(Vector p1,Vector p2,Vector p3,Vector p4,Vector *pa,Vector *pb, double *mua, double *mub)
{
	Vector p13,p43,p21;
	double d1343,d4321,d1321,d4343,d2121;
	double numer,denom;

	p13 = p1 - p3;
	p43 = p4 - p3;

	if (abs(p43.x)  < EPSILON && abs(p43.y)  < EPSILON && abs(p43.z)  < EPSILON)
	  return(false);

	p21 = p2 - p1;

	if (abs(p21.x)  < EPSILON && abs(p21.y)  < EPSILON && abs(p21.z)  < EPSILON)
	  return(false);

	d1343 = p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
	d4321 = p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
	d1321 = p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
	d4343 = p43.x * p43.x + p43.y * p43.y + p43.z * p43.z;
	d2121 = p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;

	denom = d2121 * d4343 - d4321 * d4321;
	if (abs(denom) < EPSILON)
	  return(false);
	numer = d1343 * d4321 - d1321 * d4343;

	*mua = numer / denom;
	*mub = (d1343 + d4321 * (*mua)) / d4343;

	*pa = p1 + float(*mua) * p21;
	*pb = p3 + float(*mub) * p43;

	// return(true);

	//line segments
	if(*mua<1.0 && *mua>0.0 && *mub<1.0 && *mub>0.0)
	   return(true);

	return(false);
}
void PlaneWidgetInCube::eval_viewaligned_slices(int numb_slices, float rot_mat[16], Vector eye, Vector at)
{
	for(int i=0; i<view_algined_slices.size(); i++) view_algined_slices[i].clear();
	view_algined_slices.clear();

	Vector m_pVertices[8];
	Edge m_pEdges[12];

	m_pVertices[0] = Vector(-0.5,-0.5,-0.5, 0.0);
	m_pVertices[1] = Vector( 0.5,-0.5,-0.5, 0.0);
	m_pVertices[2] = Vector( 0.5, 0.5,-0.5, 0.0);
	m_pVertices[3] = Vector(-0.5, 0.5,-0.5, 0.0);
															
	m_pVertices[4] = Vector(-0.5,-0.5, 0.5, 0.0);
	m_pVertices[5] = Vector( 0.5,-0.5, 0.5, 0.0);
	m_pVertices[6] = Vector( 0.5, 0.5, 0.5, 0.0);
	m_pVertices[7] = Vector(-0.5, 0.5, 0.5, 0.0);
		
	m_pEdges[0]  = Edge(0,1);
	m_pEdges[1]  = Edge(1,2);
	m_pEdges[2]  = Edge(2,3);
	m_pEdges[3]  = Edge(3,0);
	m_pEdges[4]  = Edge(0,4);
	m_pEdges[5]  = Edge(1,5);
	m_pEdges[6]  = Edge(2,6);
	m_pEdges[7]  = Edge(3,7);
	m_pEdges[8]  = Edge(4,5);
	m_pEdges[9]  = Edge(5,6);
	m_pEdges[10] = Edge(6,7);
	m_pEdges[11] = Edge(7,4);

	const int m_pEdgeList[8][12] = 
	{
		{ 0,1,5,6,   4,8,11,9,  3,7,2,10 }, // v0 is front
		{ 0,4,3,11,  1,2,6,7,   5,9,8,10 }, // v1 is front
		{ 1,5,0,8,   2,3,7,4,   6,10,9,11}, // v2 is front
		{ 7,11,10,8, 2,6,1,9,   3,0,4,5  }, // v3 is front
		{ 8,5,9,1,   11,10,7,6, 4,3,0,2  }, // v4 is front
		{ 9,6,10,2,  8,11,4,7,  5,0,1,3  }, // v5 is front
		{ 9,8,5,4,   6,1,2,0,   10,7,11,3}, // v6 is front
		{ 10,9,6,5,  7,2,3,1,   11,4,8,0 }  // v7 is front
	}; 

	Vector viewVec = eye-at;
	viewVec.normalize();

	double dMaxDist = viewVec.dot(m_pVertices[0]);
	double dMinDist = dMaxDist;
	int nMaxIdx = 0;
	for(int i = 1; i < 8; ++i) 
	{
		double dist = viewVec.dot(m_pVertices[i]);
		if ( dist > dMaxDist) 
		{
			dMaxDist = dist;
			nMaxIdx = i;
		}
		if ( dist < dMinDist) 
		{
			dMinDist = dist;
		}

	}
	
	Vector vecStart[12];
	Vector vecDir[12];
	float lambda[12];
	float lambda_inc[12];
	double denom;

	dMinDist += EPSILON;
	dMaxDist -= EPSILON;
	double dPlaneDist    =  dMinDist;   
	double dPlaneDistInc =  (dMaxDist-dMinDist)/double(numb_slices);

	float dVertices[12*3];
	float dEdges[12*3];

	for(int i = 0; i < 12; i++) 
	{
		vecStart[i] = m_pVertices[m_pEdges[m_pEdgeList[nMaxIdx][i]].v0];
		vecDir[i]   = m_pVertices[m_pEdges[m_pEdgeList[nMaxIdx][i]].v1] - m_pVertices[m_pEdges[m_pEdgeList[nMaxIdx][i]].v0];

		denom = vecDir[i].dot(viewVec);

		if (1.0 + denom != 1.0) 
		{
			lambda_inc[i] =  dPlaneDistInc/denom;
			lambda[i]     = (dPlaneDist - vecStart[i].dot(viewVec))/denom;
		} else 
		{
			lambda[i]     = -1.0;
			lambda_inc[i] =  0.0;	
		}

		dVertices[3*i]   = vecStart[i][0];
		dVertices[3*i+1] = vecStart[i][1];
		dVertices[3*i+2] = vecStart[i][2];
		dEdges[3*i]      = vecDir[i][0];
		dEdges[3*i+1]    = vecDir[i][1];
		dEdges[3*i+2]    = vecDir[i][2];
	};


	Vector intersection[6];
	float lmb[12];

	view_algined_slices.resize(numb_slices);

	for(int n = numb_slices-1; n >= 0; --n) 
	{

		for(int e = 0; e < 12; e++) 
		{
			lmb[e] = lambda[e] + n*lambda_inc[e];
		}

		if      ((lmb[0] >= 0.0) && (lmb[0] < 1.0)) intersection[0] = vecStart[0] + lmb[0] * vecDir[0];
		else if ((lmb[1] >= 0.0) && (lmb[1] < 1.0)) intersection[0] = vecStart[1] + lmb[1] * vecDir[1];
		else if ((lmb[3] >= 0.0) && (lmb[3] < 1.0)) intersection[0] = vecStart[3] + lmb[3] * vecDir[3];
		else continue;
		
		if	    ((lmb[2] >= 0.0) && (lmb[2] < 1.0)) intersection[1] = vecStart[2] + lmb[2] * vecDir[2];
		else if ((lmb[0] >= 0.0) && (lmb[0] < 1.0)) intersection[1] = vecStart[0] + lmb[0] * vecDir[0];
		else if ((lmb[1] >= 0.0) && (lmb[1] < 1.0)) intersection[1] = vecStart[1] + lmb[1] * vecDir[1];
		else intersection[1] = vecStart[3] + lmb[3] * vecDir[3];

		if      ((lmb[4] >= 0.0) && (lmb[4] < 1.0)) intersection[2] = vecStart[4] + lmb[4] * vecDir[4];
		else if ((lmb[5] >= 0.0) && (lmb[5] < 1.0)) intersection[2] = vecStart[5] + lmb[5] * vecDir[5];
		else intersection[2] = vecStart[7] + lmb[7] * vecDir[7];
		
		if	    ((lmb[6] >= 0.0) && (lmb[6] < 1.0)) intersection[3] = vecStart[6] + lmb[6] * vecDir[6];
		else if ((lmb[4] >= 0.0) && (lmb[4] < 1.0)) intersection[3] = vecStart[4] + lmb[4] * vecDir[4];
		else if ((lmb[5] >= 0.0) && (lmb[5] < 1.0)) intersection[3] = vecStart[5] + lmb[5] * vecDir[5];
		else intersection[3] = vecStart[7] + lmb[7] * vecDir[7];

		if	    ((lmb[8] >= 0.0) && (lmb[8] < 1.0)) intersection[4] = vecStart[8] + lmb[8] * vecDir[8] ;
		else if ((lmb[9] >= 0.0) && (lmb[9] < 1.0)) intersection[4] = vecStart[9] + lmb[9] * vecDir[9] ;
		else intersection[4] = vecStart[11]+ lmb[11]* vecDir[11];
		
		if	    ((lmb[10]>= 0.0) && (lmb[10]< 1.0)) intersection[5] = vecStart[10]+ lmb[10]* vecDir[10];
		else if ((lmb[8] >= 0.0) && (lmb[8] < 1.0)) intersection[5] = vecStart[8] + lmb[8] * vecDir[8] ;
		else if ((lmb[9] >= 0.0) && (lmb[9] < 1.0)) intersection[5] = vecStart[9] + lmb[9] * vecDir[9] ;
		else intersection[5] = vecStart[11]+ lmb[11]* vecDir[11];

		for(int i = 0; i < 6; ++i) 
		{
			view_algined_slices[n].push_back(intersection[i]);
			//double dLength = intersection[i].length();
		}

		/*
		//glEnable(GL_TEXTURE_3D);
		//glBindTexture(GL_TEXTURE_3D, m_pTextureIds[0]);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		//glAlphaFunc(GL_GREATER,0.2);
		//glEnable(GL_ALPHA_TEST);
		
		float c = 1.0f-float(n)/float(numb_slices);
		glColor3f(c,c,c);
		glBegin(GL_TRIANGLE_FAN);
		for(int i = 0; i < 6; ++i) 
		{
			intersection[i].glVertex(false,true);
		}
		glEnd();

		glDisable(GL_BLEND);
		glDisable(GL_TEXTURE_3D);*/
	}
	/*
	for(int i=0; i<view_algined_slices.size(); i++) view_algined_slices[i].clear();
	view_algined_slices.clear();

	Vector v1_rot = v1;
	Vector v2_rot = v2;
	Vector v3_rot = v3;
	Vector v4_rot = v4;
	Vector v5_rot = v5;
	Vector v6_rot = v6;
	Vector v7_rot = v7;
	Vector v8_rot = v8;

	Vector normal = eye-at;
	normal.normalize();

	float max_Z = -999999999999;
	float min_Z =  999999999999;
	Vector vmax, vmin;

	vector < Vector* > vec;
	vec.push_back(&v1_rot);
	vec.push_back(&v2_rot);
	vec.push_back(&v3_rot);
	vec.push_back(&v4_rot);
	vec.push_back(&v5_rot);
	vec.push_back(&v6_rot);
	vec.push_back(&v7_rot);
	vec.push_back(&v8_rot);

	Vector eye_rot = eye.mult_matrix(rot_mat);

	//find the BB furthest and closet points from view dir
	Vector vtemp;
	for(int i=0; i<vec.size(); i++)
	{
		float l = vec[i]->dot(eye);

		if(l>max_Z)
		{
			max_Z = vtemp.length();
			vmax = *vec[i];
		}
		if(l<min_Z)
		{
			min_Z = vtemp.length();
			vmin = *vec[i];
		}
	}

	bbvmin = vmin;
	bbvmax = vmax;

	Vector vdist = vmax-vmin;
	float distance = vdist.length();
	vdist.normalize();
	
//	vdist = at-eye;
//	vdist.normalize();

	float u1, u2, u3, u4, u5, u6, u7, u8, u9, u10, u11, u12;
	u1 = u2 = u3 = u4 = u5 = u6 = u7 = u8 = u9 = u10 = u11 = u12 = 0.0;

//	printf("z range: %f to %f\n", min_Z, max_Z);
//	printf("distance: %f\n", distance);
	float interp=0.0;
	for(int i=0; i<numb_slices; i++)
	{
		{
			interp = (float)i/(float)numb_slices;
			Vector centre = vmin + (interp*distance)*vdist;

//			printf("centre: %f\n", centre.z);

			points.clear();

			//line1 (v1_rot -> v3_rot)
			u1 = normal.dot(centre - v1_rot) / normal.dot(v3_rot-v1_rot);
			
			//line2 (v1_rot -> v2_rot)
			u2 = normal.dot(centre - v1_rot) / normal.dot(v2_rot-v1_rot);

			//line3 (v1_rot -> v5_rot)
			u3 = normal.dot(centre - v1_rot) / normal.dot(v5_rot-v1_rot);

			//line4 (v3_rot -> v7_rot)
			u4 = normal.dot(centre - v3_rot) / normal.dot(v7_rot-v3_rot);

			//line5 (v3_rot -> v4_rot)
			u5 = normal.dot(centre - v3_rot) / normal.dot(v4_rot-v3_rot);

			//line6 (v4_rot -> v2_rot)
			u6 = normal.dot(centre - v4_rot) / normal.dot(v2_rot-v4_rot);

			//line7 (v4_rot -> v8_rot)
			u7 = normal.dot(centre - v4_rot) / normal.dot(v8_rot-v4_rot);

			//line8 (v2_rot -> v6_rot)
			u8 = normal.dot(centre - v2_rot) / normal.dot(v6_rot-v2_rot);

			//line9 (v5_rot -> v6_rot)
			u9 = normal.dot(centre - v5_rot) / normal.dot(v6_rot-v5_rot);

			//line10 (v5_rot -> v7_rot)
			u10 = normal.dot(centre - v5_rot) / normal.dot(v7_rot-v5_rot);

			//line11 (v8_rot -> v6_rot)
			u11 = normal.dot(centre - v8_rot) / normal.dot(v6_rot-v8_rot);

			//line12 (v8_rot -> v7_rot)
			u12 = normal.dot(centre - v8_rot) / normal.dot(v7_rot-v8_rot);

			//line1 (v1_rot -> v3_rot)
			if(u1>=0.0 && u1<=1.0)
			{
				points.push_back(Vector(v1_rot + u1*(v3_rot-v1_rot)));
			}
			//line2 (v1_rot -> v2_rot)
			if(u2>=0.0 && u2<=1.0)
			{
				points.push_back(Vector(v1_rot + u2*(v2_rot-v1_rot)));
			}
			//line3 (v1_rot -> v5_rot)
			if(u3>=0.0 && u3<=1.0)
			{
				points.push_back(Vector(v1_rot + u3*(v5_rot-v1_rot)));
			}
			//line4 (v3_rot -> v7_rot)
			if(u4>=0.0 && u4<=1.0)
			{
				points.push_back(Vector(v3_rot + u4*(v7_rot-v3_rot)));
			}
			//line5 (v3_rot -> v4_rot)
			if(u5>=0.0 && u5<=1.0)
			{
				points.push_back(Vector(v3_rot + u5*(v4_rot-v3_rot)));
			}
			//line6 (v4_rot -> v2_rot)
			if(u6>=0.0 && u6<=1.0)
			{
				points.push_back(Vector(v4_rot + u6*(v2_rot-v4_rot)));
			}
			//line7 (v4_rot -> v8_rot)
			if(u7>=0.0 && u7<=1.0)
			{
				points.push_back(Vector(v4_rot + u7*(v8_rot-v4_rot)));
			}
			//line8 (v2_rot -> v6_rot)
			if(u8>=0.0 && u8<=1.0)
			{
				points.push_back(Vector(v2_rot + u8*(v6_rot-v2_rot)));
			}
			//line9 (v5_rot -> v6_rot)
			if(u9>=0.0 && u9<=1.0)
			{
				points.push_back(Vector(v5_rot + u9*(v6_rot-v5_rot)));
			}
			//line10 (v5_rot -> v7_rot)
			if(u10>=0.0 && u10<=1.0)
			{
				points.push_back(Vector(v5_rot + u10*(v7_rot-v5_rot)));
			}
			//line11 (v8_rot -> v6_rot)
			if(u11>=0.0 && u11<=1.0)
			{
				points.push_back(Vector(v8_rot + u11*(v6_rot-v8_rot)));
			}
			//line12 (v8_rot -> v7_rot)
			if(u12>=0.0 && u12<=1.0)
			{
				points.push_back(Vector(v8_rot + u12*(v7_rot-v8_rot)));
			}
			
			//Now sort the intersection points.
			if(points.size()>1)
			{
				//compute our planes centroid...
				Vector planecentre = 0.0;
				for(int j=0; j<points.size(); j++)
				{
					planecentre += points[j];
				}
				planecentre /= (float)points.size();


				//copy points into original_points[]
				vector <Vector> original_points;
				original_points.resize(points.size());
				for(int ii=0; ii<points.size(); ii++)
				{
					original_points[ii] = points[ii];
					original_points[ii].w=0.0;
				}
				
				//this will hold our sorted points (ordered)
				vector <Vector> sorted_points;

				//use original_points[0] as our
				//reference point...
				original_points[0].w=1.0;
				sorted_points.push_back(original_points[0]);

				Vector v00 = original_points[0]-planecentre;
				v00.normalize();

				Vector vn  = normal;
				vn.normalize();

				vector<float> angles;
				angles.resize(original_points.size());

				Vector v11;
				Vector v22 = original_points[1];
				v22.normalize();

				Vector vcross;
				float vdot;
				for(int ii=1; ii<original_points.size(); ii++)
				{
					v11 = original_points[ii]-planecentre;
					v11.normalize();
					
					vcross.cross(v00, v11);
					
					vdot = v11.dot(v22);

					float signed_angle = atan2(  vn.dot(vcross), v00.dot(v11)); 
					signed_angle *= (180.0/3.141592654);
					
					if(signed_angle<0.0) signed_angle += 360.0;
					angles[ii]=signed_angle;
				}

				float angle=999999.0;
				int index=-1;
				for(int ii=1; ii<original_points.size(); ii++)
				{
					angle=999999.0;
					index=-1;

					for(int j=1; j<original_points.size(); j++)
					{
						if(original_points[j].w != 1.0)
						{
							if(angle>angles[j])
							{
								angle = angles[j];
								index = j;
							}
						}
					}

					if(index!=-1)
					{
						sorted_points.push_back(original_points[index]);
						original_points[index].w=1.0;
						//if(selected[i]==1) printf("angles: %f\n", angles[index]);
					}
				}

				//now copy sorted_points over to points
				for(int ii=0; ii<sorted_points.size(); ii++)
					points[ii] = sorted_points[ii];


				//printf("points size: %d\n", points.size());
			}
			else
			{
				//printf("no points!\n");
			}

			//printf("points size: %d\n", points.size());
			view_algined_slices.push_back(points);
		}
	}*/
}
void PlaneWidgetInCube::eval_planeintersections(void)
{
	//evaluate the intersetion with the plane and the cube
	//======================================================

	//The equation of a plane (points P are on the plane with normal N and point P3 on the plane) 
	//can be written as : N dot (P - P3) = 0

	//The equation of the line (points P on the line passing through points P1 and P2) 
	//can be written as : P = P1 + u (P2 - P1)

	//The intersection of these two occurs when
	// N dot (P1 + u (P2 - P1)) = N dot P3

	//Solving for u gives
	// u = (N dot(P3 - P1)) / (N dot(P2 - P1))
	
	//if 1 > u < 0 then we have a valid intersection.
	
	//note special case
	//if u==0 the line is perpendicular to N
	
	plane_clippoints.clear();
	eval_centre();

	float u1, u2, u3, u4, u5, u6, u7, u8, u9, u10, u11, u12;
	u1 = u2 = u3 = u4 = u5 = u6 = u7 = u8 = u9 = u10 = u11 = u12 = 0.0;

	for(int i=0; i<6; i++)
	{
		{
			points.clear();

			//line1 (v1 -> v3)
			u1 = planes[i].normal.dot(planes[i].centre - v1) / planes[i].normal.dot(v3-v1);
			
			//line2 (v1 -> v2)
			u2 = planes[i].normal.dot(planes[i].centre - v1) / planes[i].normal.dot(v2-v1);

			//line3 (v1 -> v5)
			u3 = planes[i].normal.dot(planes[i].centre - v1) / planes[i].normal.dot(v5-v1);

			//line4 (v3 -> v7)
			u4 = planes[i].normal.dot(planes[i].centre - v3) / planes[i].normal.dot(v7-v3);

			//line5 (v3 -> v4)
			u5 = planes[i].normal.dot(planes[i].centre - v3) / planes[i].normal.dot(v4-v3);

			//line6 (v4 -> v2)
			u6 = planes[i].normal.dot(planes[i].centre - v4) / planes[i].normal.dot(v2-v4);

			//line7 (v4 -> v8)
			u7 = planes[i].normal.dot(planes[i].centre - v4) / planes[i].normal.dot(v8-v4);

			//line8 (v2 -> v6)
			u8 = planes[i].normal.dot(planes[i].centre - v2) / planes[i].normal.dot(v6-v2);

			//line9 (v5 -> v6)
			u9 = planes[i].normal.dot(planes[i].centre - v5) / planes[i].normal.dot(v6-v5);

			//line10 (v5 -> v7)
			u10 = planes[i].normal.dot(planes[i].centre - v5) / planes[i].normal.dot(v7-v5);

			//line11 (v8 -> v6)
			u11 = planes[i].normal.dot(planes[i].centre - v8) / planes[i].normal.dot(v6-v8);

			//line12 (v8 -> v7)
			u12 = planes[i].normal.dot(planes[i].centre - v8) / planes[i].normal.dot(v7-v8);

			//line1 (v1 -> v3)
			if(u1>=0.0 && u1<=1.0)
			{
				points.push_back(Vector(v1 + u1*(v3-v1)));
			}
			//line2 (v1 -> v2)
			if(u2>=0.0 && u2<=1.0)
			{
				points.push_back(Vector(v1 + u2*(v2-v1)));
			}
			//line3 (v1 -> v5)
			if(u3>=0.0 && u3<=1.0)
			{
				points.push_back(Vector(v1 + u3*(v5-v1)));
			}
			//line4 (v3 -> v7)
			if(u4>=0.0 && u4<=1.0)
			{
				points.push_back(Vector(v3 + u4*(v7-v3)));
			}
			//line5 (v3 -> v4)
			if(u5>=0.0 && u5<=1.0)
			{
				points.push_back(Vector(v3 + u5*(v4-v3)));
			}
			//line6 (v4 -> v2)
			if(u6>=0.0 && u6<=1.0)
			{
				points.push_back(Vector(v4 + u6*(v2-v4)));
			}
			//line7 (v4 -> v8)
			if(u7>=0.0 && u7<=1.0)
			{
				points.push_back(Vector(v4 + u7*(v8-v4)));
			}
			//line8 (v2 -> v6)
			if(u8>=0.0 && u8<=1.0)
			{
				points.push_back(Vector(v2 + u8*(v6-v2)));
			}
			//line9 (v5 -> v6)
			if(u9>=0.0 && u9<=1.0)
			{
				points.push_back(Vector(v5 + u9*(v6-v5)));
			}
			//line10 (v5 -> v7)
			if(u10>=0.0 && u10<=1.0)
			{
				points.push_back(Vector(v5 + u10*(v7-v5)));
			}
			//line11 (v8 -> v6)
			if(u11>=0.0 && u11<=1.0)
			{
				points.push_back(Vector(v8 + u11*(v6-v8)));
			}
			//line12 (v8 -> v7)
			if(u12>=0.0 && u12<=1.0)
			{
				points.push_back(Vector(v8 + u12*(v7-v8)));
			}

			if(points.size()>1)
			{
				//compute our planes centroid...
				Vector planecentre = 0.0;
				for(int j=0; j<points.size(); j++)
				{
					planecentre += points[j];
				}
				planecentre /= (float)points.size();


				//copy points into original_points[]
				vector <Vector> original_points;
				original_points.resize(points.size());
				for(int ii=0; ii<points.size(); ii++)
				{
					original_points[ii] = points[ii];
					original_points[ii].w=0.0;
				}
				
				//this will hold our sorted points (ordered)
				vector <Vector> sorted_points;

				//use original_points[0] as our
				//reference point...
				original_points[0].w=1.0;
				sorted_points.push_back(original_points[0]);

				Vector v0 = original_points[0]-planecentre;
				v0.normalize();

				Vector vn  = planes[i].normal;
				vn.normalize();

				vector<float> angles;
				angles.resize(original_points.size());

				Vector v1;
				Vector v22 = original_points[1];
				v22.normalize();

				Vector vcross;
				float vdot;
				for(int ii=1; ii<original_points.size(); ii++)
				{
					v1 = original_points[ii]-planecentre;
					v1.normalize();
					
					vcross.cross(v0, v1);
					
					vdot = v1.dot(v22);

					float signed_angle = atan2(  vn.dot(vcross), v0.dot(v1)); 
					signed_angle *= (180.0/3.141592654);
					
					if(signed_angle<0.0) signed_angle += 360.0;

					angles[ii]=signed_angle;
				}

				float angle=999999.0;
				int index=-1;
				for(int ii=1; ii<original_points.size(); ii++)
				{
					angle=999999.0;
					index=-1;

					for(int j=1; j<original_points.size(); j++)
					{
						if(original_points[j].w != 1.0)
						{
							if(angle>angles[j])
							{
								angle = angles[j];
								index = j;
							}
						}
					}

					if(index!=-1)
					{
						sorted_points.push_back(original_points[index]);
						original_points[index].w=1.0;
						//if(selected[i]==1) printf("angles: %f\n", angles[index]);
					}
				}

				//now copy sorted_points over to points
				for(int ii=0; ii<sorted_points.size(); ii++)
					points[ii] = sorted_points[ii];
			}
			else
			{
				points.push_back(planes[i]._v1);
				points.push_back(planes[i]._v2);
				points.push_back(planes[i]._v3);
				points.push_back(planes[i]._v4);
			}

			plane_clippoints.push_back(points);
		}
	}

	//eval_planeplaneintersections();
}

void PlaneWidgetInCube::save(QString filename)
{
	ofstream fout;

	fout.open(filename.latin1());

	if(fout==NULL)
	{
		printf("ERROR: SAVING FILE .CLP File! PlaneWidgetInCube::save(QString filename)\n");
	}

	fout<<"SIZE: "<<planes.size()<<endl;

	for(int i=0; i<planes.size(); i++)
	{
		fout<<"P: "<<planes[i].centre.x<<" "<<planes[i].centre.y<<" "<<planes[i].centre.z<<" ";
		fout<<planes[i].normal.x<<" "<<planes[i].normal.y<<" "<<planes[i].normal.z<<" ";
		fout<<planes[i]._v1.x<<" "<<planes[i]._v1.y<<" "<<planes[i]._v1.z<<" ";
		fout<<planes[i]._v2.x<<" "<<planes[i]._v2.y<<" "<<planes[i]._v2.z<<" ";
		fout<<planes[i]._v3.x<<" "<<planes[i]._v3.y<<" "<<planes[i]._v3.z<<" ";
		fout<<planes[i]._v4.x<<" "<<planes[i]._v4.y<<" "<<planes[i]._v4.z<<" ";
		fout<<planes[i].v1.x<<" "<<planes[i].v1.y<<" "<<planes[i].v1.z<<" ";
		fout<<planes[i].v2.x<<" "<<planes[i].v2.y<<" "<<planes[i].v2.z<<" ";
		fout<<planes[i].v3.x<<" "<<planes[i].v3.y<<" "<<planes[i].v3.z<<" ";
		fout<<planes[i].v4.x<<" "<<planes[i].v4.y<<" "<<planes[i].v4.z<<" ";
		fout<<(float)planes[i].plane_equation[0]<<" "<<(float)planes[i].plane_equation[1]<<" "<<(float)planes[i].plane_equation[2]<<" "<<(float)planes[i].plane_equation[3]<<" ";
		fout<<planes[i].arcball.orien_type<<" ";
		fout<<planes[i].arcball.Transform.M[0]<<" "<<planes[i].arcball.Transform.M[1]<<" "<<planes[i].arcball.Transform.M[2]<<" "<<planes[i].arcball.Transform.M[3]<<" "<<
			  planes[i].arcball.Transform.M[4]<<" "<<planes[i].arcball.Transform.M[5]<<" "<<planes[i].arcball.Transform.M[6]<<" "<<planes[i].arcball.Transform.M[7]<<" "<<
			  planes[i].arcball.Transform.M[8]<<" "<<planes[i].arcball.Transform.M[9]<<" "<<planes[i].arcball.Transform.M[10]<<" "<<planes[i].arcball.Transform.M[11]<<" "<<
			  planes[i].arcball.Transform.M[12]<<" "<<planes[i].arcball.Transform.M[13]<<" "<<planes[i].arcball.Transform.M[14]<<" "<<planes[i].arcball.Transform.M[15]<<" ";
		fout<<planes[i].arcball.LastRot.M[0]<<" "<<planes[i].arcball.LastRot.M[1]<<" "<<planes[i].arcball.LastRot.M[2]<<" "<<
			  planes[i].arcball.LastRot.M[3]<<" "<<planes[i].arcball.LastRot.M[4]<<" "<<planes[i].arcball.LastRot.M[5]<<" "<<
			  planes[i].arcball.LastRot.M[6]<<" "<<planes[i].arcball.LastRot.M[7]<<" "<<planes[i].arcball.LastRot.M[8]<<" ";
		fout<<planes[i].arcball.ThisRot.M[0]<<" "<<planes[i].arcball.ThisRot.M[1]<<" "<<planes[i].arcball.ThisRot.M[2]<<" "<<
			  planes[i].arcball.ThisRot.M[3]<<" "<<planes[i].arcball.ThisRot.M[4]<<" "<<planes[i].arcball.ThisRot.M[5]<<" "<<
			  planes[i].arcball.ThisRot.M[6]<<" "<<planes[i].arcball.ThisRot.M[7]<<" "<<planes[i].arcball.ThisRot.M[8]<<endl;
               /*fout<<"A: "<<planes[i].angleX<<" "<<planes[i].angleY<<endl;*/        
	}

	fout<<"C: "<<curve_translate.x<<" "<<curve_translate.y<<" "<<curve_translate.z<<" "<<curve_angleX<<" "<<curve_angleY<<endl;

	fout.close();
}

void PlaneWidgetInCube::load(QString filename)
{
	printf("Loading clipping planes\n");
	FILE* infile = fopen(filename.latin1(), "rt");
	if (infile==NULL)
	{
		printf("ERROR: OPENING FILE .CLP File! PlaneWidgetInCube::load() /n");
	}

	char buf[256];
	bool done = false;
	
	int size;
	int counter=0;
	Vector centre, normal, _V1, _V2, _V3, _V4, V1, V2, V3, V4;
	float pe1, pe2, pe3, pe4;
	int type;
	float trans[16];
	float thisrot[9];
	float lastrot[9];

	while (!done)
	{
		fgets(buf, 256, infile);
		if (feof(infile))
		{
			done = true;
			fclose(infile);
		}
		else
		{
		  switch (buf[0])
		  {	      	
			case 'S':
				{
				  if (sscanf(buf, "SIZE: %i", &size) != 1)
				  {
					break;
				  }

				  planes.clear();
				  planes.resize(size);
				  //init(256.0);
				}
				break;
			case 'P':
				{
					sscanf(buf, "P: %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f", 
														&centre.x, &centre.y, &centre.z, 
														&normal.x, &normal.y, &normal.z,
														&_V1.x, &_V1.y, &_V1.z,
														&_V2.x, &_V2.y, &_V2.z,
														&_V3.x, &_V3.y, &_V3.z,
														&_V4.x, &_V4.y, &_V4.z,
														&V1.x, &V1.y, &V1.z,
														&V2.x, &V2.y, &V2.z,
														&V3.x, &V3.y, &V3.z,
														&V4.x, &V4.y, &V4.z,
														&pe1, &pe2, &pe3, &pe4,				
														&type, //35								
														&trans[0],  &trans[1],  &trans[2],  &trans[3], 
														&trans[4],  &trans[5],  &trans[6],  &trans[7], 
														&trans[8],  &trans[9],  &trans[10], &trans[11],
														&trans[12], &trans[13], &trans[14], &trans[15], //16
														&thisrot[0], &thisrot[1], &thisrot[2], 
														&thisrot[3], &thisrot[4], &thisrot[5], 
														&thisrot[6], &thisrot[7], &thisrot[8], 
														&lastrot[0], &lastrot[1], &lastrot[2], 
														&lastrot[3], &lastrot[4], &lastrot[5],		//18
														&lastrot[6], &lastrot[7], &lastrot[8]);	//64
				  
				  planes[counter].centre = centre;
				  planes[counter].normal = normal;
				  planes[counter]._v1 = _V1;
				  planes[counter]._v2 = _V2;
				  planes[counter]._v3 = _V3;
				  planes[counter]._v4 = _V4;
				  planes[counter].v1 = V1;
				  planes[counter].v2 = V2;
				  planes[counter].v3 = V3;
				  planes[counter].v4 = V4;
				  planes[counter].plane_equation[0] = (double) pe1;
				  planes[counter].plane_equation[1] = (double) pe2;
				  planes[counter].plane_equation[2] = (double) pe3;
				  planes[counter].plane_equation[3] = (double) pe4;

				 // planes[counter].arcball.setBounds(800,800);
				  planes[counter].arcball.orien_type = type;

				  for(int i=0; i<16; i++)
					  planes[counter].arcball.Transform.M[i]  = trans[i];

				  for(int k=0; k<9; k++)
				  {
					planes[counter].arcball.ThisRot.M[k]  = thisrot[k];
					planes[counter].arcball.LastRot.M[k]  = lastrot[k];
				  }

                                    planes[counter].perpX = Vector(1,0,0);
                                    planes[counter].perpY = Vector(0,1,0);

                                    planes[counter].axisX = Vector(1,0,0);
                                    planes[counter].axisY = Vector(0,1,0);
                                    planes[counter].axisZ = Vector(0,0,1);

                                    counter++;
				}
				break;                               
 				case 'C':
				{
					if(sscanf(buf, "C: %f %f %f %f %f", &curve_translate.x, &curve_translate.y, &curve_translate.z, &curve_angleX, &curve_angleY) !=5)
					{
						break;
					}

					//plane_clippoints.resize(6);
				}
				break;                      
				/*case 'A':
                                {
                                  if (sscanf(buf, "A: %f %f", planes[counter].angleX, planes[counter].angleY) != 2)
                                  {
                                        break;
                                  }
                                }
                                break;*/
			  default:
                                {
                                        // Ignore lines that we don't recognize.
                                }
                                break;
		  }
		}
	}

       /* planes[0].angleX = planes[0].angleY =
	planes[1].angleX = planes[1].angleY = 
	planes[2].angleX = planes[2].angleY = 
	planes[3].angleX = planes[3].angleY =
	planes[4].angleX = planes[4].angleY =
        planes[5].angleX = planes[5].angleY = 0.0;*/

	planes[0].translate = planes[1].translate = planes[2].translate = planes[3].translate = planes[4].translate = planes[5].translate = 0.0;

	//eval_planeintersections();
	
	setup();
	enable();
}
void PlaneWidgetInCube::interpolate(PlaneWidgetInCube src, PlaneWidgetInCube dst, float interp)
{
	*this = dst;

	//cube coords
/*	this->v1 = src.v1 + interp*dst.v1;
	this->v2 = src.v2 + interp*dst.v2;
	this->v3 = src.v3 + interp*dst.v3;
	this->v4 = src.v4 + interp*dst.v4;
	this->v5 = src.v5 + interp*dst.v5;
	this->v6 = src.v6 + interp*dst.v6;
	this->v7 = src.v7 + interp*dst.v7;
	this->v8 = src.v8 + interp*dst.v8;

	this->p1 = src.p1 + interp*dst.p1;
	this->p2 = src.p2 + interp*dst.p2;
	this->p3 = src.p3 + interp*dst.p3;
	this->p4 = src.p4 + interp*dst.p4;
*/
	Quaternion src_quat, dst_quat, result_quat;
	Matrix3f_t mat;

	for(int i=0; i<src.planes.size(); i++)
	{
		this->planes[i] = src.planes[i];

		/*src_quat.from_matrix(&src.planes[i].arcball.ThisRot);
		dst_quat.from_matrix(&dst.planes[i].arcball.ThisRot);

		src_quat.quat.normalize();
		dst_quat.quat.normalize();

		slerp(&src_quat, &dst_quat, interp, &result_quat);
		Matrix3fSetRotationFromQuat4f(&this->planes[i].arcball.ThisRot, &result_quat.quat);		// Convert Quaternion Into Matrix3fT
		Matrix3fMulMatrix3f(&this->planes[i].arcball.ThisRot, &this->planes[i].arcball.LastRot);				// Accumulate Last Rotation Into This One
		Matrix4fSetRotationFromMatrix3f(&this->planes[i].arcball.Transform, &this->planes[i].arcball.ThisRot);	// Set Our Final Transform's Rotation From This One
*/
		/*
		this->planes[i].v1 = src.planes[i].v1 + interp*dst.planes[i].v1;
		this->planes[i].v2 = src.planes[i].v2 + interp*dst.planes[i].v2;
		this->planes[i].v3 = src.planes[i].v3 + interp*dst.planes[i].v3;
		this->planes[i].v4 = src.planes[i].v4 + interp*dst.planes[i].v4;
*/
		Vector tranv1 = dst.planes[i]._v1 - src.planes[i]._v1;
		Vector tranv2 = dst.planes[i]._v2 - src.planes[i]._v2;
		Vector tranv3 = dst.planes[i]._v3 - src.planes[i]._v3;
		Vector tranv4 = dst.planes[i]._v4 - src.planes[i]._v4;

		this->planes[i]._v1 = src.planes[i]._v1 + interp*tranv1;
		this->planes[i]._v2 = src.planes[i]._v2 + interp*tranv2;
		this->planes[i]._v3 = src.planes[i]._v3 + interp*tranv3;
		this->planes[i]._v4 = src.planes[i]._v4 + interp*tranv4;
/*
		this->planes[i].angleX = src.planes[i].angleX + interp*dst.planes[i].angleX;
		this->planes[i].angleY = src.planes[i].angleY + interp*dst.planes[i].angleY;
		this->planes[i].translate = src.planes[i].translate + interp*dst.planes[i].translate;

		this->planes[i].perpX = src.planes[i].perpX + interp*dst.planes[i].perpX;
		this->planes[i].perpY = src.planes[i].perpY + interp*dst.planes[i].perpY;

		this->planes[i].axisX = src.planes[i].axisX + interp*dst.planes[i].axisX;
		this->planes[i].axisY = src.planes[i].axisY + interp*dst.planes[i].axisY;
		this->planes[i].axisZ = src.planes[i].axisZ + interp*dst.planes[i].axisZ;
*/
		this->planes[i].eval_planeequation();
	}

	eval_centre();
	eval_planeintersections();
}

const PlaneWidgetInCube& PlaneWidgetInCube::operator= (const PlaneWidgetInCube& right)
{
	clear();

	rotmode = right.rotmode;
	invert_cut = right.invert_cut;

	arbmat = right.arbmat;

	points.resize(right.points.size());
	for(int i=0; i<right.points.size(); i++)
		points[i] = right.points[i];

	cutpoints.resize(right.cutpoints.size());
	for(int i=0; i<right.cutpoints.size(); i++)
		cutpoints[i] = right.cutpoints[i];

	slice.resize(right.slice.size());
	for(int i=0; i<right.slice.size(); i++)
		slice[i] = right.slice[i];

	viewsorted.resize(right.viewsorted.size());
	for(int i=0; i<right.viewsorted.size(); i++)
		viewsorted[i] = right.viewsorted[i];

	clipvolcentre = right.clipvolcentre;

	clipvolcentre_points.resize(right.clipvolcentre_points.size());
	for(int i=0; i<right.clipvolcentre_points.size(); i++)
		clipvolcentre_points[i] = right.clipvolcentre_points[i];

	renderall = right.renderall;
	moviemode = right.moviemode;
	render_selected=right.render_selected;

	//cube size
	size = right.size;
	
	Xsize = right.Xsize;
	Ysize = right.Ysize;
	Zsize = right.Zsize;

	//cube coords
	v1 = right.v1;
	v2 = right.v2;
	v3 = right.v3;
	v4 = right.v4;
	v5 = right.v5;
	v6 = right.v6;
	v7 = right.v7;
	v8 = right.v8;

	p1 = right.p1;
	p2 = right.p2;
	p3 = right.p3;
	p4 = right.p4;

	//int selected;
	for(int i=0; i<6; i++)
		selected[i] = right.selected[i];

	planes.resize(right.planes.size());
	for(int i=0; i<right.planes.size(); i++)
		planes[i] = right.planes[i];

	planeplanepoints.resize(right.planeplanepoints.size());
	for(int i=0; i<right.planeplanepoints.size(); i++)
	{
		planeplanepoints[i].resize(right.planeplanepoints[i].size());

		for(int j=0; j<right.planeplanepoints[i].size(); j++)
		{
			planeplanepoints[i][j] = right.planeplanepoints[i][j];
		}
	}

	planeplanepoints2.resize(right.planeplanepoints2.size());
	for(int i=0; i<right.planeplanepoints2.size(); i++)
	{
		planeplanepoints2[i].resize(right.planeplanepoints2[i].size());

		for(int j=0; j<right.planeplanepoints2[i].size(); j++)
		{
			planeplanepoints2[i][j] = right.planeplanepoints2[i][j];
		}
	}

	plane_clippoints.resize(right.plane_clippoints.size());
	for(int i=0; i<right.plane_clippoints.size(); i++)
	{
		plane_clippoints[i].resize(right.plane_clippoints[i].size());

		for(int j=0; j<right.plane_clippoints[i].size(); j++)
		{
			plane_clippoints[i][j] = right.plane_clippoints[i][j];
		}
	}

	//centre of plane intersection in cube
	centre = right.centre;

	return *this;
}