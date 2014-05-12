#include "OpenCLExecuter.h"

#ifdef OPENCL_SUPPORT

const unsigned int BLOCK_N = 512;
const unsigned int THREAD_N = 512;
const unsigned int ACCUM_N = BLOCK_N * THREAD_N;
const unsigned int MAX_DEVICES = 8;
 
OpenCLExecuter::OpenCLExecuter()
{
	ocl_wrapper = NULL;
	cpProgram = NULL;
	volobj = NULL;
	filter_width=0.0;
}
OpenCLExecuter::~OpenCLExecuter()
{
	printf("OPENCL: Releasing CL program\n");
    if(cpProgram)clReleaseProgram(cpProgram);

	ocl_wrapper = NULL;
	cpProgram = NULL;
	volobj = NULL;
}
void OpenCLExecuter::ocl_init_progam(string fn)
{
	cl_int err;										// debugging variables

	//Load and compile the CL program
	char* cSourceCL = NULL;          // Buffer to hold source for compilation 
	size_t szKernelLength;			 // Byte size of kernel code
	string cSourceFile;				 // String containing our .cl kernel file
	int filelen;

	// Read the OpenCL kernel in from source file
	cSourceCL = ocl_file_contents(fn.data(), &filelen);
	
	if(cSourceCL==NULL) 
	{
		printf("ERROR: Unable to open %s for reading\n", fn.data());
		return;
	}

	szKernelLength = (size_t)filelen;
	
	//printf("file: %s\n", cSourceCL);

	// Create the program
    if(cpProgram)clReleaseProgram(cpProgram);
	cpProgram = clCreateProgramWithSource (ocl_wrapper->context, 1, (const char **)&cSourceCL, &szKernelLength, &err);
	printf("OPENCL: clCreateProgramWithSource: %s\n", ocl_wrapper->get_error(err));
 
	// Build the program
	err = clBuildProgram (cpProgram, 0, NULL, NULL, NULL, NULL);
	printf("OPENCL: clBuildProgram: %s\n", ocl_wrapper->get_error(err));

	// Get error log
	printBuildLog(&cpProgram);

	free(cSourceCL);
}
void OpenCLExecuter::ocl_filter_shared(void)
{
	cl_int err;							// debugging variables
	size_t szParmDataBytes;				// Byte size of context information        
	cl_mem src_buffer;					// OpenCL device source buffer
	cl_mem dst_buffer;					// OpenCL device source buffer
	cl_sampler sampler;					// OpenCL sampler
	cl_kernel ckKernel;					// OpenCL kernel

	int iNumElements = volobj->texwidth*volobj->texheight*volobj->texdepth; // Length of float arrays to process

	// set Local work size dimensions
//	size_t local_threads[3] ={256,256,64};
	// set Global work size dimensions
//	size_t global_threads[3] ={roundup((int) volobj->texwidth/local_threads[0], 0)*local_threads[0], roundup((int) volobj->texheight/local_threads[1], 0)*local_threads[1], roundup((int) volobj->texdepth/local_threads[2], 0)*local_threads[2]};

	// set Global work size dimensions
	size_t global_threads[3] ={volobj->texwidth, volobj->texheight, volobj->texdepth};

	// allocate the source buffer memory object
	src_buffer = clCreateFromGLTexture3D (ocl_wrapper->context, CL_MEM_READ_WRITE, GL_TEXTURE_3D, 0, volobj->TEXTURE3D_RED, &err);
	printf("OPENCL: clCreateFromGLTexture3D: %s\n", ocl_wrapper->get_error(err));

	// allocate the destination buffer memory object
	dst_buffer = clCreateBuffer (ocl_wrapper->context, CL_MEM_READ_WRITE,  sizeof(unsigned char) * iNumElements, NULL, &err);
	printf("OPENCL: clCreateBuffer: %s\n", ocl_wrapper->get_error(err));

	// create a sampler object
	sampler = clCreateSampler(ocl_wrapper->context, CL_FALSE, CL_ADDRESS_CLAMP, CL_FILTER_NEAREST, &err);
	printf("OPENCL: clCreateSampler: %s\n", ocl_wrapper->get_error(err));
 
    // Create the kernel
	ckKernel = clCreateKernel (cpProgram, "myFunc", &err);
	printf("OPENCL: clCreateKernel: %s\n", ocl_wrapper->get_error(err));
  
	// Set the Argument values
	err = clSetKernelArg (ckKernel, 0, sizeof(cl_mem), (void*)&src_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 1, sizeof(cl_mem), (void*)&dst_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 2, sizeof(sampler), (void*)&sampler);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));

	size_t local;
	err = clGetKernelWorkGroupInfo(ckKernel, ocl_wrapper->devices[ocl_wrapper->deviceUsed], CL_KERNEL_LOCAL_MEM_SIZE , sizeof(local), &local, NULL);
	printf("OPENCL: clGetKernelWorkGroupInfo (kernel memory): %s\n", ocl_wrapper->get_error(err));
	printf("OPENCL: Kernel local memory use: %d Bytes\n", (int)local);

	// grab input data from OpenGL, compute, copy the results back to OpenGL
	// Runs asynchronous to host, up until blocking clFinish at the end

	glFinish();
	glFlush();
	
	// grab the OpenGL texture object for read/writing from OpenCL
	err = clEnqueueAcquireGLObjects(ocl_wrapper->commandQue, 1, &src_buffer, 0,NULL,NULL);
	printf("OPENCL: clEnqueueAcquireGLObjects: %s\n", ocl_wrapper->get_error(err));

	// Execute a kernel
	err = clEnqueueNDRangeKernel (ocl_wrapper->commandQue, ckKernel, 3, NULL, global_threads, NULL, 0, NULL, NULL);
	printf("OPENCL: clEnqueueNDRangeKernel: %s\n", ocl_wrapper->get_error(err));

	/*
	// Blocking read of results from GPU to Host
	int size = volobj->texwidth*volobj->texheight*volobj->texdepth;
	unsigned char* result = new unsigned char[size];
	err = clEnqueueReadBuffer (ocl_wrapper->commandQue, dst_buffer, CL_TRUE, 0, sizeof(unsigned char) * iNumElements, result, 0, NULL, NULL);
	printf("OPENCL: clEnqueueReadBuffer: %s\n", ocl_wrapper->get_error(err));
	for(int i=0; i<size; i++) volobj->texture3d[3*i+0] = result[i];
	delete[] result;
	*/

	// copy OpenCL buffer to OpenGl texture
	size_t corigin[3] = {0,0,0};
	size_t cdimensions[3] = {(unsigned int)volobj->texwidth, (unsigned int)volobj->texheight, (unsigned int)volobj->texdepth};
	err = clEnqueueCopyBufferToImage(ocl_wrapper->commandQue , dst_buffer, src_buffer, 0, corigin, cdimensions, 0, NULL, NULL);
	printf("OPENCL: clEnqueueCopyBufferToImage: %s\n", ocl_wrapper->get_error(err));

	//make sure we block until we are done.
	//err = clFinish(ocl_wrapper->commandQue);
	//printf("OPENCL: clFinish: %s\n", ocl_wrapper->get_error(err));
	
	//release opengl objects now
	err = clEnqueueReleaseGLObjects(ocl_wrapper->commandQue, 1, &src_buffer, 0,0,0);
	printf("OPENCL: clEnqueueAcquireGLObjects: %s\n", ocl_wrapper->get_error(err));

	// Cleanup allocated objects
	printf("OPENCL: Releasing kernel memory\n");
    if(ckKernel)clReleaseKernel(ckKernel); 
   
    //need to release any other OpenCL memory objects here
    if(src_buffer)clReleaseMemObject(src_buffer);
    if(dst_buffer)clReleaseMemObject(dst_buffer);
}
void OpenCLExecuter::ocl_filter_multi(void)
{
	cl_int err;										// debugging variables
	size_t szParmDataBytes;							// Byte size of context information        

	cl_mem src_buffer[MAX_DEVICES];					// OpenCL device source buffer
	cl_mem dst_buffer[MAX_DEVICES];					// OpenCL device source buffer
	cl_command_queue queues[MAX_DEVICES];			// OpenCL device queue
	cl_kernel ckKernel[MAX_DEVICES];				// OpenCL kernel

	cl_event gpuDone[MAX_DEVICES];

//	int iNumElements = volobj->texwidth*volobj->texheight*volobj->texdepth*3; // Length of float arrays to process

	int xdim, ydim, zdim;
	xdim = (float)volobj->texwidth; // (float)ocl_wrapper->numDevices;
	ydim = (float)volobj->texheight; // (float)ocl_wrapper->numDevices;
	zdim = (float)volobj->texdepth / (float)ocl_wrapper->numDevices;

	//Length of array to process
	int iNumElements = (xdim*ydim*zdim);
	size_t global_threads[3] = {xdim, ydim, zdim};
	
	//temp array
	unsigned char** data = new unsigned char*[ocl_wrapper->numDevices];

	for(int i=0; i<ocl_wrapper->numDevices; i++)
		data[i] = new unsigned char[iNumElements];

	for(int i=0; i<ocl_wrapper->numDevices; i++)
	{
		printf("OPENCL: Computing Device%d\n", i);

		//create the command queue we will use to execute OpenCL commands
		queues[i] = clCreateCommandQueue(ocl_wrapper->context, ocl_wrapper->devices[i], 0, &err);
		printf("OPENCL: clCreateCommandQueue: %s\n", ocl_wrapper->get_error(err));
		
		// allocate the source buffer memory object
		src_buffer[i] = clCreateBuffer (ocl_wrapper->context, CL_MEM_READ_ONLY,  sizeof(unsigned char) * iNumElements, NULL, &err);
		printf("OPENCL: clCreateBuffer: %s\n", ocl_wrapper->get_error(err));
		
		// allocate the destination buffer memory object
		dst_buffer[i] = clCreateBuffer (ocl_wrapper->context, CL_MEM_WRITE_ONLY,  sizeof(unsigned char) * iNumElements, NULL, &err);
		printf("OPENCL: clCreateBuffer: %s\n", ocl_wrapper->get_error(err));

		// Create the kernel
		ckKernel[i] = clCreateKernel (cpProgram, "myFunc", &err);
		printf("OPENCL: clCreateKernel: %s\n", ocl_wrapper->get_error(err));
  
		// Set the Argument values
		err = clSetKernelArg (ckKernel[i], 0, sizeof(cl_mem), (void*)&src_buffer[i]);
		printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
		err = clSetKernelArg (ckKernel[i], 1, sizeof(cl_mem), (void*)&dst_buffer[i]);
		printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
		err = clSetKernelArg (ckKernel[i], 2, sizeof(int), (void*)&global_threads[0]);
		printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
		err = clSetKernelArg (ckKernel[i], 3, sizeof(int), (void*)&global_threads[1]);
		printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
		err = clSetKernelArg (ckKernel[i], 4, sizeof(int), (void*)&global_threads[2]);
		printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	
		//Prepare data to upload
		int iOffsetElements = (xdim*ydim*zdim*i);
		for(int j=iOffsetElements; j<iNumElements+iOffsetElements; j++)
			data[i][j-iOffsetElements] = volobj->texture3d[3*j+0];

		// Write data from host to GPU
		err = clEnqueueWriteBuffer (queues[i], src_buffer[i], CL_FALSE, 0, sizeof(unsigned char) * iNumElements, data[i], 0, NULL, NULL);
		printf("OPENCL: clEnqueueWriteBuffer: %s\n", ocl_wrapper->get_error(err));
	}

	for(int i=0; i<ocl_wrapper->numDevices; i++)
	{
		// Launch kernel 
		err = clEnqueueNDRangeKernel (queues[i], ckKernel[i], 3, NULL, global_threads, NULL, 0, NULL, NULL);
		printf("OPENCL: clEnqueueNDRangeKernel: %s\n", ocl_wrapper->get_error(err));
	}

	for(int i=0; i<ocl_wrapper->numDevices; i++)
	{
		// Blocking read of results from GPU to Host
		err = clEnqueueReadBuffer (queues[i], dst_buffer[i], CL_TRUE, 0, sizeof(unsigned char) * iNumElements, data[i], 0, NULL,  &gpuDone[i]);
		printf("OPENCL: clEnqueueReadBuffer: %s\n", ocl_wrapper->get_error(err));
	}

	// Synchronize with the GPUs
    printf("OPENCL: Waiting for devices to sync\n");
	clWaitForEvents(ocl_wrapper->numDevices, gpuDone);

	for(int i=0; i<ocl_wrapper->numDevices; i++)
	{
		//read data back
		int iOffsetElements = (xdim*ydim*zdim*i);
		for(int j=iOffsetElements; j<iNumElements+iOffsetElements; j++)
			volobj->texture3d[3*j+0] = data[i][j-iOffsetElements];
	}

	for(int i=0; i<ocl_wrapper->numDevices; i++)
	{
		// Cleanup allocated objects
		printf("OPENCL: Releasing kernel memory\n");
		if(ckKernel[i])clReleaseKernel(ckKernel[i]); 
   
		//need to release any other OpenCL memory objects here
		if(dst_buffer[i])clReleaseMemObject(dst_buffer[i]);
		if(src_buffer[i])clReleaseMemObject(src_buffer[i]);
	}

	for(int i=0; i<ocl_wrapper->numDevices; i++)
		delete[] data[i];

	delete[] data;
}
void OpenCLExecuter::ocl_filter(int src_chan)
{
	cl_int err;							// debugging variables
	size_t szParmDataBytes;				// Byte size of context information        
	cl_mem src_buffer;					// OpenCL device source buffer
	cl_mem dst_buffer;					// OpenCL device source buffer
	size_t szGlobalWorkSize;			// 1D var for Total # of work items
	size_t szLocalWorkSize;				// 1D var for # of work items in the work group
	cl_kernel ckKernel;					// OpenCL kernel

	int iNumElements = volobj->texwidth*volobj->texheight*volobj->texdepth; // Length of float arrays to process

	//temp array
	unsigned char* data = new unsigned char[iNumElements];

	// set Local work size dimensions
	//szLocalWorkSize = 256;
	// set Global work size dimensions
	//szGlobalWorkSize = roundup((int) iNumElements/szLocalWorkSize, 0)*szLocalWorkSize;  
	//szGlobalWorkSize = iNumElements;
//	printf("OPENCL: number of elements: %d\n", (int)iNumElements);
//	printf("OPENCL: local worksize: %d\n", (int)szLocalWorkSize);
//	printf("OPENCL: global worksize: %d\n", (int)szGlobalWorkSize);
//	printf("OPENCL: work groups: %d\n", (int)((float)szGlobalWorkSize/(float)szLocalWorkSize));

	size_t global_threads[3] ={volobj->texwidth, volobj->texheight, volobj->texdepth};

	// allocate the source buffer memory object
	src_buffer = clCreateBuffer (ocl_wrapper->context, CL_MEM_READ_ONLY,  sizeof(unsigned char) * iNumElements, NULL, &err);
	printf("OPENCL: clCreateBuffer: %s\n", ocl_wrapper->get_error(err));
		
	// allocate the destination buffer memory object
	dst_buffer = clCreateBuffer (ocl_wrapper->context, CL_MEM_WRITE_ONLY,  sizeof(unsigned char) * iNumElements, NULL, &err);
	printf("OPENCL: clCreateBuffer: %s\n", ocl_wrapper->get_error(err));

    // Create the kernel
	ckKernel = clCreateKernel (cpProgram, "myFunc", &err);
	printf("OPENCL: clCreateKernel: %s\n", ocl_wrapper->get_error(err));
  
	// Set the Argument values
	err = clSetKernelArg (ckKernel, 0, sizeof(cl_mem), (void*)&src_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 1, sizeof(cl_mem), (void*)&dst_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 2, sizeof(int), (void*)&volobj->texwidth);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 3, sizeof(int), (void*)&volobj->texheight);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 4, sizeof(int), (void*)&volobj->texdepth);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));

	size_t local;
	err = clGetKernelWorkGroupInfo(ckKernel, ocl_wrapper->devices[ocl_wrapper->deviceUsed], CL_KERNEL_LOCAL_MEM_SIZE , sizeof(local), &local, NULL);
	printf("OPENCL: clGetKernelWorkGroupInfo (kernel memory): %s\n", ocl_wrapper->get_error(err));
	printf("OPENCL: Kernel local memory use: %d Bytes\n", (int)local);

	// Copy input data to GPU, compute, copy results back
	// Runs asynchronous to host, up until blocking read at end

	//Prepare data to upload
	for(int j=0; j<iNumElements; j++)
		data[j] = volobj->texture3d[3*j+src_chan];

	// Write data from host to GPU
	err = clEnqueueWriteBuffer (ocl_wrapper->commandQue, src_buffer, CL_FALSE, 0, sizeof(unsigned char) * iNumElements, data, 0, NULL, NULL);
	printf("OPENCL: clEnqueueWriteBuffer: %s\n", ocl_wrapper->get_error(err));
	
	// Write data from host to GPU
