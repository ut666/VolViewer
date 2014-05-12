/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef GAUSSIAN_H
#define GAUSSIAN_H

#pragma warning(disable:4786)

#include <qgl.h>
#include <math.h>
#include <iostream>

#include <vector>

using namespace std;

#define PI 			3.141592653589793238462643383279f
#define TWOPI 		6.283185307179586476925286766558f


class Gaussian
{

public:

	//(NB) for a guassian function
	// the integral from +infity to -infinity is always == 1.0
	vector<float> fmask1d;
	vector<double> mask1d;
	vector< vector<double> > mask2d;
	vector< vector< vector<double> > > mask3d;

	//kernel size
	int size;

	//the standard deviation of our gaussian (spread)
	double sigma;

	//this really is the full width at half height
	double half_width;
	
	Gaussian();
	~Gaussian();

	void clear_all(void);
	
	void eval_half_width(void);
	void eval_sigma(int size);

	double gaussian1d(double x);
	double gaussian2d(double x, double y);
	double gaussian3d(double x, double y, double z);

	const vector<float>							gaussianmask1Df(float s, float size);
	const vector<double>						gaussianmask1D(double sigma, double size);
	const vector< vector<double> >				gaussianmask2D(double sigma, double size);
	const vector< vector< vector<double> > >	gaussianmask3D(double sigma, double size);

	void display1d(void);
	void display2d(void);
	void display3d(void);
};

#endif //GAUSSIAN_H