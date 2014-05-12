/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef MATH_H
#define MATH_H

#include <stdlib.h>
#include <cmath>
#include <math.h>

//using namespace std;

#define PI 								3.141592653589793238462643383279f
#define TWOPI 							6.283185307179586476925286766558f

float roundup(float number, int decplaces);
float randomFloat(float min, float max);

/*
float fcos(float a);
float fsin(float a);
#define cosf(x) 						((float)fcos((x)))
#define sinf(x) 						((float)fsin((x)))
*/


#endif // MATH_H