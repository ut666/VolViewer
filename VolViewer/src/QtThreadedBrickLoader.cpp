/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "QtThreadedBrickLoader.h"

QtThreadedBrickLoader::QtThreadedBrickLoader(int ind, VolIO* vio, QStringList sList, int xs, int ys, int zs, int bX, int bY, int bZ, vector<unsigned char>* d, vector<bool>* eb)
{
	index = ind;
	volio = vio;
	data = d;

	xstart = xs;
	ystart = ys;
	zstart = zs;
	brickresX = bX;
	brickresY = bY;
	brickresZ = bZ;

	strList = sList;
	empty_brick = eb;
}
QtThreadedBrickLoader::~QtThreadedBrickLoader()
{
	//printf("Thread Finished: %d\n", index);
	//printf("isnonempty: %d\n", (int)empty_brick[index]);

	//emit isfinished_signal(index, isnonempty);
}

void QtThreadedBrickLoader::run()
{
	//bool* myarray = &(empty_brick->front());
	//myarray[index] = false;
	bool empty = volio->load_subimage(strList, xstart, ystart, zstart, brickresX, brickresY, brickresZ, &(data->front()));
	//if(empty==true) data->clear();
	//printf("empty: %d\n", (int)empty);
	//if(empty_brick[index]==true) data->clear();
}