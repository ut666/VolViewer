/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "Filter.h"

Filter::Filter()
{
	showGUI = true;

	source_array = NULL;
	source_array2 = NULL;
	sobel_result_array = NULL;
	width=height=depth=0;

#ifdef OPENCL_SUPPORT
	ocl_gaussian_ptr = NULL;
	ocl_dilate_ptr = NULL;
	ocl_erode_ptr = NULL;
	ocl_peronamalik_ptr = NULL;
#endif
}

Filter::~Filter()
{
	source_array = NULL;

#ifdef OPENCL_SUPPORT
	ocl_gaussian_ptr = NULL;
	ocl_dilate_ptr = NULL;
	ocl_erode_ptr = NULL;
	ocl_peronamalik_ptr = NULL;
#endif
}

void Filter::allocate_source3D(void)
{
	if(source_array) delete[] source_array;
	source_array = new unsigned char [width*height*depth*3];
}
void Filter::allocate_result3D(void)
{
	if(result_array) delete[] result_array;
	result_array = new unsigned char [width*height*depth*3];
}

void Filter::allocate_sobel3D(void)
{
	if(sobel_result_array) delete[] sobel_result_array;
	sobel_result_array = new short [width*height*depth*1];
}

void Filter::result2source3D(void)
{
	if(result_array==0) return;
	if(source_array==0) return;

	int size = width*height*depth*4;
	
	for(int i=0; i<size; i++)
	{		
		source_array[i] = result_array[i];	
	}
}
void Filter::result2newarray3D(unsigned char* new_array)
{
	if(result_array==0)return;


	int size = width*height*depth*3;

	for(int i=0; i<size; i++)
	{
		new_array[i] = result_array[i];	
	}
}

void Filter::sobel_result2newarray3D(short* new_array)
{
	if(sobel_result_array==0)return;

	int size = width*height*depth*1;
	for(int i=0; i<size; i++)
	{
		new_array[i] = sobel_result_array[i];	
	}
}
void Filter::set_source3D(vector<unsigned char> *dataarray, int channel)
{
	printf("Saving source image\n");

	int subvol_xval, subvol_yval, subvol_zval;

	int index;
	int index2;
	for(int x=volobj->boundingboxMin.x; x<volobj->boundingboxMax.x; x++)
		for(int y=volobj->boundingboxMin.y; y<volobj->boundingboxMax.y; y++)
			for(int z=volobj->boundingboxMin.z; z<volobj->boundingboxMax.z; z++)
			{
				subvol_xval = x-volobj->boundingboxMin.x;
				subvol_yval = y-volobj->boundingboxMin.y;
				subvol_zval = z-volobj->boundingboxMin.z;
				index = ((((subvol_zval*height)+(subvol_yval))*width)+(subvol_xval));	

				index2 = volobj->get_index1D(x,y,z);

				volobj->texture3d[3*index2+channel] = dataarray->at(index);
				//volobj->texture3d[3*index2+2] = 255;
			}
			
	printf("done\n");
}
void Filter::get_sources3D(vector<unsigned char> *dataarray0, int channel0, vector<unsigned char> *dataarray1, int channel1)
{
	printf("Copying source images\n");
	dataarray0->resize(width*height*depth);
	dataarray1->resize(width*height*depth);

	int subvol_xval, subvol_yval, subvol_zval;

	int index;
	int index2;
	for(int x=volobj->boundingboxMin.x; x<volobj->boundingboxMax.x; x++)
		for(int y=volobj->boundingboxMin.y; y<volobj->boundingboxMax.y; y++)
			for(int z=volobj->boundingboxMin.z; z<volobj->boundingboxMax.z; z++)
			{
				subvol_xval = x-volobj->boundingboxMin.x;
				subvol_yval = y-volobj->boundingboxMin.y;
				subvol_zval = z-volobj->boundingboxMin.z;

				index = ((((subvol_zval*height)+(subvol_yval))*width)+(subvol_xval));	
				//index2 = volobj->get_index1D(x,y,z);
				index2 = ((((z*volobj->texheight)+(y))*volobj->texwidth)+(x));

				dataarray0->at(index) = volobj->texture3d[3*index2+channel0];
				dataarray1->at(index) = volobj->texture3d[3*index2+channel1];
				//printf("array: %d\n", (int)dataarray[index]);
			}


	printf("done.\n");
}
void Filter::get_source3D(vector<unsigned char> *dataarray, int channel)
{
	printf("Copying source image\n");
	dataarray->resize(width*height*depth);

	int subvol_xval, subvol_yval, subvol_zval;

	int index;
	int index2;
	for(int x=volobj->boundingboxMin.x; x<volobj->boundingboxMax.x; x++)
		for(int y=volobj->boundingboxMin.y; y<volobj->boundingboxMax.y; y++)
			for(int z=volobj->boundingboxMin.z; z<volobj->boundingboxMax.z; z++)
			{
				subvol_xval = x-volobj->boundingboxMin.x;
				subvol_yval = y-volobj->boundingboxMin.y;
				subvol_zval = z-volobj->boundingboxMin.z;

				index = ((((subvol_zval*height)+(subvol_yval))*width)+(subvol_xval));	
				index2 = volobj->get_index1D(x,y,z);

				dataarray->at(index) = volobj->texture3d[3*index2+channel];
				//printf("array: %d\n", (int)dataarray[index]);
			}


	printf("done.\n");
}
void Filter::copy_source3D(void)
{
	Vector bbsize = volobj->boundingboxMax - volobj->boundingboxMin;
	bbsize+=1.0;
	int size = bbsize.x*bbsize.y*bbsize.z*3;
	
	width = bbsize.x;
	height = bbsize.y;
	depth = bbsize.z;

	//printf("bb: %d , %d , %d\n", width, height, depth);

	if(source_array) delete[] source_array;
	source_array = new unsigned char [size];

	int subvol_xval, subvol_yval, subvol_zval;

	int index;
	int index2;
	for(int x=volobj->boundingboxMin.x; x<volobj->boundingboxMax.x; x++)
		for(int y=volobj->boundingboxMin.y; y<volobj->boundingboxMax.y; y++)
			for(int z=volobj->boundingboxMin.z; z<volobj->boundingboxMax.z; z++)
			{
				subvol_xval = x-volobj->boundingboxMin.x;
				subvol_yval = y-volobj->boundingboxMin.y;
				subvol_zval = z-volobj->boundingboxMin.z;
				index = ((((subvol_zval*height)+(subvol_yval))*width)+(subvol_xval));	

				index2 = volobj->get_index1D(x,y,z);

				source_array[3*index+0] = volobj->texture3d[3*index2+0];
				source_array[3*index+1] = volobj->texture3d[3*index2+1];
				source_array[3*index+2] = volobj->texture3d[3*index2+2];
			}
}

double Filter::invtan(double x, double y)
{
	double theta;

	// handle divide by zero
	if(x == 0)   
	{
	   if(y==0) theta = 0.0;
	   else if (y<0)   
	   {
			y = -y;
			theta = 90.0;
	   }
	   else theta = 90.0;
	}
	// handle invtan of angle in 2nd Quad
	else if(x<0 && y>0)   
	{
		x = -x;
		theta = 180 - ((atan((double)(y)/(double)(x))) * (180/PI));
	}
	// handle invtan of angle in 4th Quad
	else if(x>0 && y<0)   
	{
		y = -y;
		theta = 180 - ((atan((double)(y)/(double)(x))) * (180/PI));
	}
	// else angle is in 1st or 3rd Quad
	else theta = (atan((double)(y)/(double)(x))) * (180/PI);

	return theta;
}
/*
DESCRIPTION
   Performs Histogram equalization. It reassigns the brightness values of pixels based on the image
   histogram. Individual pixels retain their brightness order (that is, they remain brighter or darker
   than other pixels) but the values are shifted, so that an equal number of pixels have each possible
   brightness value. For each brightness level j in the original image (and its histogram, the new
   assigned value k is calculated as below.

   In many cases, this spreads out the values in regions where different regions meet, showing detail in
   areas with a high brightness gradient.


          j    Ni        Ni = number of pixels with brightness value i
   k = SUM    ---
          i=0  T         T = total number of pixels in the image
*/
void Filter::auto_levels(void)
{
	int histogram[256];
	int cumfreq[256];
	int lookup[256];
	float numb_voxels = volobj->texwidth*volobj->texheight*volobj->texdepth;
	float numb_nonblackvoxels = 0;
	
	//volume dimensions
	int size = volobj->texwidth*volobj->texheight*volobj->texdepth*3;	
	width  = volobj->texwidth;
	height = volobj->texheight;
	depth  = volobj->texdepth;

	//build histogram
	int i;
	for(i=0; i<256; i++)
	{
		histogram[i]=0;
	}
	for(i=0; i<numb_voxels; i++)
	{
		int voxel = (int)volobj->texture3d[3*i+0];

		if(voxel>0) numb_nonblackvoxels++;

		histogram[voxel]++;
	}
	histogram[0]=0;

	//build cummulative frequency table
	double sum=0;
	for(i=0; i<256; i++)
	{
		sum+=histogram[i];
		cumfreq[i] = sum;
	}
	
	//build equalisation table
	float alpha = 255.0f/numb_nonblackvoxels;
	for(i=0; i<256; i++)
	{
		lookup[i] = int( ((float)(cumfreq[i])*alpha)+0.5 );
	}

	//stretch histogram
	int voxel=0;
	for(i=0; i<numb_voxels; i++)
	{
		voxel = (int)volobj->texture3d[3*i+3];

		volobj->texture3d[3*i+0] = lookup[voxel];
		volobj->texture3d[3*i+1] = lookup[voxel];
		volobj->texture3d[3*i+2] = lookup[voxel];
	//	volobj->texture3d[3*i+3] = lookup[voxel];

	}
}

void Filter::apply_contrast_stretching(void)
{
	float numb_voxels = volobj->texwidth*volobj->texheight*volobj->texdepth;
	float min_out, max_out;
	float min_in,  max_in;

	min_out = 0.0;
	max_out = 255.0;

	//take bounds to be 5th and 95th percentile
	float lower_bound = numb_voxels*0.05;
	float upper_bound = numb_voxels*0.95;

	//build histogram
	int histogram[256];
	int i;
	for(i=0; i<256; i++)
	{
		histogram[i]=0;
	}
	for(i=0; i<numb_voxels; i++)
	{
		int voxel = (int)volobj->texture3d[3*i+0];
		histogram[voxel]++;
	}

	float counter=0;
	int help1=1;
	int help2=1;
	for(i=0; i<256; i++)
	{
		counter += histogram[i];

		if(counter>lower_bound && help1)
		{
			min_in = i;
			help1=0;
		}

		if(counter<upper_bound) max_in = i;

	}
	for(i=0; i<numb_voxels; i++)
	{
		float voxel = (float)volobj->texture3d[3*i+3];

		if(voxel)
		{
			float result = (voxel-min_in)*((max_out-min_out)/(max_in-min_in))+min_out;
		
			if(result>255)	  result = 255;
			if(result<0)	  result = 0;

			volobj->texture3d[3*i]=result;
			volobj->texture3d[3*i+1]=0;
			volobj->texture3d[3*i+2]=0;
		}
	}
}
vector< double > Filter::create_gaussianmask1D(int size, double sigma)
{
	Gaussian gauss;

	gauss.sigma = sigma;
	gauss.size = size;
	int kernel_radius = (size-1)/2;

	//evaluate our guassian kernel mask
	vector < double > mask = gauss.gaussianmask1D(gauss.sigma, gauss.size);
	return mask;
}
vector< vector< vector<double> > > Filter::create_gaussianmask3D(int size, double sigma)
{
	Gaussian gauss;

	//evaluate our sigma value for a given 
	//kernel mask radius
	//gauss.eval_sigma(kernel_radius);
	gauss.sigma = sigma;
	gauss.size = size;
	int kernel_radius = (size-1)/2;

	//allocate some memory for our kernel mask
	vector< vector< vector<double> > > mask;
	mask.resize((int)gauss.size);
	for(int n=0; n<gauss.size; n++)
		mask[n].resize(gauss.size);

	for(int nn=0; nn<gauss.size; nn++)
		for(int nnn=0; nnn<gauss.size; nnn++)
		mask[nn][nnn].resize((int)gauss.size);

	//evaluate our guassian kernel mask
	mask = gauss.gaussianmask3D(gauss.sigma, gauss.size);
	
	return mask;
}

void Filter::apply_gaussian3D(int size, double sigma, int qual)
{
	//printf("Gaussian Smoothing: (sigma %f size %d)\n\n", sigma,size); 
	Gaussian gauss;

	//allocate_result3D();
	copy_source3D();

	//evaluate our sigma value for a given 
	//kernel mask radius
	//gauss.eval_sigma(kernel_radius);
	gauss.sigma = sigma;
	gauss.size = size;
	int kernel_radius = (size-1)/2;

	//allocate some memory for our kernel mask
	vector< vector< vector<double> > > mask;
	mask.resize((int)gauss.size);
	for(int n=0; n<gauss.size; n++)
		mask[n].resize(gauss.size);

	for(int nn=0; nn<gauss.size; nn++)
		for(int nnn=0; nnn<gauss.size; nnn++)
		mask[nn][nnn].resize((int)gauss.size);

	//evaluate our guassian kernel mask
	mask = gauss.gaussianmask3D(gauss.sigma, gauss.size);

	int index=0;
	int x_iter=0;
	int y_iter=0;
	int z_iter=0;
	
	double filtered_valueR=0.0;
	double filtered_valueG=0.0;
	double filtered_valueB=0.0;
	double filtered_valueA=0.0;

	float datasize= (float)volobj->maxres*((float)qual/100.0);
	float itersteps = (float)(volobj->maxres)/datasize;
	
	//progressbar = new QProgressDialog("Computing Gaussian Smooth...", "Cancel", 0, depth-2.0*kernel_radius, this, 0);		
	//progressbar->setCaption("Please Wait");
	
	float kernX, kernY, kernZ;

	//int 
	for(float z=kernel_radius; z<depth-kernel_radius; z+=itersteps)
	{
		//progress(z, depth-kernel_radius);
		//progressbar->setValue(z);
		//if (progressbar->wasCancelled())
		//	break;

		for(float x=kernel_radius; x<width-kernel_radius; x+=itersteps)
		{
			for(float y=kernel_radius; y<height-kernel_radius; y+=itersteps)
			{				
					x_iter=0;
					y_iter=0;
					z_iter=0;
					filtered_valueR=0.0;
					filtered_valueG=0.0;
					filtered_valueB=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						for(int j=-kernel_radius; j<=kernel_radius; j++)
						{
							for(int k=-kernel_radius; k<=kernel_radius; k++)
							{
								kernX = x+k;
								kernY = y+j;
								kernZ = z+i;
								
								/*if(kernX<0) kernX=0;
								if(kernX>(width-1)) kernX=width-1;

								if(kernY<0) kernY=0;
								if(kernY>(height-1)) kernY=height-1;

								if(kernZ<0) kernZ=0;
								if(kernZ>(depth-1)) kernZ=depth-1;*/

								index = 3*(((((kernZ)*height)+(kernY))*width)+(kernX));
										
								filtered_valueR += (double)(source_array[index+0])*mask[x_iter][y_iter][z_iter];
								filtered_valueG += (double)(source_array[index+1])*mask[x_iter][y_iter][z_iter];
								filtered_valueB += (double)(source_array[index+2])*mask[x_iter][y_iter][z_iter];
								
								z_iter++;
							}
							
							y_iter++;
							z_iter=0;
						}

						x_iter++;
						y_iter=0;
					}
					
					index = 3*((((((int)z)*height)+((int)y))*width)+((int)x));
										
					volobj->texture3d[index+0] = (int)filtered_valueR+0.5;
					volobj->texture3d[index+1] = (int)filtered_valueG+0.5;
					volobj->texture3d[index+2] = (int)filtered_valueB+0.5;
			}
		}
	}

	//progressbar->setValue(depth-2.0*kernel_radius);
	//delete progressbar;
	//progressbar = NULL;

	delete[] source_array;
}

void Filter::apply_unsharpmask3D(int brightness, int contrast, float sigma, int gsize)
{
	//volume dimensions
	int size = volobj->texwidth*volobj->texheight*volobj->texdepth*3;	
	width  = volobj->texwidth;
	height = volobj->texheight;
	depth  = volobj->texdepth;

	//copy original
	copy_source3D();
	allocate_result3D();

	//=====================================
	//apply gaussian, low-pass image
	//=====================================
	printf("Applying Gaussian Smoothing: (sigma: %.2f, size: %d)\n", sigma,gsize); 
	Gaussian gauss;

	//evaluate our sigma value for a given 
	//kernel mask radius
	//gauss.eval_sigma(kernel_radius);
	gauss.sigma = sigma;
	gauss.size = gsize;
	int kernel_radius = (gsize-1)/2;

	//allocate some memory for our kernel mask
	vector< vector< vector<double> > > mask;
	mask.resize((int)gauss.size);
	for(int n=0; n<gauss.size; n++)
		mask[n].resize(gauss.size);

	for(int nn=0; nn<gauss.size; nn++)
		for(int nnn=0; nnn<gauss.size; nnn++)
		mask[nn][nnn].resize((int)gauss.size);

	//evaluate our guassian kernel mask
	mask = gauss.gaussianmask3D(gauss.sigma, gauss.size);

	int index=0;
	int x_iter=0;
	int y_iter=0;
	int z_iter=0;
	
	double filtered_valueR=0.0;
	double filtered_valueG=0.0;
	double filtered_valueB=0.0;
//	double filtered_valueA=0.0;
	int i;
	for(i=0; i<size; i++)
	{
		result_array[i] = volobj->texture3d[i];
	}

	//int 
	int dsize = depth-kernel_radius;
	for(int z=kernel_radius; z<depth-kernel_radius; z++)
	{
		//progress(z, dsize);

		for(int x=kernel_radius; x<width-kernel_radius; x++)
		{
			for(int y=kernel_radius; y<height-kernel_radius; y++)
			{				
					x_iter=0;
					y_iter=0;
					z_iter=0;
					filtered_valueR=0.0;
					filtered_valueG=0.0;
					filtered_valueB=0.0;
					//filtered_valueA=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						for(int j=-kernel_radius; j<=kernel_radius; j++)
						{
							for(int k=-kernel_radius; k<=kernel_radius; k++)
							{
								index = 3*volobj->get_index1D(x+k, y+j, z+i);
								filtered_valueR += (double)(source_array[index+0])*mask[x_iter][y_iter][z_iter];
								filtered_valueG += (double)(source_array[index+1])*mask[x_iter][y_iter][z_iter];
								filtered_valueB += (double)(source_array[index+2])*mask[x_iter][y_iter][z_iter];
							//	filtered_valueA += (double)(source_array[index+3])*mask[x_iter][y_iter][z_iter];
								z_iter++;
							}
							
							y_iter++;
							z_iter=0;
						}

						x_iter++;
						y_iter=0;
					}

					index = volobj->get_index1D(x, y, z);

					result_array[3*index+0] = (int)filtered_valueR+0.5;
					result_array[3*index+1] = (int)filtered_valueG+0.5;
					result_array[3*index+2] = (int)filtered_valueB+0.5;
					//result_array[3*index+3] = (int)filtered_valueA+0.5;
			}
		}
	}

	{
	for(int nn=0; nn<gauss.size; nn++)
		for(int nnn=0; nnn<gauss.size; nnn++)
		{
			mask[nn][nnn].clear();
		}
	}
	{
		for(int nnn=0; nnn<gauss.size; nnn++)
		{
			mask[nnn].clear();
		}

		mask.clear();
	}

	//=====================================
	//substract low-pass from original
	//to get high-pass image
	//=====================================
	for(i=0; i<size; i++)
	{
		int voxel = volobj->texture3d[i] - result_array[i];

		if(voxel>255) voxel = 255;
		if(voxel<0)	  voxel = 0;

		//get voxel value
		result_array[i] = voxel;
	}

	//=====================================
	//add high-pass to original
	//to get edge enhanced
	//=====================================
	for(i=0; i<size; i++)
	{
		int voxel = volobj->texture3d[i] + result_array[i];

		if(voxel>255) voxel = 255;
		if(voxel<0)	  voxel = 0;

		//get voxel value
		volobj->texture3d[i] = voxel;
	}
}

void Filter::apply_mean(int scale)
{
	copy_source3D();

	vector<char> kernel_values;
			
	int current_index = 0;
	int filtered_value = 0;
	int x_iter=0;
	int y_iter=0;
	int z_iter=0;
	int kernel_radius = scale;
	int sizehelper=0;
	
	for(int z=kernel_radius; z<depth-kernel_radius; z++)
	{
//		progress(z, depth-kernel_radius);

		for(int x=kernel_radius; x<width-kernel_radius; x++)
		{
			for(int y=kernel_radius; y<height-kernel_radius; y++)
			{				
					x_iter=0;
					y_iter=0;
					z_iter=0;
			
					sizehelper = 0;
					
					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						for(int j=-kernel_radius; j<=kernel_radius; j++)
						{
							for(int k=-kernel_radius; k<=kernel_radius; k++)
							{
								int index = 3*(((((z+i)*height)+(y+j))*width)+(x+k));
								
								//kernel_values.push_back(source_array[index+0]);							
								filtered_value += source_array[index+0];		
								sizehelper++;
								
								z_iter++;
							}
							
							y_iter++;
							z_iter=0;
						}

						x_iter++;
						y_iter=0;
					}
									
					filtered_value /= (float) sizehelper;
					
					current_index = 1*((((z*height)+y)*width)+x);

					volobj->texture3d[3*current_index+0]=filtered_value;
					volobj->texture3d[3*current_index+1]=0;
					volobj->texture3d[3*current_index+2]=0;
			}
		}
	}
	
	if(source_array) delete[] source_array;
	source_array=NULL;
}
void Filter::apply_median(int scale)
{
	copy_source3D();

	vector<int> kernel_values;
			
	int current_index = 0;
	int filtered_value = 0;
	int x_iter=0;
	int y_iter=0;
	int z_iter=0;
	int kernel_radius = scale;
	
	float interp = (float)depth/(float)width;
	int i = 0;
	int value;
	for(int z=kernel_radius; z<depth-kernel_radius; z++)
	{
		progress(z, depth-kernel_radius);

		for(int x=kernel_radius; x<width-kernel_radius; x++)
		{
			for(int y=kernel_radius; y<height-kernel_radius; y++)
			{				
					x_iter=0;
					y_iter=0;
					z_iter=0;
			
					//for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						for(int j=-kernel_radius; j<=kernel_radius; j++)
						{
							for(int k=-kernel_radius; k<=kernel_radius; k++)
							{
								int zval = z+i+1;
								if(zval>depth-1) zval = depth-1;
								
								int index = 3*(((((z+i)*height)+(y+j))*width)+(x+k));
								int index2 = 3*(((((zval)*height)+(y+j))*width)+(x+k));
								
								//value = (float)(1-interp)*(float)source_array[index+0] + (float)(interp)*(float)source_array[index2+0];
								value = source_array[index+0];
								
								kernel_values.push_back(value);							
								z_iter++;
							}
							
							y_iter++;
							z_iter=0;
						}

						x_iter++;
						y_iter=0;
					}
					
					sort(kernel_values.begin(), kernel_values.end());
					
					filtered_value = kernel_values[kernel_values.size()/2.0];				
					current_index = 3*((((z*height)+y)*width)+x);

					volobj->texture3d[current_index+0]=filtered_value;
					
					kernel_values.clear();
			}
		}
	}
	
	if(source_array) delete[] source_array;
	source_array=NULL;

}

