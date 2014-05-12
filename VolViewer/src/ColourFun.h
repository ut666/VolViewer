/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef COLOURFUN_H
#define COLOURFUN_H

#include <math.h>
#include "Vector.h"

void HSVtoRGB(Vector hsv, Vector* rgb);
void RGBtoHSV(Vector rgb, Vector* hsv); 
void CompHSV(Vector hsv, Vector* comphsv);

#endif // COLOURFUN_H