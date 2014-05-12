/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef PCA_H
#define PCA_H

#include <qgl.h>
#include <iostream>
#include <vector>
using namespace std;

#include "Vector.h"

//Principle component class
//for the time being, covariance matrix limited to max 3d data
//as is hardcoded in... (yuck)

class PCA
{

public:

	//our 3D data
	vector< Vector > bounding_box;
	vector< Vector > data;
	vector< Vector > aligned_data;
	vector< Vector > aligned_data2;

	//statistical of the data
	float originalmeanx, originalmeany, originalmeanz;
	float meanx, meany, meanz;
	float variancex, variancey, variancez;
	float covariance;
	vector< vector<float> > covariance_matrix;
	vector< Vector > eigenvectors;

	PCA();
	~PCA();

	void reset(void);
	void gen_rnd1D(int size);
	void gen_rnd2D(int size);
	void gen_rnd3D(int size);

	void eval_meanX(void);
	void eval_meanY(void);
	void eval_meanZ(void);

	void eval_varX(void);
	void eval_varY(void);
	void eval_varZ(void);

	void eval_covariance(void);
	void eval_covariance_matrix(void);

	void eval_PCA(void);
	void eigen(int mv, int n, long double* a, long double* r) const;
	void eval_eigenvectors(void);
	void eval_aligned_data(void);
	void eval_boundingbox(float xscale, float yscale, float zscale);
	void eval_scaled_eigensvectors(float xscale, float yscale, float zscale);

	void display(void);

};

#endif	//PCA_H