void Filter::apply_harriscorner(void)
{
    Sobel sobel;
	Gaussian gauss;
  
	copy_source3D();

	//clear target image
	for(int i=0; i<width*depth*height*3; i++)
		volobj->texture3d[i+0]=0;
		
	//evaluate our sigma value for a given 
	//kernel mask radius
	//gauss.eval_sigma(kernel_radius);
	gauss.sigma = 0.5;
	int kernel_radius = 1;

	gauss.size = 2*kernel_radius+1;
    int window_size = 1;
    int grad_kernel_radius = 1;
    int imagesize = window_size+window_size+grad_kernel_radius;

	//allocate some memory for our kernel mask
	vector< vector< vector<double> > > mask;
	mask.resize((int)gauss.size);
	for(int n=0; n<gauss.size; n++)
		mask[n].resize(gauss.size);

	for(int nn=0; nn<gauss.size; nn++)
		for(int nnn=0; nnn<gauss.size; nnn++)
		mask[nn][nnn].resize((int)gauss.size);

	//evaluate our guassian kernel mask
	mask = gauss.gaussianmask3D(gauss.sigma, gauss.size);

    double results[4];
    int current_index=0;


    int point_indexX=0;
    int point_indexY=0;
    int point_indexZ=0;


    int window_index = 0;
    int window_indexX=0;
    int window_indexY=0;
    int window_indexZ=0;
    int window_x_iter=0;
    int window_y_iter=0;
    int window_z_iter=0;

    int grad_index=0;
    int grad_x_iter=0;
    int grad_y_iter=0;
    int grad_z_iter=0;

    double filtered_valueRx=0.0;
    double filtered_valueRy=0.0;
    double filtered_valueRz=0.0;

    double original_value=0.0;

	double harris_value = 0.0;
	
    PCA pca;
    float pca_eigenval0;
    float pca_eigenval1;
    float pca_eigenval2;

 //	progressbar = new Q3ProgressDialog("Computing Harris Corner Detector...", "Cancel", depth-imagesize, 0, "progress", TRUE);		
//	progressbar->setCaption("Please Wait");

	float maxvalue = 0.0;
	
	//traverse each point in our image.
    for(int z=imagesize; z<depth-imagesize; z++)
    {
           //progress(z, depth-imagesize);
	//		progressbar->setProgress(z);
	//		if (progressbar->wasCancelled())
	//			break;
            for(int x=imagesize; x<width-imagesize; x++)
            {
                    for(int y=imagesize; y<height-imagesize; y++)
                    {
                        pca.reset();
					
                        //for the current point look around our window
                        for(int iii=-window_size; iii<=window_size; iii++)
                        {
                                for(int jjj=-window_size; jjj<=window_size; jjj++)
                                {
                                        for(int kkk=-window_size; kkk<=window_size; kkk++)
                                        {
                                                point_indexX = x+kkk;
                                                point_indexY = y+jjj;
                                                point_indexZ = z+iii;
												
                                                //point_indexZ=z;

                                                window_x_iter=0;
                                                window_y_iter=0;
                                                window_z_iter=0;
						results[0]=results[1]=results[2]=results[3]=0;

                                                for(int i=-window_size; i<=window_size; i++)
                                                {
                                                        for(int j=-window_size; j<=window_size; j++)
                                                        {
                                                                for(int k=-window_size; k<=window_size; k++)
                                                                {
                                                                        window_indexX = point_indexX+k;
                                                                        window_indexY = point_indexY+j;
                                                                        window_indexZ = point_indexZ+i;

                                                                                                                        //window_indexZ=point_indexZ;

                                                                        window_index = 1*(((((window_indexZ)*height)+(window_indexY))*width)+(window_indexX));

                                                                        //compute the gradient(sobel) at window point position.
                                                                        grad_x_iter=0;
                                                                        grad_y_iter=0;
                                                                        grad_z_iter=0;
                                                                        filtered_valueRx = filtered_valueRy = filtered_valueRz = 0;

                                                                        for(int ii=-grad_kernel_radius; ii<=grad_kernel_radius; ii++)
                                                                        {
                                                                                for(int jj=-grad_kernel_radius; jj<=grad_kernel_radius; jj++)
                                                                                {
                                                                                        for(int kk=-grad_kernel_radius; kk<=grad_kernel_radius; kk++)
                                                                                        {
                                                                                                grad_index = 1*(((((window_indexZ+ii)*height)+(window_indexY+jj))*width)+(window_indexX+kk));

                                                                                                                                                                        filtered_valueRx += (double)(source_array[3*grad_index+0])*sobel.sobelX3d[grad_x_iter][grad_y_iter][grad_z_iter];
                                                                                                filtered_valueRy += (double)(source_array[3*grad_index+0])*sobel.sobelY3d[grad_x_iter][grad_y_iter][grad_z_iter];
                                                                                                filtered_valueRz += (double)(source_array[3*grad_index+0])*sobel.sobelZ3d[grad_x_iter][grad_y_iter][grad_z_iter];

                                                                                                grad_z_iter++;
                                                                                        }

                                                                                        grad_y_iter++;
                                                                                        grad_z_iter=0;
                                                                                }
                                                                                grad_x_iter++;
                                                                                grad_y_iter=0;
                                                                        }

                                                                        filtered_valueRx /= 52.0;
                                                                        filtered_valueRy /= 52.0;
                                                                        filtered_valueRz /= 52.0;

                                                                                                                        results[0] += (filtered_valueRx*filtered_valueRx)*mask[window_x_iter][window_y_iter][window_z_iter];
                                                                                                                        results[1] += (filtered_valueRy*filtered_valueRy)*mask[window_x_iter][window_y_iter][window_z_iter];
                                                                                                                        results[2] += (filtered_valueRx*filtered_valueRy)*mask[window_x_iter][window_y_iter][window_z_iter];


                                                                                                                        /*results[0] += (filtered_valueRx*filtered_valueRx*filtered_valueRx)*mask[window_x_iter][window_y_iter][window_z_iter];
                                                                                                                        results[1] += (filtered_valueRy*filtered_valueRy*filtered_valueRy)*mask[window_x_iter][window_y_iter][window_z_iter];
                                                                                                                        results[2] += (filtered_valueRz*filtered_valueRz*filtered_valueRz)*mask[window_x_iter][window_y_iter][window_z_iter];
                                                                                                                        results[3] += (filtered_valueRx*filtered_valueRy*filtered_valueRz)*mask[window_x_iter][window_y_iter][window_z_iter];*/



                                                                        //original_value = source_array[3*window_index+0];
                                                                        //pca.data.push_back(Vector(filtered_valueRx,filtered_valueRy,filtered_valueRz));

                                                                        /*result[0][0] += (filtered_valueRx*filtered_valueRx);
                                                                        result[1][0] += (filtered_valueRx*filtered_valueRy);
                                                                        result[1][0] += (filtered_valueRx*filtered_valueRy);
                                                                        result[1][1] += (filtered_valueRy*filtered_valueRy);*/

                                                                        window_z_iter++;
                                                                }

                                                                window_y_iter++;
                                                                window_z_iter=0;
                                                        }
                                                        window_x_iter++;
                                                        window_y_iter=0;
                                                }

						pca.data.push_back(Vector(results[0]/(float)(window_size*2+1), results[2]/(float)(window_size*2+1), 0.0));
						pca.data.push_back(Vector(results[2]/(float)(window_size*2+1), results[1]/(float)(window_size*2+1), 0.0));

						
						/*pca.data.push_back(Vector(results[0], results[3], results[3]));
						pca.data.push_back(Vector(results[3], results[1], results[3]));
						pca.data.push_back(Vector(results[3], results[3], results[2]));*/
						}
					}
				}
						
                        //eval eigenvectors.
                        pca.eval_PCA();
                        pca.eval_scaled_eigensvectors(1,1,1);

                        pca_eigenval0 = pca.eigenvectors[0].length();
                        pca_eigenval1 = pca.eigenvectors[1].length();
                        pca_eigenval2 = pca.eigenvectors[2].length();

                        pca_eigenval0 = pow((float)pca_eigenval0, 1.0f/3.0f);
                        pca_eigenval1 = pow((float)pca_eigenval1, 1.0f/3.0f);
                        pca_eigenval2 = pow((float)pca_eigenval2, 1.0f/3.0f);

                        if(maxvalue<pca_eigenval0) maxvalue = pca_eigenval0;
						
                        //harris_value = pca_eigenval0*pca_eigenval1*pca_eigenval2 - 0.06*((pca_eigenval0+pca_eigenval1*pca_eigenval2)*(pca_eigenval0+pca_eigenval1*pca_eigenval2));
                        //harris_value = pca_eigenval0*pca_eigenval1 - 0.04*((pca_eigenval0+pca_eigenval1)*(pca_eigenval0+pca_eigenval1));
                        //harris_value = pow(harris_value, 1.0/3.0);

                        //printf("harris_value: %f\n", harris_value);

                        //if(pca_eigenval0>0 && pca_eigenval1>0) printf("egienvalues: %f, %f, %f \n", pca_eigenval0, pca_eigenval1, pca_eigenval2);
                        current_index = 3*((((z*height)+y)*width)+x);

                        //if(pca_eigenval0>10000 && pca_eigenval1>10000 && pca_eigenval2>10000) //
                        //if(harris_value>150)
                        // if(pca_eigenval0>10 && pca_eigenval1>10) //
                        {
                            //volobj->texture3d[current_index+0]=0;
                            volobj->texture3d[current_index+1]=pca_eigenval0+pca_eigenval1;
							
                            //if(volobj->texture3d[current_index+1]>255) volobj->texture3d[current_index+1] = 255;
                            //if(volobj->texture3d[current_index+1]<0) volobj->texture3d[current_index+1] = 0;
							
                            //volobj->texture3d[current_index+2]=0;
                        }
                        /*else
                        {
                            //volobj->texture3d[current_index+0]=0;
                            volobj->texture3d[current_index+1]=0;
                            volobj->texture3d[current_index+2]=0;
                        }*/
                    }
                }
        }
	//progressbar->setProgress(depth-imagesize);

        printf("max: %f\n", pca_eigenval0);

/*	
	//traverse each point in our image.
    for(int z=window_size; z<depth-window_size; z++)
    {
           //progress(z, depth-imagesize);
			progressbar->setProgress(z);
			if (progressbar->wasCancelled())
				break;
				
            for(int x=window_size; x<width-window_size; x++)
            {
                    for(int y=window_size; y<height-window_size; y++)
                    {
                        //for the current point look around our window
						current_index = 1*(((((window_indexZ)*height)+(window_indexY))*width)+(window_indexX));

                        for(int iii=-window_size; iii<=window_size; iii++)
                        {
                                for(int jjj=-window_size; jjj<=window_size; jjj++)
                                {
                                        for(int kkk=-window_size; kkk<=window_size; kkk++)
                                        {
                                                window_indexX = x+kkk;
                                                window_indexY = y+jjj;
                                                window_indexZ = z+iii;

                                                window_index = 1*(((((window_indexZ)*height)+(window_indexY))*width)+(window_indexX));
										}
								}
						}
					}
			}
	}
	*/											
	//delete progressbar;
	//progressbar = NULL;

	delete[] source_array;
}
void Filter::apply_contrast_stretching_filter(vector<bool> channels)
{
	printf("apply_contrast_stretching_filter\n");

	int offset=0;
	if(channels[1]==true) offset=1;
	else if(channels[2]==true) offset=2;

	float numb_voxels = volobj->texwidth*volobj->texheight*volobj->texdepth;
	float min_out, max_out;
	float min_in,  max_in;

	min_out = 0.0;
	max_out = 255.0;

	//take bounds to be 5th and 95th percentile
	float lower_bound = numb_voxels*0.05;
	float upper_bound = numb_voxels*0.95;

	//build histogram
	int histogram[256];
	int i;
	for(i=0; i<256; i++)
	{
		histogram[i]=0;
	}
	for(i=0; i<numb_voxels; i++)
	{
		int voxel = (int)volobj->texture3d[3*i+offset];
		histogram[voxel]++;
	}

	float counter=0;
	int help1=1;
	int help2=1;
	for(i=0; i<256; i++)
	{
		counter += histogram[i];

		if(counter>lower_bound && help1)
		{
			min_in = i;
			help1=0;
		}

		if(counter<upper_bound) max_in = i;

	}
	
	/*for(i=0; i<256; i++)
	{
		volobj->histogram[3*i+offset]=0;
	}*/

	for(i=0; i<numb_voxels; i++)
	{
		float voxel = (float)volobj->texture3d[3*i+offset];

		if(voxel)
		{
			float result = (voxel-min_in)*((max_out-min_out)/(max_in-min_in))+min_out;
		
			if(result>255)	  result = 255;
			if(result<0)	  result = 0;

			volobj->texture3d[3*i+offset]=result;

			//volobj->histogram[3*(int)result+offset]++;
		}
	}
}
void Filter::apply_auto_levels_filter(vector<bool> channels)
{
	printf("apply_auto_levels_filter\n");

	int offset=0;
	if(channels[1]==true) offset=1;
	else if(channels[2]==true) offset=2;

	int histogram[256];
	int cumfreq[256];
	int lookup[256];
	float numb_voxels = volobj->texwidth*volobj->texheight*volobj->texdepth;
	float numb_nonblackvoxels = 0;
	
	//volume dimensions
	int size = volobj->texwidth*volobj->texheight*volobj->texdepth;	
	width  = volobj->texwidth;
	height = volobj->texheight;
	depth  = volobj->texdepth;

	//build histogram
	int i;
	for(i=0; i<256; i++)
	{
		histogram[i]=0;
	}
	for(i=0; i<numb_voxels; i++)
	{
		int voxel = (int)volobj->texture3d[3*i+offset];

		if(voxel>0) numb_nonblackvoxels++;

		histogram[voxel]++;
	}
	histogram[0]=0;

	//build cummulative frequency table
	double sum=0;
	for(i=0; i<256; i++)
	{
		sum+=histogram[i];
		cumfreq[i] = sum;
	}
	
	//build equalisation table
	float alpha = 255.0f/numb_nonblackvoxels;
	for(i=0; i<256; i++)
	{
		lookup[i] = int( ((float)(cumfreq[i])*alpha)+0.5 );
	//	volobj->histogram[i]=0;
	}

	//stretch histogram
	int voxel=0;
	for(i=0; i<numb_voxels; i++)
	{
		voxel = (int)volobj->texture3d[3*i+offset];

		volobj->texture3d[3*i+offset] = lookup[voxel];
		//volobj->histogram[3*voxel+offset]++;
	}
}

void Filter::apply_sobel3D_filter(vector<bool> channels)
{
	Sobel sobel;

	copy_source3D();
	allocate_sobel3D();

	int kernel_radius = 1;
			
	int current_index = 0;
	int x_iter=0;
	int y_iter=0;
	int z_iter=0;
	

	double filtered_valuex=0.0;
	double filtered_valuey=0.0;
	double filtered_valuez=0.0;
	double filtered_value=0.0;

	int index=0;
	int xval=0;
	int yval=0;
	int zval=0;
	
	int offset=0;
	if(channels[1]==true) offset=1;
	else if(channels[2]==true) offset=2;

	if(volobj->is_greyscale==true) offset=0;

	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Applying Filter...", "Cancel", depth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
	}

	int subvol_xval, subvol_yval, subvol_zval;
	int helper2=0;
	for(float z=volobj->boundingboxMin.z; z<volobj->boundingboxMax.z; z++)
	{
		if(showGUI)
		{
			//progress(z, depth-kernel_radius);
			progressbar->setProgress(helper2);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(helper2, depth);

		helper2++;

		for(float x=volobj->boundingboxMin.x; x<volobj->boundingboxMax.x; x++)
		{
			for(float y=volobj->boundingboxMin.y; y<volobj->boundingboxMax.y; y++)
			{	
					x_iter=0;
					y_iter=0;
					z_iter=0;

					filtered_valuex=0.0;
					filtered_valuey=0.0;
					filtered_valuez=0.0;
				
					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						for(int j=-kernel_radius; j<=kernel_radius; j++)
						{
							for(int k=-kernel_radius; k<=kernel_radius; k++)
							{
								zval = z+i;
								yval = y+j;
								xval = x+k;

								if(zval>volobj->boundingboxMax.z-1) zval = volobj->boundingboxMax.z-1;
								if(yval>volobj->boundingboxMax.y-1) yval = volobj->boundingboxMax.y-1;
								if(xval>volobj->boundingboxMax.x-1) xval = volobj->boundingboxMax.x-1;
								if(zval<volobj->boundingboxMin.z) zval = volobj->boundingboxMin.z;
								if(yval<volobj->boundingboxMin.y) yval = volobj->boundingboxMin.y;
								if(xval<volobj->boundingboxMin.x) xval = volobj->boundingboxMin.x;
								
								//index = 3*(((((zval-volobj->boundingboxMin.z)*height)+(yval-volobj->boundingboxMin.y))*width)+(xval-volobj->boundingboxMin.x));
	
								subvol_xval = xval-volobj->boundingboxMin.x;
								subvol_yval = yval-volobj->boundingboxMin.y;
								subvol_zval = zval-volobj->boundingboxMin.z;
								index = ((((subvol_zval*height)+(subvol_yval))*width)+(subvol_xval));	

								filtered_valuex += (double)(source_array[3*index+offset])*sobel.sobelX3d[x_iter][y_iter][z_iter];
								filtered_valuey += (double)(source_array[3*index+offset])*sobel.sobelY3d[x_iter][y_iter][z_iter];
								filtered_valuez += (double)(source_array[3*index+offset])*sobel.sobelZ3d[x_iter][y_iter][z_iter];
	
								z_iter++;
							}
							
							y_iter++;
							z_iter=0;
						}

						x_iter++;
						y_iter=0;
					}

					filtered_valuex /= 52.0;
					filtered_valuey /= 52.0;
					filtered_valuez /= 52.0;

					//current_index = ((((z*height)+y)*width)+x);
					//current_index = (((((zval-volobj->boundingboxMin.z)*height)+(yval-volobj->boundingboxMin.y))*width)+(xval-volobj->boundingboxMin.x));
	
					subvol_xval = xval-volobj->boundingboxMin.x;
					subvol_yval = yval-volobj->boundingboxMin.y;
					subvol_zval = zval-volobj->boundingboxMin.z;
					current_index = ((((subvol_zval*height)+(subvol_yval))*width)+(subvol_xval));	
					
					filtered_value =sqrt((filtered_valuex*filtered_valuex)+(filtered_valuey*filtered_valuey)+(filtered_valuez*filtered_valuez));
					sobel_result_array[current_index+offset] = filtered_value;
					
					//==========================================
					// SAVE IT INTO OUR RESULT ARRAY
					//==========================================
					filtered_value = sobel_result_array[current_index+offset];
					if(filtered_value>255) filtered_value = 255;
					if(filtered_value<0) filtered_value = 0;

					//3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					current_index = volobj->get_index1D(x,y,z); 
					volobj->texture3d[3*current_index+offset]=filtered_value;

					if(volobj->is_greyscale==true)
					{
						volobj->texture3d[3*current_index+2] =
						volobj->texture3d[3*current_index+1] =
						volobj->texture3d[3*current_index+0];
					}
			}
		}
	}
	
	delete source_array;
	delete sobel_result_array;
	source_array = NULL;
	sobel_result_array = NULL;

	if(showGUI)
	{
		progressbar->setProgress(depth);
		delete progressbar;
		progressbar = NULL;
	}
	else progress(depth, depth);
}

