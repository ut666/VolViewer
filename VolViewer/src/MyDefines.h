/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef MYDEFINES_H
#define MYDEFINES_H

//#define BIOPTONICS_SUPPORT
//#define OMERO_SUPPORT
//#define LIBTIFF_SUPPORT
//#define BROWSER5D_SUPPORT
//#define OPENCL_SUPPORT

#ifdef OMERO_SUPPORT
	#include <omero/all.h>
	using namespace Ice;
#endif //OMERO_SUPPORT

#endif //MYDEFINES_H