//	err = clEnqueueWriteBuffer (ocl_wrapper->commandQue, src_buffer, CL_FALSE, 0, sizeof(unsigned char) * iNumElements, volobj->texture3d, 0, NULL, NULL);
//	printf("OPENCL: clEnqueueWriteBuffer: %s\n", ocl_wrapper->get_error(err));

	// Launch kernel 
	err = clEnqueueNDRangeKernel (ocl_wrapper->commandQue, ckKernel, 3, NULL, global_threads, NULL, 0, NULL, NULL);
	printf("OPENCL: clEnqueueNDRangeKernel: %s\n", ocl_wrapper->get_error(err));

	// Blocking read of results from GPU to Host
//	err = clEnqueueReadBuffer (ocl_wrapper->commandQue, dst_buffer, CL_TRUE, 0, sizeof(unsigned char) * iNumElements, volobj->texture3d, 0, NULL, NULL);
//	printf("OPENCL: clEnqueueReadBuffer: %s\n", ocl_wrapper->get_error(err));
		// Blocking read of results from GPU to Host

	// Blocking read of results from GPU to Host
	err = clEnqueueReadBuffer (ocl_wrapper->commandQue, dst_buffer, CL_TRUE, 0, sizeof(unsigned char) * iNumElements, data, 0, NULL,  NULL);
	printf("OPENCL: clEnqueueReadBuffer: %s\n", ocl_wrapper->get_error(err));

	//read data back
	for(int i=0; i<iNumElements; i++)
	{
			if(volobj->is_greyscale)
				volobj->texture3d[3*i+0] = volobj->texture3d[3*i+1] = volobj->texture3d[3*i+2] = data[i];
			else
				volobj->texture3d[3*i+src_chan] = data[i];
	}

	// Cleanup allocated objects
	printf("OPENCL: Releasing kernel memory\n");
    if(ckKernel)clReleaseKernel(ckKernel); 
   
    //need to release any other OpenCL memory objects here
    if(dst_buffer)clReleaseMemObject(dst_buffer);
    if(src_buffer)clReleaseMemObject(src_buffer);

	delete[] data;
}
void OpenCLExecuter::ocl_filterBoundingBox(int channel, int window_size)
{
	cl_int err;							// debugging variables
	size_t szParmDataBytes;				// Byte size of context information        
	cl_mem src_buffer;					// OpenCL device source buffer
	cl_mem bbmin_buffer;				// OpenCL device source buffer
	cl_mem bbmax_buffer;				// OpenCL device source buffer
	size_t szGlobalWorkSize;			// 1D var for Total # of work items
	size_t szLocalWorkSize;				// 1D var for # of work items in the work group
	cl_kernel ckKernel;					// OpenCL kernel

	cl_int4 minbb;
	cl_int4 maxbb;

	minbb.s[0] = minbb.s[1] = minbb.s[2] = 8192;
	maxbb.s[0] = maxbb.s[1] = maxbb.s[2] = -8192;

	int iNumElements = 3*volobj->texwidth*volobj->texheight*volobj->texdepth; // Length of float arrays to process

	size_t global_threads[3] ={volobj->texwidth, volobj->texheight, volobj->texdepth};

	// allocate the source buffer memory object
	src_buffer = clCreateBuffer (ocl_wrapper->context, CL_MEM_READ_ONLY,  sizeof(unsigned char) * iNumElements, NULL, &err);
	printf("OPENCL: clCreateBuffer: %s\n", ocl_wrapper->get_error(err));
		
	// allocate the destination buffer memory object
	bbmin_buffer = clCreateBuffer (ocl_wrapper->context, CL_MEM_READ_WRITE,  sizeof(cl_int4), NULL, &err);
	printf("OPENCL: clCreateBuffer: %s\n", ocl_wrapper->get_error(err));

 	bbmax_buffer = clCreateBuffer (ocl_wrapper->context, CL_MEM_READ_WRITE,  sizeof(cl_int4), NULL, &err);
	printf("OPENCL: clCreateBuffer: %s\n", ocl_wrapper->get_error(err));

	// Create the kernel
	ckKernel = clCreateKernel (cpProgram, "myFunc", &err);
	printf("OPENCL: clCreateKernel: %s\n", ocl_wrapper->get_error(err));
  
	// Set the Argument values
	err = clSetKernelArg (ckKernel, 0, sizeof(cl_mem), (void*)&src_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 1, sizeof(cl_mem), (void*)&bbmin_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 2, sizeof(cl_mem), (void*)&bbmax_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 2, sizeof(int), (void*)&volobj->texwidth);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 3, sizeof(int), (void*)&volobj->texheight);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 4, sizeof(int), (void*)&volobj->texdepth);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 5, sizeof(int), (void*)&channel);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));

	// Copy input data to GPU, compute, copy results back
	// Runs asynchronous to host, up until blocking read at end

	// Write data from host to GPU
	err = clEnqueueWriteBuffer (ocl_wrapper->commandQue, src_buffer, CL_FALSE, 0, sizeof(unsigned char) * iNumElements, volobj->texture3d, 0, NULL, NULL);
	printf("OPENCL: clEnqueueWriteBuffer: %s\n", ocl_wrapper->get_error(err));
	
	err = clEnqueueWriteBuffer (ocl_wrapper->commandQue, bbmin_buffer, CL_FALSE, 0, sizeof(cl_int4), (void*)&minbb, 0, NULL, NULL);
	printf("OPENCL: clEnqueueWriteBuffer: %s\n", ocl_wrapper->get_error(err));

	err = clEnqueueWriteBuffer (ocl_wrapper->commandQue, bbmax_buffer, CL_FALSE, 0, sizeof(cl_int4), (void*)&maxbb, 0, NULL, NULL);
	printf("OPENCL: clEnqueueWriteBuffer: %s\n", ocl_wrapper->get_error(err));

	// Launch kernel
	err = clEnqueueNDRangeKernel (ocl_wrapper->commandQue, ckKernel, 3, NULL, global_threads, NULL, 0, NULL, NULL);
	printf("OPENCL: clEnqueueNDRangeKernel: %s\n", ocl_wrapper->get_error(err));

	// Blocking read of results from GPU to Host
	err = clEnqueueReadBuffer (ocl_wrapper->commandQue, bbmin_buffer, CL_TRUE, 0, sizeof(cl_int4), (void*)&minbb, 0, NULL,  NULL);
	printf("OPENCL: clEnqueueReadBuffer: %s\n", ocl_wrapper->get_error(err));

	err = clEnqueueReadBuffer (ocl_wrapper->commandQue, bbmax_buffer, CL_TRUE, 0, sizeof(cl_int4), (void*)&maxbb, 0, NULL,  NULL);
	printf("OPENCL: clEnqueueReadBuffer: %s\n", ocl_wrapper->get_error(err));

	// Cleanup allocated objects
	printf("OPENCL: Releasing kernel memory\n");
    if(ckKernel)clReleaseKernel(ckKernel); 
   
    //need to release any other OpenCL memory objects here
    if(src_buffer)clReleaseMemObject(src_buffer);
    if(bbmin_buffer)clReleaseMemObject(bbmin_buffer);
    if(bbmax_buffer)clReleaseMemObject(bbmax_buffer);
	
	maxbb.s[0] += (float)window_size/2.0;
	maxbb.s[1] += (float)window_size/2.0;
	maxbb.s[2] += (float)window_size/2.0;

	minbb.s[0] -= (float)window_size/2.0;
	minbb.s[1] -= (float)window_size/2.0;
	minbb.s[2] -= (float)window_size/2.0;

	maxbb.s[0] += 2;
	maxbb.s[1] += 2;
	maxbb.s[2] += 2;

	minbb.s[0] -= 2;
	minbb.s[1] -= 2;
	minbb.s[2] -= 2;	
	
	if(maxbb.s[0]>volobj->texwidth-1) maxbb.s[0]  =volobj->texwidth-1;
	if(maxbb.s[1]>volobj->texheight-1) maxbb.s[1] =volobj->texheight-1;
	if(maxbb.s[2]>volobj->texdepth-1) maxbb.s[2] =volobj->texdepth-1;

	if(minbb.s[0]<0) minbb.s[0]=0;
	if(minbb.s[1]<0) minbb.s[1]=0;
	if(minbb.s[2]<0) minbb.s[2]=0;

	volobj->boundingboxSize.x = ((maxbb.s[0])-(minbb.s[0]-1));
	volobj->boundingboxSize.y = ((maxbb.s[1])-(minbb.s[1]-1));
	volobj->boundingboxSize.z = ((maxbb.s[2])-(minbb.s[2]-1));
	volobj->boundingboxCentre.x = 0.0; //-(((float)boundingboxSize.x)/2.0);
	volobj->boundingboxCentre.y = 0.0; //-(((float)boundingboxSize.y)/2.0);
	volobj->boundingboxCentre.z = 0.0; //-(((float)boundingboxSize.z)/2.0);
	volobj->boundingboxMin = Vector(minbb.s[0], minbb.s[1], minbb.s[2]);
	volobj->boundingboxMax = Vector(maxbb.s[0], maxbb.s[1], maxbb.s[2]);

	printf("min: %f, %f, %f\n", volobj->boundingboxMin.x, volobj->boundingboxMin.y, volobj->boundingboxMin.z);
	printf("max: %f, %f, %f\n", volobj->boundingboxMax.x, volobj->boundingboxMax.y, volobj->boundingboxMax.z);
}
void OpenCLExecuter::ocl_filterBilateral(int spatial, float range, unsigned char* src_array, unsigned char* dst_array, int w, int h, int d)
{
	cl_int err;							// debugging variables
	size_t szParmDataBytes;				// Byte size of context information        
	cl_mem src_buffer;					// OpenCL device source buffer
	cl_mem dst_buffer;					// OpenCL device source buffer
	size_t szGlobalWorkSize;			// 1D var for Total # of work items
	size_t szLocalWorkSize;				// 1D var for # of work items in the work group
	cl_kernel ckKernel;					// OpenCL kernel

	int iNumElements = w*h*d; // Length of float arrays to process

	size_t global_threads[3] ={w,h,d};

	// allocate the source buffer memory object
	src_buffer = clCreateBuffer (ocl_wrapper->context, CL_MEM_READ_ONLY,  sizeof(unsigned char) * iNumElements, NULL, &err);
	printf("OPENCL: clCreateBuffer: %s\n", ocl_wrapper->get_error(err));
		
	// allocate the destination buffer memory object
	dst_buffer = clCreateBuffer (ocl_wrapper->context, CL_MEM_WRITE_ONLY,  sizeof(unsigned char) * iNumElements, NULL, &err);
	printf("OPENCL: clCreateBuffer: %s\n", ocl_wrapper->get_error(err));

    // Create the kernel
	ckKernel = clCreateKernel (cpProgram, "bilateral", &err);
	printf("OPENCL: clCreateKernel: %s\n", ocl_wrapper->get_error(err));
  
	// Set the Argument values
	err = clSetKernelArg (ckKernel, 0, sizeof(cl_mem), (void*)&src_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 1, sizeof(cl_mem), (void*)&dst_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 2, sizeof(int), (void*)&spatial);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 3, sizeof(float), (void*)&range);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 4, sizeof(int), (void*)&volobj->texwidth);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 5, sizeof(int), (void*)&volobj->texheight);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 6, sizeof(int), (void*)&volobj->texdepth);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));

	size_t local;
	err = clGetKernelWorkGroupInfo(ckKernel, ocl_wrapper->devices[ocl_wrapper->deviceUsed], CL_KERNEL_LOCAL_MEM_SIZE , sizeof(local), &local, NULL);
	printf("OPENCL: clGetKernelWorkGroupInfo (kernel memory): %s\n", ocl_wrapper->get_error(err));
	printf("OPENCL: Kernel local memory use: %d Bytes\n", (int)local);

	// Copy input data to GPU, compute, copy results back
	// Runs asynchronous to host, up until blocking read at end

	// Write data from host to GPU
	err = clEnqueueWriteBuffer (ocl_wrapper->commandQue, src_buffer, CL_FALSE, 0, sizeof(unsigned char) * iNumElements, src_array, 0, NULL, NULL);
	printf("OPENCL: clEnqueueWriteBuffer: %s\n", ocl_wrapper->get_error(err));

	// Launch kernel 
	err = clEnqueueNDRangeKernel (ocl_wrapper->commandQue, ckKernel, 3, NULL, global_threads, NULL, 0, NULL, NULL);
	printf("OPENCL: clEnqueueNDRangeKernel: %s\n", ocl_wrapper->get_error(err));

	// Blocking read of results from GPU to Host
	err = clEnqueueReadBuffer (ocl_wrapper->commandQue, dst_buffer, CL_TRUE, 0, sizeof(unsigned char) * iNumElements, dst_array, 0, NULL,  NULL);
	printf("OPENCL: clEnqueueReadBuffer: %s\n", ocl_wrapper->get_error(err));

	// Cleanup allocated objects
	printf("OPENCL: Releasing kernel memory\n");
    if(ckKernel)clReleaseKernel(ckKernel); 
   
    //need to release any other OpenCL memory objects here
    if(dst_buffer)clReleaseMemObject(dst_buffer);
    if(src_buffer)clReleaseMemObject(src_buffer);
}