void Filter::apply_sobel3D(int mode)
{
	Sobel sobel;

	copy_source3D();
	allocate_sobel3D();

	int kernel_radius = 1;
			
	int current_index = 0;
	int x_iter=0;
	int y_iter=0;
	int z_iter=0;
	
	double filtered_valueRx=0.0;
	double filtered_valueGx=0.0;
	double filtered_valueBx=0.0;

	double filtered_valueRy=0.0;
	double filtered_valueGy=0.0;
	double filtered_valueBy=0.0;
	
	double filtered_valueRz=0.0;
	double filtered_valueGz=0.0;
	double filtered_valueBz=0.0;
	
	double filtered_value=0.0;

	int highThreshold = 10;

	for(int z=kernel_radius; z<depth-kernel_radius; z++)
	{
//		progress(z, depth-kernel_radius);

		for(int x=kernel_radius; x<width-kernel_radius; x++)
		{
			for(int y=kernel_radius; y<height-kernel_radius; y++)
			{				
					x_iter=0;
					y_iter=0;
					z_iter=0;

					filtered_valueRx=0.0;
					filtered_valueRy=0.0;
					filtered_valueRz=0.0;
					
					filtered_valueGx=0.0;
					filtered_valueGy=0.0;
					filtered_valueGz=0.0;

					filtered_valueBx=0.0;
					filtered_valueBy=0.0;
					filtered_valueBz=0.0;
				
					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						for(int j=-kernel_radius; j<=kernel_radius; j++)
						{
							for(int k=-kernel_radius; k<=kernel_radius; k++)
							{
								int index = 1*(((((z+i)*height)+(y+j))*width)+(x+k));
											
								filtered_valueRx += (double)(source_array[3*index+0])*sobel.sobelX3d[x_iter][y_iter][z_iter];
								filtered_valueRy += (double)(source_array[3*index+0])*sobel.sobelY3d[x_iter][y_iter][z_iter];
								filtered_valueRz += (double)(source_array[3*index+0])*sobel.sobelZ3d[x_iter][y_iter][z_iter];

								filtered_valueGx += (double)(source_array[index+1])*sobel.sobelX3d[x_iter][y_iter][z_iter];
								filtered_valueGy += (double)(source_array[index+1])*sobel.sobelY3d[x_iter][y_iter][z_iter];
								filtered_valueGz += (double)(source_array[index+1])*sobel.sobelZ3d[x_iter][y_iter][z_iter];
								
								filtered_valueBx += (double)(source_array[index+2])*sobel.sobelX3d[x_iter][y_iter][z_iter];
								filtered_valueBy += (double)(source_array[index+2])*sobel.sobelY3d[x_iter][y_iter][z_iter];
								filtered_valueBz += (double)(source_array[index+2])*sobel.sobelZ3d[x_iter][y_iter][z_iter];
							
								z_iter++;
							}
							
							y_iter++;
							z_iter=0;
						}

						x_iter++;
						y_iter=0;
					}

					filtered_valueRx /= 52.0;
					filtered_valueRy /= 52.0;
					filtered_valueRz /= 52.0;

					filtered_valueGx /= 52.0;
					filtered_valueGy /= 52.0;
					filtered_valueGz /= 52.0;

					filtered_valueBx /= 52.0;
					filtered_valueBy /= 52.0;
					filtered_valueBz /= 52.0;
					
					current_index = 1*((((z*height)+y)*width)+x);
					
					if(mode==SOBEL_HORIZ)
					{
						sobel_result_array[current_index+0] = filtered_valueRx;
						sobel_result_array[current_index+0] = filtered_valueGx;
						sobel_result_array[current_index+0] = filtered_valueBx;

					}
					else if(mode==SOBEL_VERTI)
					{
						sobel_result_array[current_index+0] = filtered_valueRy;
						sobel_result_array[current_index+0] = filtered_valueGy;
						sobel_result_array[current_index+0] = filtered_valueBy;

					}
					else if(mode==SOBEL_DEPTH)
					{
						sobel_result_array[current_index+0] = filtered_valueRz;
						sobel_result_array[current_index+0] = filtered_valueGz;
						sobel_result_array[current_index+0] = filtered_valueBz;

					}
					else if(mode==SOBEL_HVD)
					{
						//RED
						filtered_value =sqrt((filtered_valueRx*filtered_valueRx)+(filtered_valueRy*filtered_valueRy)+(filtered_valueRz*filtered_valueRz));
						sobel_result_array[current_index+0] = filtered_value;
						
						//GREEN
						filtered_value =sqrt((filtered_valueGx*filtered_valueGx)+(filtered_valueGy*filtered_valueGy)+(filtered_valueGz*filtered_valueGz));
						sobel_result_array[current_index+1] = filtered_value;
						
						//BLUE
						filtered_value =sqrt((filtered_valueBx*filtered_valueBx)+(filtered_valueBy*filtered_valueBy)+(filtered_valueBz*filtered_valueBz));
						sobel_result_array[current_index+2] = filtered_value;
					
						//ALPHA
						//sobel_result_array[current_index+3] = source_array[current_index+3];	
					}
					
					//==========================================
					// SAVE IT INTO OUR RESULT ARRAY
					//==========================================

					//RED
					filtered_value = sobel_result_array[current_index+0];
					if(filtered_value>255) filtered_value = 255;
					if(filtered_value<0) filtered_value = 0;
					volobj->texture3d[3*current_index+0]=filtered_value;
					
					//GREEN
					filtered_value = sobel_result_array[current_index+0];
					if(filtered_value>255) filtered_value = 255;
					if(filtered_value<0) filtered_value = 0;
					volobj->texture3d[3*current_index+1]=filtered_value;
					
					//BLUE
					filtered_value = sobel_result_array[current_index+0];
					if(filtered_value>255) filtered_value = 255;
					if(filtered_value<0) filtered_value = 0;
					volobj->texture3d[3*current_index+2]=filtered_value;
			}
		}
	}
}



