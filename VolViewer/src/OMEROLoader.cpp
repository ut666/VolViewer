/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "OMEROLoader.h"

#ifdef OMERO_SUPPORT

OMEROLoader::OMEROLoader()
{
	omero = NULL; 
	sf = NULL;
	pixelsService = NULL;
	rawPixelsStore = NULL;
	queryService = NULL;
	renderingEngine = NULL;
	containerService = NULL;

	svrname = "";
	username = "";
	psswd = "";
	sessionID = "-1";
	port = 0;
	imageID = -1;
	showGUI=true;
}
void OMEROLoader::reset(void)
{
	omero = NULL; 
	sf = NULL;
	pixelsService = NULL;
	rawPixelsStore = NULL;
	queryService = NULL;
	renderingEngine = NULL;
	containerService = NULL;

	svrname = "";
	username = "";
	psswd = "";
	sessionID = "-1";
	port = 0;
	imageID = -1;
}
OMEROLoader::~OMEROLoader()
{
}
void OMEROLoader::disconnect(void)
{
	printf("OMERO: Disconnecting all services & sessions\n");
	if(rawPixelsStore!=NULL) rawPixelsStore->close();
	if(renderingEngine!=NULL) renderingEngine->close();

	if(sf!=NULL) sf->close();
	if(omero!=NULL) omero->closeSession();

	omero = NULL; 
	sf = NULL;
	pixelsService = NULL;
	rawPixelsStore = NULL;
	queryService = NULL;
	renderingEngine = NULL;
	containerService = NULL;
	imageID = -1;

	//reset();
}

