/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef OPENCL_EXEC_H
#define OPENCL_EXEC_H

#include "MyDefines.h"

#ifdef OPENCL_SUPPORT

#include <math.h>
#include <fstream>
#include <string.h>
#include <iostream>
#include <vector>
#include <qgl.h>
#include "glext.h"

using namespace std;

// All OpenCL headers
#if defined (__APPLE__) || defined(MACOSX)
    #include <OpenCL/opencl.h>
	#define GL_SHARING_EXTENSION "cl_APPLE_gl_sharing"
#else
    #include <CL/opencl.h>
    #define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#endif

#include "Timer.h"
#include "MyMath.h"
#include "VolumeObject.h"
#include "Gaussian.h"
#include "OpenCLWrapper.h"

class OpenCLExecuter
{
public:

	OpenCLWrapper*	ocl_wrapper;		  // OpenCL wrapper class
	cl_program		cpProgram;            // OpenCL program

	VolumeObject*	volobj;				  // Volume object

	int filter_width;
	vector < float > filter_kernel;

	OpenCLExecuter();
	~OpenCLExecuter();

	char* ocl_file_contents(const char *filename, int *length);
	void printBuildLog(cl_program* prog);

	void ocl_filter(int src_chan);					//own context
	void ocl_filter_multi(void);			//own context, multi GPU
	void ocl_filter_shared(void);			//shared gl/cl context, single device

	void ocl_init_progam(string fn);		//load and compile the cl program

	void ocl_filter(unsigned char* src_array, unsigned char* dst_array, int w, int h, int d);

	//own context, 3D decomposed as 3 x 1D gaussians
	void ocl_filterGaussian_init(float sigma, float size);
	void ocl_filterGaussian(unsigned char* src_array, unsigned char* dst_array, int w, int h, int d);

	void ocl_filterBoundingBox(int channel, int window_size);

	void ocl_filterBilateral(int spatial, float range, unsigned char* src_array, unsigned char* dst_array, int w, int h, int d);

	void ocl_filterPeronaMalik(float lambda, float dT, unsigned char* src_array, unsigned char* dst_array, int w, int h, int d);
	
	void ocl_parrallelReduction(void);

};

#endif	//OPENCL_SUPPORT

#endif	//OPENCL_EXEC_H
