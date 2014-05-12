// *********************************************************************
// OpenCL Kernel Function for data invert
// *********************************************************************
float computeDiffusion(float v, float lambda2)
{
	//The function (f1) privileges wider regions over smaller ones, 
	//while the function (f2) privileges high-contrast edges over low-contrast ones.
	
	//f(1) 
	//return ( 1.0 / ( 1.0 + (v/lambda)*(v/lambda)));

	//f(2) 
	//return ( exp(-(x/lambda)*(x/lambda)) );

	//speedup f(1)
	//simplification of f(1) = f(1) * (lambda^2*lambda^2)/(lambda^2*lambda^2)
	//f(1) = lambda^2/(lambda^2+(v*v))
	//lamda2 = lambda^2
	
	return (lambda2)/((lambda2) + (v*v));
}
float compute3d_6connect(__global unsigned char* src_buffer, const int width,const int height,const int depth, int x, int y, int z, float lambda2)
{
	int index = ((((z*height)+y)*width)+x);	
    float current = src_buffer[index]; 
	
	float dx = 1.0f;
	float dy = 1.0f;
	float dz = 1.0f;
	
    int px = x-1;
    int nx = x+1;
    int py = y-1;
    int ny = y+1;
    int pz = z-1;
    int nz = z+1;

    if (px<0) px=0;
    if (nx>width-1) nx=width-1;
    if (py<0) py=0;
    if (ny>height-1) ny=height-1;
    if (pz<0) pz=0;
    if (nz>depth-1) nz=depth-1;

	index = ((((z*height)+(y))*width)+(px));	
    float ixp = src_buffer[index];
    
	index = ((((z*height)+(y))*width)+(nx));	
	float ixn = src_buffer[index];
    
	index = ((((z*height)+(py))*width)+(x));	
	float iyp = src_buffer[index];
    
	index = ((((z*height)+(ny))*width)+(x));	
	float iyn = src_buffer[index];
 
	index = ((((pz*height)+(y))*width)+(x));	
	float izp = src_buffer[index];

	index = ((((nz*height)+(y))*width)+(x));	
	float izn = src_buffer[index];

	float diffxn = computeDiffusion(current - ixn, lambda2);
    float diffxp = computeDiffusion(current - ixp, lambda2);
    float diffyn = computeDiffusion(current - iyn, lambda2);
    float diffyp = computeDiffusion(current - iyp, lambda2);
    float diffzn = computeDiffusion(current - izn, lambda2);
    float diffzp = computeDiffusion(current - izp, lambda2);
 
    float delta =   (  ((1.0f/pow(dx,2.0f)) * (diffxn * (ixn - current)) + ((1.0f/pow(dx,2.0f)) * diffxp * (ixp - current))) ) 
                  + ( (((1.0f/pow(dy,2.0f)) * diffyp * (iyp - current)) + ((1.0f/pow(dy,2.0f)) * diffyn * (iyn - current))) )
				  + ( (((1.0f/pow(dz,2.0f)) * diffzp * (izp - current)) + ((1.0f/pow(dz,2.0f)) * diffzn * (izn - current))) );
 
    return delta;
}
__kernel void myFunc( __global unsigned char* src_buffer, __global unsigned char* dst_buffer,const float lambda2, float dT, const int width,const int height,const int depth)
{
	// get index into global data array
	int3 index = (int3)(get_global_id(0),get_global_id(1),get_global_id(2));
	int3 dim = (int3)(width,height,depth);

	int index1d = ((((index.z*dim.y)+(index.y))*dim.x)+(index.x));
	float current = src_buffer[index1d];
	float value;

	//dont waste time on black voxels...
	if(current==0)
	{
		value = current;
	}
	else
	{
		value = dT * ( compute3d_6connect(src_buffer, width, height, depth, index.x, index.y, index.z, lambda2)) + current;

		if(value>255) value=255;
		if(value<0) value = 0;
	}

	//save results to our output buffer
	dst_buffer[index1d] = value;	
}