int OMEROLoader::createServices(void)
{
	//================
	//pixelsService
	//================
	try
	{
		pixelsService = sf->getPixelsService();
	} 
	catch (const omero::ServerError& serverError) 
	{
		printf("ERROR: sf->getPixelsService();\n");
		return -1;
	} 
	catch (const Ice::LocalException& localException)
	{
		printf("ERROR: sf->getPixelsService();\n");
		return -1;
	}

	//================
	//rawPixelsStore
	//================
	try 
	{
		rawPixelsStore = sf->createRawPixelsStore();
	}
	catch(const omero::ServerError& serverError)
	{
		printf("ERROR: sf->createRawPixelsStore();\n");
		return -1;
	}
	catch (const Ice::LocalException& localException)
	{
		printf("ERROR: sf->createRawPixelsStore();\n");
		return -1;
	}

	//================
	//containerService
	//================	
	try 
	{
		containerService = sf->getContainerService();
	}
	catch(const omero::ServerError& serverError)
	{
		printf("ERROR: sf->getContainerService();\n");
		return -1;
	}
	catch (const Ice::LocalException& localException)
	{
		printf("ERROR: sf->getContainerService();\n");
		return -1;
	}
	
	//================
	//renderingEngine
	//================	
	try 
	{
		renderingEngine = sf->createRenderingEngine();
	}
	catch(const omero::ServerError& serverError)
	{
		printf("ERROR: sf->createRenderingEngine();\n");
		return -1;
	}
	catch (const Ice::LocalException& localException)
	{
		printf("ERROR: sf->createRenderingEngine();\n");
		return -1;
	}

	//================
	//queryService
	//================	
	try 
	{
		queryService = sf->getQueryService();
	}
	catch(const omero::ServerError& serverError)
	{
		printf("ERROR: sf->getQueryService();\n");
		return -1;
	}
	catch (const Ice::LocalException& localException)
	{
		printf("ERROR: sf->getQueryService();\n");
		return -1;
	}

	if(pixelsService==NULL) printf("ERROR: No pixelsService!\n");
	if(rawPixelsStore==NULL) printf("ERROR: No rawPixelsStore!\n");
	if(queryService==NULL) printf("ERROR: No queryService!\n");
	if(renderingEngine==NULL) printf("ERROR: No renderingEngine!\n");
	if(containerService==NULL) printf("ERROR: No containerService!\n");

	omero::api::ServiceList services;
	services.push_back(rawPixelsStore);
	services.push_back(pixelsService);
	sf->keepAllAlive(services);

	return 0;
}
int OMEROLoader::connect(string sn, string sid)
{
	svrname = sn;
	sessionID = sid;
	
	omero = new omero::client(svrname);

	try
	{
		sf = omero->joinSession(sid);
	}
	catch (const Glacier2::CannotCreateSessionException& ccse) 
	{
		printf("ERROR: omero->joinSession();\n");
		return -1;
	}

	if(createServices()==-1)
	{
		printf("ERROR: Creating OMERO services;\n");
		return -1;
	}

	return 0;
}
int OMEROLoader::connect(string sn, string un, string pd, int p)
{
	svrname = sn;
	username = un;
	psswd = pd;
	port = p;

	omero = new omero::client(svrname, port);
	try 
	{
		sf = omero->createSession(username, psswd);
	} 
	catch (const Glacier2::CannotCreateSessionException& ccse) 
	{
		printf("ERROR: omero->createSession();\n");
		return -1;
	}
	catch (const Glacier2::PermissionDeniedException pde)
	{
		printf("ERROR: omero->createSession();\n");
		return -1;
	}

	if(createServices()==-1)
	{
		printf("ERROR: Creating OMERO services;\n");
		return -1;
	}

	return 0;
}
int OMEROLoader::connect(void)
{
	printf("OMERO: Creating client\n");

	omero = new omero::client(svrname); //port

	printf("OMERO: Creating session\n");

	if(sessionID=="-1")
	{
		try 
		{
			sf = omero->createSession(username, psswd);
		} 
		catch (const Glacier2::CannotCreateSessionException& ccse) 
		{
			printf("ERROR: omero->createSession();\n");
			return -1;
		}
		catch (const Glacier2::PermissionDeniedException pde)
		{
			printf("ERROR: omero->createSession();\n");
			return -1;
		}
	}
	else
	{
		try
		{
			sf = omero->joinSession(sessionID);
		}
		catch (const Glacier2::CannotCreateSessionException& ccse) 
		{
			printf("ERROR: omero->joinSession();\n");
			return -1;
		}
		catch (const Glacier2::PermissionDeniedException pde)
		{
			printf("ERROR: omero->joinSession();\n");
			return -1;
		}
	}

	if(createServices()==-1)
	{
		printf("ERROR: Creating OMERO services;\n");
		return -1;
	}

	return 0;
}
int OMEROLoader::loadImageMetadata(long imgId)
{
	//get the Image
	imageID = imgId;
	omero::sys::LongList imglist;
	imglist.push_back(imgId);
	omero::model::ImagePtr img = containerService->getImages("Image",imglist,0)[0];

	//get the pixels
	omero::model::PixelsPtr pixels = img->getPrimaryPixels();
	long int pixelID = pixels->getId()->getValue();

	//reload the pixel object with the pixel descriptions
	omero::model::PixelsPtr pixelsObject = pixelsService->retrievePixDescription(pixelID);

	//now we can get the metadata
	long pixelId = pixelsObject->getId()->getValue();
	rawPixelsStore->setPixelsId(pixelId, false);
	xdim = pixelsObject->getSizeX()->getValue();
    ydim = pixelsObject->getSizeY()->getValue();
    zdim = pixelsObject->getSizeZ()->getValue();
    cdim = pixelsObject->getSizeC()->getValue();
    tdim = pixelsObject->getSizeT()->getValue();
	xphysize = pixelsObject->getPhysicalSizeX()->getValue();
	yphysize = pixelsObject->getPhysicalSizeY()->getValue();
	zphysize = pixelsObject->getPhysicalSizeZ()->getValue();

	pixtype = pixelsObject->getPixelsType()->getValue()->getValue();

	if(xphysize==0.0) xphysize=1.0;
	if(yphysize==0.0) yphysize=1.0;
	if(zphysize==0.0) zphysize=1.0;

	cout<< "------------------------------------------------------------"<<endl;
	cout<< " OMERO Image("<<imgId<<") metadata:"<<endl;
	cout<< " Dimensions(x,y,z,c,t) = ("<<xdim<<","<<ydim<<","<<zdim<<","<<cdim<<","<<tdim<<")"<<endl;
	cout<< " PhyicalSize(x,y,z) = ("<<xphysize<<","<<yphysize<<","<<zphysize<<")"<<endl;
	cout<< " PixelType: "<<pixtype<<endl;
	cout<< "------------------------------------------------------------"<<endl;

	double maxpix = 1.0;
	double min, max;
	channel_min.clear();
	channel_max.clear();
	for(int i=0; i<cdim; i++)
	{	
		min = pixelsObject->getChannel(i)->getStatsInfo()->getGlobalMin()->getValue();
		max = pixelsObject->getChannel(i)->getStatsInfo()->getGlobalMax()->getValue();
		cout<<" Channel["<<i<<"]: min="<<min<<" max="<<max<<endl;

		channel_min.push_back(min);
		channel_max.push_back(max);
	}
	cout<< "------------------------------------------------------------"<<endl;

	return 0;
}
unsigned char* OMEROLoader::getImagePlane(long z, long t)
{
	unsigned char* data = new unsigned char[3*xdim*ydim];
	for(int i=0; i<3*xdim*ydim; i++) data[i]=0;

	Ice::ByteSeq rpixs;
	Ice::ByteSeq gpixs;
	Ice::ByteSeq bpixs;
	
	bool rchan, gchan, bchan;
	rchan=gchan=bchan=false;

	if(cdim==1)
	{
		rpixs = gpixs = bpixs = rawPixelsStore->getPlane(z,0,t);
		rchan = true;
	}
	else if(cdim==2)
	{
		rpixs = rawPixelsStore->getPlane(z,0,t);
		gpixs = bpixs = rawPixelsStore->getPlane(z,1,t);
		rchan = true;
		gchan = true;
	}
	else
	{
		rpixs = rawPixelsStore->getPlane(z,0,t);
		gpixs = rawPixelsStore->getPlane(z,1,t);
		bpixs = rawPixelsStore->getPlane(z,2,t);
		rchan = true;
		gchan = true;
		bchan = true;
	}


	int index2d=0;
	int index=0;
	double red, green, blue;
	red=green=blue=0.0;

	for(int x=0; x<xdim; x++)
	{
		for(int y=0; y<ydim; y++)
		{
			index2d = (x*ydim)+(y);
			index = (y*xdim)+(x);
			
			if(pixtype=="uint8")
			{			
				if(rchan==true)
				{
					unsigned char r = rpixs[index];
					red = (r-channel_min[0])/(channel_max[0]-channel_min[0]);
					red *= 255.0;
					green = blue = red;
				}
				if(gchan==true)
				{
					unsigned char g = gpixs[index];
					green = (g-channel_min[1])/(channel_max[1]-channel_min[1]);
					green *= 255.0;
					blue = green;
				}
				if(bchan==true)
				{
					unsigned char b = bpixs[index];
					blue = (b-channel_min[2])/(channel_max[2]-channel_min[2]);
					blue *= 255.0;
				}
			}
			else if(pixtype=="uint16")
			{	
				if(rchan==true)
				{
					unsigned short int r = (unsigned short int) (rpixs[2*index] << 8 | rpixs[2*index+1]);
					red = (r-channel_min[0])/(channel_max[0]-channel_min[0]);
					red *= 255.0;
					green = blue = red;
				}
				if(gchan==true)
				{
					unsigned short int g = (unsigned short int) (gpixs[2*index] << 8 | gpixs[2*index+1]);
					green = (g-channel_min[1])/(channel_max[1]-channel_min[1]);
					green *= 255.0;
					blue = green;
				}
				if(bchan==true)
				{
					unsigned short int b = (unsigned short int) (bpixs[2*index] << 8 | bpixs[2*index+1]);
					blue = (b-channel_min[2])/(channel_max[2]-channel_min[2]);
					blue *= 255.0;
				}
			}
			else if(pixtype=="int8")
			{
				if(rchan==true)
				{
					unsigned char r = rpixs[index];
					red = (r-channel_min[0])/(channel_max[0]-channel_min[0]);
					red *= 255.0;
					green = blue = red;
				}
				if(gchan==true)
				{
					unsigned char g = gpixs[index];
					green = (g-channel_min[1])/(channel_max[1]-channel_min[1]);
					green *= 255.0;
					blue = green;
				}
				if(bchan==true)
				{
					unsigned char b = bpixs[index];
					blue = (b-channel_min[2])/(channel_max[2]-channel_min[2]);
					blue *= 255.0;
				}
			}
			else if(pixtype=="int16")
			{	
				if(rchan==true)
				{
					unsigned short int r = (unsigned short int) (rpixs[2*index] << 8 | rpixs[2*index+1]);
					red = (r-channel_min[0])/(channel_max[0]-channel_min[0]);
					red *= 255.0;
					green = blue = red;
				}
				if(gchan==true)
				{
					unsigned short int g = (unsigned short int) (gpixs[2*index] << 8 | gpixs[2*index+1]);
					green = (g-channel_min[1])/(channel_max[1]-channel_min[1]);
					green *= 255.0;
					blue = green;
				}
				if(bchan==true)
				{
					unsigned short int b = (unsigned short int) (bpixs[2*index] << 8 | bpixs[2*index+1]);
					blue = (b-channel_min[2])/(channel_max[2]-channel_min[2]);
					blue *= 255.0;
				}
			}
 
			data[3*index2d+0] = (unsigned char) red;			
			data[3*index2d+1] = (unsigned char) green;
			data[3*index2d+2] = (unsigned char) blue;
		}
	}
	return data;
}

