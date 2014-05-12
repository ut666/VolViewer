/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef OPENCL_WRAP_H
#define OPENCL_WRAP_H

#include "MyDefines.h"

#ifdef OPENCL_SUPPORT
//#pragma OPENCL EXTENSION cl_khr_3d_image_writes : enable

#include <qgl.h>
#include <vector>
using namespace std;

#include "glext.h"

// All OpenCL headers
#if defined (__APPLE__) || defined(MACOSX)
    #include <OpenCL/opencl.h>
    #define GL_SHARING_EXTENSION "cl_APPLE_gl_sharing"
#else
    #include <CL/opencl.h>
   #define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#endif

class OpenCLWrapper
{
public:

	OpenCLWrapper();
	~OpenCLWrapper();
	
	int							platform_index;
	vector< string >			platform_list;

	cl_platform_id*				platformIDs;	// The available OpenCL platforms

	cl_uint						num_platforms;	// Number of OpenCL platforms
	cl_platform_id				platformUsed;	// OpenCL platform
	cl_context					context;		// OpenCL context
	cl_device_id*				devices;		// OpenCL device list    
	cl_command_queue			commandQue;		// OpenCL command queue
	cl_uint						numDevices;		// Number of OpenCL devices
	unsigned int				deviceUsed;		// OpenCL device used
	cl_int						err;			// debugging variables

	cl_int		get_PlatformID(cl_platform_id* clSelectedPlatformID);	//Get the avialable OpenCL platforms
	void		init_PlatformAndDevices(void);							//Init our Platform and Device list (only GPUs atm)
	void		create_NewContext(void);								//Create a new OpenCL context
	void		create_NewMultiDeviceContext(void);						//Create a new OpenCL multi-device context
	void		create_SharedContext(void);								//Create a shared OpenGL/OpenCL context (so can share objects)
	void		print_DeviceInfo(void);									//Prints general information about the used device

	void		switch_Platform(int i);									//Switch the current platform
	const char* get_error(cl_int error);
};

#endif	//OPENCL_SUPPORT

#endif	//OPENCL_WRAP_H