void Filter::gaussian_filter_decomp_init(int kr, double sigma)
{
	//-------------------------------------------------------------
	//Build our 3D Gaussian kernel
	//-------------------------------------------------------------
	
	gauss.sigma = sigma;
	gauss.size = kr;
	int kernel_radius = (kr-1)/2;

	mask = gauss.gaussianmask1D(gauss.sigma, gauss.size);
	/*vector < vector < double > > valsX;
	vector < vector < double > > valsY;
	vector < vector < double > > valsZ;

	valsX.resize(kr);
	valsY.resize(kr);
	valsZ.resize(kr);

	for(int i=0; i<valsX.size(); i++)
	{
		valsX[i].resize(kr);
		valsY[i].resize(kr);
		valsZ[i].resize(kr);

		for(int j=0; j<valsX[i].size(); j++)
		{
			valsX[i][j] = 0.0;
			valsY[i][j] = 0.0;
			valsZ[i][j] = 0.0;
		}
	}*/

	sum=0.0;
	for(int i=0; i<mask.size(); i++)
		sum += mask[i];
}
void Filter::gaussian_filter_decomp_dealloc(void)
{
	gauss.clear_all();
}
void Filter::apply_gaussian_filter_decomp(int kr, double sigma, int dimensionality, vector<bool> channels)
{
	//printf("gaussian: %d, %f, %d\n", kr, sigma, dimensionality);
	//printf("chans: %i, %i, %i\n", (int)channels[0], (int)channels[1], (int)channels[2]);

	int kernel_radius = (kr-1)/2;

	//printf("kr: %d\n", kr);

	Vector bbsize = volobj->boundingboxMax - volobj->boundingboxMin;
	bbsize += 1.0;

//	bbsize = Vector(volobj->texwidth, volobj->texheight, volobj->texdepth);

	int size = 3*bbsize.x*bbsize.y*bbsize.z;
	int index, index2;
	int helper=0;

	int offset=0;
	if(channels[1]==true) offset=1;
	else if(channels[2]==true) offset=2;

	if(volobj->is_greyscale==true) offset=0;

	printf("offset: %d\n", offset);

	width = bbsize.x;
	height = bbsize.y;
	depth = bbsize.z;

	Vector bboutermin = volobj->boundingboxMin;
	Vector bboutermax = volobj->boundingboxMax;

	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Applying Filter...", "Cancel", 4*depth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
		progressbar->setMinimumDuration(100);
	    progressbar->setWindowModality(Qt::WindowModal);
	}

	source_array = 0;
	source_array = new unsigned char [size];
	if(source_array==0) return;
	for(int i=0; i<size; i++) source_array[i]=0;

	unsigned char* source_array2;
	source_array2 = 0;
	source_array2 = new unsigned char [size];
	if(source_array2==0) return;
	for(int i=0; i<size; i++) source_array2[i]=0;

	int xval, yval, zval;
		
	//-------------------------------------------------------------
	//Make a copy data of the original data.
	//-------------------------------------------------------------
	for(int z=bboutermin.z; z<bboutermax.z; z++)
	{
		if(showGUI)
		{
			progressbar->setProgress(helper);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(helper, 4*depth);

		helper++;

		zval = z-bboutermin.z;
		for(int y=bboutermin.y; y<bboutermax.y; y++)
		{
			yval = y-bboutermin.y;
			for(int x=bboutermin.x; x<bboutermax.x; x++)
			{
				xval = x-bboutermin.x;
				index = ((((zval*height)+(yval))*width)+(xval));	
				index2 = volobj->get_index1D(x,y,z);

				source_array[3*index+offset] = volobj->texture3d[3*index2+offset];
			}
		}
	}
	
	int mask_iter=0;
	double filtered_valueR=0.0;
	double filtered_valueG=0.0;
	double filtered_valueB=0.0;
	double filtered_valueA=0.0;
	float kernX, kernY, kernZ;

	//-------------------------------------------------------------
	//Apply Gaussian in the X dir
	//-------------------------------------------------------------
	for(float z=bboutermin.z; z<bboutermax.z; z++)
	{
		if(showGUI)
		{
			progressbar->setProgress(helper);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(helper, 4*depth);

		helper++;

		zval = z-bboutermin.z;
		for(float x=bboutermin.x; x<bboutermax.x; x++)
		{
			for(float y=bboutermin.y; y<bboutermax.y; y++)
			{				
					yval = y-bboutermin.y;

					mask_iter=0;
					filtered_valueR=0.0;
					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x+i;
						if(kernX>bboutermax.x-1) kernX = bboutermax.x-1;
						if(kernX<bboutermin.x) kernX = bboutermin.x;

						xval = kernX-bboutermin.x;
						index = ((((zval*height)+(yval))*width)+(xval));	

						filtered_valueR += (double)(source_array[3*index+offset])*mask[mask_iter];
						mask_iter++;
					}

					xval = x-bboutermin.x;
					index = ((((zval*height)+(yval))*width)+(xval));	
					
					filtered_valueR /= sum;
					source_array2[3*index+offset] = filtered_valueR;
			}
		}
	}
 
	//-------------------------------------------------------------
	//Apply Gaussian in the Y dir
	//-------------------------------------------------------------
	for(float z=bboutermin.z; z<bboutermax.z; z++)
	{
		if(showGUI)
		{
			//progress(z, depth-kernel_radius);
			progressbar->setProgress(helper);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(helper, 4*depth);

		helper++;

		zval = z-bboutermin.z;
		for(float x=bboutermin.x; x<bboutermax.x; x++)
		{
			xval = x-bboutermin.x;
			for(float y=bboutermin.y; y<bboutermax.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;
					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernY = y+i;
						if(kernY>bboutermax.y-1) kernY = bboutermax.y-1;
						if(kernY<bboutermin.y) kernY = bboutermin.y;

						yval = kernY-bboutermin.y;
						index = ((((zval*height)+(yval))*width)+(xval));

						filtered_valueR += (double)(source_array2[3*index+offset])*mask[mask_iter];
						mask_iter++;
					}
					filtered_valueR /= sum;

					yval = y-bboutermin.y;
					index = ((((zval*height)+(yval))*width)+(xval));
					source_array[3*index+offset] = filtered_valueR;
			}
		}
	}
 
	//-------------------------------------------------------------
	//Apply Gaussian in the Z dir
	//-------------------------------------------------------------
	for(float z=bboutermin.z; z<bboutermax.z; z++)
	{
		if(showGUI)
		{
			progressbar->setProgress(helper);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(helper, 4*depth);

		helper++;

		for(float x=bboutermin.x; x<bboutermax.x; x++)
		{
			xval = x-bboutermin.x;
			for(float y=bboutermin.y; y<bboutermax.y; y++)
			{				
				yval = y-bboutermin.y;

				mask_iter=0;
				filtered_valueR=0.0;
				for(int i=-kernel_radius; i<=kernel_radius; i++)
				{
					kernZ = z+i;
					if(kernZ>bboutermax.z-1) kernZ = bboutermax.z-1;
					if(kernZ<bboutermin.z) kernZ = bboutermin.z;
										
					zval = kernZ-bboutermin.z;
					index = ((((zval*height)+(yval))*width)+(xval));	

					filtered_valueR += (double)(source_array[3*index+offset])*mask[mask_iter];
					mask_iter++;
				}

				filtered_valueR /= sum;

				index = 3*volobj->get_index1D(x,y,z);
				volobj->texture3d[index+offset] = filtered_valueR+0.5;

				if(volobj->is_greyscale==true)
				{
					volobj->texture3d[index+2] =
					volobj->texture3d[index+1] =
					volobj->texture3d[index+0];
				}
			}
		}
	}
	
	delete source_array;
	source_array = NULL;
	delete source_array2;
	source_array2 = NULL;
	
	if(showGUI)
	{
		progressbar->setProgress(4*depth);
		delete progressbar;
		progressbar = NULL;
	}
	else progress(4*depth, 4*depth);
}
void Filter::apply_gaussian_filter_decomp_window(int kr, double sigma, int dimensionality, vector<bool> channels)
{
	//printf("gaussian: %d, %f, %d\n", kr, sigma, dimensionality);
	//printf("chans: %i, %i, %i\n", (int)channels[0], (int)channels[1], (int)channels[2]);

	int kernel_radius = (kr-1)/2;

	//volobj->compute_boundingbox_channel(0, kr);
	//printf("kr: %d\n", kr);

	Vector bbsize = volobj->boundingboxMax - volobj->boundingboxMin;
	//int size = volobj->texwidth*volobj->texheight*volobj->texdepth*3;//bbsize.x*bbsize.y*bbsize.z*3;
	int size = bbsize.x*bbsize.y*bbsize.z*3;
	int index, index2;
	int helper=0;

	int offset=0;
	if(channels[1]==true) offset=1;
	else if(channels[2]==true) offset=2;

/*	width = volobj->texwidth;
	height = volobj->texheight;
	depth = volobj->texdepth;
*/
	width = bbsize.x;
	height = bbsize.y;
	depth = bbsize.z;

	Vector bboutermin = volobj->boundingboxMin;
	Vector bboutermax = volobj->boundingboxMax;

	Vector bbinnermin = volobj->boundingboxMin+2*kr;
	Vector bbinnermax = volobj->boundingboxMax-2*kr;

	/*progressbar = new Q3ProgressDialog("Computing Gaussian Smooth...", "Cancel", 4*depth, 0, "progress", TRUE);		
	progressbar->setCaption("Please Wait");*/
	//progressbar->setMinimumDuration(100);

	source_array = new unsigned char [size];
	for(int i=0; i<size; i++) source_array[i]=0;

	unsigned char* source_array2;
	source_array2 = new unsigned char [size];
	for(int i=0; i<size; i++) source_array2[i]=0;

	/*int size2 = volobj->texwidth*volobj->texheight*volobj->texdepth;
	for(int i=0; i<size2; i++)
	{
		volobj->texture3d[3*i+1]=0;
		volobj->texture3d[3*i+2]=0;
	}*/

	//-------------------------------------------------------------
	//Make a copy data of the original data.
	//-------------------------------------------------------------
	for(int z=bboutermin.z; z<bboutermax.z; z++)
	{
		for(int y=bboutermin.y; y<bboutermax.y; y++)
			for(int x=bboutermin.x; x<bboutermax.x; x++)
			{
				index = 3*(((((((int)z)-bboutermin.z)*height)+((int)y-bboutermin.y))*width)+((int)x-bboutermin.x));
				index2 = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
				source_array[index+offset] = volobj->texture3d[index2+offset];
			}
	}


	int mask_iter=0;
	double filtered_valueR=0.0;
	double filtered_valueG=0.0;
	double filtered_valueB=0.0;
	double filtered_valueA=0.0;
	float kernX, kernY, kernZ;
	
	//-------------------------------------------------------------
	//Apply Gaussian in the X dir
	//-------------------------------------------------------------
	
	//TOP WINDOW
	for(float z=bboutermin.z; z<bbinnermin.z; z++)
	{
		for(float x=bboutermin.x; x<bboutermax.x; x++)
		{
			for(float y=bboutermin.y; y<bboutermax.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x+i;
						kernY = y;
						kernZ = z;

						if(kernX<bboutermin.x) kernX = bboutermin.x;
						if(kernX>bboutermax.x-1) kernX = bboutermax.x-1;

						//index = 3*((((((int)kernZ)*volobj->texheight)+((int)kernY))*volobj->texwidth)+((int)kernX));
						index = 3*(((((((int)kernZ)-bboutermin.z)*height)+((int)kernY-bboutermin.y))*width)+((int)kernX-bboutermin.x));
						
						filtered_valueR += (double)(source_array[index+offset])*mask[mask_iter];
						mask_iter++;
					}

					index = 3*(((((((int)z)-bboutermin.z)*height)+((int)y-bboutermin.y))*width)+((int)x-bboutermin.x));
					//index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));					
					filtered_valueR /= sum;
					source_array2[index+offset] = filtered_valueR;
			}
		}
	}

	for(float z=bbinnermin.z; z<bbinnermax.z; z++)
	{
		//LEFT WINDOW
		for(float x=bboutermin.x; x<bbinnermin.x; x++)
		{
			for(float y=bboutermin.y; y<bboutermax.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x+i;
						kernY = y;
						kernZ = z;

						if(kernX<bboutermin.x) kernX = bboutermin.x;
						if(kernX>bbinnermin.x-1) kernX = bbinnermin.x-1;

						//index = 3*((((((int)kernZ)*volobj->texheight)+((int)kernY))*volobj->texwidth)+((int)kernX));
						index = 3*(((((((int)kernZ)-bboutermin.z)*height)+((int)kernY-bboutermin.y))*width)+((int)kernX-bboutermin.x));
						
						filtered_valueR += (double)(source_array[index+offset])*mask[mask_iter];
						mask_iter++;
					}

	//				index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));					
					index = 3*(((((((int)z)-bboutermin.z)*height)+((int)y-bboutermin.y))*width)+((int)x-bboutermin.x));
					filtered_valueR /= sum;
					source_array2[index+offset] = filtered_valueR;		
			}
		}
		//RIGHT WINDOW
		for(float x=bbinnermax.x; x<bboutermax.x; x++)
		{
			for(float y=bboutermin.y; y<bboutermax.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x+i;
						kernY = y;
						kernZ = z;

						if(kernX<bbinnermax.x) kernX = bbinnermax.x;
						if(kernX>bboutermax.x-1) kernX = bboutermax.x-1;

					//	index = 3*((((((int)kernZ)*volobj->texheight)+((int)kernY))*volobj->texwidth)+((int)kernX));
						index = 3*(((((((int)kernZ)-bboutermin.z)*height)+((int)kernY-bboutermin.y))*width)+((int)kernX-bboutermin.x));
						
						filtered_valueR += (double)(source_array[index+offset])*mask[mask_iter];
						mask_iter++;
					}

	//				index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));					
					index = 3*(((((((int)z)-bboutermin.z)*height)+((int)y-bboutermin.y))*width)+((int)x-bboutermin.x));
					filtered_valueR /= sum;
					source_array2[index+offset] = filtered_valueR;	
			}
		}
		//FRONT WINDOW
		for(float x=bbinnermin.x; x<bbinnermax.x; x++)
		{
			for(float y=bboutermin.y; y<bbinnermin.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x+i;
						kernY = y;
						kernZ = z;

						if(kernX<bbinnermin.x) kernX = bbinnermin.x;
						if(kernX>bbinnermax.x-1) kernX = bbinnermax.x-1;

					//	index = 3*((((((int)kernZ)*volobj->texheight)+((int)kernY))*volobj->texwidth)+((int)kernX));
						index = 3*(((((((int)kernZ)-bboutermin.z)*height)+((int)kernY-bboutermin.y))*width)+((int)kernX-bboutermin.x));
						
						filtered_valueR += (double)(source_array[index+offset])*mask[mask_iter];
						mask_iter++;
					}

		//			index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));					
					index = 3*(((((((int)z)-bboutermin.z)*height)+((int)y-bboutermin.y))*width)+((int)x-bboutermin.x));
					filtered_valueR /= sum;
					source_array2[index+offset] = filtered_valueR;	
			}
		}
		//BACK WINDOW
		for(float x=bbinnermin.x; x<bbinnermax.x; x++)
		{
			for(float y=bbinnermax.y; y<bboutermax.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x+i;
						kernY = y;
						kernZ = z;

						if(kernX<bbinnermin.x) kernX = bbinnermin.x;
						if(kernX>bbinnermax.x-1) kernX = bbinnermax.x-1;

					//	index = 3*((((((int)kernZ)*volobj->texheight)+((int)kernY))*volobj->texwidth)+((int)kernX));
						index = 3*(((((((int)kernZ)-bboutermin.z)*height)+((int)kernY-bboutermin.y))*width)+((int)kernX-bboutermin.x));
						
						filtered_valueR += (double)(source_array[index+offset])*mask[mask_iter];
						mask_iter++;
					}

		//			index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));					
					index = 3*(((((((int)z)-bboutermin.z)*height)+((int)y-bboutermin.y))*width)+((int)x-bboutermin.x));
					filtered_valueR /= sum;
					source_array2[index+offset] = filtered_valueR;	
			}
		}
	}

	//BOTTOM WINDOW
	for(float z=bbinnermax.z; z<bboutermax.z; z++)
	{
		for(float x=bboutermin.x; x<bboutermax.x; x++)
		{
			for(float y=bboutermin.y; y<bboutermax.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x+i;
						kernY = y;
						kernZ = z;

						if(kernX<bboutermin.x) kernX = bboutermin.x;
						if(kernX>bboutermax.x-1) kernX = bboutermax.x-1;

			//			index = 3*((((((int)kernZ)*volobj->texheight)+((int)kernY))*volobj->texwidth)+((int)kernX));
						index = 3*(((((((int)kernZ)-bboutermin.z)*height)+((int)kernY-bboutermin.y))*width)+((int)kernX-bboutermin.x));
						
						filtered_valueR += (double)(source_array[index+offset])*mask[mask_iter];
						mask_iter++;
					}

	//				index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));					
					index = 3*(((((((int)z)-bboutermin.z)*height)+((int)y-bboutermin.y))*width)+((int)x-bboutermin.x));
					filtered_valueR /= sum;
					source_array2[index+offset] = filtered_valueR;	
			}
		}
	}

	//-------------------------------------------------------------
	//Apply Gaussian in the Y dir
	//-------------------------------------------------------------


	//TOP WINDOW
	for(float z=bboutermin.z; z<bbinnermin.z; z++)
	{
		for(float x=bboutermin.x; x<bboutermax.x; x++)
		{
			for(float y=bboutermin.y; y<bboutermax.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x;
						kernY = y+i;
						kernZ = z;

						if(kernY<bboutermin.y) kernY = bboutermin.y;
						if(kernY>bboutermax.y-1) kernY = bboutermax.y-1;

				//		index = 3*((((((int)kernZ)*volobj->texheight)+((int)kernY))*volobj->texwidth)+((int)kernX));
						index = 3*(((((((int)kernZ)-bboutermin.z)*height)+((int)kernY-bboutermin.y))*width)+((int)kernX-bboutermin.x));
						
						filtered_valueR += (double)(source_array2[index+offset])*mask[mask_iter];
						mask_iter++;
					}

		//			index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					index = 3*(((((((int)z)-bboutermin.z)*height)+((int)y-bboutermin.y))*width)+((int)x-bboutermin.x));
					filtered_valueR /= sum;
					source_array[index+offset] = filtered_valueR;
			}
		}
	}

	for(float z=bbinnermin.z; z<bbinnermax.z; z++)
	{
		//LEFT WINDOW
		for(float x=bboutermin.x; x<bbinnermin.x; x++)
		{
			for(float y=bboutermin.y; y<bboutermax.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x;
						kernY = y+i;
						kernZ = z;

						if(kernY<bboutermin.y) kernY = bboutermin.y;
						if(kernY>bboutermax.y-1) kernY = bboutermax.y-1;

				//		index = 3*((((((int)kernZ)*volobj->texheight)+((int)kernY))*volobj->texwidth)+((int)kernX));
						index = 3*(((((((int)kernZ)-bboutermin.z)*height)+((int)kernY-bboutermin.y))*width)+((int)kernX-bboutermin.x));
						
						filtered_valueR += (double)(source_array2[index+offset])*mask[mask_iter];
						mask_iter++;
					}

//					index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					index = 3*(((((((int)z)-bboutermin.z)*height)+((int)y-bboutermin.y))*width)+((int)x-bboutermin.x));
					filtered_valueR /= sum;
					source_array[index+offset] = filtered_valueR;	
			}
		}
		//RIGHT WINDOW
		for(float x=bbinnermax.x; x<bboutermax.x; x++)
		{
			for(float y=bboutermin.y; y<bboutermax.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x;
						kernY = y+i;
						kernZ = z;

						if(kernY<bboutermin.y) kernY = bboutermin.y;
						if(kernY>bboutermax.y-1) kernY = bboutermax.y-1;

					//	index = 3*((((((int)kernZ)*volobj->texheight)+((int)kernY))*volobj->texwidth)+((int)kernX));
						index = 3*(((((((int)kernZ)-bboutermin.z)*height)+((int)kernY-bboutermin.y))*width)+((int)kernX-bboutermin.x));
						
						filtered_valueR += (double)(source_array2[index+offset])*mask[mask_iter];
						mask_iter++;
					}

//					index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					index = 3*(((((((int)z)-bboutermin.z)*height)+((int)y-bboutermin.y))*width)+((int)x-bboutermin.x));
					filtered_valueR /= sum;
					source_array[index+offset] = filtered_valueR;
			}
		}
		//FRONT WINDOW
		for(float x=bbinnermin.x; x<bbinnermax.x; x++)
		{
			for(float y=bboutermin.y; y<bbinnermin.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x;
						kernY = y+i;
						kernZ = z;

						if(kernY<bboutermin.y) kernY = bboutermin.y;
						if(kernY>bbinnermin.y-1) kernY = bbinnermin.y-1;

			//			index = 3*((((((int)kernZ)*volobj->texheight)+((int)kernY))*volobj->texwidth)+((int)kernX));
						index = 3*(((((((int)kernZ)-bboutermin.z)*height)+((int)kernY-bboutermin.y))*width)+((int)kernX-bboutermin.x));
						
						filtered_valueR += (double)(source_array2[index+offset])*mask[mask_iter];
						mask_iter++;
					}

	//				index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					index = 3*(((((((int)z)-bboutermin.z)*height)+((int)y-bboutermin.y))*width)+((int)x-bboutermin.x));
					filtered_valueR /= sum;
					source_array[index+offset] = filtered_valueR;
			}
		}
		//BACK WINDOW
		for(float x=bbinnermin.x; x<bbinnermax.x; x++)
		{
			for(float y=bbinnermax.y; y<bboutermax.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x;
						kernY = y+i;
						kernZ = z;

						if(kernY<bbinnermax.y) kernY = bbinnermax.y;
						if(kernY>bboutermax.y-1) kernY = bboutermax.y-1;

				//		index = 3*((((((int)kernZ)*volobj->texheight)+((int)kernY))*volobj->texwidth)+((int)kernX));
						index = 3*(((((((int)kernZ)-bboutermin.z)*height)+((int)kernY-bboutermin.y))*width)+((int)kernX-bboutermin.x));
						
						filtered_valueR += (double)(source_array2[index+offset])*mask[mask_iter];
						mask_iter++;
					}

//					index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					index = 3*(((((((int)z)-bboutermin.z)*height)+((int)y-bboutermin.y))*width)+((int)x-bboutermin.x));
					filtered_valueR /= sum;
					source_array[index+offset] = filtered_valueR;
			}
		}
	}

	//BOTTOM WINDOW
	for(float z=bbinnermax.z; z<bboutermax.z; z++)
	{
		for(float x=bboutermin.x; x<bboutermax.x; x++)
		{
			for(float y=bboutermin.y; y<bboutermax.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x;
						kernY = y+i;
						kernZ = z;

						if(kernY<bboutermin.y) kernY = bboutermin.y;
						if(kernY>bboutermax.y-1) kernY = bboutermax.y-1;

		//				index = 3*((((((int)kernZ)*volobj->texheight)+((int)kernY))*volobj->texwidth)+((int)kernX));
						index = 3*(((((((int)kernZ)-bboutermin.z)*height)+((int)kernY-bboutermin.y))*width)+((int)kernX-bboutermin.x));
					
						filtered_valueR += (double)(source_array2[index+offset])*mask[mask_iter];
						mask_iter++;
					}

//					index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					index = 3*(((((((int)z)-bboutermin.z)*height)+((int)y-bboutermin.y))*width)+((int)x-bboutermin.x));
					filtered_valueR /= sum;
					source_array[index+offset] = filtered_valueR;
			}
		}
	}

	//-------------------------------------------------------------
	//Apply Gaussian in the Z dir
	//-------------------------------------------------------------
	
	//TOP WINDOW
	for(float z=bboutermin.z; z<bbinnermin.z; z++)
	{
		for(float x=bboutermin.x; x<bboutermax.x; x++)
		{
			for(float y=bboutermin.y; y<bboutermax.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x;
						kernY = y;
						kernZ = z+i;

						if(kernZ<bboutermin.z) kernZ = bboutermin.z;
						if(kernZ>bbinnermin.z-1) kernZ = bbinnermin.z-1;

		//				index = 3*((((((int)kernZ)*volobj->texheight)+((int)kernY))*volobj->texwidth)+((int)kernX));
						index = 3*(((((((int)kernZ)-bboutermin.z)*height)+((int)kernY-bboutermin.y))*width)+((int)kernX-bboutermin.x));
						
						filtered_valueR += (double)(source_array[index+offset])*mask[mask_iter];
						mask_iter++;
					}

					index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					filtered_valueR /= sum;
					volobj->texture3d[index+offset] = filtered_valueR+0.5;
			}
		}
	}

	for(float z=bbinnermin.z; z<bbinnermax.z; z++)
	{
		//LEFT WINDOW
		for(float x=bboutermin.x; x<bbinnermin.x; x++)
		{
			for(float y=bboutermin.y; y<bboutermax.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x;
						kernY = y;
						kernZ = z+i;

						if(kernZ<bbinnermin.z) kernZ = bbinnermin.z;
						if(kernZ>bbinnermax.z-1) kernZ = bbinnermax.z-1;

	//					index = 3*((((((int)kernZ)*volobj->texheight)+((int)kernY))*volobj->texwidth)+((int)kernX));
						index = 3*(((((((int)kernZ)-bboutermin.z)*height)+((int)kernY-bboutermin.y))*width)+((int)kernX-bboutermin.x));
					
						filtered_valueR += (double)(source_array[index+offset])*mask[mask_iter];
						mask_iter++;
					}

					index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					filtered_valueR /= sum;
					volobj->texture3d[index+offset] = filtered_valueR+0.5;	
			}
		}
		//RIGHT WINDOW
		for(float x=bbinnermax.x; x<bboutermax.x; x++)
		{
			for(float y=bboutermin.y; y<bboutermax.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x;
						kernY = y;
						kernZ = z+i;

						if(kernZ<bbinnermin.z) kernZ = bbinnermin.z;
						if(kernZ>bbinnermax.z-1) kernZ = bbinnermax.z-1;

	//					index = 3*((((((int)kernZ)*volobj->texheight)+((int)kernY))*volobj->texwidth)+((int)kernX));
						index = 3*(((((((int)kernZ)-bboutermin.z)*height)+((int)kernY-bboutermin.y))*width)+((int)kernX-bboutermin.x));
						
						filtered_valueR += (double)(source_array[index+offset])*mask[mask_iter];
						mask_iter++;
					}

					index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					filtered_valueR /= sum;
					volobj->texture3d[index+offset] = filtered_valueR+0.5;	
			}
		}
		//FRONT WINDOW
		for(float x=bbinnermin.x; x<bbinnermax.x; x++)
		{
			for(float y=bboutermin.y; y<bbinnermin.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x;
						kernY = y;
						kernZ = z+i;

						if(kernZ<bbinnermin.z) kernZ = bbinnermin.z;
						if(kernZ>bbinnermax.z-1) kernZ = bbinnermax.z-1;

	//					index = 3*((((((int)kernZ)*volobj->texheight)+((int)kernY))*volobj->texwidth)+((int)kernX));
						index = 3*(((((((int)kernZ)-bboutermin.z)*height)+((int)kernY-bboutermin.y))*width)+((int)kernX-bboutermin.x));
						
						filtered_valueR += (double)(source_array[index+offset])*mask[mask_iter];
						mask_iter++;
					}

					index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					filtered_valueR /= sum;
					volobj->texture3d[index+offset] = filtered_valueR+0.5;	
			}
		}
		//BACK WINDOW
		for(float x=bbinnermin.x; x<bbinnermax.x; x++)
		{
			for(float y=bbinnermax.y; y<bboutermax.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x;
						kernY = y;
						kernZ = z+i;

						if(kernZ<bbinnermin.z) kernZ = bbinnermin.z;
						if(kernZ>bbinnermax.z-1) kernZ = bbinnermax.z-1;

	//					index = 3*((((((int)kernZ)*volobj->texheight)+((int)kernY))*volobj->texwidth)+((int)kernX));
						index = 3*(((((((int)kernZ)-bboutermin.z)*height)+((int)kernY-bboutermin.y))*width)+((int)kernX-bboutermin.x));
						
						filtered_valueR += (double)(source_array[index+offset])*mask[mask_iter];
						mask_iter++;
					}

					index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					filtered_valueR /= sum;
					volobj->texture3d[index+offset] = filtered_valueR+0.5;	
			}
		}
	}

	//BOTTOM WINDOW
	for(float z=bbinnermax.z; z<bboutermax.z; z++)
	{
		for(float x=bboutermin.x; x<bboutermax.x; x++)
		{
			for(float y=bboutermin.y; y<bboutermax.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x;
						kernY = y;
						kernZ = z+i;

						if(kernZ<bbinnermax.z) kernZ = bbinnermax.z;
						if(kernZ>bboutermax.z-1) kernZ = bboutermax.z-1;

		//				index = 3*((((((int)kernZ)*volobj->texheight)+((int)kernY))*volobj->texwidth)+((int)kernX));
						index = 3*(((((((int)kernZ)-bboutermin.z)*height)+((int)kernY-bboutermin.y))*width)+((int)kernX-bboutermin.x));
						
						filtered_valueR += (double)(source_array[index+offset])*mask[mask_iter];
						mask_iter++;
					}

					index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					filtered_valueR /= sum;
					volobj->texture3d[index+offset] = filtered_valueR+0.5;	
			}
		}
	}
	/*

	//TOP WINDOW
	for(float z=bboutermin.z; z<bbinnermin.z; z++)
	{
		for(float x=bboutermin.x; x<bboutermax.x; x++)
		{
			for(float y=bboutermin.y; y<bboutermax.y; y++)
			{				
					index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					volobj->texture3d[index+2] = 255;
			}
		}
	}

	for(float z=bbinnermin.z; z<bbinnermax.z; z++)
	{
		//LEFT WINDOW
		for(float x=bboutermin.x; x<bbinnermin.x; x++)
		{
			for(float y=bboutermin.y; y<bboutermax.y; y++)
			{				
					index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					volobj->texture3d[index+1] = 32;
			}
		}
		//RIGHT WINDOW
		for(float x=bbinnermax.x; x<bboutermax.x; x++)
		{
			for(float y=bboutermin.y; y<bboutermax.y; y++)
			{				
					index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					volobj->texture3d[index+1] = 64;
			}
		}
		//FRONT WINDOW
		for(float x=bbinnermin.x; x<bbinnermax.x; x++)
		{
			for(float y=bboutermin.y; y<bbinnermin.y; y++)
			{				
					index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					volobj->texture3d[index+1] = 128;
			}
		}
		//BACK WINDOW
		for(float x=bbinnermin.x; x<bbinnermax.x; x++)
		{
			for(float y=bbinnermax.y; y<bboutermax.y; y++)
			{				
					index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					volobj->texture3d[index+1] = 255;
			}
		}
	}

	//BOTTOM WINDOW
	for(float z=bbinnermax.z; z<bboutermax.z; z++)
	{
		for(float x=bboutermin.x; x<bboutermax.x; x++)
		{
			for(float y=bboutermin.y; y<bboutermax.y; y++)
			{				
					index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					volobj->texture3d[index+2] = 128;
			}
		}
	}

	*/
 	/*for(float z=bbinnermax.z; z<bboutermax.z; z++)
	{
		for(float x=bbinnermax.x; x<bboutermax.x; x++)
		{
			for(float y=bbinnermax.y; y<bboutermax.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x+i;
						kernY = y;
						kernZ = z;

						if(kernX>bboutermax.x-1) kernX = bboutermax.x-1;
						if(kernX<bbinnermax.x) kernX = bbinnermax.x;

						//index = 3*(((((kernZ-bboutermin.z)*height)+(kernY-bboutermin.y))*width)+(kernX-bboutermin.x));
						index = 3*((((((int)kernZ)*volobj->texheight)+((int)kernY))*volobj->texwidth)+((int)kernX));
						
						filtered_valueR += (double)(source_array[index+offset])*mask[mask_iter];
						mask_iter++;
					}

					//index = 3*(((((z-bboutermin.z)*height)+(y-bboutermin.y))*width)+(x-bboutermin.x));
					index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					
					filtered_valueR /= sum;
					source_array2[index+offset] = filtered_valueR;
					volobj->texture3d[index+1] = 255;
			}
		}
	}*/
	/*
	//-------------------------------------------------------------
	//Apply Gaussian in the Y dir
	//-------------------------------------------------------------
	for(float z=bboutermin.z; z<bbinnermin.z; z++)
	{
		for(float x=bboutermin.x; x<bbinnermin.x; x++)
		{
			for(float y=bboutermin.y; y<bbinnermin.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x;
						kernY = y+i;
						kernZ = z;

						if(kernY>bbinnermin.y-1) kernY = bbinnermin.y-1;
						if(kernY<bboutermin.y) kernY = bboutermin.y;

						//index = 3*(((((kernZ-bboutermin.z)*height)+(kernY-bboutermin.y))*width)+(kernX-bboutermin.x));
						index = 3*((((((int)kernZ)*volobj->texheight)+((int)kernY))*volobj->texwidth)+((int)kernX));
						
						filtered_valueR += (double)(source_array2[index+offset])*mask[mask_iter];
						mask_iter++;
					}

					//index = 3*(((((z-bboutermin.z)*height)+(y-bboutermin.y))*width)+(x-bboutermin.x));
					index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					filtered_valueR /= sum;

					source_array[index+offset] = filtered_valueR;
					volobj->texture3d[index+1] = 255;
			}
		}
	}
	for(float z=bbinnermax.z; z<bboutermax.z; z++)
	{
		for(float x=bbinnermax.x; x<bboutermax.x; x++)
		{
			for(float y=bbinnermax.y; y<bboutermax.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x;
						kernY = y+i;
						kernZ = z;

						if(kernY>bboutermax.y-1) kernY = bboutermax.y-1;
						if(kernY<bbinnermax.y) kernY = bbinnermax.y;

						//index = 3*(((((kernZ-bboutermin.z)*height)+(kernY-bboutermin.y))*width)+(kernX-bboutermin.x));
						index = 3*((((((int)kernZ)*volobj->texheight)+((int)kernY))*volobj->texwidth)+((int)kernX));
						
						filtered_valueR += (double)(source_array2[index+offset])*mask[mask_iter];
						mask_iter++;
					}

					//index = 3*(((((z-bboutermin.z)*height)+(y-bboutermin.y))*width)+(x-bboutermin.x));
					index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					filtered_valueR /= sum;

					source_array[index+offset] = filtered_valueR;
					volobj->texture3d[index+1] = 255;
			}
		}
	}
  
	//-------------------------------------------------------------
	//Apply Gaussian in the Z dir
	//-------------------------------------------------------------
	for(float z=bboutermin.z; z<bbinnermin.z; z++)
	{
		for(float x=bboutermin.x; x<bbinnermin.x; x++)
		{
			for(float y=bboutermin.y; y<bbinnermin.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x;
						kernY = y;
						kernZ = z+i;

						if(kernZ>bbinnermin.z-1) kernZ = bbinnermin.z-1;
						if(kernZ<bboutermin.z) kernZ = bboutermin.z;

						//index = 3*(((((kernZ-bboutermin.z)*height)+(kernY-bboutermin.y))*width)+(kernX-bboutermin.x));
						index = 3*((((((int)kernZ)*volobj->texheight)+((int)kernY))*volobj->texwidth)+((int)kernX));
						
						filtered_valueR += (double)(source_array[index+offset])*mask[mask_iter];
						mask_iter++;
					}

					index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					filtered_valueR /= sum;
					volobj->texture3d[index+offset] = filtered_valueR+0.5;
					volobj->texture3d[index+1] = 255;
			}
		}
	}
	for(float z=bbinnermax.z; z<bboutermax.z; z++)
	{
		for(float x=bbinnermax.x; x<bboutermax.x; x++)
		{
			for(float y=bbinnermax.y; y<bboutermax.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;

					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x;
						kernY = y;
						kernZ = z+i;

						if(kernZ>bbinnermax.z-1) kernZ = bbinnermax.z-1;
						if(kernZ<bboutermin.z) kernZ = bboutermin.z;

						//index = 3*(((((kernZ-bboutermin.z)*height)+(kernY-bboutermin.y))*width)+(kernX-bboutermin.x));
						index = 3*((((((int)kernZ)*volobj->texheight)+((int)kernY))*volobj->texwidth)+((int)kernX));
						
						filtered_valueR += (double)(source_array[index+offset])*mask[mask_iter];
						mask_iter++;
					}

					index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					filtered_valueR /= sum;
					volobj->texture3d[index+offset] = filtered_valueR+0.5;
					volobj->texture3d[index+1] = 255;
			}
		}
	}
*/
	delete source_array;
	source_array = NULL;
	delete source_array2;
	source_array2 = NULL;
	
	/*progressbar->setProgress(4*depth);
	delete progressbar;
	progressbar = NULL;*/
}
void Filter::apply_arithmoperator(int src, int msk, int arithmoperator)
{
	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Arithmetic Operator...", "Cancel", volobj->texdepth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
	}

	printf("Arithmetic Operator: %d\n", arithmoperator);

	int index, valueA, valueB, valueC, dst;

	if(src==0 && msk==1) dst=2;
	else if(src==1 && msk==0) dst=2;
	else if(src==0 && msk==2) dst=1;
	else if(src==2 && msk==0) dst=1;
	else if(src==1 && msk==2) dst=0;
	else if(src==2 && msk==1) dst=0;

	int maxpix, minpix;
	maxpix = -9999999;
	minpix = 9999999;

	for(float z=0; z<volobj->texdepth; z++)
	{
		if(showGUI)
		{
			progressbar->setProgress(z);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(z, volobj->texdepth);

		for(float x=0; x<volobj->texwidth; x++)
		{
			for(float y=0; y<volobj->texheight; y++)
			{
				index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));

				valueA = volobj->texture3d[index+src];
				valueB = volobj->texture3d[index+msk];
				
				if(arithmoperator==0)
				{
					//A + B
					valueC = valueA + valueB;

					//if(valueC>maxpix) maxpix = valueC;
					//if(valueC<minpix) minpix = valueC;
					
					if(valueC<0) valueC=-valueC;
					if(valueC>255) valueC=255;
					volobj->texture3d[index+dst] = valueC;
				}
				if(arithmoperator==1)
				{
					//A - B
					valueC = valueA - valueB;

					//if(valueC>maxpix) maxpix = valueC;
					//if(valueC<minpix) minpix = valueC;

					if(valueC<0) valueC=0;
					if(valueC>255) valueC=255;
					volobj->texture3d[index+dst] = valueC;
				}

			}
		}
	}

