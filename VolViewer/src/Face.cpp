/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include <math.h>
#include <vector>
//#include <algorithm>
using namespace std;

#include "Face.h"

Face::Face()
{
	for(int x = 0; x<3; x++)
	{
		vert[x] = 0;
		facenormal = 0;
		vertindex[x] = 0;
		uv[x][0] = 0.0f;
		uv[x][1] = 0.0f;
	}
}
void Face::orient(void)
{
/*	vector<Vector> face;
	
	Vector v0 = *vert[0];
	Vector v1 = *vert[1];
	Vector v2 = *vert[2];
	int i0 = vertindex[0];
	int i1 = vertindex[1];
	int i2 = vertindex[2];

	face.push_back(v0);
	face.push_back(v1);
	face.push_back(v2);

	sort(face.begin(), face.end());

	*vert[0] = v0 = face[2];
	*vert[1] = v1 = face[1];		
	*vert[2] = v2 = face[0];

	if(face[0]==v0) 
	{
		vertindex[0] = i0;
	}
	else if(face[0]==v1) 
	{
		vertindex[0] = i1;

	}
	else if(face[0]==v2) 
	{
		vertindex[0] = i2;
	}
	//====================
	if(face[1]==v0) 
	{
		vertindex[1] = i0;
	}
	else if(face[1]==v1) 
	{
		vertindex[1] = i1;

	}
	else if(face[1]==v2) 
	{
		vertindex[1] = i2;
	}
	//====================
	if(face[2]==v0) 
	{
		vertindex[2] = i0;
	}
	else if(face[2]==v1) 
	{
		vertindex[2] = i1;

	}
	else if(face[2]==v2) 
	{
		vertindex[2] = i2;
	}
*/
//	facenormal.get_normal(v0, v1, v2);
}
void Face::evaluate_face_normal(void)
{
	//orient();
	facenormal.get_normal(*vert[0], *vert[1], *vert[2]);
}	

void Face::inverse_map(Vector& p, float& u, float& v)
{
	Vector v1, v2, ip;

	v1 = vert[1] - vert[0];
	v2 = vert[2] - vert[0];

	ip.x = p.x - vert[0]->x;
	ip.y = p.y - vert[0]->y;
	ip.z = p.z - vert[0]->z;
	
	float det;

	det=v1.x*v2.y-v1.y*v2.x;
	if(det!=0.0f)
	{
		u=(ip.x*v2.y-ip.y*v2.x)/det;
		v=(v1.x*ip.y-v1.y*ip.x)/det;
		return;
	}

	det=v1.x*v2.z-v1.z*v2.x;
	if(det!=0.0f)
	{
		u=(ip.x*v2.z-ip.z*v2.x)/det;
		v=(v1.x*ip.z-v1.z*ip.x)/det;
		return;
	}

	det=v1.y*v2.z-v1.z*v2.y;
	u=(ip.y*v2.z-ip.z*v2.y)/det;
	v=(v1.y*ip.z-v1.z*ip.y)/det;
}

void Face::forward_map(float& u,float& v, Vector& p)
{
	p.x=vert[0]->x+
		(vert[1]->x-vert[0]->x)*u+
		(vert[2]->x-vert[0]->x)*v;
	p.y=vert[0]->y+
		(vert[1]->y-vert[0]->y)*u+
		(vert[2]->y-vert[0]->y)*v;
	p.z=vert[0]->z+
		(vert[1]->z-vert[0]->z)*u+
		(vert[2]->z-vert[0]->z)*v;
}
float Face::signed_area(void)
{
	float ax = vert[0]->x - vert[2]->x;
	float ay = vert[0]->y - vert[2]->y;
	float bx = vert[1]->x - vert[2]->x;
	float by = vert[1]->y - vert[1]->y;

	float areaTrapezoid = ax*by - ay*bx;
	float areaTriangle = areaTrapezoid / 2.0;
	return areaTriangle;

	//float LeftTurn = areaTrapezoid > 0.0;

//	float CompareAngle = 

//	if (areaTrapezoid > 0.0) return 1;
//    else if (areaTrapezoid == 0.0) return 0;
//    else return -1;
}

int Face::line_intersection (Vector V1, Vector V2, Vector &VHit)
{
	Vector VIntersect;

	// Find Triangle Normal
	evaluate_face_normal();

	// Find distance from V1 and V2 to the plane defined by the triangle
	Vector v;
	v = V1-*vert[0];
	float fDst1 = v.dot(facenormal);	
	v = V2-*vert[0];
	float fDst2 = v.dot(facenormal);

	if ( (fDst1 * fDst2) >= 0.0f) return 0;  // line doesn't cross the triangle.
	if ( fDst1 == fDst2) {return 0;} // line and plane are parallel

	// Find point on the ray that intersects with the plane
	VIntersect = V1 + (V2-V1) * ( -fDst1/(fDst2-fDst1) );

	// Find if the interesection point lies inside the triangle by testing it against all edges
	Vector VTest;
	VTest.cross(facenormal, *vert[1]-*vert[0]);
	if ( VTest.dot(VIntersect-*vert[0]) < 0.0f ) return 0;
	VTest.cross(facenormal, *vert[2]-*vert[1]);
	if ( VTest.dot(VIntersect-*vert[1]) < 0.0f ) return 0;
	VTest.cross(facenormal, *vert[0]-*vert[2]);
	if ( VTest.dot(VIntersect-*vert[0]) < 0.0f ) return 0;
	
	VHit = VIntersect;

	return 1;
}
float Face::perimeter(void)
{
	Vector v0 = *vert[1]-*vert[0];
	Vector v1 = *vert[2]-*vert[1];
	Vector v2 = *vert[0]-*vert[2];

	float perim = v0.length()+v1.length()+v2.length();
	return perim;
}
float Face::area(void)
{
	//Heron's Formula for the area of a triangle

	float p = perimeter() / 2.0;

	Vector v0 = *vert[1]-*vert[0];
	Vector v1 = *vert[2]-*vert[1];
	Vector v2 = *vert[0]-*vert[2];

	float a = v0.length();
	float b = v1.length();
	float c = v2.length();

	float area = p*(p-a)*(p-b)*(p-c);
	area = sqrt(area);
	return area;
}