void OpenCLExecuter::ocl_filterPeronaMalik(float lambda, float dT, unsigned char* src_array, unsigned char* dst_array, int w, int h, int d)
{
	float lambda2 = lambda*lambda;

	cl_int err;							// debugging variables
	size_t szParmDataBytes;				// Byte size of context information        
	cl_mem src_buffer;					// OpenCL device source buffer
	cl_mem dst_buffer;					// OpenCL device source buffer
	size_t szGlobalWorkSize;			// 1D var for Total # of work items
	size_t szLocalWorkSize;				// 1D var for # of work items in the work group
	cl_kernel ckKernel;					// OpenCL kernel

	int iNumElements = w*h*d; // Length of float arrays to process

	size_t global_threads[3] ={w,h,d};

	// allocate the source buffer memory object
	src_buffer = clCreateBuffer (ocl_wrapper->context, CL_MEM_READ_ONLY,  sizeof(unsigned char) * iNumElements, NULL, &err);
	printf("OPENCL: clCreateBuffer: %s\n", ocl_wrapper->get_error(err));
		
	// allocate the destination buffer memory object
	dst_buffer = clCreateBuffer (ocl_wrapper->context, CL_MEM_WRITE_ONLY,  sizeof(unsigned char) * iNumElements, NULL, &err);
	printf("OPENCL: clCreateBuffer: %s\n", ocl_wrapper->get_error(err));

    // Create the kernel
	ckKernel = clCreateKernel (cpProgram, "myFunc", &err);
	printf("OPENCL: clCreateKernel: %s\n", ocl_wrapper->get_error(err));
  
	// Set the Argument values
	err = clSetKernelArg (ckKernel, 0, sizeof(cl_mem), (void*)&src_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 1, sizeof(cl_mem), (void*)&dst_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 2, sizeof(float), (void*)&lambda2);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 3, sizeof(float), (void*)&dT);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 4, sizeof(int), (void*)&volobj->texwidth);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 5, sizeof(int), (void*)&volobj->texheight);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 6, sizeof(int), (void*)&volobj->texdepth);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));

	size_t local;
	err = clGetKernelWorkGroupInfo(ckKernel, ocl_wrapper->devices[ocl_wrapper->deviceUsed], CL_KERNEL_LOCAL_MEM_SIZE , sizeof(local), &local, NULL);
	printf("OPENCL: clGetKernelWorkGroupInfo (kernel memory): %s\n", ocl_wrapper->get_error(err));
	printf("OPENCL: Kernel local memory use: %d Bytes\n", (int)local);

	// Copy input data to GPU, compute, copy results back
	// Runs asynchronous to host, up until blocking read at end

	// Write data from host to GPU
	err = clEnqueueWriteBuffer (ocl_wrapper->commandQue, src_buffer, CL_FALSE, 0, sizeof(unsigned char) * iNumElements, src_array, 0, NULL, NULL);
	printf("OPENCL: clEnqueueWriteBuffer: %s\n", ocl_wrapper->get_error(err));

	// Launch kernel 
	err = clEnqueueNDRangeKernel (ocl_wrapper->commandQue, ckKernel, 3, NULL, global_threads, NULL, 0, NULL, NULL);
	printf("OPENCL: clEnqueueNDRangeKernel: %s\n", ocl_wrapper->get_error(err));

	// Blocking read of results from GPU to Host
	err = clEnqueueReadBuffer (ocl_wrapper->commandQue, dst_buffer, CL_TRUE, 0, sizeof(unsigned char) * iNumElements, dst_array, 0, NULL,  NULL);
	printf("OPENCL: clEnqueueReadBuffer: %s\n", ocl_wrapper->get_error(err));

	// Cleanup allocated objects
	printf("OPENCL: Releasing kernel memory\n");
    if(ckKernel)clReleaseKernel(ckKernel); 
   
    //need to release any other OpenCL memory objects here
    if(dst_buffer)clReleaseMemObject(dst_buffer);
    if(src_buffer)clReleaseMemObject(src_buffer);
}
void OpenCLExecuter::ocl_filterGaussian_init(float sigma, float size)
{
	Gaussian gauss;
	gauss.sigma = sigma;
	gauss.size = size;
	filter_width = (size-1)/2.0;
	filter_kernel = gauss.gaussianmask1Df(gauss.sigma, gauss.size);
}
void OpenCLExecuter::ocl_filter(unsigned char* src_array, unsigned char* dst_array, int w, int h, int d)
{
	cl_int err;							// debugging variables
	size_t szParmDataBytes;				// Byte size of context information        
	cl_mem src_buffer;					// OpenCL device source buffer
	cl_mem dst_buffer;					// OpenCL device source buffer
	size_t szGlobalWorkSize;			// 1D var for Total # of work items
	size_t szLocalWorkSize;				// 1D var for # of work items in the work group
	cl_kernel ckKernel;					// OpenCL kernel

	int iNumElements = w*h*d; // Length of float arrays to process

	//temp array
	unsigned char* data = new unsigned char[iNumElements];

	size_t global_threads[3] ={w,h,d};

	// allocate the source buffer memory object
	src_buffer = clCreateBuffer (ocl_wrapper->context, CL_MEM_READ_ONLY,  sizeof(unsigned char) * iNumElements, NULL, &err);
	printf("OPENCL: clCreateBuffer: %s\n", ocl_wrapper->get_error(err));
		
	// allocate the destination buffer memory object
	dst_buffer = clCreateBuffer (ocl_wrapper->context, CL_MEM_WRITE_ONLY,  sizeof(unsigned char) * iNumElements, NULL, &err);
	printf("OPENCL: clCreateBuffer: %s\n", ocl_wrapper->get_error(err));

    // Create the kernel
	ckKernel = clCreateKernel (cpProgram, "myFunc", &err);
	printf("OPENCL: clCreateKernel: %s\n", ocl_wrapper->get_error(err));
  
	// Set the Argument values
	err = clSetKernelArg (ckKernel, 0, sizeof(cl_mem), (void*)&src_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 1, sizeof(cl_mem), (void*)&dst_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 2, sizeof(int), (void*)&w);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 3, sizeof(int), (void*)&h);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 4, sizeof(int), (void*)&d);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));

	/*
	size_t local;
	err = clGetKernelWorkGroupInfo(ckKernel, ocl_wrapper->devices[ocl_wrapper->deviceUsed], CL_KERNEL_LOCAL_MEM_SIZE , sizeof(local), &local, NULL);
	printf("OPENCL: clGetKernelWorkGroupInfo (kernel memory): %s\n", ocl_wrapper->get_error(err));
	printf("OPENCL: Kernel local memory use: %d Bytes\n", (int)local);
	*/

	// Copy input data to GPU, compute, copy results back
	// Runs asynchronous to host, up until blocking read at end

	// Write data from host to GPU
	err = clEnqueueWriteBuffer (ocl_wrapper->commandQue, src_buffer, CL_FALSE, 0, sizeof(unsigned char) * iNumElements, src_array, 0, NULL, NULL);
	printf("OPENCL: clEnqueueWriteBuffer: %s\n", ocl_wrapper->get_error(err));
	
	// Launch kernel 
	err = clEnqueueNDRangeKernel (ocl_wrapper->commandQue, ckKernel, 3, NULL, global_threads, NULL, 0, NULL, NULL);
	printf("OPENCL: clEnqueueNDRangeKernel: %s\n", ocl_wrapper->get_error(err));

	// Blocking read of results from GPU to Host
	err = clEnqueueReadBuffer (ocl_wrapper->commandQue, dst_buffer, CL_TRUE, 0, sizeof(unsigned char) * iNumElements, dst_array, 0, NULL,  NULL);
	printf("OPENCL: clEnqueueReadBuffer: %s\n", ocl_wrapper->get_error(err));

	// Cleanup allocated objects
	printf("OPENCL: Releasing kernel memory\n");
    if(ckKernel)clReleaseKernel(ckKernel); 
   
    //need to release any other OpenCL memory objects here
    if(dst_buffer)clReleaseMemObject(dst_buffer);
    if(src_buffer)clReleaseMemObject(src_buffer);
}