/*
	//normalize
	for(int i=0; i<volobj->texdepth*volobj->texheight*volobj->texwidth; i++)
	{
		volobj->texture3d[3*i+dst] = (((float)volobj->texture3d[3*i+dst]-(float)minpix)/(float)maxpix)*255.0f;
	}
*/

	volobj->is_greyscale = false;
	volobj->has_red = true;
	volobj->has_green = true;
	volobj->has_blue = true;
	
	if(showGUI)
	{
		progressbar->setProgress(volobj->texdepth);
		delete progressbar;
		progressbar = NULL;
	}
	else progress(volobj->texdepth, volobj->texdepth);
}
void Filter::apply_setoperator(int src, int msk, int setoperator)
{
	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Set Operator...", "Cancel", volobj->texdepth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
	}

	int index, valueA, valueB, dst;

	if(src==0 && msk==1) dst=2;
	else if(src==1 && msk==0) dst=2;
	else if(src==0 && msk==2) dst=1;
	else if(src==2 && msk==0) dst=1;
	else if(src==1 && msk==2) dst=0;
	else if(src==2 && msk==1) dst=0;

	for(float z=0; z<volobj->texdepth; z++)
	{
		if(showGUI)
		{
			progressbar->setProgress(z);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(z, volobj->texdepth);

		for(float x=0; x<volobj->texwidth; x++)
		{
			for(float y=0; y<volobj->texheight; y++)
			{
				index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));

				valueA = volobj->texture3d[index+src];
				valueB = volobj->texture3d[index+msk];
				
				if(setoperator==0)
				{
					//A not B
					if(valueA != 0 && valueB==0) volobj->texture3d[index+dst] = valueA;
					else volobj->texture3d[index+dst] = 0;
				}
				if(setoperator==1)
				{
					//B not A
					if(valueA == 0 && valueB!=0) volobj->texture3d[index+dst] = valueB;
					else volobj->texture3d[index+dst] = 0;
				}
				if(setoperator==2)
				{
					volobj->texture3d[index+dst] = 0;

					//A diff B
					if(valueA == 0 && valueB!=0) volobj->texture3d[index+dst] = valueB;
					if(valueA != 0 && valueB==0) volobj->texture3d[index+dst] = valueA;
				}
				if(setoperator==3)
				{
					//A and B
					if(valueA != 0 && valueB!=0) volobj->texture3d[index+dst] = valueA;
					else volobj->texture3d[index+dst] = 0;
				}
			}
		}
	}

	volobj->is_greyscale = false;
	volobj->has_red = true;
	volobj->has_green = true;
	volobj->has_blue = true;

	if(showGUI)
	{
		progressbar->setProgress(volobj->texdepth);
		delete progressbar;
		progressbar = NULL;
	}
	else progress(volobj->texdepth, volobj->texdepth);
}
void Filter::apply_addto(int src, int dst)
{
	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Add to...", "Cancel", depth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
	}

	printf("src %d dst %d \n", src, dst);
	int index, value;
	for(float z=0; z<volobj->texdepth; z++)
	{
		if(showGUI)
		{
			progressbar->setProgress(z);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(z, depth);

		for(float x=0; x<volobj->texwidth; x++)
		{
			for(float y=0; y<volobj->texheight; y++)
			{
				index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));

				value = volobj->texture3d[index+src];
				if(value!=0) volobj->texture3d[index+dst] = value;
			}
		}
	}

	volobj->is_greyscale = false;

	if(src==0) volobj->has_red = true;
	if(src==1) volobj->has_green = true;
	if(src==2) volobj->has_blue = true;

	if(dst==0) volobj->has_red = true;
	if(dst==1) volobj->has_green = true;
	if(dst==2) volobj->has_blue = true;

	if(showGUI)
	{
		progressbar->setProgress(depth);
		delete progressbar;
		progressbar = NULL;
	}
	else progress(depth, depth);
}
void Filter::apply_replace(int src, int repop, int in, int with)
{
	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Replace with...", "Cancel", depth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
	}

	int index, value;
	for(float z=0; z<volobj->texdepth; z++)
	{
		if(showGUI)
		{
			progressbar->setProgress(z);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(z, depth);

		for(float x=0; x<volobj->texwidth; x++)
		{
			for(float y=0; y<volobj->texheight; y++)
			{
				index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));

				value = volobj->texture3d[index+src];

				if(repop==0)
				{
					if(value!=0) volobj->texture3d[index+in] = volobj->texture3d[index+with];
				}
				else if(repop==1)
				{
					if(value==0) volobj->texture3d[index+in] = volobj->texture3d[index+with];
				}
			}
		}
	}

	if(showGUI)
	{
		progressbar->setProgress(depth);
		delete progressbar;
		progressbar = NULL;
	}
	else progress(depth, depth);
}
void Filter::apply_swap(int srcA, int srcB)
{
	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Swapping ...", "Cancel", depth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
	}

	int index, valueA, valueB;
	for(float z=0; z<volobj->texdepth; z++)
	{
		if(showGUI)
		{
			progressbar->setProgress(z);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(z, depth);

		for(float x=0; x<volobj->texwidth; x++)
		{
			for(float y=0; y<volobj->texheight; y++)
			{
				index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));

				valueA = volobj->texture3d[index+srcA];
				valueB = volobj->texture3d[index+srcB];

				volobj->texture3d[index+srcA] = valueB;
				volobj->texture3d[index+srcB] = valueA;
			}
		}
	}

	volobj->is_greyscale = false;

	if(srcA==0) volobj->has_red = true;
	if(srcA==1) volobj->has_green = true;
	if(srcA==2) volobj->has_blue = true;

	if(srcB==0) volobj->has_red = true; 
	if(srcB==1) volobj->has_green = true;
	if(srcB==2) volobj->has_blue = true;

	if(showGUI)
	{
		progressbar->setProgress(depth);
		delete progressbar;
		progressbar = NULL;
	}
	else progress(depth, depth);
}

void Filter::apply_copyto(int src, int dst)
{
	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Copy to...", "Cancel", depth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
	}

	int index, value;
	for(float z=0; z<volobj->texdepth; z++)
	{
		if(showGUI)
		{
			progressbar->setProgress(z);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(z, depth);

		for(float x=0; x<volobj->texwidth; x++)
		{
			for(float y=0; y<volobj->texheight; y++)
			{
				index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));

				value = volobj->texture3d[index+src];
				volobj->texture3d[index+dst] = value;
			}
		}
	}

	volobj->is_greyscale = false;

	if(src==0) volobj->has_red = true;
	if(src==1) volobj->has_green = true;
	if(src==2) volobj->has_blue = true;

	if(dst==0) volobj->has_red = true;
	if(dst==1) volobj->has_green = true;
	if(dst==2) volobj->has_blue = true;

	if(showGUI)
	{
		progressbar->setProgress(depth);
		delete progressbar;
		progressbar = NULL;
	}
	else progress(depth, depth);
}
#ifdef OPENCL_SUPPORT
void Filter::apply_dilate_filter_gpu(int kernel_radius, int dimensionality, vector<bool> channels)
{
	int offset=0;
	if(channels[1]==true) offset=1;
	else if(channels[2]==true) offset=2;
	
	ocl_dilate_ptr->ocl_filter(offset);
}
void Filter::apply_erode_filter_gpu(int kernel_radius, int dimensionality, vector<bool> channels)
{
	int offset=0;
	if(channels[1]==true) offset=1;
	else if(channels[2]==true) offset=2;
	
	ocl_erode_ptr->ocl_filter(offset);
}
void Filter::apply_median_filter_gpu(int kernel_radius, int dimensionality, vector<bool> channels)
{
	int offset=0;
	if(channels[1]==true) offset=1;
	else if(channels[2]==true) offset=2;

	ocl_median_ptr->ocl_filter(offset);
}
void Filter::apply_gaussian_filter_gpu(int kernel_radius, double sigma, int dimensionality, vector<bool> channels)
{
	int offset=0;
	if(channels[1]==true) offset=1;
	else if(channels[2]==true) offset=2;

	//temp array
	int arraysize = volobj->texwidth*volobj->texheight*volobj->texdepth;
	unsigned char* data = new unsigned char[arraysize];
	for(int i=0; i<arraysize; i++) data[i] = volobj->texture3d[3*i+offset];
	ocl_gaussian_ptr->ocl_filterGaussian_init(sigma, kernel_radius);
	ocl_gaussian_ptr->ocl_filterGaussian(data, data, volobj->texwidth, volobj->texheight, volobj->texdepth);
	for(int i=0; i<arraysize; i++)
	{
			if(volobj->is_greyscale)
				volobj->texture3d[3*i+0] = volobj->texture3d[3*i+1] = volobj->texture3d[3*i+2] = data[i];
			else
				volobj->texture3d[3*i+offset] = data[i];
	}
	delete[] data;
}
void Filter::apply_peronamalik_gpu(int dim, double l, double dt, int iters, vector<bool> channels)
{
	if(volobj->texwidth==1 || volobj->texheight==1 || volobj->texdepth==1) return;

	int offset=0;
	if(channels[1]==true) offset=1;
	else if(channels[2]==true) offset=2;

	//temp array
	int arraysize = volobj->texwidth*volobj->texheight*volobj->texdepth;
	unsigned char* data = new unsigned char[arraysize];
	for(int i=0; i<arraysize; i++) data[i] = volobj->texture3d[3*i+offset];

	for(int i=0; i<iters; i++)
		ocl_peronamalik_ptr->ocl_filterPeronaMalik(l, dt, data, data, volobj->texwidth, volobj->texheight, volobj->texdepth);

	for(int i=0; i<arraysize; i++)
	{
			if(volobj->is_greyscale)
				volobj->texture3d[3*i+0] = volobj->texture3d[3*i+1] = volobj->texture3d[3*i+2] = data[i];
			else
				volobj->texture3d[3*i+offset] = data[i];
	}
	delete[] data;
}
void Filter::apply_bilateral_filter_gpu(float s, float r, vector<bool> channels)
{
	if(volobj->texwidth==1 || volobj->texheight==1 || volobj->texdepth==1) return;

	int offset=0;
	if(channels[1]==true) offset=1;
	else if(channels[2]==true) offset=2;

	//temp array
	int arraysize = volobj->texwidth*volobj->texheight*volobj->texdepth;
	unsigned char* data = new unsigned char[arraysize];
	for(int i=0; i<arraysize; i++) data[i] = volobj->texture3d[3*i+offset];

	ocl_bilateral_ptr->ocl_filterBilateral(s, r, data, data, volobj->texwidth, volobj->texheight, volobj->texdepth);
	
	for(int i=0; i<arraysize; i++)
	{
			if(volobj->is_greyscale)
				volobj->texture3d[3*i+0] = volobj->texture3d[3*i+1] = volobj->texture3d[3*i+2] = data[i];
			else
				volobj->texture3d[3*i+offset] = data[i];
	}
	delete[] data;
}
int Filter::apply_stepperdilate_gpu(Vector p, int srcchan, int dstchan, int seed_window, int minsize, int gaussian_width, float gaussian_strength, int gaussian_window)
{
	Timer timer;

	std::vector<unsigned char> greyscale_array;
	std::vector<unsigned char> binaryregion_array;

	unsigned char* voxel0 = NULL;
	unsigned char* voxel1 = NULL;

	//size = volobj->texwidth*volobj->texheight*volobj->texdepth;

	volobj->texture3d[3*volobj->get_index1D(p.x, p.y, p.z)+dstchan]=255;
	float seed_srcval=volobj->texture3d[3*volobj->get_index1D(p.x, p.y, p.z)+srcchan];

	//bool cont = true;
	int region_size=0;

	timer.startTimer();
	//ocl_boundingbox_ptr->ocl_filterBoundingBox(dstchan, gaussian_width);
	//volobj->compute_boundingbox_channel(dstchan, gaussian_width);
	volobj->compute_boundingbox_channel_bounded(dstchan, gaussian_width*10);
	//volobj->compute_fastboundingbox_channel(p, dstchan, gaussian_width, tempsize);

	timer.endTimer("bounding box: ");

	Vector bbsize = volobj->boundingboxMax - volobj->boundingboxMin;
	width = bbsize.x;
	height = bbsize.y;
	depth = bbsize.z;
	
	printf("compute window: %d, %d, %d\n", width, height, depth);
	int size = width*height*depth;

	timer.startTimer();
	get_sources3D(&binaryregion_array, dstchan, &greyscale_array, srcchan);
	timer.endTimer("allocation: ");

	ocl_dilate_ptr->ocl_filter(&binaryregion_array[0], &binaryregion_array[0], width, height, depth);
	
	for(int i=0; i<size; i++)
	{
		voxel0 = &greyscale_array[i];
		voxel1 = &binaryregion_array[i];

		if(*voxel1==255)
		{
			if(*voxel0>(seed_srcval-seed_window) && *voxel0<(seed_srcval+seed_window))
			{
				*voxel1=255;
				region_size++;
			}
			else *voxel1=0;
		}
	}	
	voxel0 = NULL;
	voxel1 = NULL;

	/*timer.startTimer();
	set_source3D(&binaryregion_array, dstchan);
	timer.endTimer("setting: ");*/

	if(region_size>minsize)
	{

		//volobj->compute_boundingbox_channel(dstchan, gaussian_width);
		size = width*height*depth;
		 
	//	get_source3D(&binaryregion_array, dstchan);

		ocl_gaussian_ptr->ocl_filterGaussian(&binaryregion_array[0], &binaryregion_array[0], width, height, depth);

		for(int i=0; i<size; i++)
		{
			voxel0 = &binaryregion_array[i];

			if(*voxel0>gaussian_window) *voxel0=255;
			else *voxel0=0;
		}
		voxel0 = NULL;
		voxel1 = NULL;

	}

	timer.startTimer();
	set_source3D(&binaryregion_array, dstchan);
	timer.endTimer("setting: ");

	/*
	double rs = region_size;
	double si = pow(region_size, 1.0/3.0);
	tempsize = si*2.0;
	//printf("volume: %f, size: %f\n", rs, si);*/

	return region_size;
}
#endif
int Filter::apply_stepperdilate(Vector p, int srcchan, int dstchan, int seed_window, int minsize, int gaussian_width, float gaussian_strength, int gaussian_window)
{
	int DST_CHANNEL = dstchan;
	int SRC_CHANNEL = srcchan;
	int WINDOW_SIZE = seed_window;
	int GAUSSIAN_SIZE = gaussian_width;
	float GAUSSIAN_SIGMA = gaussian_strength;
	int GAUSSIAN_THRESHOLD = gaussian_window;

	int size, helper;

	size = volobj->texwidth*volobj->texheight*volobj->texdepth;

	vector< bool > gchannels;
	gchannels.resize(3);
	gchannels[0]=false;
	gchannels[1]=true;
	gchannels[2]=false;

	volobj->texture3d[3*volobj->get_index1D(p.x, p.y, p.z)+DST_CHANNEL]=255;

	float seed_srcval=volobj->texture3d[3*volobj->get_index1D(p.x, p.y, p.z)+SRC_CHANNEL];
	/*
	float mean=0;
	int curr_size=0;
	for(int i=0; i<size; i++)
	{
		if(volobj->texture3d[3*i+DST_CHANNEL]==255)
		{
			mean += volobj->texture3d[3*i+SRC_CHANNEL];
			curr_size++;
		}
	}
	mean /= (float)curr_size;

	float accum=0.0;
	for(int i=0; i<size; i++)
	{
		if(volobj->texture3d[3*i+DST_CHANNEL]==255)
		{
			accum += (volobj->texture3d[3*i+SRC_CHANNEL]-mean)*(volobj->texture3d[3*i+SRC_CHANNEL]-mean);
		}
	}
	float stdev = sqrt(accum/curr_size);

	printf("mean: %f\n", mean);
	printf("stdev: %f\n", stdev);*/

	//bool cont = true;
	int old_helper=0;
	int region_size=0;

	/*while(cont)
	{*/

		//printf("computing dilate...\n");
		apply_dilate_filter(1,0,gchannels);

		//printf("computing dilate threshold...\n");

		for(int i=0; i<size; i++)
		{
			if(volobj->texture3d[3*i+DST_CHANNEL]==255)
			{
				if(volobj->texture3d[3*i+SRC_CHANNEL]>(seed_srcval-WINDOW_SIZE) && volobj->texture3d[3*i+SRC_CHANNEL]<(seed_srcval+WINDOW_SIZE))
				//if(volobj->texture3d[3*i+SRC_CHANNEL]>=(mean-2.0*stdev) && volobj->texture3d[3*i+SRC_CHANNEL]<=(mean-2.0*stdev))
				{
					volobj->texture3d[3*i+DST_CHANNEL]=255;
					region_size++;
				}
				else
				{
					volobj->texture3d[3*i+DST_CHANNEL]=0;
				}
			}
		}

		//printf("computing bounding box...\n");
		//volobj->compute_boundingbox_channel(DST_CHANNEL, GAUSSIAN_SIZE);
		volobj->compute_boundingbox_channel_bounded(DST_CHANNEL, GAUSSIAN_SIZE);
	//	printf("region_size: %d\n", region_size);

		if(region_size>minsize)
		{
			//printf("computing gaussian...\n");
			//if(region_size>gaussian_width*gaussian_width*gaussian_width*8) apply_gaussian_filter_decomp_window(GAUSSIAN_SIZE, GAUSSIAN_SIGMA, 0, gchannels);
			//else apply_gaussian_filter_decomp(GAUSSIAN_SIZE, GAUSSIAN_SIGMA, 0, gchannels);		
			//apply_sobel3D_filter(1, gchannels);
			apply_gaussian_filter_decomp(GAUSSIAN_SIZE, GAUSSIAN_SIGMA, 0, gchannels);

			//apply_gaussian_filter_decomp_window(GAUSSIAN_SIZE, GAUSSIAN_SIGMA, 0, gchannels);
			//apply_median_filter(1,1,gchannels);

			//printf("computing gaussian threshold...\n");
			helper=0;
			for(int i=0; i<size; i++)
			{
			//	if(volobj->texture3d[3*i+DST_CHANNEL]!=0) volobj->texture3d[3*i+2]=50;
			//	else  volobj->texture3d[3*i+2]=0;

				if(volobj->texture3d[3*i+DST_CHANNEL]>GAUSSIAN_THRESHOLD)
				{
					volobj->texture3d[3*i+DST_CHANNEL] = 255;
					//volobj->texture3d[3*i+2]=255;
					helper++;
				}
				else
				{
					volobj->texture3d[3*i+DST_CHANNEL]=0;
					//volobj->texture3d[3*i+2]=0;
				}
			}
		}

		return region_size;
	/*
		helper++;
		printf("helper: %d\n", helper);
		if(helper-old_helper<THRESHOLD_SIZE) cont=false;
		old_helper=helper;
	}*/
}
void Filter::apply_dilate_filter(int kernel_radius, int dimensionality, vector<bool> channels)
{
	int offset;

	if(channels[0]==true) offset=0;
	else if(channels[1]==true) offset=1;
	else offset = 2;

	vector<bool> axis;
	if(channels.size()==6)
	{
		axis.push_back(channels[3]);
		axis.push_back(channels[4]);
		axis.push_back(channels[5]);
	}

	int size = volobj->texdepth*volobj->texheight*volobj->texwidth*3;

	source_array = NULL;
	source_array = new unsigned char [size];
	if(source_array==NULL) 
	{
		printf("ERROR: Allocating memory!\n");
		return;
		//system("pause");
	}

	for(int i=0; i<size; i++) source_array[i]=volobj->texture3d[i];

	int prog_max = volobj->boundingboxMax.z- volobj->boundingboxMin.z;
	int prog_step=0;

	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Computing Dilate...", "Cancel", prog_max, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
	}

	//volobj->compute_boundingbox();
	volobj->compute_boundingbox_channel(offset, 2);
	int index, value, x_iter, y_iter, z_iter, xval, yval, zval;
