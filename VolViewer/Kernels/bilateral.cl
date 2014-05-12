// *********************************************************************
// OpenCL Kernel Function for 3D Bi-lateral filter
// *********************************************************************
__kernel void bilateral( __global unsigned char* src_buffer, __global unsigned char* dst_buffer, int sigma_spatial, float sigma_range, const int width, const int height, const int depth)
{
	// get index into global data array
	int3 index = (int3)(get_global_id(0),get_global_id(1),get_global_id(2));
	int3 dim = (int3)(width,height,depth);
	int3 ipos;
	
	int index1d = ((((index.z*dim.y)+(index.y))*dim.x)+(index.x));
	float vox_value = src_buffer[index1d]/255.0f;

	float two_sigma_range_squared = 2.0f*(sigma_range*sigma_range);
	float two_sigma_spatial_squared = 2.0f*(sigma_spatial*sigma_spatial);
	
	float range;
	float domain;
	float domain_mult_range;
	float norm = 0.0;
	float filtered_value=0.0;
	
	//iterate over the neighbourhood
	int i,j,k;
	for (i=-sigma_spatial;i<=sigma_spatial;i++)
	{
		for (j=-sigma_spatial;j<=sigma_spatial;j++)
		{ 
			for (k=-sigma_spatial;k<=sigma_spatial;k++)
			{
				ipos.x = index.x+i;
				ipos.y = index.y+j;
				ipos.z = index.z+k;
				
				if(ipos.x<0) ipos.x=0;
				if(ipos.y<0) ipos.y=0;
				if(ipos.z<0) ipos.z=0;
				
				if(ipos.x>dim.x-1) ipos.x=dim.x-1;
				if(ipos.y>dim.y-1) ipos.y=dim.y-1;
				if(ipos.z>dim.z-1) ipos.z=dim.z-1;
				
				index1d = ((((ipos.z*dim.y)+(ipos.y))*dim.x)+(ipos.x));
				float vox_value_i = src_buffer[index1d]/255.0f;
				
				range = -((vox_value_i-vox_value)*(vox_value_i-vox_value))/(two_sigma_range_squared);
				range = exp(range);
				
				domain = -((i*i)+(j*j)+(k*k)) / (two_sigma_spatial_squared);
				domain = exp(domain);

				domain_mult_range = domain*range;
				filtered_value += (float)(vox_value_i)*domain_mult_range;
				norm += domain_mult_range;
			}	
		}
	}
	
	//save results to our output buffer
	index1d = ((((index.z*dim.y)+(index.y))*dim.x)+(index.x));
	dst_buffer[index1d] = 255.0f*(filtered_value/norm);;	
}