void OpenCLExecuter::ocl_filterGaussian(unsigned char* src_array, unsigned char* dst_array, int w, int h, int d)
{
//	printf("gaussian_sum: %f\n", gaussian_sum);
	printf("gaussian_width: %d\n", filter_width);
	printf("gaussian_mask size: %d\n", filter_kernel.size());

	cl_int err;							// debugging variables
	size_t szParmDataBytes;				// Byte size of context information        
	cl_mem src_buffer;					// OpenCL device source buffer
	cl_mem gauss_buffer;				// OpenCL device source buffer
	cl_mem dst_buffer;					// OpenCL device source buffer
	size_t szGlobalWorkSize;			// 1D var for Total # of work items
	size_t szLocalWorkSize;				// 1D var for # of work items in the work group
	cl_kernel ckKernel;					// OpenCL kernel

	int iNumElements = w*h*d; // Length of float arrays to process

	size_t global_threads[3] ={w,h,d};

	// allocate the source buffer memory object
	src_buffer = clCreateBuffer (ocl_wrapper->context, CL_MEM_READ_WRITE,  sizeof(unsigned char) * iNumElements, NULL, &err);
	printf("OPENCL: clCreateBuffer: %s\n", ocl_wrapper->get_error(err));
		
	gauss_buffer = clCreateBuffer (ocl_wrapper->context, CL_MEM_READ_ONLY,  sizeof(float) * filter_kernel.size(), NULL, &err);
	printf("OPENCL: clCreateBuffer: %s\n", ocl_wrapper->get_error(err));

	// allocate the destination buffer memory object
	dst_buffer = clCreateBuffer (ocl_wrapper->context, CL_MEM_READ_WRITE,  sizeof(unsigned char) * iNumElements, NULL, &err);
	printf("OPENCL: clCreateBuffer: %s\n", ocl_wrapper->get_error(err));
 
	//==================================================
	// X axis 
	//==================================================

	// Create the kernel
	ckKernel = clCreateKernel (cpProgram, "gaussianX", &err);
	printf("OPENCL: clCreateKernel: %s\n", ocl_wrapper->get_error(err));
  
	// Set the Argument values
	err = clSetKernelArg (ckKernel, 0, sizeof(cl_mem), (void*)&src_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 1, sizeof(cl_mem), (void*)&dst_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 2, sizeof(cl_mem), (void*)&gauss_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 3, sizeof(int), (void*)&filter_width);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 4, sizeof(int), (void*)&w);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 5, sizeof(int), (void*)&h);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 6, sizeof(int), (void*)&d);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));

	/*size_t local;
	err = clGetKernelWorkGroupInfo(ckKernel, ocl_wrapper->devices[ocl_wrapper->deviceUsed], CL_KERNEL_LOCAL_MEM_SIZE , sizeof(local), &local, NULL);
	printf("OPENCL: clGetKernelWorkGroupInfo (kernel memory): %s\n", ocl_wrapper->get_error(err));
	printf("OPENCL: Kernel local memory use: %d Bytes\n", (int)local);*/

	// Copy input data to GPU, compute, copy results back
	// Runs asynchronous to host, up until blocking read at end

	// Write data from host to GPU
	err = clEnqueueWriteBuffer (ocl_wrapper->commandQue, src_buffer, CL_FALSE, 0, sizeof(unsigned char) * iNumElements, src_array, 0, NULL, NULL);
	printf("OPENCL: clEnqueueWriteBuffer: %s\n", ocl_wrapper->get_error(err));
	
	err = clEnqueueWriteBuffer (ocl_wrapper->commandQue, gauss_buffer, CL_FALSE, 0, sizeof(float) * filter_kernel.size(), &filter_kernel[0], 0, NULL, NULL);
	printf("OPENCL: clEnqueueWriteBuffer: %s\n", ocl_wrapper->get_error(err));

	// Launch kernel 
	err = clEnqueueNDRangeKernel (ocl_wrapper->commandQue, ckKernel, 3, NULL, global_threads, NULL, 0, NULL, NULL);
	printf("OPENCL: clEnqueueNDRangeKernel: %s\n", ocl_wrapper->get_error(err));

	// Blocking read of results from GPU to Host
	//err = clEnqueueReadBuffer (ocl_wrapper->commandQue, dst_buffer, CL_TRUE, 0, sizeof(unsigned char) * iNumElements, dst_array, 0, NULL,  NULL);
	//printf("OPENCL: clEnqueueReadBuffer: %s\n", ocl_wrapper->get_error(err));

	// Cleanup allocated objects
	printf("OPENCL: Releasing kernel memory\n");
    if(ckKernel)clReleaseKernel(ckKernel); 
   
	//==================================================
	// Y axis 
	//==================================================

	// Create the kernel
	ckKernel = clCreateKernel (cpProgram, "gaussianY", &err);
	printf("OPENCL: clCreateKernel: %s\n", ocl_wrapper->get_error(err));
  
	// Set the Argument values
	err = clSetKernelArg (ckKernel, 0, sizeof(cl_mem), (void*)&dst_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 1, sizeof(cl_mem), (void*)&src_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 2, sizeof(cl_mem), (void*)&gauss_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 3, sizeof(int), (void*)&filter_width);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 4, sizeof(int), (void*)&w);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 5, sizeof(int), (void*)&h);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 6, sizeof(int), (void*)&d);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));

