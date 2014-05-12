#include "PeronaMalikFlow.h"

PeronaMalikFlow::PeronaMalikFlow()
{
	lambda = 5.0;
	data=NULL;
}
PeronaMalikFlow::~PeronaMalikFlow()
{
}
void PeronaMalikFlow::init_data2d(int w, int h)
{
	width = w;
	height = h;

	if(data) delete[] data ;
	data = new unsigned char [width*height];
}
void PeronaMalikFlow::init_data3d(int w, int h, int d, unsigned char* imgdata)
{
	data = imgdata;
	width = w;
	height = h;
	depth = d;
}
void PeronaMalikFlow::set_dataValue(int index, float val)
{
	//printf("i: %d\n", (int)val);
	data[index] = (unsigned char) val;
}
float PeronaMalikFlow::computeDiffusion(float v)
{
	//The function (f1) privileges wider regions over smaller ones, 
	//while the function (f2) privileges high-contrast edges over low-contrast ones.
	
	//f(1) 
	return ( 1.0 / ( 1.0 + (v/lambda)*(v/lambda)));

	//f(2) 
	//return ( exp(-(x/lambda)*(x/lambda)) );

	//speedup f(1)
	//simplification of f(1) = f(1) * (lambda^2*lambda^2)/(lambda^2*lambda^2)
	//lamda2 = lambda^2
	//return (lambda2)/((lambda2) + (v*v));
}
float PeronaMalikFlow::compute2d_8connect(int x, int y)
{
	int index = x*height+y;
    float current = data[index];
	 
    int px = x-1;
    int nx = x+1;
    int py = y-1;
    int ny = y+1;
    if (px<0) px=0;
    if (nx>=width) nx=width-1;
    if (py<0) py=0;
    if (ny>=height) ny=height-1;
 
	index = px*height+y;
    float ixp = data[index];
    
	index = nx*height+y;
	float ixn = data[index];
    
	index = x*height+py;
	float iyp = data[index];
    
	index = x*height+ny;
	float iyn = data[index];

    float diffxn = computeDiffusion(current- ixn);
    float diffxp = computeDiffusion(current- ixp);
    float diffyn = computeDiffusion(current- iyn);
    float diffyp = computeDiffusion(current- iyp);
 
	index = nx*height+py;
    float iNE = data[index];
    
	index = px*height+ny;
	float iSW = data[index];

	index = px*height+py;
    float iNW = data[index];

	index = nx*height+ny;
    float iSE = data[index];

	float diffNE = computeDiffusion(current  - iNE);
    float diffSW = computeDiffusion(current- iSW);
    float diffNW = computeDiffusion(current- iNW);
    float diffSE = computeDiffusion(current- iSE);
  
    float delta =   (1.0/dx) * diffxn * (ixn - current)
                  + (1.0/dx) * diffxp * (ixp - current)
                  + (1.0/dy) * diffyp * (iyp - current)
                  + (1.0/dy) * diffyn * (iyn - current)
                  + (1.0/dd) * diffNE * (iNE- current)
				  + (1.0/dd) * diffSW * (iSW - current)
				  + (1.0/dd) * diffSE * (iSE - current)
				  + (1.0/dd) * diffNW * (iNW -current);
 
    return delta;
 
}
float PeronaMalikFlow::compute2d_4connect(int x, int y)
{
	int index = x*height+y;
    float current = data[index]; 

    int px = x-1;
    int nx = x+1;
    int py = y-1;
    int ny = y+1;

    if (px<0) px=0;
    if (nx>=width) nx=width-1;
    if (py<0) py=0;
    if (ny>=height) ny=height-1;
 
	index = px*height+y;
    float ixp = data[index];
    
	index = nx*height+y;
	float ixn = data[index];
    
	index = x*height+py;
	float iyp = data[index];
    
	index = x*height+ny;
	float iyn = data[index];
 
    float diffxn = computeDiffusion(current - ixn);
    float diffxp = computeDiffusion(current - ixp);
    float diffyn = computeDiffusion(current - iyn);
    float diffyp = computeDiffusion(current - iyp);
 
    float delta =   ( ((diffxn * (ixn - current)) + (diffxp * (ixp - current))))
                  + ( ((diffyp * (iyp - current)) + (diffyn * (iyn - current))));
 
    return delta;
}
float PeronaMalikFlow::compute3d_26connect(int x, int y, int z)
{
	int index = 3*((((z*height)+(y))*width)+(x));	
    float current = data[index+offset]; 

    int px = x-1;
    int nx = x+1;
    int py = y-1;
    int ny = y+1;
    int pz = z-1;
    int nz = z+1;

    if (px<0) px=0;
    if (nx>=width) nx=width-1;
    if (py<0) py=0;
    if (ny>=height) ny=height-1;
    if (pz<0) pz=0;
    if (nz>=depth) nz=depth-1;

	//======================================================
	//-1, 1, 1
	index = 3*((((pz*height)+(py))*width)+(nx));	
    float i_nx_py_pz = data[index+offset];
	// 0, 1, 1
	index = 3*((((pz*height)+(py))*width)+(x));	
    float i_x_py_pz = data[index+offset];
	// 1, 1, 1
	index = 3*((((pz*height)+(py))*width)+(px));	
    float i_px_py_pz = data[index+offset];

	//-1, 0, 1
	index = 3*((((pz*height)+(y))*width)+(nx));	
    float i_nx_y_pz = data[index+offset];
	// 0, 0, 1
	index = 3*((((pz*height)+(y))*width)+(x));	
    float i_x_y_pz = data[index+offset];
	// 1, 0, 1
	index = 3*((((pz*height)+(y))*width)+(px));	
    float i_px_y_pz = data[index+offset];

	//-1, -1, 1
	index = 3*((((pz*height)+(ny))*width)+(nx));	
    float i_nx_ny_pz = data[index+offset];
	// 0, -1, 1
	index = 3*((((pz*height)+(ny))*width)+(x));	
    float i_x_ny_pz = data[index+offset];
	// 1, -1, 1
	index = 3*((((pz*height)+(ny))*width)+(px));	
    float i_px_ny_pz = data[index+offset];

	//======================================================
	//-1, 1, 0
	index = 3*((((z*height)+(py))*width)+(nx));	
    float i_nx_py_z = data[index+offset];
	// 0, 1, 0
	index = 3*((((z*height)+(py))*width)+(x));	
    float i_x_py_z = data[index+offset];
	// 1, 1, 0
	index = 3*((((z*height)+(py))*width)+(px));	
    float i_px_py_z = data[index+offset];

	//-1, 0, 0
	index = 3*((((z*height)+(y))*width)+(nx));	
    float i_nx_y_z = data[index+offset];

	// 0, 0, 0
	//CURRENT

	// 1, 0, 0
	index = 3*((((z*height)+(y))*width)+(px));	
    float i_px_y_z = data[index+offset];

	//-1, -1, 0
	index = 3*((((z*height)+(ny))*width)+(nx));	
    float i_nx_ny_z = data[index+offset];
	// 0, -1, 0
	index = 3*((((z*height)+(ny))*width)+(x));	
    float i_x_ny_z = data[index+offset];
	// 1, -1, 0
	index = 3*((((z*height)+(ny))*width)+(px));	
    float i_px_ny_z = data[index+offset];

	//======================================================
	//-1, 1, -1
	index = 3*((((nz*height)+(py))*width)+(nx));	
    float i_nx_py_nz = data[index+offset];
	// 0, 1, -1
	index = 3*((((nz*height)+(py))*width)+(x));	
    float i_x_py_nz = data[index+offset];
	// 1, 1, -1
	index = 3*((((nz*height)+(py))*width)+(px));	
    float i_px_py_nz = data[index+offset];

	//-1, 0, -1
	index = 3*((((nz*height)+(y))*width)+(nx));	
    float i_nx_y_nz = data[index+offset];
	// 0, 0, -1
	index = 3*((((nz*height)+(y))*width)+(x));	
    float i_x_y_nz = data[index+offset];
	// 1, 0, -1
	index = 3*((((nz*height)+(y))*width)+(px));	
    float i_px_y_nz = data[index+offset];

	//-1, -1, -1
	index = 3*((((nz*height)+(ny))*width)+(nx));	
    float i_nx_ny_nz = data[index+offset];
	// 0, -1, -1
	index = 3*((((nz*height)+(ny))*width)+(x));	
    float i_x_ny_nz = data[index+offset];
	// 1, -1, -1
	index = 3*((((nz*height)+(ny))*width)+(px));	
    float i_px_ny_nz = data[index+offset];

	float diff_nx_py_pz		= computeDiffusion(current - i_nx_py_pz);
    float diff_x_py_pz		= computeDiffusion(current - i_x_py_pz);
    float diff_px_py_pz		= computeDiffusion(current - i_px_py_pz);
    float diff_nx_y_pz		= computeDiffusion(current - i_nx_y_pz);
    float diff_x_y_pz		= computeDiffusion(current - i_x_y_pz);
    float diff_px_y_pz		= computeDiffusion(current - i_px_y_pz);
    float diff_nx_ny_pz		= computeDiffusion(current - i_nx_ny_pz);
    float diff_x_ny_pz		= computeDiffusion(current - i_x_ny_pz);
    float diff_px_ny_pz		= computeDiffusion(current - i_px_ny_pz);

	float diff_nx_py_z		= computeDiffusion(current - i_nx_py_z);
    float diff_x_py_z		= computeDiffusion(current - i_x_py_z);
    float diff_px_py_z		= computeDiffusion(current - i_px_py_z);
    float diff_nx_y_z		= computeDiffusion(current - i_nx_y_z);
//    float diff_x_y_z		= computeDiffusion(current - i_x_y_z);
    float diff_px_y_z		= computeDiffusion(current - i_px_y_z);
    float diff_nx_ny_z		= computeDiffusion(current - i_nx_ny_z);
    float diff_x_ny_z		= computeDiffusion(current - i_x_ny_z);
    float diff_px_ny_z		= computeDiffusion(current - i_px_ny_z);

	float diff_nx_py_nz		= computeDiffusion(current - i_nx_py_nz);
    float diff_x_py_nz		= computeDiffusion(current - i_x_py_nz);
    float diff_px_py_nz		= computeDiffusion(current - i_px_py_nz);
    float diff_nx_y_nz		= computeDiffusion(current - i_nx_y_nz);
    float diff_x_y_nz		= computeDiffusion(current - i_x_y_nz);
    float diff_px_y_nz		= computeDiffusion(current - i_px_y_nz);
    float diff_nx_ny_nz		= computeDiffusion(current - i_nx_ny_nz);
    float diff_x_ny_nz		= computeDiffusion(current - i_x_ny_nz);
    float diff_px_ny_nz		= computeDiffusion(current - i_px_ny_nz);

 
    float delta =   (  ((1.0/pow(dx,2.0)) * (diff_nx_y_z * (i_nx_y_z - current)) + ((1.0/pow(dx,2.0)) * diff_px_y_z * (i_px_y_z - current))) ) 
                  + ( (((1.0/pow(dy,2.0)) * diff_x_ny_z * (i_x_ny_z - current)) + ((1.0/pow(dy,2.0)) * diff_x_py_z * (i_x_py_z - current))) )
				  + ( (((1.0/pow(dz,2.0)) * diff_x_y_nz * (i_x_y_nz - current)) + ((1.0/pow(dz,2.0)) * diff_x_y_pz * (i_x_y_pz - current))) )
				  
				  + ( (((1.0/pow(dc,2.0)) * diff_nx_ny_nz * (i_nx_ny_nz - current)) + ((1.0/pow(dc,2.0)) * diff_px_ny_nz * (i_px_ny_nz - current))) )
				  + ( (((1.0/pow(dc,2.0)) * diff_px_py_nz * (i_px_py_nz - current)) + ((1.0/pow(dc,2.0)) * diff_nx_py_nz * (i_nx_py_nz - current))) )
				  + ( (((1.0/pow(dc,2.0)) * diff_nx_ny_pz * (i_nx_ny_pz - current)) + ((1.0/pow(dc,2.0)) * diff_px_ny_pz * (i_px_ny_pz - current))) )
				  + ( (((1.0/pow(dc,2.0)) * diff_px_py_pz * (i_px_py_pz - current)) + ((1.0/pow(dc,2.0)) * diff_nx_py_pz * (i_nx_py_pz - current))) )
			  

				  + ( (((1.0/pow(dh,2.0)) * diff_x_py_nz * (i_x_py_nz - current)) + ((1.0/pow(dh,2.0)) * diff_x_ny_nz * (i_x_ny_nz - current))) )
				  + ( (((1.0/pow(dh,2.0)) * diff_px_y_nz * (i_px_y_nz - current)) + ((1.0/pow(dh,2.0)) * diff_nx_y_nz * (i_nx_y_nz - current))) )

				  + ( (((1.0/pow(dh,2.0)) * diff_x_py_pz * (i_x_py_pz - current)) + ((1.0/pow(dh,2.0)) * diff_x_ny_pz * (i_x_ny_pz - current))) )
				  + ( (((1.0/pow(dh,2.0)) * diff_px_y_pz * (i_px_y_pz - current)) + ((1.0/pow(dh,2.0)) * diff_nx_y_pz * (i_nx_y_pz - current))) )

				  + ( (((1.0/pow(dd,2.0)) * diff_nx_py_z * (i_nx_py_z - current)) + ((1.0/pow(dd,2.0)) * diff_px_py_z * (i_px_py_z - current))) )
				  + ( (((1.0/pow(dd,2.0)) * diff_nx_ny_z * (i_nx_ny_z - current)) + ((1.0/pow(dd,2.0)) * diff_px_ny_z * (i_px_ny_z - current))) );
 
	return delta;
}
float PeronaMalikFlow::compute3d_6connect(int x, int y, int z)
{
	int index = 3*((((z*height)+(y))*width)+(x));	
    float current = data[index+offset]; 
	
    int px = x-1;
    int nx = x+1;
    int py = y-1;
    int ny = y+1;
    int pz = z-1;
    int nz = z+1;

    if (px<0) px=0;
    if (nx>=width) nx=width-1;
    if (py<0) py=0;
    if (ny>=height) ny=height-1;
    if (pz<0) pz=0;
    if (nz>=depth) nz=depth-1;

	index = 3*((((z*height)+(y))*width)+(px));	
    float ixp = data[index+offset];
    
	index = 3*((((z*height)+(y))*width)+(nx));	
	float ixn = data[index+offset];
    
	index = 3*((((z*height)+(py))*width)+(x));	
	float iyp = data[index+offset];
    
	index = 3*((((z*height)+(ny))*width)+(x));	
	float iyn = data[index+offset];
 
	index = 3*((((pz*height)+(y))*width)+(x));	
	float izp = data[index+offset];

	index = 3*((((nz*height)+(y))*width)+(x));	
	float izn = data[index+offset];

	float diffxn = computeDiffusion(current - ixn);
    float diffxp = computeDiffusion(current - ixp);
    float diffyn = computeDiffusion(current - iyn);
    float diffyp = computeDiffusion(current - iyp);
    float diffzn = computeDiffusion(current - izn);
    float diffzp = computeDiffusion(current - izp);
 
    float delta =   (  ((1.0/pow(dx,2.0)) * (diffxn * (ixn - current)) + ((1.0/pow(dx,2.0)) * diffxp * (ixp - current))) ) 
                  + ( (((1.0/pow(dy,2.0)) * diffyp * (iyp - current)) + ((1.0/pow(dy,2.0)) * diffyn * (iyn - current))) )
				  + ( (((1.0/pow(dz,2.0)) * diffzp * (izp - current)) + ((1.0/pow(dz,2.0)) * diffzn * (izn - current))) );
 
    return delta;
}