//	for(float z=0; z<volobj->texdepth; z++)
	for(int z=volobj->boundingboxMin.z; z<volobj->boundingboxMax.z; z++)
	{
		if(showGUI)
		{
			progressbar->setProgress(prog_step);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(prog_step, prog_max);

		prog_step++;

		for(int x=volobj->boundingboxMin.x; x<volobj->boundingboxMax.x; x++)
//		for(float x=0; x<volobj->texwidth; x++)
		{
//			for(float y=0; y<volobj->texheight; y++)
			for(int y=volobj->boundingboxMin.y; y<volobj->boundingboxMax.y; y++)
			{
				index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
				//index = 3*(((((((int)z)-volobj->boundingboxMin.z)*height)+((int)y-volobj->boundingboxMin.y))*width)+((int)x-volobj->boundingboxMin.x));

				value = source_array[index+offset];
				
				if(value!=0)
				{
					x_iter=0;
					y_iter=0;
					z_iter=0;
				
				//	if(dimensionality==1)
				//	{
						for(int i=-kernel_radius; i<=kernel_radius; i++)
						{
							for(int j=-kernel_radius; j<=kernel_radius; j++)
							{
								for(int k=-kernel_radius; k<=kernel_radius; k++)
								{
									zval = z+i;
									yval = y+j;
									xval = x+k;
									
									if(!axis.empty())
									{
										if(axis[0]==false) xval=x;
										if(axis[1]==false) yval=y;
										if(axis[2]==false) zval=z;
									}

									if(zval>volobj->texdepth-1) zval = volobj->texdepth-1;
									if(yval>volobj->texheight-1) yval = volobj->texheight-1;
									if(xval>volobj->texwidth-1) xval = volobj->texwidth-1;
									if(zval<0) zval = 0;
									if(yval<0) yval = 0;
									if(xval<0) xval = 0;

									index = 3*((((((int)zval)*volobj->texheight)+((int)yval))*volobj->texwidth)+((int)xval));
									volobj->texture3d[index+offset] = value;
								}
							}
						}
				//	}
				}
			}
		}
	}

	if(showGUI)
	{
		progressbar->setProgress(prog_max);
		delete progressbar;
		progressbar = NULL;
	}
	else progress(prog_max, prog_max);

	if(source_array!=NULL) delete[] source_array;
	source_array = NULL;
}
void Filter::apply_erode_filter(int kernel_radius, int dimensionality, vector<bool> channels)
{
/*	int offset;

	if(channels[0]==true) offset=0;
	else if(channels[1]==true) offset=1;
	else offset = 2;

	vector<bool> axis;
	if(channels.size()==6)
	{
		axis.push_back(channels[3]);
		axis.push_back(channels[4]);
		axis.push_back(channels[5]);
	}

	int size = volobj->texdepth*volobj->texheight*volobj->texwidth*3;

	source_array = new unsigned char [size];
	for(int i=0; i<size; i++) source_array[i]=volobj->texture3d[i];

	progressbar = new Q3ProgressDialog("Computing Erode...", "Cancel", volobj->texdepth, 0, "progress", TRUE);		
	progressbar->setCaption("Please Wait");
	
	int index, value, x_iter, y_iter, z_iter, xval, yval, zval;
	for(float z=0; z<volobj->texdepth; z++)
	{
		progressbar->setProgress(z);
		if (progressbar->wasCancelled())
			break;

		for(float x=0; x<volobj->texwidth; x++)
		{
			for(float y=0; y<volobj->texheight; y++)
			{
				index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));

				value = source_array[index+offset];
				
				if(value==0)
				{
					x_iter=0;
					y_iter=0;
					z_iter=0;
				
				//	if(dimensionality==1)
				//	{
						for(int i=-kernel_radius; i<=kernel_radius; i++)
						{
							for(int j=-kernel_radius; j<=kernel_radius; j++)
							{
								for(int k=-kernel_radius; k<=kernel_radius; k++)
								{
									zval = z+i;
									yval = y+j;
									xval = x+k;
									
									if(!axis.empty())
									{
										if(axis[0]==false) xval=x;
										if(axis[1]==false) yval=y;
										if(axis[2]==false) zval=z;
									}

									if(zval>volobj->texdepth-1) zval = volobj->texdepth-1;
									if(yval>volobj->texheight-1) yval = volobj->texheight-1;
									if(xval>volobj->texwidth-1) xval = volobj->texwidth-1;
									if(zval<0) zval = 0;
									if(yval<0) yval = 0;
									if(xval<0) xval = 0;

									index = 3*((((((int)zval)*volobj->texheight)+((int)yval))*volobj->texwidth)+((int)xval));
									volobj->texture3d[index+offset] = 0;
								}
							}
						}
				//	}
				}
			}
		}
	}

	progressbar->setProgress(volobj->texdepth);
	delete progressbar;
	progressbar = NULL;

	delete source_array;
	source_array = NULL;*/

	int offset;

	if(channels[0]==true) offset=0;
	else if(channels[1]==true) offset=1;
	else offset = 2;

	vector<bool> axis;
	if(channels.size()==6)
	{
		axis.push_back(channels[3]);
		axis.push_back(channels[4]);
		axis.push_back(channels[5]);
	}

	int size = volobj->texdepth*volobj->texheight*volobj->texwidth*3;

	source_array = new unsigned char [size];
	for(int i=0; i<size; i++) source_array[i]=volobj->texture3d[i];

	int prog_max = volobj->boundingboxMax.z- volobj->boundingboxMin.z;
	int prog_step=0;

	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Computing Erode...", "Cancel", prog_max, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
	}

	volobj->compute_boundingbox();

	int index, value, x_iter, y_iter, z_iter, xval, yval, zval;
//	for(float z=0; z<volobj->texdepth; z++)
	for(int z=volobj->boundingboxMin.z; z<volobj->boundingboxMax.z; z++)
	{
		if(showGUI)
		{
			progressbar->setProgress(prog_step);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(prog_step, prog_max);

		prog_step++;

		for(int x=volobj->boundingboxMin.x; x<volobj->boundingboxMax.x; x++)
//		for(float x=0; x<volobj->texwidth; x++)
		{
//			for(float y=0; y<volobj->texheight; y++)
			for(int y=volobj->boundingboxMin.y; y<volobj->boundingboxMax.y; y++)
			{
				index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
				//index = 3*(((((((int)z)-volobj->boundingboxMin.z)*height)+((int)y-volobj->boundingboxMin.y))*width)+((int)x-volobj->boundingboxMin.x));

				value = source_array[index+offset];
				
				if(value==0)
				{
					x_iter=0;
					y_iter=0;
					z_iter=0;
				
				//	if(dimensionality==1)
				//	{
						for(int i=-kernel_radius; i<=kernel_radius; i++)
						{
							for(int j=-kernel_radius; j<=kernel_radius; j++)
							{
								for(int k=-kernel_radius; k<=kernel_radius; k++)
								{
									zval = z+i;
									yval = y+j;
									xval = x+k;
									
									if(!axis.empty())
									{
										if(axis[0]==false) xval=x;
										if(axis[1]==false) yval=y;
										if(axis[2]==false) zval=z;
									}

									if(zval>volobj->texdepth-1) zval = volobj->texdepth-1;
									if(yval>volobj->texheight-1) yval = volobj->texheight-1;
									if(xval>volobj->texwidth-1) xval = volobj->texwidth-1;
									if(zval<0) zval = 0;
									if(yval<0) yval = 0;
									if(xval<0) xval = 0;

									index = 3*((((((int)zval)*volobj->texheight)+((int)yval))*volobj->texwidth)+((int)xval));
									volobj->texture3d[index+offset] = 0;
								}
							}
						}
				//	}
				}
			}
		}
	}

	if(showGUI)
	{
		progressbar->setProgress(prog_max);
		delete progressbar;
		progressbar = NULL;
	}
	else progress(prog_max, prog_max);

	delete source_array;
	source_array = NULL;
}
void Filter::apply_bilateral_filter(float s, float r, vector<bool> channels)
{
	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Applying Filter...", "Cancel", depth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
		progressbar->setMinimumDuration(10);
	}
	
	int offset;
	if(channels[0]==true) offset=0;
	else if(channels[1]==true) offset=1;
	else offset = 2;

	copy_source3D();

	double sigma_r, sigma_s;
	sigma_r = r;
	sigma_s = s;

	int index;
	int index1, index2, index3, index4, index5, index6;
	int xval, yval, zval;
	int x_iter, y_iter, z_iter;
	int xdis, ydis, zdist;
	Vector normal;
	double norm;
	double range;
	double domain;
	double vox_value;
	double vox_value_i;
	double filtered_value=0.0;
	double rterm;
	double two_sigmar_squared = 2*(sigma_r*sigma_r);
	double two_sigmas_squared = 2*(sigma_s*sigma_s);
	double domain_mult_range;
	int helper=0;
	for(float z=volobj->boundingboxMin.z; z<volobj->boundingboxMax.z; z++)
	{	
		if(showGUI)
		{
			//progress(z, depth-kernel_radius);
			progressbar->setProgress(helper);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(helper, depth);

		helper++;

		for(float x=volobj->boundingboxMin.x; x<volobj->boundingboxMax.x; x++)
		{
			for(float y=volobj->boundingboxMin.y; y<volobj->boundingboxMax.y; y++)
			{				
				//compute gradient * gaussian
				x_iter=0;
				y_iter=0;
				z_iter=0;
				filtered_value=0.0;
				norm = 0.0;
				
				index1 = 3*volobj->get_index1DinBoundingBox(x,y,z);
				vox_value = source_array[index1+offset]/255.0;
				for(int i=-sigma_s; i<=sigma_s; i++)
				{
					for(int j=-sigma_s; j<=sigma_s; j++)
					{
						for(int k=-sigma_s; k<=sigma_s; k++)
						{
							zval = z+i;
							yval = y+j;
							xval = x+k;
							
							//compute gradient
							index2 = 3*volobj->get_index1DinBoundingBox(xval,yval,zval);
							vox_value_i = source_array[index2+offset]/255.0; 

							range = -((vox_value_i-vox_value)*(vox_value_i-vox_value))/(two_sigmar_squared);
							range = exp(range);

							domain = -((i*i)+(j*j)+(k*k)) / (two_sigmas_squared);
							domain = exp(domain);

							domain_mult_range = domain*range;
							filtered_value += (double)(vox_value_i)*domain_mult_range;
							norm += domain_mult_range;
							z_iter++;
						}
								
						y_iter++;
						z_iter=0;
					}

					x_iter++;
					y_iter=0;
				}

				index = 3*volobj->get_index1D(x,y,z);
				volobj->texture3d[index+offset] = 255.0*(filtered_value/norm);

				if(volobj->is_greyscale==true)
				{
					volobj->texture3d[3*index+2] =
					volobj->texture3d[3*index+1] =
					volobj->texture3d[3*index+0];
				}
			}
			
		}

	}

	if(showGUI)
	{
		progressbar->setProgress(depth);
		delete progressbar;
		progressbar = NULL;
	}
	else progress(depth, depth);

	delete source_array;
	source_array = NULL;
}
void Filter::apply_gaussian_filter(int size, double sigma, int dimensionality, vector<bool> channels)
{
	//printf("Gaussian Smoothing: (sigma %f size %d)\n\n", sigma,size); 
	Gaussian gauss;

	//allocate_result3D();
	copy_source3D();

	//evaluate our sigma value for a given 
	//kernel mask radius
	//gauss.eval_sigma(kernel_radius);
	gauss.sigma = sigma;
	gauss.size = size;
	int kernel_radius = (size-1)/2;

	vector< vector< vector<double> > > mask3d;
	vector< vector< double > > mask2d;

	if(dimensionality==1)
	{
		//allocate some memory for our kernel mask
		mask3d.resize((int)gauss.size);
		for(int n=0; n<gauss.size; n++)
			mask3d[n].resize(gauss.size);

		for(int nn=0; nn<gauss.size; nn++)
			for(int nnn=0; nnn<gauss.size; nnn++)
			mask3d[nn][nnn].resize((int)gauss.size);

		//evaluate our guassian kernel mask
		mask3d = gauss.gaussianmask3D(gauss.sigma, gauss.size);
	}
	else
	{
		mask2d.resize((int)gauss.size);
		for(int n=0; n<gauss.size; n++)
			mask2d[n].resize(gauss.size);

		mask2d = gauss.gaussianmask2D(gauss.sigma, gauss.size);
	}
	
	int index=0;
	int x_iter=0;
	int y_iter=0;
	int z_iter=0;
	
	double filtered_value=0.0;

	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Applying Filter...", "Cancel", depth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
	}

	float kernX, kernY, kernZ;
	int xval, yval, zval;
	
	int offset=0;
	if(channels[1]==true) offset=1;
	else if(channels[2]==true) offset=2;

	//int 
	int helper=0;
	for(float z=volobj->boundingboxMin.z; z<volobj->boundingboxMax.z; z++)
	{
		if(showGUI)
		{
			//progress(z, depth-kernel_radius);
			progressbar->setProgress(helper);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(helper, depth);

		helper++;

		for(float x=volobj->boundingboxMin.x; x<volobj->boundingboxMax.x; x++)
		{
			for(float y=volobj->boundingboxMin.y; y<volobj->boundingboxMax.y; y++)
			{				
					x_iter=0;
					y_iter=0;
					z_iter=0;
					filtered_value=0.0;
				
					if(dimensionality==1)
					{
						for(int i=-kernel_radius; i<=kernel_radius; i++)
						{
							for(int j=-kernel_radius; j<=kernel_radius; j++)
							{
								for(int k=-kernel_radius; k<=kernel_radius; k++)
								{
									zval = z+i;
									yval = y+j;
									xval = x+k;


									if(zval>volobj->boundingboxMax.z-1) zval = volobj->boundingboxMax.z-1;
									if(yval>volobj->boundingboxMax.y-1) yval = volobj->boundingboxMax.y-1;
									if(xval>volobj->boundingboxMax.x-1) xval = volobj->boundingboxMax.x-1;

									if(zval<volobj->boundingboxMin.z) zval = volobj->boundingboxMin.z;
									if(yval<volobj->boundingboxMin.y) yval = volobj->boundingboxMin.y;
									if(xval<volobj->boundingboxMin.x) xval = volobj->boundingboxMin.x;
									
									index = 3*(((((zval-volobj->boundingboxMin.z)*height)+(yval-volobj->boundingboxMin.y))*width)+(xval-volobj->boundingboxMin.x));

									filtered_value += (double)(source_array[index+offset])*mask3d[x_iter][y_iter][z_iter];
																		
									z_iter++;
								}
								
								y_iter++;
								z_iter=0;
							}

							x_iter++;
							y_iter=0;
						}
					}
					else
					{
						for(int j=-kernel_radius; j<=kernel_radius; j++)
						{
							for(int k=-kernel_radius; k<=kernel_radius; k++)
							{
								xval = x+k;
								yval = y+j;

								if(yval>volobj->boundingboxMax.y-1) yval = volobj->boundingboxMax.y-1;
								if(xval>volobj->boundingboxMax.x-1) xval = volobj->boundingboxMax.x-1;
								if(yval<volobj->boundingboxMin.y) yval = volobj->boundingboxMin.y;
								if(xval<volobj->boundingboxMin.x) xval = volobj->boundingboxMin.x;
								
								index = 3*(((((z-volobj->boundingboxMin.z)*height)+(yval-volobj->boundingboxMin.y))*width)+(xval-volobj->boundingboxMin.x));
										
								filtered_value += (double)(source_array[index+offset])*mask2d[x_iter][y_iter];

								y_iter++;
							}
							
							x_iter++;
							y_iter=0;
						}
					}
					
					//index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
										
					volobj->texture3d[index+offset] = (int)filtered_value+0.5;
			}
		}
	}

	if(showGUI)
	{
		progressbar->setProgress(depth);
		delete progressbar;
		progressbar = NULL;
	}
	else progress(depth, depth);

	if(dimensionality==1)
	{
		for(int nn=0; nn<gauss.size; nn++)
			for(int nnn=0; nnn<gauss.size; nnn++)
			{
				mask3d[nn][nnn].clear();
			}		
		
		for(int nnn=0; nnn<gauss.size; nnn++)
		{
			mask3d[nnn].clear();
		}

		mask3d.clear();
	}
	else
	{
		for(int nnn=0; nnn<gauss.size; nnn++)
		{
			mask2d[nnn].clear();
		}

		mask2d.clear();	
	}
	
	delete source_array;
	source_array = NULL;
	gauss.clear_all();
}
void Filter::apply_unsharpmask_filter(int gsize, double sigma, int dimensionality, vector<bool> channels)
{
	//=====================================
	//apply gaussian, low-pass image
	//=====================================
	printf("Applying Gaussian Smoothing: (sigma: %.2f, size: %d)\n", sigma,gsize); 
	Gaussian gauss;

	//evaluate our sigma value for a given 
	//kernel mask radius
	//gauss.eval_sigma(kernel_radius);
	gauss.sigma = sigma;
	gauss.size = gsize;
	int kernel_radius = (gsize-1)/2;

	mask = gauss.gaussianmask1D(gauss.sigma, gauss.size);

	sum=0.0;
	for(int i=0; i<mask.size(); i++)
		sum += mask[i];

	//printf("kr: %d\n", kr);

	Vector bbsize = volobj->boundingboxMax - volobj->boundingboxMin;
	bbsize += 1.0;

//	bbsize = Vector(volobj->texwidth, volobj->texheight, volobj->texdepth);

	int size = 3*bbsize.x*bbsize.y*bbsize.z;
	int index, index2;
	int helper=0;

	int offset=0;
	if(channels[1]==true) offset=1;
	else if(channels[2]==true) offset=2;

	if(volobj->is_greyscale==true) offset=0;

	printf("offset: %d\n", offset);

	width = bbsize.x;
	height = bbsize.y;
	depth = bbsize.z;

	Vector bboutermin = volobj->boundingboxMin;
	Vector bboutermax = volobj->boundingboxMax;

	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Applying Filter...", "Cancel", 4*depth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
	    progressbar->setWindowModality(Qt::WindowModal);
		progressbar->setMinimumDuration(100);
	}

	source_array = 0;
	source_array = new unsigned char [size];
	if(source_array==0) return;
	for(int i=0; i<size; i++) source_array[i]=0;

	unsigned char* source_array2;
	source_array2 = 0;
	source_array2 = new unsigned char [size];
	if(source_array2==0) return;
	for(int i=0; i<size; i++) source_array2[i]=0;

	int xval, yval, zval;
		
	//-------------------------------------------------------------
	//Make a copy data of the original data.
	//-------------------------------------------------------------
	for(int z=bboutermin.z; z<bboutermax.z; z++)
	{
		if(showGUI)
		{
			progressbar->setProgress(helper);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(helper, 4*depth);

		helper++;

		zval = z-bboutermin.z;
		for(int y=bboutermin.y; y<bboutermax.y; y++)
		{
			yval = y-bboutermin.y;
			for(int x=bboutermin.x; x<bboutermax.x; x++)
			{
				xval = x-bboutermin.x;
				index = ((((zval*height)+(yval))*width)+(xval));	
				index2 = volobj->get_index1D(x,y,z);

				source_array[3*index+offset] = volobj->texture3d[3*index2+offset];
			}
		}
	}

	int mask_iter=0;
	double filtered_valueR=0.0;
	double filtered_valueG=0.0;
	double filtered_valueB=0.0;
	double filtered_valueA=0.0;
	float kernX, kernY, kernZ;

	//-------------------------------------------------------------
	//Apply Gaussian in the X dir
	//-------------------------------------------------------------
	for(float z=bboutermin.z; z<bboutermax.z; z++)
	{
		if(showGUI)
		{
			progressbar->setProgress(helper);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(helper, 4*depth);

		helper++;

		zval = z-bboutermin.z;
		for(float x=bboutermin.x; x<bboutermax.x; x++)
		{
			for(float y=bboutermin.y; y<bboutermax.y; y++)
			{				
					yval = y-bboutermin.y;

					mask_iter=0;
					filtered_valueR=0.0;
					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernX = x+i;
						if(kernX>bboutermax.x-1) kernX = bboutermax.x-1;
						if(kernX<bboutermin.x) kernX = bboutermin.x;

						xval = kernX-bboutermin.x;
						index = ((((zval*height)+(yval))*width)+(xval));	

						filtered_valueR += (double)(source_array[3*index+offset])*mask[mask_iter];
						mask_iter++;
					}

					xval = x-bboutermin.x;
					index = ((((zval*height)+(yval))*width)+(xval));	
					
					filtered_valueR /= sum;
					source_array2[3*index+offset] = filtered_valueR;		
			}
		}
	}
 
	//-------------------------------------------------------------
	//Apply Gaussian in the Y dir
	//-------------------------------------------------------------
	for(float z=bboutermin.z; z<bboutermax.z; z++)
	{
		if(showGUI)
		{
			//progress(z, depth-kernel_radius);
			progressbar->setProgress(helper);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(helper, 4*depth);

		helper++;

		zval = z-bboutermin.z;
		for(float x=bboutermin.x; x<bboutermax.x; x++)
		{
			xval = x-bboutermin.x;
			for(float y=bboutermin.y; y<bboutermax.y; y++)
			{				
					mask_iter=0;
					filtered_valueR=0.0;
					for(int i=-kernel_radius; i<=kernel_radius; i++)
					{
						kernY = y+i;
						if(kernY>bboutermax.y-1) kernY = bboutermax.y-1;
						if(kernY<bboutermin.y) kernY = bboutermin.y;

						yval = kernY-bboutermin.y;
						index = ((((zval*height)+(yval))*width)+(xval));

						filtered_valueR += (double)(source_array2[3*index+offset])*mask[mask_iter];
						mask_iter++;
					}
					filtered_valueR /= sum;

					yval = y-bboutermin.y;
					index = ((((zval*height)+(yval))*width)+(xval));
					source_array[3*index+offset] = filtered_valueR;
			}
		}
	}
 
	//-------------------------------------------------------------
	//Apply Gaussian in the Z dir
	//-------------------------------------------------------------
	for(float z=bboutermin.z; z<bboutermax.z; z++)
	{
		if(showGUI)
		{
			progressbar->setProgress(helper);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(helper, 4*depth);

		helper++;

		for(float x=bboutermin.x; x<bboutermax.x; x++)
		{
			xval = x-bboutermin.x;
			for(float y=bboutermin.y; y<bboutermax.y; y++)
			{				
				yval = y-bboutermin.y;

				mask_iter=0;
				filtered_valueR=0.0;
				for(int i=-kernel_radius; i<=kernel_radius; i++)
				{
					kernZ = z+i;
					if(kernZ>bboutermax.z-1) kernZ = bboutermax.z-1;
					if(kernZ<bboutermin.z) kernZ = bboutermin.z;
										
					zval = kernZ-bboutermin.z;
					index = ((((zval*height)+(yval))*width)+(xval));	

					filtered_valueR += (double)(source_array[3*index+offset])*mask[mask_iter];
					mask_iter++;
				}

				filtered_valueR /= sum;

				xval = x-volobj->boundingboxMin.x;
				yval = y-volobj->boundingboxMin.y;
				zval = z-volobj->boundingboxMin.z;
				index = ((((zval*height)+(yval))*width)+(xval));	

				source_array2[3*index+offset] = filtered_valueR+0.5;
				//index = 3*volobj->get_index1D(x,y,z);
				//volobj->texture3d[index+offset] = filtered_valueR+0.5;
			}
		}
	}
	
	
	//=====================================
	//compute high-pass of original
	//=====================================
	printf("computing high-pass\n");
	int subindex, bigindex;
	for(float z=volobj->boundingboxMin.z; z<volobj->boundingboxMax.z; z++)
	{
		for(float x=volobj->boundingboxMin.x; x<volobj->boundingboxMax.x; x++)
		{
			for(float y=volobj->boundingboxMin.y; y<volobj->boundingboxMax.y; y++)
			{
				xval = x-volobj->boundingboxMin.x;
				yval = y-volobj->boundingboxMin.y;
				zval = z-volobj->boundingboxMin.z;
				subindex = ((((zval*height)+(yval))*width)+(xval));	

				bigindex = volobj->get_index1D(x,y,z);

				int voxel = volobj->texture3d[3*bigindex+offset] - source_array2[3*subindex+offset];				

				if(voxel>255) voxel = 255;
				if(voxel<0)   voxel = 0;

				//get voxel value
				source_array2[3*subindex+offset] = voxel;
			}
		}
	}

	//=====================================
	//add high-pass to original
	//to get edge enhanced
	//=====================================
	printf("adding high-pass\n");
	for(float z=volobj->boundingboxMin.z; z<volobj->boundingboxMax.z; z++)
	{
		for(float x=volobj->boundingboxMin.x; x<volobj->boundingboxMax.x; x++)
		{
			for(float y=volobj->boundingboxMin.y; y<volobj->boundingboxMax.y; y++)
			{
				xval = x-volobj->boundingboxMin.x;
				yval = y-volobj->boundingboxMin.y;
				zval = z-volobj->boundingboxMin.z;
				subindex = ((((zval*height)+(yval))*width)+(xval));	

				bigindex = volobj->get_index1D(x,y,z);
				
				int voxel = volobj->texture3d[3*bigindex+offset] + source_array2[3*subindex+offset];				

				if(voxel>255) voxel = 255;
				if(voxel<0)   voxel = 0;

				//get voxel value
				volobj->texture3d[3*bigindex+offset] = voxel;

				if(volobj->is_greyscale==true)
				{
					volobj->texture3d[3*bigindex+2] =
					volobj->texture3d[3*bigindex+1] =
					volobj->texture3d[3*bigindex+0];
				}
			}
		}
	}

	delete source_array;
	source_array = NULL;
	delete source_array2;
	source_array2 = NULL;
	
	if(showGUI)
	{
		progressbar->setProgress(4*depth);
		delete progressbar;
		progressbar = NULL;
	}
	else progress(4*depth, 4*depth);

	gauss.clear_all();
}
void Filter::apply_median_filter(int kernel_radius, int dimensionality, vector<bool> channels)
{
	copy_source3D();
	
	int kernel_size = (kernel_radius*2+1)*(kernel_radius*2+1)*(kernel_radius*2+1);
	unsigned char* kernel_values = new unsigned char[kernel_size];

//	vector<unsigned char> kernel_values;
//	if(dimensionality==1) kernel_values.resize( (kernel_radius*2+1)*(kernel_radius*2+1)*(kernel_radius*2+1) );
//	else  kernel_values.resize((kernel_radius*2+1)*(kernel_radius*2+1));
	
	int current_index = 0;
	int filtered_value = 0;
	int xval, yval, zval;
	
	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Applying Filter...", "Cancel", depth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
		progressbar->setWindowModality(Qt::WindowModal);
		progressbar->setMinimumDuration(100);
	}

	int offset=0;
	if(channels[1]==true) offset=1;
	if(channels[2]==true) offset=2;

	if(volobj->is_greyscale==true) offset=0;

	int subvol_xval, subvol_yval, subvol_zval;
	
	int index;

	int value;
	int helper=0;
	int helper2=0;
	for(float z=volobj->boundingboxMin.z; z<=volobj->boundingboxMax.z; z++)
	{
		if(showGUI)
		{
			//progress(z, depth-kernel_radius);
			progressbar->setProgress(helper2);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(helper2, depth);

		helper2++;

		for(float y=volobj->boundingboxMin.y; y<=volobj->boundingboxMax.y; y++)
		{
			for(float x=volobj->boundingboxMin.x; x<=volobj->boundingboxMax.x; x++)
			{				
					//if(dimensionality==1)	//3D
					{
						helper=0;
						for(int i=-kernel_radius; i<=kernel_radius; i++)
						{
							for(int j=-kernel_radius; j<=kernel_radius; j++)
							{
								for(int k=-kernel_radius; k<=kernel_radius; k++)
								{
									zval = z+i;
									yval = y+j;
									xval = x+k;
									
									if(zval>volobj->boundingboxMax.z) zval = volobj->boundingboxMax.z;
									if(yval>volobj->boundingboxMax.y) yval = volobj->boundingboxMax.y;
									if(xval>volobj->boundingboxMax.x) xval = volobj->boundingboxMax.x;
									if(zval<volobj->boundingboxMin.z) zval = volobj->boundingboxMin.z;
									if(yval<volobj->boundingboxMin.y) yval = volobj->boundingboxMin.y;
									if(xval<volobj->boundingboxMin.x) xval = volobj->boundingboxMin.x;
									
									//index = 3*(((((zval-volobj->boundingboxMin.z)*height)+(yval-volobj->boundingboxMin.y))*width)+(xval-volobj->boundingboxMin.x));

									subvol_xval = xval-volobj->boundingboxMin.x;
									subvol_yval = yval-volobj->boundingboxMin.y;
									subvol_zval = zval-volobj->boundingboxMin.z;
									index = ((((subvol_zval*height)+(subvol_yval))*width)+(subvol_xval));	

									value = source_array[3*index+offset];
									
									kernel_values[helper] = value;
									helper++;
								}
							}
						}
					}
					/*else		//2D
					{
						helper=0;
						for(int i=-kernel_radius; i<=kernel_radius; i++)
						{
							for(int j=-kernel_radius; j<=kernel_radius; j++)
							{
									xval = x+i;
									yval = y+j;

									if(yval>volobj->boundingboxMax.y) yval = volobj->boundingboxMax.y;
									if(xval>volobj->boundingboxMax.x) xval = volobj->boundingboxMax.x;
									if(yval<volobj->boundingboxMin.y) yval = volobj->boundingboxMin.y;
									if(xval<volobj->boundingboxMin.x) xval = volobj->boundingboxMin.x;
									
									subvol_xval = xval-volobj->boundingboxMin.x;
									subvol_yval = yval-volobj->boundingboxMin.y;
									subvol_zval = zval-volobj->boundingboxMin.z;
									index = ((((subvol_zval*height)+(subvol_yval))*width)+(subvol_xval));	

//									index = 3*(((((z-volobj->boundingboxMin.z)*height)+(yval-volobj->boundingboxMin.y))*width)+(xval-volobj->boundingboxMin.x));
									
									value = source_array[3*index+offset];
									
									kernel_values[helper] = value;
									helper++;
							}
						}
					}*/
					
					//sort(kernel_values.begin(), kernel_values.end());

					for (int i=0;i<kernel_size;i++)
					{
						for (int j=0;j<kernel_size;j++)
						{
							if (kernel_values[i]>kernel_values[j])     
							{
								float v = kernel_values[i];
								kernel_values[i] = kernel_values[j];
								kernel_values[j] = v;
							}
						}
					}

					filtered_value = kernel_values[(int)((float)kernel_size/2.0f)];

					//index = 3*(((((((int)z)-volobj->boundingboxMin.z)*height)+((int)y-volobj->boundingboxMin.y))*width)+((int)x-volobj->boundingboxMin.x));
					//filtered_value = source_array[index+offset];

					current_index = 3*((((((int)z)*volobj->texheight)+((int)y))*volobj->texwidth)+((int)x));
					volobj->texture3d[current_index+offset]=filtered_value;

					if(volobj->is_greyscale==true)
					{
						volobj->texture3d[current_index+2] =
						volobj->texture3d[current_index+1] =
						volobj->texture3d[current_index+0];
					}

			}
		}
	}
	
	if(showGUI)
	{
		progressbar->setProgress(depth);
		delete progressbar;
		progressbar = NULL;
	}
	else progress(depth, depth);

	delete[] kernel_values;

	delete source_array;
	source_array=NULL;
}
void Filter::apply_mean_filter(int kernel_radius, int dimensionality, vector<bool> channels)
{
	copy_source3D();
	
	int current_index = 0;
	int filtered_value = 0;
	int xval, yval, zval;
	int sizehelper=0;
	
	//progressbar = new Q3ProgressDialog("Applying Filter...", "Cancel", depth, 0, "progress", TRUE);		
	//progressbar->setCaption("Please Wait");

	for(int z=0; z<depth; z++)
	{
		//progress(z, depth);

		//progressbar->setProgress(z);
		//if (progressbar->wasCancelled())
		//	break;
			
		for(int x=0; x<width; x++)
		{
			for(int y=0; y<height; y++)
			{					
					sizehelper = 0;
					
					if(dimensionality==1)
					{
						for(int i=-kernel_radius; i<=kernel_radius; i++)
						{
							for(int j=-kernel_radius; j<=kernel_radius; j++)
							{
								for(int k=-kernel_radius; k<=kernel_radius; k++)
								{
									zval = z+i;
									yval = y+j;
									xval = x+k;

									if(zval>depth-1) zval = depth-1;
									if(xval>width-1) xval = width-1;
									if(yval>height-1) yval = height-1;
									if(zval<0) zval = 0;
									if(xval<0) xval = 0;
									if(yval<0) yval = 0;
									
									int index = 3*(((((zval)*height)+(yval))*width)+(xval));
									
									if(channels[0]==true) filtered_value = source_array[index+0];
									else if(channels[1]==true) filtered_value = source_array[index+1];
									else if(channels[2]==true) filtered_value = source_array[index+2];
									sizehelper++;
								}
							}
						}
					}
					else
					{
						for(int j=-kernel_radius; j<=kernel_radius; j++)
						{
							for(int k=-kernel_radius; k<=kernel_radius; k++)
							{
								yval = y+j;
								xval = x+k;

								if(xval>width-1) xval = width-1;
								if(yval>height-1) yval = height-1;
								if(xval<0) xval = 0;
								if(yval<0) yval = 0;
								
								int index = 3*(((((z)*height)+(yval))*width)+(xval));
								
								if(channels[0]==true) filtered_value = source_array[index+0];
								else if(channels[1]==true) filtered_value = source_array[index+1];
								else if(channels[2]==true) filtered_value = source_array[index+2];
								sizehelper++;
							}
						}
					}
					
					filtered_value /= (float) sizehelper;
					
					current_index = 3*((((z*height)+y)*width)+x);

					if(channels[0]==true) volobj->texture3d[current_index+0]=filtered_value;
					else if(channels[1]==true) volobj->texture3d[current_index+1]=filtered_value;
					else if(channels[2]==true) volobj->texture3d[current_index+2]=filtered_value;
			}
		}
	}
	
	//progressbar->setProgress(depth);
	//delete progressbar;
	//progressbar = NULL;
	
	delete source_array;
	source_array=NULL;
}
void Filter::apply_peronamalik(int dim, double l, double dt, int inter, vector<bool> channels)
{
	int offset=0;
	if(channels[1]==true) offset=1;
	else if(channels[2]==true) offset=2;

	if(volobj->is_greyscale==true) offset=0;

	peronamalik.init_data3d(volobj->texwidth, volobj->texheight, volobj->texdepth, volobj->texture3d);
	peronamalik.lambda = l;

	peronamalik.peronaMalikOperator3d(dt, inter, offset, dim);

	if(volobj->is_greyscale==true)
	{
		int index;
		for(int z=0; z<volobj->texdepth; z++)
		{	
			for(int x=0; x<volobj->texwidth; x++)
			{
				for(int y=0; y<volobj->texheight; y++)
				{					
					index = 3*volobj->get_index1D(x,y,z);

					volobj->texture3d[index+2] =
					volobj->texture3d[index+1] =
					volobj->texture3d[index+0];
				}
			}
		}
	}
}
void Filter::apply_maxflow(int src, int dst, double w, double v, double k, vector<int> bg, vector<int> fg)
{
	printf("init");
	maxflow.init_data3d(volobj->texwidth, volobj->texheight, volobj->texdepth, volobj->texture3d);
	maxflow.w_weight = w;
	maxflow.v_weight = v;
	maxflow.k_weight = k;

	Vector vox;
	int minx, miny, minz, maxx, maxy, maxz;

	minx = miny = minz = 999999999;
	maxx = maxy = maxz = -999999999;

	printf("build histos\n");
	maxflow.clear_histograms();
	for(int i=0; i<bg.size(); i++)
	{
		double val = volobj->texture3d[3*bg[i]+src];
		maxflow.add_background(bg[i], val);

		vox = volobj->get_index3D(bg[i]);
		//volobj->texture3d[3*bg[i]+2] = 255;

		minx = min((float)minx, (float)vox.x);
		maxx = max((float)maxx, (float)vox.x);

		miny = min((float)miny, (float)vox.y);
		maxy = max((float)maxy, (float)vox.y);

		minz = min((float)minz, (float)vox.z);
		maxz = max((float)maxz, (float)vox.z);
	}
	for(int i=0; i<fg.size(); i++)
	{
		double val = volobj->texture3d[3*fg[i]+src];
		maxflow.add_foreground(fg[i], val);

		vox = volobj->get_index3D(fg[i]);
		//volobj->texture3d[3*fg[i]+1] = 255;

		minx = min((float)minx, (float)vox.x);
		maxx = max((float)maxx, (float)vox.x);

		miny = min((float)miny, (float)vox.y);
		maxy = max((float)maxy, (float)vox.y);

		minz = min((float)minz, (float)vox.z);
		maxz = max((float)maxz, (float)vox.z);
	}
	maxflow.bb_minX = minx;
	maxflow.bb_maxX = maxx;
	maxflow.bb_minY = miny;
	maxflow.bb_maxY = maxy;
	maxflow.bb_minZ = minz;
	maxflow.bb_maxZ = maxz;

	printf("BB: %d, %d, %d\n", maxx-minx, maxy-miny, maxz-minz);

	//maxflow.numnodes = (maxx-minx)*(maxy-miny)*(maxz-minz);
	//return;

	printf("compute 3d\n");
	maxflow.compute3d(src);

	printf("build results\n");
	int index;
	for(int x=0; x<volobj->texwidth; x++)
	for(int y=0; y<volobj->texheight; y++)
	for(int z=0; z<volobj->texdepth; z++)
	{
		index = ((((z*volobj->texheight)+y)*volobj->texwidth)+x);

		volobj->texture3d[3*index+2]=0;

		if (maxflow.belongsToSource(index))
		{
			//printf("source\n");
			volobj->texture3d[3*index+dst]=0;
		}
		else
		{
			//printf("dest\n");
			volobj->texture3d[3*index+dst]=255;
		}
	}
 
	bg.clear();
	fg.clear();
	maxflow.del_graph();

	volobj->is_greyscale = false;

	if(src==0) volobj->has_red = true;
	if(src==1) volobj->has_green = true;
	if(src==2) volobj->has_blue = true;
	if(dst==0) volobj->has_red = true;
	if(dst==1) volobj->has_green = true;
	if(dst==2) volobj->has_blue = true;
}
void Filter::apply_threshold(int channel, int value)
{
	//printf("Thresholding...\n");
	Q3ProgressDialog* progressbar = NULL;

	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Thresholding...", "Cancel", volobj->texdepth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
		progressbar->setMinimumDuration(10);
	}

	int index1d, voxel;

	//for(int z=volobject.boundingboxMin.z; z<volobject.boundingboxMax.z; z++)
	for(int z = 0; z <volobj->texdepth; z++)
	{	
		if(showGUI)
		{
			progressbar->setProgress(z);
			if (progressbar->wasCancelled()) break;
		}
		else progress(z, volobj->texdepth);

		//for(int x=volobject.boundingboxMin.x; x<volobject.boundingboxMax.x; x++)
		for(int x = 0; x <volobj->texwidth; x++)
		{
			//for(int y=volobject.boundingboxMin.y; y<volobject.boundingboxMax.y; y++)
			for(int y = 0; y <volobj->texheight; y++)
			{
				index1d = ((((z*volobj->texheight)+y)*volobj->texwidth)+x);						
				voxel = volobj->texture3d[3*index1d+channel];				
				if(voxel<=value)	volobj->texture3d[3*index1d+channel] = 0;
			}
		}
	}

	if(volobj->is_greyscale==true)
	{
		int index;
		for(int z=0; z<volobj->texdepth; z++)
		{	
			for(int x=0; x<volobj->texwidth; x++)
			{
				for(int y=0; y<volobj->texheight; y++)
				{					
					index = 3*volobj->get_index1D(x,y,z);

					volobj->texture3d[index+2] =
					volobj->texture3d[index+1] =
					volobj->texture3d[index+0];
				}
			}
		}
	}

	if(showGUI)
	{
		progressbar->setProgress(volobj->texdepth);
		delete progressbar;
	}
	else progress(volobj->texdepth, volobj->texdepth);
}
void Filter::apply_collapse(void)
{
	int index1d;
	int voxel;

	Q3ProgressDialog* progressbar = NULL;
	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Collapsing...", "Cancel", volobj->texdepth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
		progressbar->setMinimumDuration(10);
	}

	int divhelper=0;
	if(volobj->has_red) divhelper++;
	if(volobj->has_green) divhelper++;
	if(volobj->has_blue) divhelper++;

	if(divhelper==0) return;

	//for(int z=volobject.boundingboxMin.z; z<volobject.boundingboxMax.z; z++)
	for(int z = 0; z <volobj->texdepth; z++)
	{	
		if(showGUI)
		{
			progressbar->setProgress(z);
			if (progressbar->wasCancelled()) break;
		}
		else progress(z, volobj->texdepth);

		//for(int x=volobject.boundingboxMin.x; x<volobject.boundingboxMax.x; x++)
		for(int x = 0; x <volobj->texwidth; x++)
		{
			//for(int y=volobject.boundingboxMin.y; y<volobject.boundingboxMax.y; y++)
			for(int y = 0; y <volobj->texheight; y++)
			{
				index1d = ((((z*volobj->texheight)+y)*volobj->texwidth)+x);	
				voxel = 0.0;
				if(volobj->has_red) voxel = volobj->texture3d[3*index1d+0];						
				if(volobj->has_green) voxel += volobj->texture3d[3*index1d+1];						
				if(volobj->has_blue) voxel += volobj->texture3d[3*index1d+2];						
				voxel /= divhelper;

 				volobj->texture3d[3*index1d+0] = 
 				volobj->texture3d[3*index1d+1] = 
 				volobj->texture3d[3*index1d+2] = voxel; 
			}
		}
	}

	volobj->is_greyscale = true;
	volobj->has_red = false;
	volobj->has_green = false;
	volobj->has_blue = false;

	if(showGUI)
	{
		progressbar->setProgress(volobj->texdepth);
		delete progressbar;
	}
	else progress(volobj->texdepth, volobj->texdepth);
}

