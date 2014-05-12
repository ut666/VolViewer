/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef CONSTANT_H
#define CONSTANT_H

#include <cmath>

const int n=1;
const int FOUR_NEIGHBORS=4;
const int EIGH_NEIGHBORS=8;
const int TWENTYSIX_NEIGHBORS=26;
//const double PI=3.141592653589793238462643;
//const double SQRT2=sqrt(2);
const double INFTY = 1.0e+10;
const double ZERO_POSITIVE=0.000000000001;//1.0e-20;
const double ZERO_NEGATIVE=-0.000000000001;//-1.0e-20;

const int FILTER_SMOOTHED=1;
//const int FILTER_SOBEL=2;
const int FILTER_SMOOTHED_H=3;
const int FILTER_SOBEL_X=4;
const int FILTER_SMOOTHED_V=5;
const int FILTER_SOBEL_Y=6;
const int FILTER_ISOTROPIC=7; //see "fundamental of digital image processing" Jain89
const int FILTER_ROBERTS=8;   //see "fundamental of digital image processing" Jain89

const int INFORMATION_GAIN=3;

const int RED_CHANNEL=0;
const int GREEN_CHANNEL=1;
const int BLUE_CHANNEL=2;
const int ALPHA_CHANNEL=3;
const int RGBA_COLOR=4;


#endif