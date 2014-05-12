// *********************************************************************
// OpenCL Kernel Function for data invert
// *********************************************************************
void bubblesort(unsigned char* values, int length)
{
    for (int i=0;i<length;i++)
	{
		for (int j=0;j<length;j++)
		{
			if (values[i]>values[j])     
			{
				unsigned char v = values[i];
				values[i] = values[j];
				values[j] = v;
			}
		}
	}
}
void insertionsort(unsigned char* values, int length)
{
	int i, j, key;
	for(j = 1; j < length; j++)    // Start with 1 (not 0)
	{
		key = values[j];
		for(i = j - 1; (i >= 0) && (values[i] < key); i--)   // Smaller values move up
		{
			values[i+1] = values[i];
		}
		values[i+1] = key;    //Put key into its proper location
	}
     return;
}
void shellsort(unsigned char* values, int length)
{
     int i, temp, flag = 1;
     int d = length;
     while( flag || (d > 1))      // boolean flag (true when not equal to 0)
     {
          flag = 0;           // reset flag to 0 to check for future swaps
          d = (d+1) / 2;
          for (i = 0; i < (length - d); i++)
        {
               if (values[i + d] > values[i])
              {
                      temp = values[i + d];      // swap positions i+d and i
                      values[i + d] = values[i];
                      values[i] = temp;
                      flag = 1;                  // tells swap has occurred
              }
         }
     }
     return;
}
__kernel void myFunc( __global unsigned char* src_buffer, __global unsigned char* dst_buffer,const int width,const int height,const int depth)
{
	// get index into global data array
	int3 index = (int3)(get_global_id(0),get_global_id(1),get_global_id(2));
	int3 dim = (int3)(width,height,depth);

	//helpers
    int4 ipos;
    int i,j,k;
	int index1d;
    int acum = 0;
	
	//filter window
    unsigned char sample[9];
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

				
				if(ipos.x<0) ipos.x=0;
				if(ipos.y<0) ipos.y=0;
				if(ipos.z<0) ipos.z=0;
				
				if(ipos.x>dim.x-1) ipos.x=dim.x-1;
				if(ipos.y>dim.y-1) ipos.y=dim.y-1;
				if(ipos.z>dim.z-1) ipos.z=dim.z-1;
				
				index1d = ((((ipos.z*dim.y)+(ipos.y))*dim.x)+(ipos.x));
				
				//keep the values of the voxel
				sample[acum]= src_buffer[index1d];
				acum ++;
			}
		}
	}

	//sort our array
	insertionsort(sample, acum);
    //bubblesort(sample, acum);
	//shellsort(sample, acum);
	
	//save results to our output buffer
	index1d = ((((index.z*dim.y)+(index.y))*dim.x)+(index.x));
	dst_buffer[index1d] = sample[4];	
}
