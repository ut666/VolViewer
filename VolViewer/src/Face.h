/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef FACE_H
#define FACE_H
 
#include "Vector.h"

class Face
{
	public:
	
		Vector	*vert[3];			//pointer to 3 face vertices
		Vector	facenormal;			//the one face normal
		int		vertindex[3];		//the three vertex indeces

		float uv[3][2];
					
		Face();

		void orient(void);
		void evaluate_face_normal(void);
		void inverse_map(Vector& p, float& u, float& v);
		void forward_map(float& u,float& v, Vector& p);
		float signed_area(void);
		int line_intersection (Vector V1, Vector V2, Vector &VHit);
		float perimeter(void);
		float area(void);
};

#endif // FACE_H