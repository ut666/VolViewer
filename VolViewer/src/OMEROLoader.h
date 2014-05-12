/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef OMEROLOADER_H
#define OMEROLOADER_H

#include "Global.h"
#include "MyDefines.h"

#ifdef OMERO_SUPPORT

#include <Q3ProgressDialog.h>

// Domain
#include <omero/all.h>
#include <omero/client.h>
#include <omero/model/Experimenter.h>
#include <omero/model/Project.h>
#include <omero/model/Dataset.h>
#include <omero/model/Image.h>
#include <omero/model/Pixels.h>
#include <omero/model/PixelsType.h>
#include <omero/model/Channel.h>
#include <omero/model/LogicalChannel.h>
#include <omero/model/StatsInfo.h>
#include <omero/model/DatasetImageLink.h>
#include <omero/model/DatasetImageLinkI.h>

#include <omero/sys/ParametersI.h>

// Std
#include <stdio.h>
#include <iostream>
#include <cassert>
#include <vector>
#include <time.h>
#include <map>

using namespace std;

class OMEROLoader
{
private:
	omero::client_ptr omero; 
	omero::api::ServiceFactoryPrx sf;
	omero::api::IPixelsPrx pixelsService;
	omero::api::RawPixelsStorePrx rawPixelsStore;
	omero::api::IContainerPrx containerService;
	omero::api::IQueryPrx queryService;
	omero::api::RenderingEnginePrx renderingEngine;

public:
	bool showGUI;

	omero::api::ServiceList services;
	int xdim, ydim, zdim, cdim, tdim;
	double xphysize, yphysize, zphysize;
	string pixtype;
	vector < double > channel_min;
	vector < double > channel_max;

	string svrname;
	string username;
	string psswd;
	string sessionID;
	int port;

	long int imageID;

	OMEROLoader();
	~OMEROLoader();

	void reset(void);
	int connect(string sn, string sid);
	int connect(string sn, string un, string pd, int p);
	int connect(void);
	void disconnect(void);

	int createServices(void);

	int loadImageMetadata(long imgId);
	unsigned char* getImagePlane(long z, long t);
	unsigned char* getImagePlane(long z, long c, long t);
	void getImagePlane(long z, long c, long t, unsigned char* dest);

	void saveImage(string name, string description, int x, int y, int z, int c, int t, double physicalX, double physicalY, double physicalZ, unsigned char* src);
	void saveImage(int long datasetID, string name, string description, int x, int y, int z, int c, int t, double physicalX, double physicalY, double physicalZ, unsigned char* src);

	void getAllProjectsDatasetsImages(void);
	void getProjectImages(int long projid, vector<int long> *imgIDs);
	void getDatasetImages(int long dsid, vector<int long> *imgIDs);

	//void debug(void);
};
#endif //OMERO_SUPPORT

#endif	//OMEROLoader
