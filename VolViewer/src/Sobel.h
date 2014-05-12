/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef SOBEL_H
#define SOBEL_H

#define SOBEL_HORIZ 			0
#define SOBEL_VERTI 			1
#define SOBEL_DEPTH 			2
#define SOBEL_HV	 			3
#define SOBEL_HVD 				4
#define SOBEL_HVDIAG			5

class Sobel
{
	public:

		double sobelX3d[3][3][3];
		double sobelY3d[3][3][3];
		double sobelZ3d[3][3][3];

		double sobelX2d[3][3];
		double sobelY2d[3][3];

		Sobel();
		~Sobel();
};
#endif //SOBEL_H