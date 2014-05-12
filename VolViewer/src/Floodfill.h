/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef FLOODFILL_H
#define FLOODFILL_H

#include <math.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#include "VolumeObject.h"
#include "Filter.h"
#include "Point3D.h"
#include "ColourFun.h"
#include "Vector.h"
#include "PCA.h"
#include "PlaneWidgetInCube.h"

#define COLOUR_RULE	0
#define FILL_RULE	1
#define EDGE_RULE	2
#define DILATE_RULE	3
#define RCOLOUR_RULE	4
#define GCOLOUR_RULE	5
#define BCOLOUR_RULE	6
#define RCOLOURWINDOW_RULE	7
#define GCOLOURWINDOW_RULE	8
#define BCOLOURWINDOW_RULE	9

#define CONNECT6	0
#define CONNECT26	1

#define VISITEDARRAY	0
#define VISITEDDIRECT	1

class Floodfill
{

	public:

		int SRC_CHANNEL;
		int DST_CHANNEL;

		int WINDOW_SIZE;

		VolumeObject* volobj;
		
		int width,height,depth;
		bool*** visited_voxels;
		
		vector< vector< vector<bool> > > edges;
	
		vector< vector<int> > region_indices;
		
		vector< int > region_sizes;
		vector< Vector > region_colours;
		vector<Vector> region_colours_tokeep;
		vector<bool> regions_todelete;
	
		int regions;
		float scaleX, scaleY, scaleZ;
		int maxrecursiondepth;
		
		Floodfill();
		~Floodfill();
		
		void progress(int i, int size);

		void init(int w, int h, int d, int visitedmode);
		void init_visited(void);
		void floodfill(int rule);
		
		bool isVisited(int x, int y, int z, int mode);
		void markVisited(int x, int y, int z, int mode);
		void markUnVisited(int x, int y, int z, int mode);

		void applyfloodfillseeded(Point3D seedpoint, int src, int dst, int window);

		void applyfloodfillseededsmoothed(Point3D seedpoint, int src, int dst);

		void delete_visited(void);
		
		int get1Dindex(Point3D p);
		Point3D get3Dindex(int index);
		
		void search_neighborhood(int x, int y, int z, vector<Point3D>* stack, int rule, int connectivity, int visitmode);

		bool apply_rule(int x0, int y0, int z0, int x1, int y1, int z1, int rule);
		int getValue(Point3D p, bool red, bool green, bool blue);
		
		void eval_connectivity(void);
		
		void apply_regiothreshold(void);
		void apply_result(int minsize, int maxsize);
};

#endif //FLOODFILL_H