/*	size_t local;
	err = clGetKernelWorkGroupInfo(ckKernel, ocl_wrapper->devices[ocl_wrapper->deviceUsed], CL_KERNEL_LOCAL_MEM_SIZE , sizeof(local), &local, NULL);
	printf("OPENCL: clGetKernelWorkGroupInfo (kernel memory): %s\n", ocl_wrapper->get_error(err));
	printf("OPENCL: Kernel local memory use: %d Bytes\n", (int)local);
*/
	// Copy input data to GPU, compute, copy results back
	// Runs asynchronous to host, up until blocking read at end

	// Write data from host to GPU
	//err = clEnqueueWriteBuffer (ocl_wrapper->commandQue, src_buffer, CL_FALSE, 0, sizeof(unsigned char) * iNumElements, src_array, 0, NULL, NULL);
	//printf("OPENCL: clEnqueueWriteBuffer: %s\n", ocl_wrapper->get_error(err));
	
	err = clEnqueueWriteBuffer (ocl_wrapper->commandQue, gauss_buffer, CL_FALSE, 0, sizeof(float) * filter_kernel.size(), &filter_kernel[0], 0, NULL, NULL);
	printf("OPENCL: clEnqueueWriteBuffer: %s\n", ocl_wrapper->get_error(err));

	// Launch kernel 
	err = clEnqueueNDRangeKernel (ocl_wrapper->commandQue, ckKernel, 3, NULL, global_threads, NULL, 0, NULL, NULL);
	printf("OPENCL: clEnqueueNDRangeKernel: %s\n", ocl_wrapper->get_error(err));

	// Blocking read of results from GPU to Host
	//err = clEnqueueReadBuffer (ocl_wrapper->commandQue, dst_buffer, CL_TRUE, 0, sizeof(unsigned char) * iNumElements, dst_array, 0, NULL,  NULL);
	//printf("OPENCL: clEnqueueReadBuffer: %s\n", ocl_wrapper->get_error(err));

	// Cleanup allocated objects
	printf("OPENCL: Releasing kernel memory\n");
    if(ckKernel)clReleaseKernel(ckKernel); 
   
	//==================================================
	// Z axis 
	//==================================================

	// Create the kernel
	ckKernel = clCreateKernel (cpProgram, "gaussianZ", &err);
	printf("OPENCL: clCreateKernel: %s\n", ocl_wrapper->get_error(err));
  
	// Set the Argument values
	err = clSetKernelArg (ckKernel, 0, sizeof(cl_mem), (void*)&src_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 1, sizeof(cl_mem), (void*)&dst_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 2, sizeof(cl_mem), (void*)&gauss_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 3, sizeof(int), (void*)&filter_width);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 4, sizeof(int), (void*)&w);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 5, sizeof(int), (void*)&h);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 6, sizeof(int), (void*)&d);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));

	/*size_t local;
	err = clGetKernelWorkGroupInfo(ckKernel, ocl_wrapper->devices[ocl_wrapper->deviceUsed], CL_KERNEL_LOCAL_MEM_SIZE , sizeof(local), &local, NULL);
	printf("OPENCL: clGetKernelWorkGroupInfo (kernel memory): %s\n", ocl_wrapper->get_error(err));
	printf("OPENCL: Kernel local memory use: %d Bytes\n", (int)local);
	*/

	// Copy input data to GPU, compute, copy results back
	// Runs asynchronous to host, up until blocking read at end

	//Prepare data to upload
	//for(int j=0; j<iNumElements; j++)
	//	data[j] = volobj->texture3d[3*j+0];

	// Write data from host to GPU
	//err = clEnqueueWriteBuffer (ocl_wrapper->commandQue, src_buffer, CL_FALSE, 0, sizeof(unsigned char) * iNumElements, src_array, 0, NULL, NULL);
	//printf("OPENCL: clEnqueueWriteBuffer: %s\n", ocl_wrapper->get_error(err));
	
	err = clEnqueueWriteBuffer (ocl_wrapper->commandQue, gauss_buffer, CL_FALSE, 0, sizeof(float) * filter_kernel.size(), &filter_kernel[0], 0, NULL, NULL);
	printf("OPENCL: clEnqueueWriteBuffer: %s\n", ocl_wrapper->get_error(err));

	// Launch kernel 
	err = clEnqueueNDRangeKernel (ocl_wrapper->commandQue, ckKernel, 3, NULL, global_threads, NULL, 0, NULL, NULL);
	printf("OPENCL: clEnqueueNDRangeKernel: %s\n", ocl_wrapper->get_error(err));

	// Blocking read of results from GPU to Host
	err = clEnqueueReadBuffer (ocl_wrapper->commandQue, dst_buffer, CL_TRUE, 0, sizeof(unsigned char) * iNumElements, dst_array, 0, NULL,  NULL);
	printf("OPENCL: clEnqueueReadBuffer: %s\n", ocl_wrapper->get_error(err));

	// Cleanup allocated objects
	printf("OPENCL: Releasing kernel memory\n");
    if(ckKernel)clReleaseKernel(ckKernel); 
   
    //need to release any other OpenCL memory objects here
    if(dst_buffer)clReleaseMemObject(dst_buffer);
    if(src_buffer)clReleaseMemObject(src_buffer);
    if(gauss_buffer)clReleaseMemObject(gauss_buffer);
}

