// *********************************************************************
// OpenCL Kernel Function for data invert
// *********************************************************************
__kernel void gaussianX( __global unsigned char* src_buffer, __global unsigned char* dst_buffer,__global float* gaussiankern, const int gaussian_width, const int width,const int height,const int depth)
{
	// get index into global data array
	int3 index = (int3)(get_global_id(0),get_global_id(1),get_global_id(2));
	int3 dim = (int3)(width,height,depth);

	//helpers
    int4 ipos;
    int i;
	int	index1d;

	float gaussian_value;
	float voxel_value;
	
	ipos.y = index.y;
	ipos.z = index.z;

	float filtered_value=0.0;
	int maskiter=0;
    for (i=-gaussian_width;i<=gaussian_width;i++)
	{	
		ipos.x = index.x+i;
		if(ipos.x<0) ipos.x=0;
		if(ipos.x>dim.x-1) ipos.x=dim.x-1;	
		
		index1d = ((((ipos.z*dim.y)+(ipos.y))*dim.x)+(ipos.x));

		gaussian_value = gaussiankern[maskiter];
		voxel_value = src_buffer[index1d];
		filtered_value += (gaussian_value*voxel_value);	

		maskiter++;		
	}

	//save results to our output buffer
	index1d = ((((index.z*dim.y)+(index.y))*dim.x)+(index.x));
	dst_buffer[index1d] = filtered_value;	
}
__kernel void gaussianY( __global unsigned char* src_buffer, __global unsigned char* dst_buffer, __global float* gaussiankern, const int gaussian_width, const int width,const int height,const int depth)
{
	// get index into global data array
	int3 index = (int3)(get_global_id(0),get_global_id(1),get_global_id(2));
	int3 dim = (int3)(width,height,depth);

	//helpers
    int4 ipos;
    int i,j,k;
	int index1d;

	float gaussian_value;
	float voxel_value;
	float filtered_value=0.0;
	int maskiter = 0;
	
	ipos.x = index.x;
	ipos.z = index.z;
    for (i=-gaussian_width;i<=gaussian_width;i++)
	{
		ipos.y = index.y+i;
		if(ipos.y<0) ipos.y=0;
		if(ipos.y>height-1) ipos.y=height-1;	
		
		index1d = ((((ipos.z*dim.y)+(ipos.y))*dim.x)+(ipos.x));

		gaussian_value = gaussiankern[maskiter];
		voxel_value = src_buffer[index1d];
		filtered_value += gaussian_value*voxel_value;		
		
		maskiter++;		
	}
	
	//save results to our output buffer
	index1d = ((((index.z*dim.y)+(index.y))*dim.x)+(index.x));
	dst_buffer[index1d] = filtered_value;	
}
__kernel void gaussianZ( __global unsigned char* src_buffer, __global unsigned char* dst_buffer,__global float* gaussiankern, const int gaussian_width, const int width,const int height,const int depth)
{
	// get index into global data array
	int3 index = (int3)(get_global_id(0),get_global_id(1),get_global_id(2));
	int3 dim = (int3)(width,height,depth);

	//helpers
    int4 ipos;
    int i,j,k;
	int index1d;

	float gaussian_value;
	float voxel_value;
	float filtered_value=0.0;
	int maskiter = 0;
	
	ipos.x = index.x;
	ipos.y = index.y;
    for (i=-gaussian_width;i<=gaussian_width;i++)
	{
		ipos.z = index.z+i;
		if(ipos.z<0) ipos.z=0;
		if(ipos.z>depth-1) ipos.z=depth-1;
		
		index1d = ((((ipos.z*dim.y)+(ipos.y))*dim.x)+(ipos.x));
		
		gaussian_value = gaussiankern[maskiter];
		voxel_value = src_buffer[index1d];
		filtered_value += gaussian_value*voxel_value;		

		maskiter++;		
	}
	
	//save results to our output buffer
	index1d = ((((index.z*dim.y)+(index.y))*dim.x)+(index.x));
	dst_buffer[index1d] = filtered_value;	
}
