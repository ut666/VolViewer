// *********************************************************************
// OpenCL Kernel Function for a 3D median filter
// *********************************************************************
void sort(float* values, int length)
{
    for (int i=0;i<length;i++)
	{
		for (int j=0;j<length;j++)
		{
			if (values[i]>values[j])     
			{
				float v = values[i];
				values[i] = values[j];
				values[j] = v;
			}
		}
	}
}  
__kernel void myFunc(read_only image3d_t src_img, __global unsigned char* dst_buffer, sampler_t smp)
{
	// get index into global data array
	int4 index = (int4)(get_global_id(0),get_global_id(1),get_global_id(2),0.0);
	int4 dim = get_image_dim(src_img);
	
	//helpers
    int4 ipos;
    int i,j,k;
    int acum = 0;
	
	//filter window
    float sample[9];
    const int window_size = 1;

	//iterate over the neighbourhood
    for (i=-window_size;i<=window_size;i++)
	{
		for (j=-window_size;j<=window_size;j++)
		{ 
			for (k=-window_size;k<=window_size;k++)
			{
				ipos.x = index.x+i;
				ipos.y = index.y+j;
				ipos.z = index.z+k;
				
				//keep the values of the voxel
				sample[acum]= read_imagef(src_img, smp, ipos).x;
				acum ++;
			}
		}
	}

	//apply a bubble sort
    sort(sample, acum);

	//save results to our output buffer
	int index1d = ((((index.z*dim.y)+(index.y))*dim.x)+(index.x));
	dst_buffer[index1d] = sample[4]*255.0f;
}
