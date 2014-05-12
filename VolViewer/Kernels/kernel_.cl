// *********************************************************************
// OpenCL Kernel Function for data invert
// *********************************************************************
__kernel void myFunc( __global unsigned char* src_buffer, __global unsigned char* dst_buffer,const int width,const int height,const int depth)
{
	// get index into global data array
	int3 index = (int3)(get_global_id(0),get_global_id(1),get_global_id(2));
	int3 dim = (int3)(width,height,depth);
	int3 ipos;
	
	int index1d = ((((index.z*dim.y)+(index.y))*dim.x)+(index.x));
	float current = src_buffer[index1d];
	int i,j,k;
    const int window_size = 1;
	
	if(current>0)
	{
		current=255;
		
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

					
					if(ipos.x<0) ipos.x=0;
					if(ipos.y<0) ipos.y=0;
					if(ipos.z<0) ipos.z=0;
					
					if(ipos.x>dim.x-1) ipos.x=dim.x-1;
					if(ipos.y>dim.y-1) ipos.y=dim.y-1;
					if(ipos.z>dim.z-1) ipos.z=dim.z-1;
					
					index1d = ((((ipos.z*dim.y)+(ipos.y))*dim.x)+(ipos.x));
					
					//keep the values of the voxel
					dst_buffer[index1d] = 255;
				}
			}
		}
	}
	else
	{
		current=0;
	}
	
	//save results to our output buffer
	index1d = ((((index.z*dim.y)+(index.y))*dim.x)+(index.x));
	dst_buffer[index1d] = current;	
}