unsigned char* OMEROLoader::getImagePlane(long z, long c, long t)
{
	unsigned char* data = new unsigned char[xdim*ydim];

	Ice::ByteSeq pixs = rawPixelsStore->getPlane(z,c,t);
	int index2d;
	for(int x=0; x<xdim; x++)
	{
		for(int y=0; y<ydim; y++)
		{
			index2d = y*xdim+x;
			data[index2d] = (unsigned char)pixs[index2d];
		}
	}

	return data;
}
void OMEROLoader::getImagePlane(long z, long c, long t, unsigned char* dest)
{
	Ice::ByteSeq pixs = rawPixelsStore->getPlane(z,c,t);
	int index2d;
	int index3d;
	for(int x=0; x<xdim; x++)
	{
		for(int y=0; y<ydim; y++)
		{
			index2d = y*xdim+x;
			index3d = 3*(((((int)z*(int)ydim)+(int)y)*(int)xdim)+(int)x);
			dest[index3d+c] = (unsigned char)pixs[index2d];
		}
	}
}
void OMEROLoader::saveImage(string name, string description, int xsize, int ysize, int zsize, int csize, int tsize, double physicalX, double physicalY, double physicalZ, unsigned char* src)
{
	Q3ProgressDialog* progressbar;

	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Saving OMERO Image...", "Cancel", zsize, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
		progressbar->raise();
	}

	//Find the PixelType we need
	printf("OMERO: Find the PixelType we need\n");

    std::string query_string = "from PixelsType as p where p.value = :pixtype";
    omero::sys::ParametersIPtr p = new omero::sys::ParametersI();
	p->add("pixtype", omero::rtypes::rstring("uint8"));
	omero::api::IObjectList results = queryService->findAllByQuery(query_string, p);

	if(results.empty())
	{
		printf("ERROR: PixelType not Found!!\n");
		return;
	}

	omero::model::PixelsTypePtr pixelsType = omero::model::PixelsTypePtr::dynamicCast(results[0]);

	//Build up the channel info
	printf("OMERO: Build up the channel info\n");
	omero::sys::IntList clist;
	for(int i=0; i<csize; i++)
		clist.push_back(i);

	//create the image
	printf("OMERO: Create the Image\n");
	omero::RLongPtr imgD = pixelsService->createImage(xsize,ysize,zsize,tsize,clist,pixelsType, name, description);
	long int imageId = imgD->getValue();

	//get the image object
	printf("OMERO: Get the Image object\n");
	omero::sys::LongList imglist;
	imglist.push_back(imageId);
	omero::model::ImagePtr img = containerService->getImages("Image",imglist,0)[0];

	//get the primary pixels of the image
	//and setup our raw pixel store
	printf("OMERO: Get the Primary Pixels\n");
	omero::model::PixelsPtr pixels = img->getPrimaryPixels();

	printf("OMERO: Set metadata\n");
	pixels->setPhysicalSizeX(omero::rtypes::rdouble(physicalX));
	pixels->setPhysicalSizeY(omero::rtypes::rdouble(physicalY));
	pixels->setPhysicalSizeZ(omero::rtypes::rdouble(physicalZ));
	sf->getUpdateService()->saveObject(pixels);

	//now we can upload the pixels
	printf("OMERO: Uploading the Pixels\n");
	Ice::ByteSeq data;
	data.resize(xsize*ysize);
	int long pixid = pixels->getId()->getValue();
	rawPixelsStore->setPixelsId(pixid, true);
 
	int index2d;
	int index3d;
	for(int z=0; z<zsize; z++)
	{
		if(showGUI)
		{
			//update our progress bar
			progressbar->setProgress(z);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(z, zsize);

		for(int c=0; c<csize; c++)
		{
			for(int t=0; t<tsize; t++)
			{
				for(int x=0; x<xsize; x++)
				{
					for(int y=0; y<ysize; y++)
					{
						index3d = (((z*ysize)+y)*xsize)+x;
						index2d = y*xsize+x;
						data[index2d] = src[3*index3d+c];
					}
				}

				//now upload the plane
				rawPixelsStore->setPlane(data,z,c,t);
			}
		}
	}

    std::string query = "select p from Pixels p join fetch p.channels as c join fetch c.logicalChannel as lc where p.id=:pid";
    omero::sys::ParametersIPtr params = new omero::sys::ParametersI();
	params->add("pid", omero::rtypes::rlong(pixid));
	omero::api::IObjectList resultobjs = queryService->findAllByQuery(query, params);
	if(resultobjs.empty()) 
	{
		printf("ERROR: Could not find pixels object\n");
		return;
	}

	//setup the channel info
	pixels = omero::model::PixelsPtr::dynamicCast(resultobjs[0]);
	for(int c=0; c<csize; c++)
	{
		omero::model::ChannelPtr chan = pixels->getChannel(c);
		omero::model::IObjectPtr obj = queryService->get("Channel", chan->getId()->getValue());
		chan = omero::model::ChannelPtr::dynamicCast(obj);

		//omero::model::LogicalChannelPtr logchan = pixels->getChannel(c)->getLogicalChannel();
		//logchan->setName(omero::rtypes::rstring("hello world"));

		if(c==0)
		{
			chan->setRed(omero::rtypes::rint(255));
			chan->setGreen(omero::rtypes::rint(0));
			chan->setBlue(omero::rtypes::rint(0));
		}
		else if(c==1)
		{
			chan->setRed(omero::rtypes::rint(0));
			chan->setGreen(omero::rtypes::rint(255));
			chan->setBlue(omero::rtypes::rint(0));
		}
		else if(c==2)
		{
			chan->setRed(omero::rtypes::rint(0));
			chan->setGreen(omero::rtypes::rint(0));
			chan->setBlue(omero::rtypes::rint(255));
		}
		else
		{
			chan->setRed(omero::rtypes::rint(255));
			chan->setGreen(omero::rtypes::rint(255));
			chan->setBlue(omero::rtypes::rint(255));
		}

		chan->setAlpha(omero::rtypes::rint(255));
		sf->getUpdateService()->saveObject(chan);
	}
	sf->getDeleteService()->deleteSettings(imgD);

	//set the global min/max per channel
	printf("OMERO: Set the global min/max per channel\n");
	for(int i=0; i<csize; i++)
		pixelsService->setChannelGlobalMinMax(pixid, i, 0, 255);

	//set the channel rendering colours
	printf("OMERO: Load rendering defaults\n");
	renderingEngine->lookupPixels(pixid);

	//load defaults
    if(!renderingEngine->lookupRenderingDef(pixid))
	{
        renderingEngine->resetDefaults();
	}
    if(!renderingEngine->lookupRenderingDef(pixid))
	{
		printf("ERROR: OMERO renderingEngine could not load definition\n");
		return;
	}

	if(showGUI)
	{
		delete progressbar;
		progressbar=NULL;
	}
	else progress(zsize, zsize);
}
void OMEROLoader::saveImage(int long datasetID, string name, string description, int xsize, int ysize, int zsize, int csize, int tsize, double physicalX, double physicalY, double physicalZ, unsigned char* src)
{
	Q3ProgressDialog* progressbar;

	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Saving OMERO Image...", "Cancel", zsize, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
		progressbar->raise();
	}

	//Find the PixelType we need
	printf("OMERO: Find the PixelType we need\n");

    std::string query_string = "from PixelsType as p where p.value = :pixtype";
    omero::sys::ParametersIPtr p = new omero::sys::ParametersI();
	p->add("pixtype", omero::rtypes::rstring("uint8"));
	omero::api::IObjectList results = queryService->findAllByQuery(query_string, p);

	if(results.empty())
	{
		printf("ERROR: PixelType not Found!!\n");
		return;
	}

	omero::model::PixelsTypePtr pixelsType = omero::model::PixelsTypePtr::dynamicCast(results[0]);

	//Build up the channel info
	printf("OMERO: Build up the channel info\n");
	omero::sys::IntList clist;
	for(int i=0; i<csize; i++)
		clist.push_back(i);

	//create the image
	printf("OMERO: Create the Image\n");
	//long int imageId = pixelsService->createImage(xsize,ysize,zsize,tsize,clist,pixelsType, name, description)->getValue();
	omero::RLongPtr imgD = pixelsService->createImage(xsize,ysize,zsize,tsize,clist,pixelsType, name, description);
	long int imageId = imgD->getValue();

	//get the image object
	printf("OMERO: Get the Image object\n");
	omero::sys::LongList imglist;
	imglist.push_back(imageId);
	omero::model::ImagePtr img = containerService->getImages("Image",imglist,0)[0];

	//get the primary pixels of the image
	//and setup our raw pixel store
	printf("OMERO: Get the Primary Pixels\n");
	omero::model::PixelsPtr pixels = img->getPrimaryPixels();

	printf("OMERO: Set metadata\n");
	pixels->setPhysicalSizeX(omero::rtypes::rdouble(physicalX));
	pixels->setPhysicalSizeY(omero::rtypes::rdouble(physicalY));
	pixels->setPhysicalSizeZ(omero::rtypes::rdouble(physicalZ));
	sf->getUpdateService()->saveObject(pixels);

	//now we can upload the pixels
	printf("OMERO: Uploading the Pixels\n");
	Ice::ByteSeq data;
	data.resize(xsize*ysize);
	int long pixid = pixels->getId()->getValue();
	rawPixelsStore->setPixelsId(pixid, true);
 
	int index2d;
	int index3d;
	for(int z=0; z<zsize; z++)
	{
		if(showGUI)
		{
			//update our progress bar
			progressbar->setProgress(z);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(z, zsize);

		for(int c=0; c<csize; c++)
		{
			for(int t=0; t<tsize; t++)
			{
				for(int x=0; x<xsize; x++)
				{
					for(int y=0; y<ysize; y++)
					{
						index3d = (((z*ysize)+y)*xsize)+x;
						index2d = y*xsize+x;
						data[index2d] = src[3*index3d+c];
					}
				}

				//now upload the plane
				rawPixelsStore->setPlane(data,z,c,t);
			}
		}
	}

    std::string query = "select p from Pixels p join fetch p.channels as c join fetch c.logicalChannel as lc where p.id=:pid";
    omero::sys::ParametersIPtr params = new omero::sys::ParametersI();
	params->add("pid", omero::rtypes::rlong(pixid));
	omero::api::IObjectList resultobjs = queryService->findAllByQuery(query, params);
	if(resultobjs.empty()) 
	{
		printf("ERROR: Could not find pixels object\n");
		return;
	}

	//setup the channel info
	pixels = omero::model::PixelsPtr::dynamicCast(resultobjs[0]);
	for(int c=0; c<csize; c++)
	{
		omero::model::ChannelPtr chan = pixels->getChannel(c);
		omero::model::IObjectPtr obj = queryService->get("Channel", chan->getId()->getValue());
		chan = omero::model::ChannelPtr::dynamicCast(obj);

		//omero::model::LogicalChannelPtr logchan = pixels->getChannel(c)->getLogicalChannel();
		//logchan->setName(omero::rtypes::rstring("hello world"));

		if(c==0)
		{
			chan->setRed(omero::rtypes::rint(255));
			chan->setGreen(omero::rtypes::rint(0));
			chan->setBlue(omero::rtypes::rint(0));
		}
		else if(c==1)
		{
			chan->setRed(omero::rtypes::rint(0));
			chan->setGreen(omero::rtypes::rint(255));
			chan->setBlue(omero::rtypes::rint(0));
		}
		else if(c==2)
		{
			chan->setRed(omero::rtypes::rint(0));
			chan->setGreen(omero::rtypes::rint(0));
			chan->setBlue(omero::rtypes::rint(255));
		}
		else
		{
			chan->setRed(omero::rtypes::rint(255));
			chan->setGreen(omero::rtypes::rint(255));
			chan->setBlue(omero::rtypes::rint(255));
		}

		chan->setAlpha(omero::rtypes::rint(255));
		sf->getUpdateService()->saveObject(chan);
	}
	sf->getDeleteService()->deleteSettings(imgD);

	//set the global min/max per channel
	printf("OMERO: Set the global min/max per channel\n");
	for(int i=0; i<csize; i++)
		pixelsService->setChannelGlobalMinMax(pixid, i, 0, 255);

	//set the channel rendering colours
	printf("OMERO: Load rendering defaults\n");
	renderingEngine->lookupPixels(pixid);

	//load defaults
    if(!renderingEngine->lookupRenderingDef(pixid))
	{
        renderingEngine->resetDefaults();
	}
    if(!renderingEngine->lookupRenderingDef(pixid))
	{
		printf("ERROR: OMERO renderingEngine could not load definition\n");
		return;
	}

	printf("OMERO: Link the Image to the Dataset\n");  
    std::string dsquery_string = "select ds from Dataset ds where ds.id = :id";
    omero::sys::ParametersIPtr pds = new omero::sys::ParametersI();
	pds->add("id", omero::rtypes::rlong(datasetID));
    omero::api::IObjectList dsresults = queryService->findAllByQuery(dsquery_string, pds);
	if(dsresults.empty())
	{
		printf("ERROR: Dataset could not be found!\n");
		return;
	}
	omero::model::DatasetPtr ds = omero::model::DatasetPtr::dynamicCast(dsresults[0]);

    std::string iquery_string = "select i from Image i left outer join fetch i.datasetLinks where i.id = :id";
    omero::sys::ParametersIPtr pi = new omero::sys::ParametersI();
	pi->add("id", omero::rtypes::rlong(imageId));
    omero::api::IObjectList iresults = queryService->findAllByQuery(iquery_string, pi);
	if(iresults.empty())
	{
		printf("ERROR: Image could not be found!\n");
		return;
	}
	omero::model::ImagePtr im = omero::model::ImagePtr::dynamicCast(iresults[0]);

	omero::model::DatasetImageLinkPtr link = new omero::model::DatasetImageLinkI();
	link->setParent(ds);
	link->setChild(im);
	sf->getUpdateService()->saveObject(link);

	if(showGUI)
	{
		delete progressbar;
		progressbar=NULL;
	}
	else progress(zsize, zsize);
}
void OMEROLoader::getProjectImages(int long projid, vector<int long> *imgIDs)
{
	imgIDs->clear();
	int long userID = sf->getAdminService()->getEventContext()->userId;
	printf("OMERO: UserID: %d\n", userID);

    omero::sys::ParametersIPtr params = new omero::sys::ParametersI();

	//indicate to load leaves
	params->leaves();
	params->addId(omero::rtypes::rlong(userID));

	omero::sys::LongList list;
	list.push_back(projid);

	omero::api::IObjectList objlist = containerService->loadContainerHierarchy("Project", list, params);

	printf("OMERO: Found %d projects\n", objlist.size());

	for(int i=0; i<objlist.size(); i++)
	{
		omero::model::ProjectPtr proj = omero::model::ProjectPtr::dynamicCast(objlist[i]);
		string projname = proj->getName()->getValue();

		printf("OMERO: Project: %s\n", projname.data());

		omero::model::ProjectLinkedDatasetSeq datasets = proj->linkedDatasetList();

		for(int j=0; j<datasets.size(); j++)
		{
			omero::model::DatasetPtr ds = datasets[j];

			string dsname = ds->getName()->getValue();
			printf("OMERO: \tDataset: %s\n", dsname.data());

			omero::model::DatasetLinkedImageSeq images = ds->linkedImageList();

			for(int k=0; k<images.size(); k++)
			{
				omero::model::ImagePtr img = images[k];

				string imgname = img->getName()->getValue();
				int long imgid = img->getId()->getValue();
				imgIDs->push_back(imgid);
				printf("OMERO: \t\tImage: ID:%d - %s\n", imgid, imgname); 
			}
		}
	}
}
void OMEROLoader::getDatasetImages(int long dsid, vector<int long> *imgIDs)
{
	imgIDs->clear();
	int long userID = sf->getAdminService()->getEventContext()->userId;
	printf("OMERO: UserID: %d\n", userID);

    omero::sys::ParametersIPtr params = new omero::sys::ParametersI();

	//indicate to load leaves
	params->leaves();
	params->addId(omero::rtypes::rlong(userID));

	omero::sys::LongList list;
	list.push_back(dsid);

	omero::api::IObjectList objlist = containerService->loadContainerHierarchy("Dataset", list, params);

	printf("OMERO: Found %d datasets\n", objlist.size());

	for(int j=0; j<objlist.size(); j++)
	{
		omero::model::DatasetPtr ds = omero::model::DatasetPtr::dynamicCast(objlist[j]);

		string dsname = ds->getName()->getValue();
		printf("OMERO: \tDataset: %s\n", dsname.data());

		omero::model::DatasetLinkedImageSeq images = ds->linkedImageList();

		for(int k=0; k<images.size(); k++)
		{
			omero::model::ImagePtr img = images[k];

			string imgname = img->getName()->getValue();
			int long imgid = img->getId()->getValue();
			imgIDs->push_back(imgid);
			printf("OMERO: \t\tImage: ID:%d - %s\n", imgid, imgname); 
		}
	}
}
void OMEROLoader::getAllProjectsDatasetsImages(void)
{
	int long userID = sf->getAdminService()->getEventContext()->userId;
	printf("OMERO: UserID: %d\n", userID);

    omero::sys::ParametersIPtr params = new omero::sys::ParametersI();

	//indicate to load leaves
	params->leaves();
	params->addId(omero::rtypes::rlong(userID));

	omero::sys::LongList list;
	//list.push_back(userID);

	omero::api::IObjectList objlist = containerService->loadContainerHierarchy("Project", list, params);

	printf("OMERO: Found %d projects\n", objlist.size());

	for(int i=0; i<objlist.size(); i++)
	{
		omero::model::ProjectPtr proj = omero::model::ProjectPtr::dynamicCast(objlist[i]);
		string projname = proj->getName()->getValue();

		printf("OMERO: Project: %s\n", projname.data());

		omero::model::ProjectLinkedDatasetSeq datasets = proj->linkedDatasetList();

		for(int j=0; j<datasets.size(); j++)
		{
			omero::model::DatasetPtr ds = datasets[j];

			string dsname = ds->getName()->getValue();
			printf("OMERO: \tDataset: %s\n", dsname.data());

			omero::model::DatasetLinkedImageSeq images = ds->linkedImageList();

			for(int k=0; k<images.size(); k++)
			{
				omero::model::ImagePtr img = images[k];

				string imgname = img->getName()->getValue();
				int long imgid = img->getId()->getValue();

				printf("OMERO: \t\tImage: ID:%d - %s\n", imgid, imgname); 
			}
		}
	}
}

/*void OMEROLoader::debug(void)
{
	printf("OMERO:  svrname: %s\n", svrname.data());
	printf("OMERO:  port: %d\n", port);
	printf("OMERO:  username: %s\n", username.data());
	printf("OMERO:  psswd: %s\n", psswd.data());
	printf("OMERO:  sessionID: %s\n", sessionID.data());
	printf("OMERO:  imageID: %d\n", imageID);
}*/

#endif //OMERO_SUPPORT
