/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef RAW_H
#define RAW_H

#include <stdarg.h>
#include <fstream>
#include <iostream>
#include <string>
#include <qgl.h>

using namespace std; 

#include "Vector.h"
#include "ColourFun.h"

class RAW_volume
{

public:

	int loadmode;
	int res_x, res_y, res_z;
	int channels;
	float WLZ_scale;

	RAW_volume();
	~RAW_volume();

	void load_DAT(const std::string &filename);
	void load_RAW(int z, FILE *f, unsigned char* tex_buffer, int mode, int texy, int texx, double* hist, bool* red, bool* green, bool* blue, bool* greyscale);

	void save_DAT(const std::string &filename, int x, int y, int z, float scale);
	void save_RAW(FILE *f, unsigned char* vol_buffer, int resx, int resy, int z);

};

#endif //RAW_H

