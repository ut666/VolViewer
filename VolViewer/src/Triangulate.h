/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef TRIANGULATE_H
#define TRIANGULATE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>  // Include STL vector class.

using namespace std;

#include "Vector.h"

class Triangulate
{
public:

  static void Sort(vector< Vector > points,vector< Vector > &sorted_points, Vector vn);

  // triangulate a contour/polygon, places results in STL vector
  // as series of triangles.
  static bool Process(const vector< Vector > &contour,
                      vector< Vector > &result);

  // compute area of a contour/polygon
  static float Area(const vector< Vector > &contour);

  // decide if point Px/Py is inside triangle defined by
  // (Ax,Ay) (Bx,By) (Cx,Cy)
  static bool InsideTriangle(float Ax, float Ay,
                      float Bx, float By,
                      float Cx, float Cy,
                      float Px, float Py);
private:
  static bool Snip(const vector< Vector > &contour,int u,int v,int w,int n,int *V);

};

#endif //TRIANGULATE_H