void Filter::apply_binarize(int channel)
{
	int index1d;
	int voxel;

	Q3ProgressDialog* progressbar=NULL;
	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Convertig to Binary...", "Cancel", volobj->texdepth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
		progressbar->setMinimumDuration(10);
	}

	//for(int z=volobject.boundingboxMin.z; z<volobject.boundingboxMax.z; z++)
	for(int z = 0; z <volobj->texdepth; z++)
	{	
		if(showGUI)
		{
			progressbar->setProgress(z);
			if (progressbar->wasCancelled()) break;
		}
		else progress(z, volobj->texdepth);

		//for(int x=volobject.boundingboxMin.x; x<volobject.boundingboxMax.x; x++)
		for(int x = 0; x <volobj->texwidth; x++)
		{
			//for(int y=volobject.boundingboxMin.y; y<volobject.boundingboxMax.y; y++)
			for(int y = 0; y <volobj->texheight; y++)
			{
				index1d = ((((z*volobj->texheight)+y)*volobj->texwidth)+x);	
				voxel = volobj->texture3d[3*index1d+channel];					
				if(voxel>0)volobj->texture3d[3*index1d+channel] = 255;
			}
		}
	}

	if(volobj->is_greyscale==true)
	{
		int index;
		for(int z=0; z<volobj->texdepth; z++)
		{	
			for(int x=0; x<volobj->texwidth; x++)
			{
				for(int y=0; y<volobj->texheight; y++)
				{					
					index = 3*volobj->get_index1D(x,y,z);

					volobj->texture3d[index+2] =
					volobj->texture3d[index+1] =
					volobj->texture3d[index+0];
				}
			}
		}
	}

	if(showGUI)
	{
		progressbar->setProgress(volobj->texdepth);
		delete progressbar;
	}
	else progress(volobj->texdepth, volobj->texdepth);
}
int Filter::apply_count(int channel)
{
	Q3ProgressDialog* progressbar = NULL;
	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Counting Voxels...", "Cancel", volobj->texdepth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
		progressbar->setMinimumDuration(10);
	}

	int index1d, voxel;
	int count=0;
	//for(int z=volobject.boundingboxMin.z; z<volobject.boundingboxMax.z; z++)
	for(int z = 0; z <volobj->texdepth; z++)
	{	
		if(showGUI)
		{
			progressbar->setProgress(z);
			if (progressbar->wasCancelled()) break;
		}
		else progress(z, volobj->texdepth);

		//for(int x=volobject.boundingboxMin.x; x<volobject.boundingboxMax.x; x++)
		for(int x = 0; x <volobj->texwidth; x++)
		{
			//for(int y=volobject.boundingboxMin.y; y<volobject.boundingboxMax.y; y++)
			for(int y = 0; y <volobj->texheight; y++)
			{
				index1d = ((((z*volobj->texheight)+y)*volobj->texwidth)+x);	
				if(volobj->texture3d[3*index1d+channel]!=0)
					count++;
			}
		}
	}	

	if(channel==0) volobj->has_red = true;
	if(channel==1) volobj->has_green = true;
	if(channel==2) volobj->has_blue = true;

	if(showGUI)
	{
		progressbar->setProgress(volobj->texdepth);
		delete progressbar;
	}
	else progress(volobj->texdepth, volobj->texdepth);

	return count;
}