void OpenCLExecuter::ocl_parrallelReduction(void)
{
	cl_int err;							// debugging variables
	size_t szParmDataBytes;				// Byte size of context information        
	cl_mem src_buffer;					// OpenCL device source buffer
	cl_mem tmp_buffer;					// OpenCL device source buffer
	cl_mem dst_buffer;					// OpenCL device source buffer
	size_t szGlobalWorkSize;			// 1D var for Total # of work items
	size_t szLocalWorkSize;				// 1D var for # of work items in the work group
	size_t numWorkGroups;
	cl_kernel ckKernel;					// OpenCL kernel

	int iNumElements = 65536; //65536 // Length of float arrays to process

	// set Local work size dimensions
	szLocalWorkSize = 512;
	// set Global work size dimensions
	szGlobalWorkSize = roundup((int) iNumElements/szLocalWorkSize, 0)*szLocalWorkSize;  
	//szGlobalWorkSize = iNumElements;
	numWorkGroups = (float)szGlobalWorkSize/(float)szLocalWorkSize;
	printf("OPENCL: number of elements: %d\n", (int)iNumElements);
	printf("OPENCL: local worksize: %d\n", (int)szLocalWorkSize);
	printf("OPENCL: global worksize: %d\n", (int)szGlobalWorkSize);
	printf("OPENCL: work groups: %d\n", (int)(numWorkGroups));
	
	//temp array
	int* data = new int[iNumElements];

	for(int i=0; i<iNumElements; i++)
		data[i] = randomFloat(1.0, (float)iNumElements);

	data[iNumElements/2] = -100.0;

	//for(int i=0; i<iNumElements; i++)
	//	printf("data: %d\n", data[i]);

	size_t global_threads[1] ={iNumElements};

	// allocate the source buffer memory object
	src_buffer = clCreateBuffer (ocl_wrapper->context, CL_MEM_READ_ONLY,  sizeof(int) * iNumElements, NULL, &err);
	printf("OPENCL: clCreateBuffer: %s\n", ocl_wrapper->get_error(err));
		
	// allocate the temp buffer memory object
	tmp_buffer = clCreateBuffer (ocl_wrapper->context, CL_MEM_READ_WRITE,  sizeof(int) * iNumElements, NULL, &err);
	printf("OPENCL: clCreateBuffer: %s\n", ocl_wrapper->get_error(err));
	
	// allocate the destination buffer memory object
	dst_buffer = clCreateBuffer (ocl_wrapper->context, CL_MEM_WRITE_ONLY,  sizeof(int) * iNumElements, NULL, &err);
	printf("OPENCL: clCreateBuffer: %s\n", ocl_wrapper->get_error(err));

    // Create the kernel
	ckKernel = clCreateKernel (cpProgram, "min_reduce", &err);
	printf("OPENCL: clCreateKernel: %s\n", ocl_wrapper->get_error(err));
  
	// Set the Argument values
	err = clSetKernelArg (ckKernel, 0, sizeof(cl_mem), (void*)&src_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 1, sizeof(int)*szLocalWorkSize, NULL);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 2, sizeof(int), (void*)&iNumElements);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));
	err = clSetKernelArg (ckKernel, 3, sizeof(cl_mem), (void*)&dst_buffer);
	printf("OPENCL: clSetKernelArg: %s\n", ocl_wrapper->get_error(err));

	// Copy input data to GPU, compute, copy results back
	// Runs asynchronous to host, up until blocking read at end
	
	int numb_iterations = sqrt((float)numWorkGroups);
	numb_iterations=0;
	bool cont = true;

	Timer timer;
	timer.startTimer();
	//for(int i=0; i<numb_iterations; i++)
	while(cont)
	{
		// Write data from host to GPU
		err = clEnqueueWriteBuffer (ocl_wrapper->commandQue, src_buffer, CL_FALSE, 0, sizeof(int) * iNumElements, data, 0, NULL, NULL);
		printf("OPENCL: clEnqueueWriteBuffer: %s\n", ocl_wrapper->get_error(err));
	
		// Launch kernel 
		err = clEnqueueNDRangeKernel (ocl_wrapper->commandQue, ckKernel, 1, NULL, &szGlobalWorkSize, &szLocalWorkSize, 0, NULL, NULL);
		printf("OPENCL: clEnqueueNDRangeKernel: %s\n", ocl_wrapper->get_error(err));

		// Blocking read of results from GPU to Host
		err = clEnqueueReadBuffer (ocl_wrapper->commandQue, dst_buffer, CL_TRUE, 0, sizeof(int) * iNumElements, data, 0, NULL,  NULL);
		printf("OPENCL: clEnqueueReadBuffer: %s\n", ocl_wrapper->get_error(err));
		
		numb_iterations++;
		if(data[1]==0) cont = false;

		//printf("min: %d\n", data[0]);
		for(int i=0; i<numWorkGroups; i++)
			printf("min: %d\n", data[i]);
	}
	timer.endTimer("GPU find min");

	timer.startTimer();
	int min=iNumElements;
	for(int i=0; i<iNumElements; i++)
		if(data[i]<min) min = data[i];
	timer.endTimer("CPU find min");

	printf("iters: %d\n", numb_iterations);
	printf("gpu-min: %d\n", data[0]);
	printf("cpu-min: %d\n", min);
	
	// Cleanup allocated objects
	printf("OPENCL: Releasing kernel memory\n");
    if(ckKernel)clReleaseKernel(ckKernel); 


    //need to release any other OpenCL memory objects here
    if(dst_buffer)clReleaseMemObject(dst_buffer);
    if(src_buffer)clReleaseMemObject(src_buffer);


