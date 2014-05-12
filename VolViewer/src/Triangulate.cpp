/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "Triangulate.h"

static const float EPSILON=0.0000000001f;

float Triangulate::Area(const vector< Vector > &contour)
{

  int n = contour.size();

  float A=0.0f;

  for(int p=n-1,q=0; q<n; p=q++)
  {
    A+= contour[p].x*contour[q].y - contour[q].x*contour[p].y;
  }
  return A*0.5f;
}

/*
 InsideTriangle decides if a point P is Inside of the triangle
 defined by A, B, C.
*/
bool Triangulate::InsideTriangle(float Ax, float Ay,
                      float Bx, float By,
                      float Cx, float Cy,
                      float Px, float Py)

{
  float ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
  float cCROSSap, bCROSScp, aCROSSbp;

  ax = Cx - Bx;  ay = Cy - By;
  bx = Ax - Cx;  by = Ay - Cy;
  cx = Bx - Ax;  cy = By - Ay;
  apx= Px - Ax;  apy= Py - Ay;
  bpx= Px - Bx;  bpy= Py - By;
  cpx= Px - Cx;  cpy= Py - Cy;

  aCROSSbp = ax*bpy - ay*bpx;
  cCROSSap = cx*apy - cy*apx;
  bCROSScp = bx*cpy - by*cpx;

  return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
};
void Triangulate::Sort(vector< Vector > points,vector< Vector > &sorted_points, Vector vn)
{
	if(!points.empty())
	{
		//compute our centroid...
		Vector centre = 0.0;
		for(int j=0; j<points.size(); j++)
		{
			centre += points[j];
		}
		centre /= (float)points.size();

		vector<float> angles;
		vector<int> visited;
		angles.resize(points.size());
		visited.resize(points.size());

		//use points[0] as our
		//reference point...
		visited[0]=1.0;
		sorted_points.push_back(points[0]);

		Vector v0 = points[0]-centre;
		v0.normalize();

		Vector v2 = points[1];
		v2.normalize();

		vn.normalize();

		Vector v1;
		Vector vcross;
		float vdot;
		for(int ii=1; ii<points.size(); ii++)
		{
			v1 = points[ii]-centre;
			v1.normalize();
			
			vcross.cross(v0, v1);
			
			vdot = v1.dot(v2);

			float signed_angle = atan2(  vn.dot(vcross), v0.dot(v1)); 
			signed_angle *= (180.0/3.141592654);
			
			if(signed_angle<0.0) signed_angle += 360.0;

			angles[ii]=signed_angle;
		}

		float angle=-999999.0;
		int index=-1;
		for(int ii=1; ii<points.size(); ii++)
		{
			angle=-999999.0;
			index=-1;

			for(int j=1; j<points.size(); j++)
			{
				if(visited[j]==0.0)
				{
					if(angle<angles[j])
					{
						angle = angles[j];
						index = j;
					}
				}
			}

			if(index!=-1)
			{
				sorted_points.push_back(points[index]);
				visited[index]=1.0;
			}
		}
	}
}
bool Triangulate::Snip(const vector< Vector > &contour,int u,int v,int w,int n,int *V)
{
  int p;
  float Ax, Ay, Bx, By, Cx, Cy, Px, Py;

  Ax = contour[V[u]].x;
  Ay = contour[V[u]].y;

  Bx = contour[V[v]].x;
  By = contour[V[v]].y;

  Cx = contour[V[w]].x;
  Cy = contour[V[w]].y;

  if ( EPSILON > (((Bx-Ax)*(Cy-Ay)) - ((By-Ay)*(Cx-Ax))) ) return false;

  for (p=0;p<n;p++)
  {
    if( (p == u) || (p == v) || (p == w) ) continue;
    Px = contour[V[p]].x;
    Py = contour[V[p]].y;
    if (InsideTriangle(Ax,Ay,Bx,By,Cx,Cy,Px,Py)) return false;
  }

  return true;
}

bool Triangulate::Process(const vector< Vector > &contour,vector< Vector > &result)
{
  /* allocate and initialize list of Vertices in polygon */

  int n = contour.size();
  if ( n < 3 ) return false;

  /*vector< Vector > sorted;
  Vector normal;
  normal.get_normal(contour[2],contour[1],contour[0]);

  Sort(contour,sorted,normal);*/

  int *V = new int[n];

  /* we want a counter-clockwise polygon in V */

  if ( 0.0f < Area(contour) )
    for (int v=0; v<n; v++) V[v] = v;
  else
    for(int v=0; v<n; v++) V[v] = (n-1)-v;

  int nv = n;

  /*  remove nv-2 Vertices, creating 1 triangle every time */
  int count = 2*nv;   /* error detection */

  for(int m=0, v=nv-1; nv>2; )
  {
    /* if we loop, it is probably a non-simple polygon */
    if (0 >= (count--))
    {
      //** Triangulate: ERROR - probable bad polygon!
      return false;
    }

    /* three consecutive vertices in current polygon, <u,v,w> */
    int u = v  ; if (nv <= u) u = 0;     /* previous */
    v = u+1; if (nv <= v) v = 0;     /* new v    */
    int w = v+1; if (nv <= w) w = 0;     /* next     */

    if ( Snip(contour,u,v,w,nv,V) )
    {
      int a,b,c,s,t;

      /* true names of the vertices */
      a = V[u]; b = V[v]; c = V[w];

      /* output Triangle */
      result.push_back( contour[a] );
      result.push_back( contour[b] );
      result.push_back( contour[c] );

      m++;

      /* remove v from remaining polygon */
      for(s=v,t=v+1;t<nv;s++,t++) V[s] = V[t]; nv--;

      /* resest error detection counter */
      count = 2*nv;
    }
  }

  delete V;

  return true;
}
