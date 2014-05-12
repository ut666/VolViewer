/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef BSPLINE_H
#define BSPLINE_H

#include <iostream>
#include <vector>
using namespace std;

#include "Vector.h"

class BSpline
{
public:

	BSpline();
	~BSpline();

	bool linear;

	//control points
	int num_control_points;
	vector<Vector> control_points;
	vector<Vector> control_points_colours;

	//curve points
	int num_curve_points;
	vector<Vector> curve_points;

	//degree of the polynomial
	int poly_degree;

	//our curve intervals
	vector<int> u;

	void clear(void);
	void init(int ncontp, int ncurvp, int d);
	void compute(void);
	void compute_with_skip(int skip);
	
	// calculate the blending value
	double blend(int k, int t, double v); 
	
	// figure out the knots
	void compute_intervals(void);

	//compute our curve point
	void compute_point(double v, Vector* output);
	void compute_point_with_skip(double v, Vector* output, int skip);

	const BSpline& operator= (const BSpline& right);

};

#endif // BSPLINE_H