//	printf("min: %d\n", data[0]);

	delete[] data;
}

void OpenCLExecuter::printBuildLog(cl_program* prog)
{
	// debugging variables
	char *build_log;
	cl_int err;
	size_t ret_val_size;

	err = clGetProgramBuildInfo(*prog, ocl_wrapper->devices[ocl_wrapper->deviceUsed], CL_PROGRAM_BUILD_LOG, 0, NULL, &ret_val_size);
	printf("OPENCL: clGetProgramBuildInfo: %s\n", ocl_wrapper->get_error(err));

	build_log = new char[ret_val_size+1];
	err = clGetProgramBuildInfo(*prog, ocl_wrapper->devices[ocl_wrapper->deviceUsed], CL_PROGRAM_BUILD_LOG, ret_val_size, build_log, NULL);
	build_log[ret_val_size] = '\0';
	printf("OPENCL BUILD LOG: \n %s", build_log);
}
char* OpenCLExecuter::ocl_file_contents(const char *filename, int *length)
{
    FILE *f = fopen(filename, "r");
    void *buffer;

    if (!f) {
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    *length = ftell(f);
    fseek(f, 0, SEEK_SET);

    buffer = malloc(*length+1);
    *length = fread(buffer, 1, *length, f);
    fclose(f);
    ((char*)buffer)[*length] = '\0';

    return (char*)buffer;
}

#endif