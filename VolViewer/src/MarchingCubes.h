/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef MCUBES_H
#define MCUBES_H

//#include "Global.h"
#include "ColourFun.h"
#include "Vector.h"
#include "Face.h"
#include "Mesh.h"
#include "VolumeObject.h"
#include "Filter.h"
#include "Gaussian.h"

#include <qgl.h>
#include <set>
#include <math.h>
#include <vector>
#include <string>
#include <fstream>
#include <q3progressdialog.h>

using namespace std;

class MCubes
{
	public:

	int helpA;
	int helpB;

	Q3ProgressDialog*		progressbar;
	VolumeObject* volobj;

	bool selected_channels[3];
	int r,g,b;

	unsigned char* tfunc_lookup;

	//our mesh object
	Mesh* mesh;

	//we use this to store gaussians, avoid recompute...
	map<int,float> gaussianvalues_map;

	//we use stl::set to make sure
	//no duplicate verts & tris are added
	Vector last_added_vert;
	set<Vector> verts_set;
	//vector<int> verts_set_size_cube;
	
	int gaussian_kernel_radius;
	vector< vector< vector<double> > > mask;
	vector < double > mask1d;

	GLint     iDataSetSize;
	GLfloat   fStepSize;
	GLfloat   fTargetValue;

	int finished;

	MCubes  ();

	void progress(int i, int size);

	void clear(void);

	GLfloat fGetOffset(GLfloat fValue1, GLfloat fValue2, GLfloat fValueDesired);
	
	GLfloat fSample(GLfloat x, GLfloat y, GLfloat z, int gaussflag);
	GLvoid vGetNormal(Vector &rfNormal, GLfloat fX, GLfloat fY, GLfloat fZ, int gaussflag);
	
	GLvoid vMarchCube(GLfloat x, GLfloat y, GLfloat z, GLfloat fScale, int gaussflag);

	GLvoid vMarchingCubes(bool* channels, int qual, int value, unsigned char* tfunc);
	GLvoid vMarchingCubes(bool* channels, int qual, int value, unsigned char* tfunc, float size, float sigma);
};

#endif // MCUBES_H