void PeronaMalikFlow::peronaMalikOperator3d(float dt, int nbiter, int chan, int dim)
{
	offset = chan;
	lambda2 = lambda*lambda;

	unsigned char* tempdataout = new unsigned char[width*height*depth];

	for(int i=0; i<width*height*depth; i++)
		tempdataout[i] = data[3*i+chan];

	int n;
	int i,j, k;
	int index;

	//voxel spacing
	float spacing_x,spacing_y,spacing_z;
	spacing_x = 1.0;
	spacing_y = 1.0;
	spacing_z = 1.0;

	dx = spacing_x;
	dy = spacing_y;
	dz = spacing_z/spacing_x;
	dd = sqrt( pow(dx,2.0) + pow(dy,2.0));
	dh = sqrt( pow(dx,2.0) + pow(dz,2.0));
	dc = sqrt( pow(dd,2.0) + pow(dz,2.0));

     QProgressDialog progress("Applying Filter...", "Cancel", 0, nbiter*depth, 0);
     progress.setWindowModality(Qt::WindowModal);
	 progress.setMinimumDuration(100);

	for(n=0; n<nbiter;n++)
	{
		for(k=0;k<depth;k++)
		{
			progress.setValue(n*depth+k);
			if (progress.wasCanceled()) break;

			for(j=0;j<height;j++)
			{
				for(i=0;i<width;i++)
				{
					index = ((((k*height)+(j))*width)+(i));	
					float current = data[3*index+chan];

					if(current!=0)
					{
						//diffusion computation
						float value = dt * ( compute3d_6connect(i, j, k)) + current;
						//float value = dt * ( compute3d_26connect(i, j, k)) + current;
					
						if(value>255) value=255;
						if(value<0) value = 0;
						current = value;
					}

					tempdataout[index] = (unsigned char) current;
				}	
			}
		}

		for(int ii=0; ii<width*height*depth; ii++)
			data[3*ii+chan] = tempdataout[ii];
	}

	progress.setValue(nbiter*depth);
	delete tempdataout;
}
void PeronaMalikFlow::peronaMalikOperator2d(float dt, int nbiter)
{
	unsigned char* tempdataout = new unsigned char[width*height];

	for(int i=0; i<width*height; i++)
		tempdataout[i] = data[i];

	int n;
	int i,j;
	int index;

	//voxel spacing
	float spacing_x,spacing_y,spacing_z;
	spacing_x = 1.0;
	spacing_y = 1.0;
	spacing_z = 1.0;

	dx = spacing_x;
	dy = spacing_y;
	dd = sqrt( pow(dx,2.0) + pow(dy,2.0));

	for(n=0; n<nbiter;n++)
	{
		for(j=0;j<height;j++)
		{
			for(i=0;i<width;i++)
			{
				index = i*height+j;
				float current = data[index];

				//diffusion computation
				float value = dt * ( compute2d_8connect(i, j))	+ current;
				if(value>255) value=255;
				if(value<0) value = 0;

				tempdataout[index] = (unsigned char) value;
			}	
		}

		for(int ii=0; ii<width*height; ii++)
			data[ii] = tempdataout[ii];
	}

	delete tempdataout;
}