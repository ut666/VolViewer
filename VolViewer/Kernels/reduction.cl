/*__kernel void min_reduce(__global int* buffer, __local int* tmp_buffer, __const int length, __global int* result) 
{
	int global_index = get_global_id(0);
	int local_index = get_local_id(0);
	
	// Load data into local memory
	if (global_index < length)
	{
		tmp_buffer[local_index] = buffer[global_index];
	} 
	else 
	{
		// Infinity is the identity element for the min operation
		tmp_buffer[local_index] = INFINITY;
	}
	
	barrier(CLK_LOCAL_MEM_FENCE);
	*/
	/*
	// FIGURE 1
	for(int offset = 1; offset < get_local_size(0); offset <<= 1)
	{
		int mask = (offset << 1) - 1;
		if ((local_index & mask) == 0) 
		{
			int other = tmp_buffer[local_index + offset];
			int mine = tmp_buffer[local_index];
			tmp_buffer[local_index] = (mine < other) ? mine : other;
		}
		
		barrier(CLK_LOCAL_MEM_FENCE);
	}*/
	
/*
	//FIGURE 2
	for(int offset = get_local_size(0) / 2; offset > 0; offset >>= 1) 
	{
		if (local_index < offset)
		{
			int other = tmp_buffer[local_index + offset];
			int mine = tmp_buffer[local_index];
			tmp_buffer[local_index] = (mine < other) ? mine : other;
		}
		barrier(CLK_LOCAL_MEM_FENCE);
	}
 
	if (local_index == 0)
	{
		result[get_group_id(0)] = tmp_buffer[0];
	}
}*/
__kernel void min_reduce(__global int* buffer, __local int* scratch, __const int length, __global int* result) 
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