#include "OpenCLWrapper.h"

#ifdef OPENCL_SUPPORT

OpenCLWrapper::OpenCLWrapper()
{
	platformIDs = NULL;
	platformUsed = NULL;
	context = NULL;
	devices = NULL;
	commandQue = NULL;
	numDevices = NULL;
	deviceUsed = NULL;
	num_platforms = NULL;
}

OpenCLWrapper::~OpenCLWrapper()
{
	printf("OPENCL: Releasing OpenCL memory\n");

 	if(commandQue) clReleaseCommandQueue(commandQue);
    if(context) clReleaseContext(context);
    if(devices) delete(devices);
	if(platformIDs) free(platformIDs);

	printf("OPENCL: Memory released\n");
}
void OpenCLWrapper::switch_Platform(int i)
{
	if(i>platform_list.size()) return;

	platform_index = i;
	platformUsed = platformIDs[i];

	// Get the number of GPU devices available to the platform
    // we should probably expose the device type to the user
    // the other common option is CL_DEVICE_TYPE_CPU, CL_DEVICE_TYPE_GPU
    err = clGetDeviceIDs(platformUsed, CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
    printf("OPENCL: clGetDeviceIDs (get number of devices): %s\n", get_error(err));
	printf("OPENCL: Found %d devices.\n", numDevices);

	if(numDevices==0) 
	{
		printf("ERROR: No OpenCL devices have been found!!!\n");
		return;
	}

    // Create the device list
    devices = new cl_device_id [numDevices];
    err = clGetDeviceIDs(platformUsed, CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
    printf("OPENCL: clGetDeviceIDs (create device list): %s\n", get_error(err));

	print_DeviceInfo();

    //for right now we just use the first device
    deviceUsed = 0;

    //create a new context
    if(context) clReleaseContext(context);
    context = clCreateContext(0, 1, devices, NULL, NULL, &err);
	printf("OPENCL: clCreateContext: %s\n", get_error(err));

    //create the command queue we will use to execute OpenCL commands
 	if(commandQue) clReleaseCommandQueue(commandQue);
    commandQue = clCreateCommandQueue(context, devices[deviceUsed], 0, &err);
	printf("OPENCL: clCreateCommandQueue: %s\n", get_error(err));
}
void OpenCLWrapper::init_PlatformAndDevices(void)
{
	//get available platforms and select a device.
    printf("OPENCL: Initialize OpenCL objects\n");
    
    //get our platform id
    err = get_PlatformID(&platformUsed);
    printf("OPENCL: oclGetPlatformID: %s\n", get_error(err));

	//Try the NVIDIA platform
	char* platstr =  "NVIDIA";
	char chBuffer[1024];
    printf("OPENCL: Requesting platform: %s\n", platstr);
    for(int i=0; i<num_platforms; ++i)
    {
        err = clGetPlatformInfo(platformIDs[i], CL_PLATFORM_NAME, 1024, &chBuffer, NULL);
        if(err == CL_SUCCESS)
        {
            if(strstr(chBuffer, platstr) != NULL)
            {
                printf("OPENCL:\t\tUsing platform %d: %s\n", i, chBuffer);
				platformUsed = platformIDs[i];
				platform_index = i;
            }
        }
    }

	//If no NVIDIA try AMD
    if(platformUsed == NULL)
	{
		platstr =  "AMD Accelerated Parallel Processing";
		printf("OPENCL: Requesting platform: %s\n", platstr);
		for(int i=0; i<num_platforms; ++i)
		{
			err = clGetPlatformInfo(platformIDs[i], CL_PLATFORM_NAME, 1024, &chBuffer, NULL);
			if(err == CL_SUCCESS)
			{
				if(strstr(chBuffer, platstr) != NULL)
				{
					printf("OPENCL:\t\tUsing platform %d: %s\n", i, chBuffer);
					platformUsed = platformIDs[i];
					platform_index = i;
				}
			}
		}
	}

    // If still nothing default to zeroeth platform
    if(platformUsed == NULL)
    {
        platformUsed = platformIDs[0];
 		platform_index = 0;
	    err = clGetPlatformInfo (platformIDs[0], CL_PLATFORM_NAME, 1024, &chBuffer, NULL);

        printf("WARNING: Requested OpenCL platforms not found!\n");
        printf("WARNING: Defaulting to platform %d: %s\n", 0, chBuffer);
    }

	// Get the number of devices available to the platform
    // we should probably expose the device type to the user
    // the other common option is CL_DEVICE_TYPE_CPU, CL_DEVICE_TYPE_GPU
    err = clGetDeviceIDs(platformUsed, CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);
    printf("OPENCL: clGetDeviceIDs (get number of devices): %s\n", get_error(err));
	printf("OPENCL: Found %d devices.\n", numDevices);

	if(numDevices==0) 
	{
		printf("ERROR: No OpenCL devices have been found!!!\n");
		return;
	}

    // Create the device list
    devices = new cl_device_id [numDevices];
    err = clGetDeviceIDs(platformUsed, CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
    printf("OPENCL: clGetDeviceIDs (create device list): %s\n", get_error(err));

	print_DeviceInfo();

    //for right now we just use the first device
    deviceUsed = 0;
}
void OpenCLWrapper::print_DeviceInfo(void)
{
	size_t ret_size;
	cl_ulong globalmembytes;
    cl_ulong localmembytes;
	cl_bool isavailable;

	for(int i=0; i<numDevices; i++)
	{

		// Get some information about the returned device
		cl_char vendor_name[1024] = {0};
		cl_char device_name[1024] = {0};

		printf("OPENCL: Device %d\n", (int)i);

		err = clGetDeviceInfo(devices[i], CL_DEVICE_VENDOR, sizeof(vendor_name), vendor_name, &ret_size);
		printf("OPENCL:\t\t%s\n", vendor_name);

		err = clGetDeviceInfo(devices[i], CL_DEVICE_NAME, sizeof(device_name), device_name, &ret_size);
		printf("OPENCL:\t\t%s\n", device_name);

		err = clGetDeviceInfo(devices[i], CL_DEVICE_AVAILABLE, sizeof(cl_bool), &isavailable, &ret_size);
		//printf("OPENCL: clGetDeviceInfo (get device availability): %s\n", get_error(err));
		printf("OPENCL:\t\tavailable: %s\n", ((int)isavailable == (int)1) ? "TRUE" : "FALSE");

		err = clGetDeviceInfo(devices[i], CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), &globalmembytes, &ret_size );
		//printf("OPENCL: clGetDeviceInfo (get device global memory): %s\n", get_error(err));
		printf("OPENCL:\t\tglobal memory: %llu MB\n", globalmembytes/1024/1024);

		err = clGetDeviceInfo(devices[i], CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), &localmembytes, &ret_size );
		//printf("OPENCL: clGetDeviceInfo (get device local memory): %s\n", get_error(err));
		printf("OPENCL:\t\tlocal memory: %llu KB\n", localmembytes/1024);

	}
}
void OpenCLWrapper::create_NewContext(void)
{
	deviceUsed = 0;

    //create a new context
    context = clCreateContext(0, 1, devices, NULL, NULL, &err);
	printf("OPENCL: clCreateContext: %s\n", get_error(err));

    //create the command queue we will use to execute OpenCL commands
    commandQue = clCreateCommandQueue(context, devices[deviceUsed], 0, &err);
	printf("OPENCL: clCreateCommandQueue: %s\n", get_error(err));
}
void OpenCLWrapper::create_NewMultiDeviceContext(void)
{
    //create a new context
    context = clCreateContext(0, numDevices, devices, NULL, NULL, &err);
	printf("OPENCL: clCreateContext: %s\n", get_error(err));
}
void OpenCLWrapper::create_SharedContext(void)
{
    unsigned int deviceEnd = numDevices - 1;
    bool bSharingSupported = false;
    for(unsigned int i = deviceUsed; (!bSharingSupported && (i <= deviceEnd)); ++i) 
    {
        size_t extensionSize;
        err = clGetDeviceInfo(devices[i], CL_DEVICE_EXTENSIONS, 0, NULL, &extensionSize );
        printf("OPENCL: clGetDeviceInfo (get device extensions): %s\n", get_error(err));
        if(extensionSize > 0) 
        {
            char* extensions = (char*)malloc(extensionSize);
            err = clGetDeviceInfo(devices[i], CL_DEVICE_EXTENSIONS, extensionSize, extensions, &extensionSize);
            printf("OPENCL: clGetDeviceInfo: (get device extensions) %s\n", get_error(err));
            std::string stdDevString(extensions);            
			free(extensions);

            size_t szOldPos = 0;
            size_t szSpacePos = stdDevString.find(' ', szOldPos); // extensions string is space delimited
            while (szSpacePos != stdDevString.npos)
            {
				printf("\t EXTENSIONS: %s\n", stdDevString.substr(szOldPos, szSpacePos - szOldPos).c_str());
    
				if( strcmp(GL_SHARING_EXTENSION, stdDevString.substr(szOldPos, szSpacePos - szOldPos).c_str()) == 0 ) 
                {
                    // Device supports context sharing with OpenGL
                    deviceUsed = i;
                    bSharingSupported = true;
                    break;
                }
                do 
                {
                    szOldPos = szSpacePos + 1;
                    szSpacePos = stdDevString.find(' ', szOldPos);
                } 
                while (szSpacePos == szOldPos);
            }
        }
    }

    if(bSharingSupported) printf("OPENCL: Using CL-GL Interop\n");
	else printf("OPENCL: No device found that supports CL/GL context sharing");

	// Define OS-specific context properties and create the OpenCL context
    // We setup OpenGL context sharing slightly differently on each OS
    // this code comes mostly from NVIDIA's SDK examples
    // we could also check to see if the device supports GL sharing
    // but that is just searching through the properties
    // an example is avaible in the NVIDIA code
    #if defined (__APPLE__) || defined(MACOSX)
        CGLContextObj kCGLContext = CGLGetCurrentContext();
        CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
        cl_context_properties props[] =
        {
            CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup,
            0
        };

        //Apple's implementation is weird, and the default values assumed by cl.hpp don't work
        //this works
        //cl_context cxGPUContext = clCreateContext(props, 0, 0, NULL, NULL, &err);
        //these dont
        //cl_context cxGPUContext = clCreateContext(props, 1,(cl_device_id*)&devices.front(), NULL, NULL, &err);
        //cl_context cxGPUContext = clCreateContextFromType(props, CL_DEVICE_TYPE_GPU, NULL, NULL, &err);

		context = clCreateContext(props, 0,0, NULL, NULL, &err);
	#else
        #if defined WIN32 // Win32
            cl_context_properties props[] =
            {
                CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
                CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
                CL_CONTEXT_PLATFORM, (cl_context_properties)platformUsed,
                0
            };
            
			context = clCreateContext(props, 1, &devices[deviceUsed], NULL, NULL, &err);
      #else
            cl_context_properties props[] =
            {
                CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
                CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
                CL_CONTEXT_PLATFORM, (cl_context_properties)platformUsed,
                0
            };
            context = clCreateContext(props, 1, &devices[deviceUsed], NULL, NULL, &err);
        #endif
    #endif

 	printf("OPENCL: clCreateContext: %s\n", get_error(err));

    //create the command queue we will use to execute OpenCL commands
    commandQue = clCreateCommandQueue(context, devices[deviceUsed], 0, &err);
	printf("OPENCL: clCreateCommandQueue: %s\n", get_error(err));
}

cl_int OpenCLWrapper::get_PlatformID(cl_platform_id* clSelectedPlatformID)
{
	char chBuffer[1024];
	cl_int ciErrNum;
	*clSelectedPlatformID = NULL;
	cl_uint i = 0;

    // Get OpenCL platform count
    ciErrNum = clGetPlatformIDs (0, NULL, &num_platforms);
    if (ciErrNum != CL_SUCCESS)
    {
        printf("OPENCL: Error %i in clGetPlatformIDs Call !!!\n\n", ciErrNum);
        return -1000;
    }
    else
    {
        if(num_platforms == 0)
        {
            printf("OPENCL: No OpenCL platform found!\n\n");
            return -2000;
        }
        else
        {
            // if there's a platform or more, make space for ID's
            if ((platformIDs = (cl_platform_id*)malloc(num_platforms * sizeof(cl_platform_id))) == NULL)
            {
                printf("OPENCL: Failed to allocate memory for cl_platform ID's!\n\n");
                return -3000;
            }

            // get platform info for each platform and trap the NVIDIA platform if found
            ciErrNum = clGetPlatformIDs (num_platforms, platformIDs, NULL);
            printf("OPENCL: Available platforms:\n");
            for(i = 0; i < num_platforms; ++i)
            {
                ciErrNum = clGetPlatformInfo (platformIDs[i], CL_PLATFORM_NAME, 1024, &chBuffer, NULL);
                if(ciErrNum == CL_SUCCESS)
                {
                    printf("OPENCL:\t\tPlatform %d: %s\n", i, chBuffer);
					platform_list.push_back(chBuffer);
                }
            }
        }
    }

    return CL_SUCCESS;
}

// Helper function to get error string
// *********************************************************************
const char* OpenCLWrapper::get_error(cl_int error)
{
    static const char* errorString[] = {
        "CL_SUCCESS",
        "CL_DEVICE_NOT_FOUND",
        "CL_DEVICE_NOT_AVAILABLE",
        "CL_COMPILER_NOT_AVAILABLE",
        "CL_MEM_OBJECT_ALLOCATION_FAILURE",
        "CL_OUT_OF_RESOURCES",
        "CL_OUT_OF_HOST_MEMORY",
        "CL_PROFILING_INFO_NOT_AVAILABLE",
        "CL_MEM_COPY_OVERLAP",
        "CL_IMAGE_FORMAT_MISMATCH",
        "CL_IMAGE_FORMAT_NOT_SUPPORTED",
        "CL_BUILD_PROGRAM_FAILURE",
        "CL_MAP_FAILURE",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "",
        "CL_INVALID_VALUE",
        "CL_INVALID_DEVICE_TYPE",
        "CL_INVALID_PLATFORM",
        "CL_INVALID_DEVICE",
        "CL_INVALID_CONTEXT",
        "CL_INVALID_QUEUE_PROPERTIES",
        "CL_INVALID_COMMAND_QUEUE",
        "CL_INVALID_HOST_PTR",
        "CL_INVALID_MEM_OBJECT",
        "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
        "CL_INVALID_IMAGE_SIZE",
        "CL_INVALID_SAMPLER",
        "CL_INVALID_BINARY",
        "CL_INVALID_BUILD_OPTIONS",
        "CL_INVALID_PROGRAM",
        "CL_INVALID_PROGRAM_EXECUTABLE",
        "CL_INVALID_KERNEL_NAME",
        "CL_INVALID_KERNEL_DEFINITION",
        "CL_INVALID_KERNEL",
        "CL_INVALID_ARG_INDEX",
        "CL_INVALID_ARG_VALUE",
        "CL_INVALID_ARG_SIZE",
        "CL_INVALID_KERNEL_ARGS",
        "CL_INVALID_WORK_DIMENSION",
        "CL_INVALID_WORK_GROUP_SIZE",
        "CL_INVALID_WORK_ITEM_SIZE",
        "CL_INVALID_GLOBAL_OFFSET",
        "CL_INVALID_EVENT_WAIT_LIST",
        "CL_INVALID_EVENT",
        "CL_INVALID_OPERATION",
        "CL_INVALID_GL_OBJECT",
        "CL_INVALID_BUFFER_SIZE",
        "CL_INVALID_MIP_LEVEL",
        "CL_INVALID_GLOBAL_WORK_SIZE",
    };

    const int errorCount = sizeof(errorString) / sizeof(errorString[0]);
    const int index = -error;
    return (index >= 0 && index < errorCount) ? errorString[index] : "";
}
#endif