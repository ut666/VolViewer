/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef POINT3D_H
#define POINT3D_H

class Point3D
{
	public:
		unsigned int x,y,z;
		Point3D()
		{
			x=0;
			y=0;
			z=0;
		} 

		Point3D(int x1, int y1, int z1)
		{
			x=x1;
			y=y1;
			z=z1;
		}
};

#endif //POINT3D_H