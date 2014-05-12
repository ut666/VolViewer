/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef VOLOBJ_H
#define VOLOBJ_H

#include <qstring.h>
#include <iostream>
#include <q3progressdialog.h>
#include <qgl.h>

#include "MyDefines.h"

#ifdef LIBTIFF_SUPPORT
	#include <tiffio.h>
	#include <tiff.h>
#endif

#include "Vector.h"

class VolumeObject
{

public:
	
	GLuint TEXTURE3D_RED;
	GLuint TEXTURE3D_GREEN;
	GLuint TEXTURE3D_BLUE;

	Q3ProgressDialog*		progressbar;

	float maxsize;
	float xsize, ysize, zsize;
	float xscale, yscale, zscale;
	float x_resize, y_resize, z_resize;

	int maxres;
	int maxsizeX, maxsizeY, maxsizeZ;

	int resized_texwidth, resized_texheight, resized_texdepth;

	Vector boundingboxCentre;
	Vector boundingboxSize;
	Vector boundingboxMin;
	Vector boundingboxMax;
	
	//filename of our data and type
	QString file_name;
	int file_type;

	//original dimensions of volume & volume array
	float maxdimension;
	int width, height, depth;

	//dimensions of texture array & texture array
	int texwidth, texheight, texdepth;

	bool is_greyscale;
	bool has_red, has_green, has_blue;
	unsigned char* texture3d;

#ifdef LIBTIFF_SUPPORT
	bool	is16bit;
	int voldatasize;
	uint16** voldata;
	double histogram16bit[65536];
#endif

	//histogram of our object
	double histogram[256*3];
		
	VolumeObject();
	~VolumeObject();

	Vector get_index3D(int index);
	int get_index1DinBoundingBox(int x, int y, int z);
	int get_index1D(int x, int y, int z);
	void resize(float factor);
	int makepower2(int val);
	void resetBoundingBox(void);
	void compute_boundingbox(void);
	void compute_boundingbox_channel(int channel, int window_size);
	void compute_boundingbox_channel_bounded(int channel, int window_size);
	void compute_fastboundingbox_channel(Vector centre, int channel, int window_size, float sizefactor);
	void create_volume(void);

	int get_value(int index1d, int c);
	int get_value(int x, int y, int z, int c);
	void set_value(int x, int y, int z, int c, int value);
	void set_value(int index1d, int c, int value);

	Vector worldspacetoimagespace(Vector v);
	Vector imagespacetoworldspace(Vector v);

	const VolumeObject& operator= (const VolumeObject& right);
};

#endif	//VOLOBJ
