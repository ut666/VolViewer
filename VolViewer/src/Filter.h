/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef FILTER_H
#define FILTER_H

#include <math.h>
#include <fstream>
#include <iostream>
#include <algorithm>

#include <Q3ProgressDialog>

#include <qgl.h>
#include "glext.h"

#include "MyDefines.h"

#ifdef OPENCL_SUPPORT
// All OpenCL headers
#if defined (__APPLE__) || defined(MACOSX)
    #include <OpenCL/opencl.h>
#else
    #include <CL/opencl.h>
#endif
#endif

#define TWOPI 		6.283185307179586476925286766558f
#define PI 			3.141592653589793238462643383279f

#include "MyDefines.h"
#include "Global.h"
#include "Gaussian.h"
#include "Sobel.h"
#include "PCA.h"
#include "Vector.h"
#include "VolumeObject.h"
#include "Sieve.h"
#include "MaxFlowSegmentation.h"
#include "PeronaMalikFlow.h"
#include "Floodfill.h"
#include "Timer.h"

#ifdef OPENCL_SUPPORT
	#include "OpenCLExecuter.h"
#endif

class Filter
{

	public:
		
		bool showGUI;

#ifdef OPENCL_SUPPORT
		OpenCLExecuter* ocl_boundingbox_ptr;
		OpenCLExecuter* ocl_gaussian_ptr;
		OpenCLExecuter* ocl_dilate_ptr;
		OpenCLExecuter* ocl_erode_ptr;
		OpenCLExecuter* ocl_median_ptr;
		OpenCLExecuter* ocl_peronamalik_ptr;
		OpenCLExecuter* ocl_bilateral_ptr;
#endif

		bool noprogressbar;
		VolumeObject* volobj;
		Q3ProgressDialog*		progressbar;
		
		//======================================================
		//image dimensions
		//======================================================
		int width, height, depth;

		//======================================================
		//source image array and size
		//======================================================
		unsigned char* source_array;
		unsigned char* source_array2;

		//======================================================
		//sobel result array
		//======================================================
		short* sobel_result_array;

		//======================================================
		//filtered image array and size
		//======================================================
		unsigned char* result_array;
		
		Gaussian gauss;
		vector < double > mask;
		float sum;

		int tempsize;

		MaxFlowSegmentation maxflow;
		PeronaMalikFlow peronamalik;

		Filter();
		~Filter();
		
		//======================================================
		//array alloc stuff 3D
		//======================================================
		void set_source3D(vector<unsigned char> *dataarray, int channel);
		void get_source3D(vector<unsigned char> *dataarray, int channel);
		void get_sources3D(vector<unsigned char> *dataarray0, int channel0, vector<unsigned char> *dataarray1, int channel1);

		void copy_source3D(void);
		void allocate_source3D(void);
		void allocate_result3D(void);
		void allocate_sobel3D(void);
		void result2source3D(void);
		void result2newarray3D(unsigned char* new_array);
		void sobel_result2newarray3D(short* new_array);
		//======================================================

		double invtan(double x, double y);

		//======================================================
		//3D image filtering
		//======================================================
		vector< vector< vector<double> > > create_gaussianmask3D(int size, double sigma);
		vector< double > create_gaussianmask1D(int size, double sigma);

		void apply_gaussian3D(int kernel_radius, double sigma, int qual);
		void apply_sobel3D(int mode);
		void apply_unsharpmask3D(int brightness, int contrast, float sigma, int gsize);
		void auto_levels(void);
		void apply_contrast_stretching(void);
		void apply_harriscorner(void);
		void apply_median(int scale);
		void apply_mean(int scale);
		
		void apply_contrast_stretching_filter(vector<bool> channels);
		void apply_auto_levels_filter(vector<bool> channels);
		void apply_sobel3D_filter(vector<bool> channels);
		void apply_gaussian_filter(int kernel_radius, double sigma, int dimensionality, vector<bool> channels);
		
		void gaussian_filter_decomp_init(int kr, double sigma);
		void apply_gaussian_filter_decomp(int kernel_radius, double sigma, int dimensionality, vector<bool> channels);
		void apply_gaussian_filter_decomp_window(int kernel_radius, double sigma, int dimensionality, vector<bool> channels);
		void gaussian_filter_decomp_dealloc(void);

		void apply_bilateral_filter(float spatial, float range, vector<bool> channels);

		void apply_unsharpmask_filter(int kernel_radius, double sigma, int dimensionality, vector<bool> channels);
		void apply_median_filter(int kernel_radius, int dimensionality, vector<bool> channels);
		void apply_mean_filter(int kernel_radius, int dimensionality, vector<bool> channels);
		void apply_dilate_filter(int kernel_radius, int dimensionality, vector<bool> channels);
		void apply_erode_filter(int kernel_radius, int dimensionality, vector<bool> channels);
		void apply_copyto(int src, int dest);
		void apply_addto(int src, int dest);
		void apply_replace(int src, int repop, int in, int with);
		void apply_setoperator(int src, int msk, int setoperator);
		void apply_arithmoperator(int src, int msk, int arithmoperator);
		void apply_maxflow(int src, int dst, double w, double v, double k, vector<int> bg, vector<int> fg);
		void apply_peronamalik(int dim, double l, double dt, int inter, vector<bool> channels);
		void apply_sieve(int src, int dst, int size);

		void apply_floodfill(int src, int dst, int window, int x, int y, int z);

		void apply_threshold(int channel, int value);
		void apply_collapse(void);
		void apply_binarize(int channel);
		void apply_clear(int channel);
		void apply_invert(int channel);
		void apply_swap(int srcA, int srcB);
		int apply_count(int channel);

		int apply_stepperdilate(Vector p, int srcchan, int dstchan, int seed_window, int minsize, int gaussian_width, float gaussian_strength, int gaussian_window);

#ifdef OPENCL_SUPPORT
		int apply_stepperdilate_gpu(Vector p, int srcchan, int dstchan, int seed_window, int minsize, int gaussian_width, float gaussian_strength, int gaussian_window);
		void apply_peronamalik_gpu(int dim, double l, double dt, int inter, vector<bool> channels);
		void apply_gaussian_filter_gpu(int kernel_radius, double sigma, int dimensionality, vector<bool> channels);
		void apply_median_filter_gpu(int kernel_radius, int dimensionality, vector<bool> channels);
		void apply_dilate_filter_gpu(int kernel_radius, int dimensionality, vector<bool> channels);
		void apply_erode_filter_gpu(int kernel_radius, int dimensionality, vector<bool> channels);
		void apply_bilateral_filter_gpu(float spatial, float range, vector<bool> channels);
#endif

/*		void apply_canny3D(int kernel_radius, double sigma, double minthresh, double maxthresh);
		void nonMaximaSuppression3D(short* sobel_v, short* sobel_h, short* sobel_d);
		void nMS3D(short* sobel_v, short* sobel_h, short* sobel_d, int x, int y, int z, int channel);
		vector<Vector> find_neighbours3D(int x, int y, int z, int channel);
		void histeresis3D(unsigned char* non_maxima_sup, double minthresh, double maxthresh);
*/
};

#endif //FILTER_H
