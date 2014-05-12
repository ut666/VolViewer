/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "Sobel.h"

/*
	========================================================
    (3d) naming convention (3x3x3)
	========================================================

	[x-1, y+1, z-1]		[x, y+1, z-1]		[x+1, y+1, z-1]

	[x-1, y,   z-1]		[x, y,   z-1]		[x+1, y,   z-1]

	[x-1, y-1, z-1]		[x, y-1, z-1]		[x+1, y-1, z-1]

	========================================================

	[x-1, y+1, z  ]		[x, y+1, z  ]		[x+1, y+1, z  ]
	
	[x-1, y,   z  ]		[x, y,   z  ]		[x+1, y,   z  ]
	
	[x-1, y-1, z  ]		[x, y-1, z  ]		[x+1, y-1, z  ]

    ========================================================

	[x-1, y+1, z+1]		[x, y+1, z+1]		[x+1, y+1, z+1]
	
	[x-1, y,   z+1]		[x, y,   z+1]		[x+1, y,   z+1]
	
	[x-1, y-1, z+1]		[x, y-1, z+1]		[x+1, y-1, z+1]

  ========================================================
*/
/*
//	Horizontal 3D Sobel
double sobelX3d[3][3][3] = {-2, 0, 2,	//z-1
							-3, 0, 3
							-2, 0, 2,

							-3, 0, 3,	//z
							-6, 0, 6,
							-3, 0, 3,

							-2, 0, 2,	//z+1
							-3, 0, 3,
							-2, 0, 2}


//	Vertical 3D Sobel
double sobelY3d[3][3][3] = {-2,-3,-2,	//z-1
							 0, 0, 0
							 2, 3, 2,

							-3,-6,-3,	//z
							 0, 0, 0,
							 3, 6, 3,

							-2,-3,-2,	//z+
							 0, 0, 0,
							 2, 3, 2}

//	Depth3D Sobel
double sobelZ3d[3][3][3] = {-2,-3,-2,	//z-1
						    -3,-6,-3,
							-2,-3,-2,

							 0, 0, 0,	//z
							 0, 0, 0,
							 0, 0, 0,

							 2, 3, 2,	//z+1
							 3, 6, 3,
							 2, 3, 2}

*/
Sobel::Sobel()
{
	//==============================================================
	
	//	Horizontal 3D Sobel kernel
	sobelX3d[0][0][0] = -2;  sobelX3d[0][0][1] = 0;  sobelX3d[0][0][2] = 2;
	sobelX3d[0][1][0] = -3;  sobelX3d[0][1][1] = 0;  sobelX3d[0][1][2] = 3;
	sobelX3d[0][2][0] = -2;  sobelX3d[0][2][1] = 0;  sobelX3d[0][2][2] = 2;

	sobelX3d[1][0][0] = -3;  sobelX3d[1][0][1] = 0;  sobelX3d[1][0][2] = 3;
	sobelX3d[1][1][0] = -6;  sobelX3d[1][1][1] = 0;  sobelX3d[1][1][2] = 6;
	sobelX3d[1][2][0] = -3;  sobelX3d[1][2][1] = 0;  sobelX3d[1][2][2] = 3;

	sobelX3d[2][0][0] = -2;  sobelX3d[2][0][1] = 0;  sobelX3d[2][0][2] = 2;
	sobelX3d[2][1][0] = -3;  sobelX3d[2][1][1] = 0;  sobelX3d[2][1][2] = 3;
	sobelX3d[2][2][0] = -2;  sobelX3d[2][2][1] = 0;  sobelX3d[2][2][2] = 2;

	//	Vertical 3D Sobel
	sobelY3d[0][0][0] = -2;  sobelY3d[0][0][1] = -3; sobelY3d[0][0][2] = -2;
	sobelY3d[0][1][0] =  0;  sobelY3d[0][1][1] = 0;  sobelY3d[0][1][2] = 0;
	sobelY3d[0][2][0] =  2;  sobelY3d[0][2][1] = 3;  sobelY3d[0][2][2] = 2;

	sobelY3d[1][0][0] = -3; sobelY3d[1][0][1] = -6; sobelY3d[1][0][2] = -3;
	sobelY3d[1][1][0] = 0;  sobelY3d[1][1][1] = 0;  sobelY3d[1][1][2] = 0;
	sobelY3d[1][2][0] = 3;  sobelY3d[1][2][1] = 6;  sobelY3d[1][2][2] = 3;

	sobelY3d[2][0][0] = -2;  sobelY3d[2][0][1] = -3; sobelY3d[2][0][2] = -2;
	sobelY3d[2][1][0] =  0;  sobelY3d[2][1][1] = 0;  sobelY3d[2][1][2] = 0;
	sobelY3d[2][2][0] =  2;  sobelY3d[2][2][1] = 3;  sobelY3d[2][2][2] = 2;

	//	Depth 3D Sobel
	sobelZ3d[0][0][0] = -2;  sobelZ3d[0][0][1] = -3; sobelZ3d[0][0][2] = -2;
	sobelZ3d[0][1][0] = -3;  sobelZ3d[0][1][1] = -6;  sobelZ3d[0][1][2] = -3;
	sobelZ3d[0][2][0] = -2;  sobelZ3d[0][2][1] = -3;  sobelZ3d[0][2][2] = -2;

	sobelZ3d[1][0][0] = 0; sobelZ3d[1][0][1]  = 0; sobelZ3d[1][0][2]  = 0;
	sobelZ3d[1][1][0] = 0;  sobelZ3d[1][1][1] = 0;  sobelZ3d[1][1][2] = 0;
	sobelZ3d[1][2][0] = 0;  sobelZ3d[1][2][1] = 0;  sobelZ3d[1][2][2] = 0;

	sobelZ3d[2][0][0] = 2;  sobelZ3d[2][0][1] = 3;  sobelZ3d[2][0][2] = 2;
	sobelZ3d[2][1][0] = 3;  sobelZ3d[2][1][1] = 6;  sobelZ3d[2][1][2] = 3;
	sobelZ3d[2][2][0] = 2;  sobelZ3d[2][2][1] = 3;  sobelZ3d[2][2][2] = 2;

	//==============================================================


	sobelX2d[0][0] = -1; sobelX2d[0][1] = 0; sobelX2d[0][2] = 1;
	sobelX2d[1][0] = -2; sobelX2d[1][1] = 0; sobelX2d[1][2] = 2; 
	sobelX2d[2][0] = -1; sobelX2d[2][1] = 0; sobelX2d[2][2] = 1;

	sobelY2d[0][0] = -1; sobelY2d[0][1] = -2; sobelY2d[0][2] = -1;
	sobelY2d[1][0] = 0;  sobelY2d[1][1] = 0;  sobelY2d[1][2] = 0;
	sobelY2d[2][0] = 1;  sobelY2d[2][1] = 2;  sobelY2d[2][2] = 1;
}

Sobel::~Sobel()
{

}