void Filter::apply_clear(int channel)
{
	Q3ProgressDialog* progressbar = NULL;
	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Clearing Voxels...", "Cancel", volobj->texdepth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
		progressbar->setMinimumDuration(10);
	}

	int index1d, voxel;

	//for(int z=volobject.boundingboxMin.z; z<volobject.boundingboxMax.z; z++)
	for(int z = 0; z <volobj->texdepth; z++)
	{	
		if(showGUI)
		{
			progressbar->setProgress(z);
			if (progressbar->wasCancelled()) break;
		}
		else progress(z, volobj->texdepth);

		//for(int x=volobject.boundingboxMin.x; x<volobject.boundingboxMax.x; x++)
		for(int x = 0; x <volobj->texwidth; x++)
		{
			//for(int y=volobject.boundingboxMin.y; y<volobject.boundingboxMax.y; y++)
			for(int y = 0; y <volobj->texheight; y++)
			{
				index1d = ((((z*volobj->texheight)+y)*volobj->texwidth)+x);	
				volobj->texture3d[3*index1d+channel] = 0;
			}
		}
	}	

	if(volobj->is_greyscale==true)
	{
		int index;
		for(int z=0; z<volobj->texdepth; z++)
		{	
			for(int x=0; x<volobj->texwidth; x++)
			{
				for(int y=0; y<volobj->texheight; y++)
				{					
					index = 3*volobj->get_index1D(x,y,z);

					volobj->texture3d[index+2] =
					volobj->texture3d[index+1] =
					volobj->texture3d[index+0];
				}
			}
		}
	}

	if(channel==0) volobj->has_red = false;
	if(channel==1) volobj->has_green = false;
	if(channel==2) volobj->has_blue = false;

	if(showGUI)
	{
		progressbar->setProgress(volobj->texdepth);
		delete progressbar;
	}
	else progress(volobj->texdepth, volobj->texdepth);
}
void Filter::apply_invert(int channel)
{
	int index1d;
	int voxel;

	Q3ProgressDialog* progressbar = NULL;
	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Inverting...", "Cancel", volobj->texdepth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
		progressbar->setMinimumDuration(10);
	}

	//for(int z=volobject.boundingboxMin.z; z<volobject.boundingboxMax.z; z++)
	for(int z = 0; z <volobj->texdepth; z++)
	{	
		if(showGUI)
		{
			progressbar->setProgress(z);
			if (progressbar->wasCancelled()) break;
		}
		else progress(z, volobj->texdepth);

		//for(int x=volobject.boundingboxMin.x; x<volobject.boundingboxMax.x; x++)
		for(int x = 0; x <volobj->texwidth; x++)
		{
			//for(int y=volobject.boundingboxMin.y; y<volobject.boundingboxMax.y; y++)
			for(int y = 0; y <volobj->texheight; y++)
			{
				index1d = ((((z*volobj->texheight)+y)*volobj->texwidth)+x);						
				voxel = volobj->texture3d[3*index1d+channel];						
				if(voxel>0)	volobj->texture3d[3*index1d+channel] = 255-voxel;
			}
		}
	}

	if(volobj->is_greyscale==true)
	{
		int index;
		for(int z=0; z<volobj->texdepth; z++)
		{	
			for(int x=0; x<volobj->texwidth; x++)
			{
				for(int y=0; y<volobj->texheight; y++)
				{					
					index = 3*volobj->get_index1D(x,y,z);

					volobj->texture3d[index+2] =
					volobj->texture3d[index+1] =
					volobj->texture3d[index+0];
				}
			}
		}
	}

	if(showGUI)
	{
		progressbar->setProgress(volobj->texdepth);
		delete progressbar;
	}
	else progress(volobj->texdepth, volobj->texdepth);
}
void Filter::apply_sieve(int srcchan, int dstchan, int size)
{
	//volobj->is_greyscale = false;

	if(srcchan==0) volobj->has_red = true;
	if(srcchan==1) volobj->has_green = true;
	if(srcchan==2) volobj->has_blue = true;
/*
	if(dstchan==0) volobj->has_red = true;
	if(dstchan==1) volobj->has_green = true;
	if(dstchan==2) volobj->has_blue = true;
*/
	sieve_gateway(volobj, srcchan, srcchan, size);

	if(volobj->is_greyscale==true)
	{
		int index;
		for(int z=0; z<volobj->texdepth; z++)
		{	
			for(int x=0; x<volobj->texwidth; x++)
			{
				for(int y=0; y<volobj->texheight; y++)
				{					
					index = 3*volobj->get_index1D(x,y,z);

					volobj->texture3d[index+2] =
					volobj->texture3d[index+1] =
					volobj->texture3d[index+0];
				}
			}
		}
	}
}
void Filter::apply_floodfill(int srcchan, int dstchan, int window, int x, int y, int z)
{
	Point3D p = Point3D(x,y,z);
	Floodfill ffill;
	ffill.volobj = volobj;
	ffill.applyfloodfillseeded(p, srcchan, dstchan, window);
	ffill.volobj = NULL;
}

/*
void Filter::apply_canny3D(int kernel_radius, double sigma, double minthresh, double maxthresh)
{
	//temp image arrays for our sobel arrays;
	int _size = width*height*depth*1;

	//allocate some temp storage
	short* sobel_h = 0;
	sobel_h = new short [_size];
	short* sobel_v = 0;
	sobel_v = new short [_size];
	short* sobel_d = 0;
	sobel_d = new short [_size];
	
	unsigned char* non_maxima = 0;
	non_maxima = new unsigned char [_size*4];

	//apply a guassian to our image
	cout<<"Gausian3D..."<<endl;
	apply_gaussian3D(kernel_radius, sigma);
	result2source3D();

	//apply a 3D sobel horizontal edge detector
	cout<<endl<<"Sobel3D_H..."<<endl;
	apply_sobel3D(SOBEL_HORIZ);
	sobel_result2newarray3D(sobel_h);

	//apply a 3D sobel vertical edge detector
	cout<<endl<<"Sobel3D_V..."<<endl;
	apply_sobel3D(SOBEL_VERTI);
	sobel_result2newarray3D(sobel_v);

	//apply a 3D sobel vertical edge detector
	cout<<endl<<"Sobel3D_D..."<<endl;
	apply_sobel3D(SOBEL_DEPTH);
	sobel_result2newarray3D(sobel_d);

	//delete our sobel_result_array
	if(sobel_result_array) delete[] sobel_result_array;

	//apply a non maxima suppression
	cout<<endl<<"Non Maxima Suppression..."<<endl;
	nonMaximaSuppression3D(sobel_v, sobel_h, sobel_d);
	result2newarray3D(non_maxima);
 
	//delete our sobel v,h,d arrays
	if(sobel_v)		delete[] sobel_v;
	if(sobel_h)		delete[] sobel_h;
	if(sobel_d)		delete[] sobel_d;

	//apply histeresis
	cout<<endl<<"Histeresis..."<<endl;
	histeresis3D(non_maxima, minthresh, maxthresh);

	//delete our temp storage
	if(non_maxima)	delete[] non_maxima;
}

void Filter::nonMaximaSuppression3D(short* sobel_v, short* sobel_h, short* sobel_d)
{
	allocate_result3D();

	for(int k=1; k<depth-1; k++)
	{
		progress(k, depth-1);

		for(int i=1; i<width-1; i++)
		{
			for(int j=1; j<height-1; j++)
			{
				nMS3D(sobel_v, sobel_h, sobel_d, i, j, k, 0);
				nMS3D(sobel_v, sobel_h, sobel_d, i, j, k, 1);
				nMS3D(sobel_v, sobel_h, sobel_d, i, j, k, 2);
			}
		}
	}
}

void Filter::nMS3D(short* sobel_v, short* sobel_h, short* sobel_d, int x, int y, int z, int channel)
{
	double prevPixel;
	double nextPixel;
	double theta;
	double sum;
	double sumX;
	double sumY;
	double sumZ;

	double alpha;

	int index = ((((z*height)+y)*width)+x);
	
	sumX = sobel_h[index];
	sumY = sobel_v[index];
	sumZ = sobel_d[index];

	//=================================
	//gradient magnitude approximation
	//=================================
	sum = abs(sumX) + abs(sumY) + abs(sumZ);

	//=================================
	//magnitude orientation
	//=================================
	theta = atan2(sumY, sumX) * (180/PI);;
	alpha = atan2(sumZ, sqrt(sumX*sumX+sumY*sumY)) * (180/PI);;

	//====================================================
	// Find edgeDirection by assigning theta a value of
	// either 0, 45, 90 or 135 degrees, depending on which
	// value theta is closest to
	//====================================================
	if		   (theta > -22.5 && theta <=  22.5)	theta = 0;
	else if	   (theta >  22.5 && theta <=  67.5)	theta = 45;
	else if	   (theta >  67.5 && theta <= 112.5)	theta = 90;
	else if	   (theta > 112.5 && theta <= 157.5)	theta = 135;
	else if	   (theta > 157.5 || theta <=-157.5)	theta = -180;
	else if	   (theta >-157.5 && theta <=-112.5)	theta = -135;
	else if	   (theta >-112.5 && theta <= -67.5)	theta = -90;
	else if	   (theta > -67.5 && theta <= -22.5)	theta = -45;
	
	if		   (alpha > -22.5 && alpha <=  22.5)	alpha = 0;
	else if	   (alpha >  22.5 && alpha <=  67.5)	alpha = 45;
	else if	   (alpha >  67.5 && alpha <= 112.5)	alpha = 90;
	else if	   (alpha > 112.5 && alpha <= 157.5)	alpha = 135;
	else if	   (alpha > 157.5 || alpha <=-157.5)	alpha = -180;
	else if	   (alpha >-157.5 && alpha <=-112.5)	alpha = -135;
	else if	   (alpha >-112.5 && alpha <= -67.5)	alpha = -90;
	else if	   (alpha > -67.5 && alpha <= -22.5)	alpha = -45;

	int MPP=1; int ZPP=1; int PPP=1; 
	int MZP=1; int ZZP=1; int PZP=1;
	int MMP=1; int ZMP=1; int PMP=1;

	int MPZ=1; int ZPZ=1; int PPZ=1;
	int MZZ=1; int ZZZ=1; int PZZ=1;
	int MMZ=1; int ZMZ=1; int PMZ=1;

	int MPM=1; int ZPM=1; int PPM=1;
	int MZM=1; int ZZM=1; int PZM=1;
	int MMM=1; int ZMM=1; int PMM=1;

	if(alpha==-90 && ZZP && ZZM)
	{
		//ZZP
		index = (((((z+1)*height)+(y))*width)+(x));
		prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

		//ZZM
		index = (((((z-1)*height)+(y))*width)+(x));
		nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
	}
	else if(alpha==90 && ZZM && ZZP)
	{
		//ZZM
		index = (((((z-1)*height)+(y))*width)+(x));
		prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
		
		//ZZP
		index = (((((z+1)*height)+(y))*width)+(x));
		nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
	}
	else if(theta == 0)   
	{
		if(alpha == 45 && MZP && PZM)   
		{
			//PZM
			index = (((((z-1)*height)+(y))*width)+(x+1));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

			//MZP
			index = (((((z+1)*height)+(y))*width)+(x-1));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
		}
		else if(alpha == 0 && PZZ && MZZ)   
		{
			//PZZ
			index = (((((z)*height)+(y))*width)+(x+1));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

			//MZZ
			index = (((((z)*height)+(y))*width)+(x-1));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
		}
		else if(alpha == -45 && PZP && MZM)   
		{
			//PZP
			index = (((((z+1)*height)+(y))*width)+(x+1));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

			//MZM
			index = (((((z-1)*height)+(y))*width)+(x-1));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
		}
	}
	else if(theta == 45)   
	{
		if(alpha == 45 && PPM && MMP)   
		{
			//PPM
			index = (((((z-1)*height)+(y+1))*width)+(x+1));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
			
			//MMP
			index = (((((z+1)*height)+(y-1))*width)+(x-1));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
		}
		else if(alpha == 0 && PPZ && MMZ)   
		{
			//PPZ
			index = (((((z)*height)+(y+1))*width)+(x+1));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

			//MMZ
			index = (((((z)*height)+(y-1))*width)+(x-1));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
		}
		else if(alpha == -45 && PPP && MMM)   
		{
			//PPP
			index = (((((z+1)*height)+(y+1))*width)+(x+1));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

			//MMM
			index = (((((z-1)*height)+(y-1))*width)+(x-1));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

		}
	}
	else if(theta == 90)   
	{
		if(alpha == 45 && ZPM && ZMP)   
		{
			//ZPM
			index = (((((z-1)*height)+(y+1))*width)+(x));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

			//ZMP
			index = (((((z+1)*height)+(y-1))*width)+(x));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
		}
		else if(alpha == 0 && ZPZ && ZMZ)   
		{
			//ZPZ
			index = (((((z)*height)+(y+1))*width)+(x));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

			//ZMZ
			index = (((((z)*height)+(y-1))*width)+(x));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
		}
		else if(alpha == -45 && ZPP && ZMM)   
		{
			//ZPP
			index = (((((z+1)*height)+(y+1))*width)+(x));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

			//ZMM
			index = (((((z-1)*height)+(y-1))*width)+(x));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
		}
	}
	else if(theta == 135)   
	{
		if(alpha == 45 && MPM && PMP)   
		{
			//MPM
			index = (((((z-1)*height)+(y+1))*width)+(x-1));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

			//PMP
			index = (((((z+1)*height)+(y-1))*width)+(x+1));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
		}
		else if(alpha == 0 && MPZ && PMZ)   
		{
			//MPZ
			index = (((((z)*height)+(y+1))*width)+(x-1));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

			//PMZ
			index = (((((z)*height)+(y-1))*width)+(x+1));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
		}
		else if(alpha == -45 && MPP && PMM)   
		{
			//MPP
			index = (((((z+1)*height)+(y+1))*width)+(x-1));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

			//PMM
			index = (((((z-1)*height)+(y-1))*width)+(x+1));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
		}
	}
	else if(theta == -180)   
	{
		if(alpha == 45 && MZM && PZP)   
		{
			//MZM
			index = (((((z-1)*height)+(y))*width)+(x-1));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

			//PZP
			index = (((((z+1)*height)+(y))*width)+(x+1));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
		}
		else if(alpha == 0 && MZZ && PZZ)   
		{
			//MZZ
			index = (((((z)*height)+(y))*width)+(x-1));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

			//PZZ
			index = (((((z)*height)+(y))*width)+(x+1));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
		}
		else if(alpha == -45 && MZP && PZM)   
		{
			//MZP
			index = (((((z+1)*height)+(y))*width)+(x-1));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

			//PZM
			index = (((((z-1)*height)+(y))*width)+(x+1));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
		}
	}
	else if(theta == -135)   
	{
		if(alpha == 45 && MMM && PPP)   
		{
			//MMM
			index = (((((z-1)*height)+(y-1))*width)+(x-1));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

			//PPP
			index = (((((z+1)*height)+(y+1))*width)+(x+1));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
		}
		else if(alpha == 0 && MMZ && PPZ)   
		{
			//MMZ
			index = (((((z)*height)+(y-1))*width)+(x-1));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

			//PPZ
			index = (((((z)*height)+(y+1))*width)+(x+1));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

		}
		else if(alpha == -45 && MMP && PPM)   
		{
			//MMP
			index = (((((z+1)*height)+(y-1))*width)+(x-1));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

			//PPM
			index = (((((z-1)*height)+(y+1))*width)+(x+1));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
		}
	}
	else if(theta == -90)   
	{
		if(alpha == 45 && ZMM && ZPP)   
		{
			//ZMM
			index = (((((z-1)*height)+(y-1))*width)+(x));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

			//ZPP
			index = (((((z+1)*height)+(y+1))*width)+(x));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
		}
		else if(alpha == 0 && ZMZ && ZPM)   
		{
			//ZMZ
			index = (((((z)*height)+(y-1))*width)+(x));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

			//ZPM
			index = (((((z)*height)+(y+1))*width)+(x));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
		}
		else if(alpha == -45 && ZMP && ZPM)   
		{
			//ZMP
			index = (((((z+1)*height)+(y-1))*width)+(x));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

			//ZPM
			index = (((((z-1)*height)+(y+1))*width)+(x));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

		}
	}
	else if(theta == -45)   
	{
		if(alpha == 45 && PMM && MPP)   
		{
			//PMM
			index = (((((z-1)*height)+(y-1))*width)+(x+1));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

			//MPP
			index = (((((z+1)*height)+(y+1))*width)+(x-1));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
		}
		else if(alpha == 0 && PMZ && MPZ)   
		{
			//PMZ
			index = (((((z)*height)+(y-1))*width)+(x+1));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

			//MPZ
			index = (((((z)*height)+(y+1))*width)+(x-1));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);

		}
		else if(alpha == -45 && PMP && MPM)   
		{
			//PMP
			index = (((((z+1)*height)+(y-1))*width)+(x+1));
			prevPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
			
			//MPM
			index = (((((z-1)*height)+(y+1))*width)+(x-1));
			nextPixel  = abs(sobel_h[index]) + abs(sobel_v[index]) + abs(sobel_d[index]);
		}
	}
	else
	{
		prevPixel = nextPixel = 0;
	}

	index = (((((z)*height)+(y))*width)+(x))*4 + channel;
	
	if(sum<prevPixel || sum<nextPixel)
	{
		result_array[index+0] = 0;
		result_array[index+1] = 0;
		result_array[index+2] = 0;
		result_array[index+3] = 0;
	}
	else
	{
		if(sum<0)	sum = 0;
		if(sum>255) sum = 255;

		result_array[index+0] = sum;
		result_array[index+1] = sum;
		result_array[index+2] = sum;
		result_array[index+3] = sum;
	}
}

vector<Vector> Filter::find_neighbours3D(int x, int y, int z, int channel)
{
	int index=0;

	vector<Vector> temp;
	Vector tempV;

	int mask[27];
	int iter = 0;

	mask[0] = 1; mask[1] = 1; mask[2] = 1;
	mask[3] = 1; mask[4] = 1; mask[5] = 1;
	mask[6] = 1; mask[7] = 1; mask[8] = 1;

	mask[9]  = 1; mask[10] = 1; mask[11] = 1;
	mask[12] = 1; mask[13] = 1; mask[14] = 1;
	mask[15] = 1; mask[16] = 1; mask[17] = 1;

	mask[18] = 1; mask[19] = 1; mask[20] = 1;
	mask[21] = 1; mask[22] = 1; mask[23] = 1;
	mask[24] = 1; mask[25] = 1; mask[26] = 1;

	for(int i=-1; i<=1; i++)
	{
		for(int j=-1; j<=1; j++)
		{
			for(int k=-1; k<=1; k++)
			{
				if(mask[iter]==1)
				{
					if(i==0 && j==0 && k==0)
					{
						//skip centre pixel
					}
					else
					{
						if((x+i)>=0 && (y+j)>=0 && (z+k)>=0 && (x+i)<width && (y+j)<height && (z+k)<depth)
						{
							tempV.x = (x+i);
							tempV.y = (y+j);
							tempV.z = (z+k);

							tempV.w = (((((z+k)*height)+(y+j))*width)+(x+i))*4 + channel;

							temp.push_back(tempV);
						}
					}
				}

				iter++;
			}
		}
	}
	
	//cout<<temp.size()<<endl;


	return temp;
}

void Filter::histeresis3D(unsigned char* non_maxima_sup, double minthresh, double maxthresh)
{
	allocate_result3D();

	vector<Vector> stack;
	vector<Vector> neighbours;

	int* visitedpixels =  new int [width*height*depth];
	for(int i=0; i<width*height*depth; i++) visitedpixels[i]=-1;

	int index;
	Vector current_index;
	Vector current_neighbour_index;
	Vector col;

	for(int z=0; z<depth; z++)
	{
		progress(z, depth);

		for(int x=0; x<width; x++)
		{
			for(int y=0; y<height; y++)
			{
				index = ((((z*height)+y)*width)+x)*4+0;

				//if pixel is not visited
				if(visitedpixels[index/4]==-1)
				{
					//if it is below the min thresh
					if(non_maxima_sup[index] < minthresh )
					{
						// safely mark it as a non edge

						//mark visited
						visitedpixels[index/4]=1;
						
						//non-edge
						result_array[index]	  = 0;
						result_array[index+1] = 0;
						result_array[index+2] = 0;
					}
					else if( non_maxima_sup[index] >= maxthresh )
					{
						//mark visited
						visitedpixels[index/4]=1;

						//col.x = (float) rand()/RAND_MAX*255;
						//col.y = (float) rand()/RAND_MAX*255;
						//col.z = (float) rand()/RAND_MAX*255;
						//col.w = 255;

						col = 255;

						//edge
						result_array[index]   = col.x;
						result_array[index+1] = col.y;
						result_array[index+2] = col.z;
						
						//add current pixel to stack
						current_index.x = x;
						current_index.y = y;
						current_index.z = z;
						current_index.w= index;

						stack.push_back(current_index);

						//while stack is not empty
						while(!stack.empty())
						{
							//pop it off
							current_index = stack[stack.size()-1];
							stack.pop_back();
							
							//is it within the min thresh
							if(non_maxima_sup[(int)current_index.w] >=  minthresh)
							{
								//mark it as visited
								visitedpixels[(int)current_index.w/4] = 1;

								//edge
								result_array[(int)current_index.w]   = col.x;
								result_array[(int)current_index.w+1] = col.y;
								result_array[(int)current_index.w+2] = col.z;

								//find its neighbours
								neighbours = find_neighbours3D(current_index.x, current_index.y, current_index.z, 0);

								while(!neighbours.empty())
								{
									//pop it off
									current_neighbour_index = neighbours[neighbours.size()-1];
									neighbours.pop_back();
									
									if( visitedpixels[(int)current_neighbour_index.w/4] == -1 )
									{
										//mark it as visited
										visitedpixels[(int)current_neighbour_index.w/4] = 1;

										//add to stack
										stack.push_back(current_neighbour_index);
									}
								} //end while neighbours not empty
							}
							else
							{
								//non edge
								result_array[index]   = 0;
								result_array[index+1] = 0;
								result_array[index+2] = 0;

								//mark it as visited
								visitedpixels[index/4] = 1;
							}
						} //end while stack is not empty

					}
				}
			}
		}
	}
}
*/
