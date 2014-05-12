// *********************************************************************
// OpenCL Kernel Function for data invert
// *********************************************************************
__kernel void myFunc( __global unsigned char* src_buffer, __global unsigned char* minbb, __global unsigned char* maxbb, const int width,const int height,const int depth, const int channel)
{
	// get index into global data array
	int3 index = (int3)(get_global_id(0),get_global_id(1),get_global_id(2));
	int3 dim = (int3)(width,height,depth);
	int index1d = ((((index.z*dim.y)+(index.y))*dim.x)+(index.x));
	unsigned char current = src_buffer[3*index1d+channel];
		
	if(current>0)
	{
		if(index.x<minbb[0]) minbb[0]=index.x;
		if(index.y<minbb[1]) minbb[1]=index.y;
		if(index.z<minbb[2]) minbb[2]=index.z;

		if(index.x>maxbb[0]) maxbb[0]=index.x;
		if(index.y>maxbb[1]) maxbb[1]=index.y;
		if(index.z>maxbb[2]) maxbb[2]=index.z;
	}	
}

__kernel void min_reduce(__global unsigned char* src_buffer, __global int* buffer, __local int* scratch, __const int length, __global int* result) 
{
	int global_index = get_global_id(0);
	int accumulator = INFINITY;
	
	// Loop sequentially over chunks of input vector
	while (global_index < length)
	{
		int element = buffer[global_index];
	
		if(accumulator < element) accumulator = accumulator;
		else accumulator = element;
		
		//accumulator = (accumulator < element) ? accumulator : element;
		
		global_index += get_global_size(0);
	}

	// Perform parallel reduction
	int local_index = get_local_id(0);
	scratch[local_index] = accumulator;
	
	barrier(CLK_LOCAL_MEM_FENCE);
	
	for(int offset = get_local_size(0) / 2; offset > 0; offset = offset / 2) 
	{
		if (local_index < offset) 
		{
			int other = scratch[local_index + offset];
			int mine = scratch[local_index];
			
			if(mine < other) scratch[local_index] = mine;
			else scratch[local_index] = other;
			
	//		scratch[local_index] = (mine < other) ? mine : other;
		}
		
		barrier(CLK_LOCAL_MEM_FENCE);
	}
	
	if (local_index == 0) 
	{
		result[get_group_id(0)] = scratch[0];
	}
}