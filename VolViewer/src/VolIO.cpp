/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "VolIO.h"

VolIO::VolIO()
{
	watcher = new SliceLoader(0);
	WLZ_rescale = 1.0;

	volobj = NULL;

	red=green=blue=false;
	
	res_x=res_y=res_z=0;
	offset_x=offset_y=offset_z=0;

	showGUI = true;
}

VolIO::~VolIO()
{
	volobj = NULL;
}

void VolIO::save_PNG(QString filename)
{
	if(volobj->texwidth&&volobj->texheight&&volobj->texdepth==1) return;
	if(volobj->texture3d==NULL) return;

	//remove the filename of first file to get only dirname
	//int pos = filename.findRev(QString("/").latin1(), -1, false);
	QString dirname = filename + "/";//filename.replace(pos+1, filename.length()-pos+1, "");

	//printf("%s\n", filename.latin1());

	//index helpers
	int index_2D;
	int index_3D;

	//filename
	QString slice_filename;
	QString numb;

	//array for slice image
	unsigned char* slicearray=NULL;
	slicearray = new unsigned char[volobj->texwidth*volobj->texheight*4];
	if(slicearray==NULL) return;

	//printf("%d, %d, %d \n", volobj->texwidth, volobj->texheight, volobj->texdepth);

	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Saving PNG Slices...", "Cancel", volobj->texdepth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
	}

	int r,g,b;

	// Loop Through the depth
	for (int z =0; z<volobj->texdepth; z++)
	{	
		//progress(z, volobj->texdepth);
		
		if(showGUI)
		{
			progressbar->setProgress(z);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(z, volobj->texdepth);

		//get the current z slice
		for(int x=0; x<volobj->texwidth; x++)
		{
			for(int y=0; y<volobj->texheight; y++)
			{
				index_2D = 4*((y*volobj->texwidth)+x);
				index_3D = 3*(((((z)*volobj->texheight)+y)*volobj->texwidth)+x);
				
				r =  volobj->texture3d[index_3D+0];
				g =  volobj->texture3d[index_3D+1];
				b =  volobj->texture3d[index_3D+2];

				/*if(volobj->is16bit)
				{
					slicearray[index_2D+0] = volobj->texture3d[index_3D+1];
					slicearray[index_2D+1] = volobj->texture3d[index_3D+1];
					slicearray[index_2D+2] = volobj->texture3d[index_3D+1];
					slicearray[index_2D+3] = 255;				
				}
				else
				{*/
					if(volobj->is_greyscale==true)
					{
						slicearray[index_2D+0] = r;
						slicearray[index_2D+1] = r;
						slicearray[index_2D+2] = r;
						slicearray[index_2D+3] = 255;

					}
					else
					{
						slicearray[index_2D+0] = slicearray[index_2D+1] = slicearray[index_2D+2] = 0;
						slicearray[index_2D+3] = 255;

						if(volobj->has_red==true) slicearray[index_2D+0] = r;
						if(volobj->has_green==true) slicearray[index_2D+1] = g;
						if(volobj->has_blue==true)  slicearray[index_2D+2] = b;
					}
				//}
			}
		}

		//create a QImage using our RAW RGB data
		QImage slice_image = QImage(slicearray, volobj->texwidth, volobj->texheight, 32, 0, 256, QImage::LittleEndian );
		slice_image.setAlphaBuffer(false);
		slice_image = slice_image.swapRGB();

		//generate our slice filename
		slice_filename = dirname + "Slice";		
		if(z<10)
		{
			numb = "";
			slice_filename += numb.setNum(0);
		}
		if(z<100)
		{
			numb = "";
			slice_filename += numb.setNum(0);
		}
		slice_filename += numb.setNum(z);
		slice_filename += ".png";

		//printf("Save file = %s\n", slice_filename.latin1());

		//convert image to a pixmap and save it
		QPixmap pm;
		pm.convertFromImage(slice_image);
		slice_image.reset();
		if(!pm.isNull()) pm.save(slice_filename, "PNG");
	}

	
	//printf("xresize: %f\n, yresize: %f\n, zresize: %f\n",volobj->xscale*volobj->x_resize, volobj->yscale*volobj->y_resize, volobj->zscale*volobj->z_resize);
	
	ofstream fout;
	QString fn = dirname + "/voxelscale.txt";
	fout.open(fn.latin1());
	fout<<"x "<<volobj->xscale*volobj->x_resize<<endl;
	fout<<"y "<<volobj->yscale*volobj->y_resize<<endl;
	fout<<"z "<<volobj->zscale*volobj->z_resize<<endl;
	fout.close();
	
	fn = dirname + "/voxelspacing.txt";
	fout.open(fn.latin1());
	fout<<"x "<<volobj->xsize<<endl;
	fout<<"y "<<volobj->ysize<<endl;
	fout<<"z "<<volobj->zsize<<endl;
	fout.close();

	if(showGUI)
	{
		delete progressbar;
		progressbar = NULL;
	}
	else progress(volobj->texdepth, volobj->texdepth);

	delete[] slicearray;
	slicearray = NULL;
}
void VolIO::save_RAW(QString filename)
{
	if(volobj->texwidth&&volobj->texheight&&volobj->texdepth==1) return;
	if(volobj->texture3d==NULL) return;

	//a raw vol file
	RAW_volume raw_vol;

	//check the user entered the extension, otherwise add it to the filename
	if(filename[int(filename.length()-4)]!='.') filename += ".dat";

	raw_vol.save_DAT(filename.latin1(), volobj->texwidth, volobj->texheight, volobj->texdepth, WLZ_rescale);
	
	filename.truncate(filename.length()-3);                            
	filename += "raw";

	// Open "filename" For Reading Bytes
	FILE *f;
	f = fopen(filename.latin1(), "wb");					
	
	if(f== NULL)
	{
		//error opening file
		QMessageBox* mb = new QMessageBox( "WARNING",
				"Could not save file. \n",QMessageBox::Warning,
				QMessageBox::Ok | QMessageBox::Default,0,0 );

		mb->exec();
		exit(1);
//		printf("ERROR: Saving RAW volume...  [VOLIO.cpp\save_RAW()]\n");
//		system("pause");
//		exit(1);
	}

	//printf("Saving RAW volume... \n");
	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Saving RAW Volume...", "Cancel", volobj->texdepth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
	}

	// Loop Through the depth
	for (int z =0; z<volobj->texdepth; z++)
	{	
		if(showGUI)
		{
			//progress(z, volobj->texdepth);
			progressbar->setProgress(z);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(z, volobj->texdepth);

		raw_vol.save_RAW(f, volobj->texture3d, volobj->texwidth, volobj->texheight, z);			
	}
	fclose(f);	// Close The File

	if(showGUI)
	{
		delete progressbar;
		progressbar = 0;
	}
	else progress(volobj->texdepth, volobj->texdepth);
	//printf("\n");
}
void VolIO::reload_data(void)
{
	printf("Reloading Data...\n");
	if(volobj->file_type==-1) return;
	else if(volobj->file_type==0) load_PNG(volobj->file_name, 4);		
	else if(volobj->file_type==1) load_RAW(volobj->file_name, 4);	
}

void VolIO::load_xyzscale_fromlogfile(QString dirname)
{
//	printf("dirname: %s\n", dirname.latin1());

	string label;

	QDir dir;
	dir.setSorting(QDir::Name);
	dir.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
	dir.setNameFilter("*.log");
	dir.cd(dirname);

	//string list of our files in the directory
	QStringList strList;
	strList = dir.entryList();
	
	if(strList.size())
	{
		QString logfile = dirname + strList[0].ascii();

		printf("LOADING: logfile: %s\n", logfile.latin1());
		
		ifstream input_logfile; 
		input_logfile.open(logfile.latin1());
		Tokenizer tokenizer(input_logfile);
		
		//until oef is reached step through each token
		while(!input_logfile.eof())
		{			
			if(tokenizer.ttype()==TT_WORD)
			{
				//get the first string lable
				label = tokenizer.sval();									
				//cout<<label<<endl;
				
				if(label=="Pixel")
				{							
					//cout<<"Found Pixel"<<endl;						
					tokenizer.nextToken();
					label = tokenizer.sval();					
					//cout<<"	"<<label<<endl;

					if(label=="Size")
					{		
						//cout<<"Found Size"<<endl;						
						tokenizer.nextToken();
						tokenizer.nextToken();
						tokenizer.nextToken();
						label = tokenizer.sval();					
						//cout<<"		"<<label<<endl;

						if(label=="um")
						{		
							//cout<<"Found scale!"<<endl;
							while(tokenizer.ttype()!=TT_NUMBER) tokenizer.nextToken();
							
							volobj->xscale = volobj->yscale = volobj->zscale = tokenizer.nval();
						}
					}				
					
				}
				//else we dont know what it is, go to next label
				else
				{
					//cout<<"WARNING: Unrecognized Label..."<<endl;
					tokenizer.nextToken();
				}
			}
			else tokenizer.nextToken();
		}
		input_logfile.close();
	}
}

void VolIO::load_xyzscale(QString filename)
{
	string label;
	ifstream input_file; 
	input_file.open(filename.latin1());

	//tokenize our input file
	Tokenizer token(input_file);
	
	if(!input_file.is_open())
	{
		volobj->xscale=1.0;
		volobj->yscale=1.0;
		volobj->zscale=1.0;
	}
	else
	{
		//until oef is reached step through each token
		while(!input_file.eof())
		{
			if(token.ttype()==TT_WORD)
			{
				//get the first string lable
				label = token.sval();
				
				if(label=="x")
				{		
					token.nextToken();
					if(token.ttype()==TT_NUMBER) volobj->xscale = token.nval();
				}
				else if(label=="y")
				{		
					token.nextToken();
					if(token.ttype()==TT_NUMBER) volobj->yscale = token.nval();
				}
				else if(label=="z")
				{		
					token.nextToken();
					if(token.ttype()==TT_NUMBER) volobj->zscale = token.nval();
				}
				//else we dont know what it is, go to next label
				else
				{
					//cout<<"WARNING: Unrecognized Label..."<<endl;
					token.nextToken();
				}
			}
			else token.nextToken();
		}
		input_file.close();
	}

	printf("LOADING: volobj->scale: %f, %f, %f\n", volobj->xscale, volobj->yscale, volobj->zscale);
}

void VolIO::load_xyzspacing(QString filename)
{
	ifstream input_file; 
	input_file.open(filename.latin1());

	Tokenizer tokenizer(input_file);

	if(!input_file.is_open())
	{
		volobj->xsize=1.0;
		volobj->ysize=1.0;
		volobj->zsize=1.0;
	}
	else
	{
		//tokenize our input file
		Tokenizer token(input_file);
		string label;

		//until oef is reached step through each token
		while(!input_file.eof())
		{
			if(token.ttype()==TT_WORD)
			{
				//get the first string lable
				label = token.sval();
				
				if(label=="x")
				{		
					token.nextToken();
					if(token.ttype()==TT_NUMBER) volobj->xsize = token.nval();
				}
				else if(label=="y")
				{		
					token.nextToken();
					if(token.ttype()==TT_NUMBER) volobj->ysize = token.nval();
				}
				else if(label=="z")
				{		
					token.nextToken();
					if(token.ttype()==TT_NUMBER) volobj->zsize = token.nval();
				}
				//else we dont know what it is, go to next label
				else
				{
					//cout<<"WARNING: Unrecognized Label..."<<endl;
					token.nextToken();
				}
			}
			else token.nextToken();
		}
		input_file.close();
	}

	printf("LOADING: volobj->spacing: %f, %f, %f\n", volobj->xsize, volobj->ysize, volobj->zsize);

	volobj->maxsize =  max((float)volobj->xsize, (float)volobj->ysize);
	volobj->maxsize =  max((float)volobj->maxsize, (float)volobj->zsize);
	
	printf("LOADING: volobj->maxsize: %f\n", volobj->maxsize);
}
void VolIO::load_RAW(QString filename, int channelmode)
{
	red=green=blue=false;

	//load in a raw vol file
	RAW_volume raw_vol;

	QString dirname;
	dirname = filename;
	int pos = dirname.findRev(QString("/").latin1(), -1, false);
	dirname.replace(pos+1, dirname.length()-pos+1, "");

	load_xyzspacing(dirname+"voxelspacing.txt");
	load_xyzscale(dirname+"voxelscale.txt");

	//load RAW description
	raw_vol.load_DAT(filename.latin1());

	WLZ_rescale = raw_vol.WLZ_scale;

	volobj->file_name = filename;

	if(raw_vol.res_x==0) return;
	if(raw_vol.res_y==0) return;
	if(raw_vol.res_z==0) return;

	volobj->width = volobj->texwidth = raw_vol.res_x;
	volobj->height =volobj->texheight = raw_vol.res_y;
	volobj->depth =volobj->texdepth = raw_vol.res_z; 

	cout<<"Volume: "<<raw_vol.res_x<<" "<<raw_vol.res_y<<" "<<raw_vol.res_z<<endl;
	
	printf("WLZ_rescale: %f\n", WLZ_rescale);

	volobj->x_resize = volobj->y_resize = volobj->z_resize = WLZ_rescale;
	
	//printf("channelmode %d\n", channelmode);

	if(volobj->texture3d==0 || channelmode==4 || channelmode==1) 
	{
		//lets be clean
		if(volobj->texture3d) delete[] volobj->texture3d;
		volobj->texture3d = NULL;

		//lets have some memory for our texture data
		volobj->texture3d = (GLubyte *)malloc(volobj->texwidth * volobj->texheight 
								  * volobj->texdepth * 3 * sizeof(unsigned char));

		//check if we managed to allocate some memory
 		if(volobj->texture3d == NULL)
		{
			QMessageBox* mb = new QMessageBox( "ERROR",
					"Failed to allocate memory in [VOLIO.cpp load_RAW()]\n",QMessageBox::Warning,
					QMessageBox::Ok | QMessageBox::Default,0,0 );

			mb->exec();
			//exit(1);
			return;
		}

		//initialise everything to zero
		for(int ii=0; ii<volobj->texwidth*volobj->texheight*volobj->texdepth*3; ii++) volobj->texture3d[ii] = 0;
	}

	filename.truncate(filename.length()-3);                            
	filename += "raw";

	FILE *f;
	f = fopen(filename.latin1(), "rb");					// Open "filename" For Reading Bytes

	if( f== NULL )
	{	
		//error opening file
//		printf("ERROR: Loading RAW volume...  [VOLIO.cpp\load_RAW()]\n");
//		system("pause");
		QMessageBox* mb = new QMessageBox( "WARNING",
				"Could not open file in [VOLIO.cpp load_RAW()] \n",QMessageBox::Warning,
				QMessageBox::Ok | QMessageBox::Default,0,0 );

		mb->exec();
		//exit(1);
		return;
	}

	//printf("Loading RAW volume... \n");

	int z_offset = (volobj->texdepth-raw_vol.res_z)/2.0;

	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Loading RAW Volume...", "Cancel", volobj->texdepth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
		progressbar->raise();
	}

	bool greyscale = false;
	int helper=0;

	if(channelmode==4)
	{
		greyscale = red = green = blue = false;
	}

	printf("channelmode: %d\n", (int)channelmode);

	for(int i=0; i<3*256; i++) volobj->histogram[i]=0.0;

	// Loop Through the depth
	for (int z =0; z<volobj->texdepth; z++)
	{	
		if(showGUI)
		{
			//progress(z, raw_vol.res_z+z_offset);
			progressbar->setProgress(z);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(z, volobj->texdepth);

		raw_vol.load_RAW(z, f, volobj->texture3d, channelmode, volobj->texwidth, volobj->texheight, volobj->histogram, &red, &green, &blue, &greyscale);
		
		if(greyscale) helper++;
	}

	if(showGUI)
	{
		delete progressbar;
		progressbar = NULL;
	}
	else progress(volobj->texdepth, volobj->texdepth);

	fclose(f);	// Close The File
	
	volobj->maxdimension = volobj->texwidth;
	volobj->maxdimension = max((float)volobj->maxdimension, (float)volobj->texheight);
	volobj->maxdimension = max((float)volobj->maxdimension, (float)volobj->texdepth);
	
	if(helper==volobj->texdepth) greyscale=true;
	else greyscale=false;

	printf("(r,b,g): %d, %d, %d\n", (int)red, (int)green, (int)blue);
	printf("greyscale: %d\n", (int) greyscale);

	if(greyscale && channelmode==4)
	{
		volobj->is_greyscale = 1;
		volobj->has_red = 0;
		volobj->has_green = 0;
		volobj->has_blue = 0;
	}
	else
	{
		volobj->is_greyscale = 0;
		volobj->has_red = red;
		volobj->has_green = green;
		volobj->has_blue = blue;
	}

	if(channelmode==1)
	{
		volobj->is_greyscale = 0;
		volobj->has_red = red;
		volobj->has_green = 0;
		volobj->has_blue = 0;
	}
	else if(channelmode==2)
	{
		volobj->is_greyscale = 0;
		volobj->has_green = green;
	}
	else if(channelmode==3)
	{
		volobj->is_greyscale = 0;
		volobj->has_blue = blue;
	}

	printf("(r,b,g): %d, %d, %d\n", (int)volobj->has_red, (int)volobj->has_green, (int)volobj->has_blue);
	printf("greyscale: %d\n", (int) volobj->is_greyscale);

	float x_resize, y_resize, z_resize;
	x_resize = volobj->width/(float) volobj->maxsizeX;
	y_resize = volobj->height/(float) volobj->maxsizeY;
	z_resize = volobj->depth/(float) volobj->maxsizeZ;

	if(x_resize<1.0) x_resize = 1;
	if(y_resize<1.0) y_resize = 1;
	if(z_resize<1.0) z_resize = 1;

	int res_x = ((float)volobj->width/x_resize)+0.5; 
	int res_y = ((float)volobj->height/y_resize)+0.5;
	int res_z = ((float)volobj->depth/z_resize)+0.5; 

	float maxres1 = 0.0;
	maxres1 = max((float)maxres1, (float)res_x);
	maxres1 = max((float)maxres1, (float)res_y);
	maxres1 = max((float)maxres1, (float)res_z);
	volobj->maxres = maxres1;

	resize();
	pad(1);
}

void VolIO::eval_resize_dimensions(void)
{
	volobj->x_resize = volobj->y_resize = volobj->z_resize = 0;
	volobj->x_resize = (float)(volobj->width)/(float)(volobj->maxsizeX);
	volobj->y_resize = (float)(volobj->height)/(float)(volobj->maxsizeY);
	volobj->z_resize = (float)(volobj->depth)/(float)(volobj->maxsizeZ); //NON ISO!!!!!!!

	if(volobj->x_resize<1.0) volobj->x_resize = 1;
	if(volobj->y_resize<1.0) volobj->y_resize = 1;
	if(volobj->z_resize<1.0) volobj->z_resize = 1; //NON ISO!!!!!!!

	bool non_iso = false;

	//if(volobj->z_resize==1.0 && volobj->x_resize!=1.0 && volobj->y_resize != 1.0)
/*	if(volobj->z_resize<volobj->x_resize)
	{
		non_iso = false;
		printf("LOADING: Using Non-ISO RESCALING\n");
	}
*/
	float maxresize = 0;
	maxresize = (float)volobj->x_resize;
	maxresize = max((float)maxresize, (float)volobj->y_resize);
	maxresize = max((float)maxresize, (float)volobj->z_resize); //NON ISO!!!!!!!
	volobj->x_resize = volobj->y_resize = volobj->z_resize = maxresize;
	
	//if(!non_iso) volobj->z_resize = maxresize;; //NON ISO!!!!!!!

	//printf("Max Resize: %f\n",maxresize);

	res_x = (((float)volobj->width/volobj->x_resize)); 
	res_y = (((float)volobj->height/volobj->y_resize));
	res_z = (((float)volobj->depth/volobj->z_resize));

	int maxres1 = 0;
	int maxres2 = 0;
	int maxres3 = 0;
	maxres1 = res_x;
	maxres2 = max((float)maxres2, (float)res_y);
	maxres3 = max((float)maxres3, (float)res_z);
	
	volobj->maxres = maxres1;
	volobj->maxres = max((float)maxres2, (float)volobj->maxres);
	volobj->maxres = max((float)maxres3, (float)volobj->maxres);
	
//	offset_x = offset_y = offset_z = 0;
	offset_x = ((makepower2(maxres1)-res_x)/2.0);
	offset_y = ((makepower2(maxres2)-res_y)/2.0); 
	offset_z = ((makepower2(maxres3)-res_z)/2.0);

	volobj->resized_texwidth = res_x = makepower2(maxres1);
	volobj->resized_texheight = res_y = makepower2(maxres2);
	volobj->resized_texdepth = res_z = makepower2(maxres3);

	volobj->maxres = volobj->resized_texwidth;
	volobj->maxres = max((float)volobj->resized_texheight, (float)volobj->maxres);
	volobj->maxres = max((float)volobj->resized_texdepth, (float)volobj->maxres);

	//NON-ISO!!!!!!!!!!
	/*if(non_iso)
	{
			volobj->zsize /= (volobj->x_resize/volobj->z_resize);
			volobj->zscale /= (volobj->x_resize/volobj->z_resize);
	}

	volobj->xsize  /= volobj->x_resize;
	volobj->xscale /= volobj->x_resize;

	volobj->ysize  /= volobj->y_resize;
	volobj->yscale /= volobj->y_resize;

	volobj->zsize  /= volobj->z_resize;
	volobj->zscale /= volobj->z_resize ;*/

	/*res_x = maxres1;
	res_y = maxres2;
	res_z = maxres3;*/

	printf("LOADING: Max Res: %d\n",volobj->maxres);
	printf("LOADING: Original Dimensions: %d, %d, %d\n", volobj->width, volobj->height, volobj->depth);
	printf("LOADING: Resized Dimensions: %d, %d, %d\n", volobj->resized_texwidth, volobj->resized_texheight, volobj->resized_texdepth);
	printf("LOADING: Resize Factor: %f, %f, %f\n", volobj->x_resize, volobj->y_resize, volobj->z_resize);
	printf("LOADING: Offsets: %d, %d, %d\n", offset_x, offset_y, offset_z);	
	//printf("LOADING: Res_: %d, %d, %d\n", res_x, res_y, res_z);	
}

#ifdef OMERO_SUPPORT
void VolIO::load_OMERO(OMEROLoader* omeroLoader)
{
	//printf("Loading from OMERO\n");
	red=green=blue=false;

	volobj->xscale=volobj->xsize=1.0;
	volobj->yscale=volobj->ysize=1.0;
	volobj->zscale=volobj->zsize=1.0;

	//read metadata
	if(omeroLoader->loadImageMetadata(omeroLoader->imageID)==-1) 
	{
		QString numb;
		numb.setNum(omeroLoader->imageID);
		QString msg ="Failed to retrieve OMERO Image at ID="+numb+".";

		QMessageBox* mb = new QMessageBox( "ERROR",
			msg, QMessageBox::Critical,
			QMessageBox::Ok | QMessageBox::Default,0,0 );

		mb->exec();

		//printf("BOHY\n");
		//openOmeroImage_event();
		return;
	}

	//set metadata scale
	volobj->xsize = volobj->xscale = omeroLoader->xphysize;
	volobj->ysize = volobj->yscale = omeroLoader->yphysize;
	volobj->zsize = volobj->zscale = omeroLoader->zphysize;

	volobj->maxsize =  max((float)volobj->xsize, (float)volobj->ysize);
	volobj->maxsize =  max((float)volobj->maxsize, (float)volobj->zsize);

	//
	//allocate memory
	//==============================================================	
	//set up our original image info
	volobj->width = omeroLoader->xdim;
	volobj->height = omeroLoader->ydim;
	volobj->depth = omeroLoader->zdim; 
	
	if(omeroLoader->cdim>3)
	{	
		printf("WARNING: Loading only the first 3 Image Channels.\n");
		/*QString msg = "Loading only the first 3 Image Channels.";

		QMessageBox* mb = new QMessageBox( "ERROR",
			msg, QMessageBox::Warning,
			QMessageBox::Ok | QMessageBox::Default,0,0 );

		mb->exec();*/

		omeroLoader->cdim = 3;
	}

	eval_resize_dimensions();
	int size = res_x*res_y*res_z*3;

	volobj->texwidth = res_x;
	volobj->texheight = res_y;
	volobj->texdepth = res_z;

	//lets have some memory
	//lets be clean
	//if(volobj->texture3d==0) 
	{
		if(volobj->texture3d) delete[] volobj->texture3d;
		volobj->texture3d = NULL;

		//lets have some memory for our texture data
		volobj->texture3d = (GLubyte *) malloc(size * sizeof(GLubyte));

		//check if we managed to allocate some memory
 		if(volobj->texture3d == NULL)
		{
			QMessageBox* mb = new QMessageBox( "ERROR",
					"Failed to allocate memory in [VOLIO.cpp load_PNG()] \n",QMessageBox::Warning,
					QMessageBox::Ok | QMessageBox::Default,0,0 );

			mb->exec();
			//exit(1);
			return;
		}

		//initialise everything to zero
		for(int ii=0; ii<size; ii++) volobj->texture3d[ii] = 0;
	}

	float xmin;
	float xmax;
	float xinterp;
	float xneginterp;
	float ymin;
	float ymax;
	float yinterp;
	float yneginterp;
	float zmin;
	float zmax;
	float zinterp;
	float zneginterp;
	QRgb pixel000, pixel100, pixel010, pixel001, pixel110, pixel101, pixel011, pixel111;
	QRgb pixel;

	float xstart, xend;
	float ystart, yend;

	bool trilinearinterpolation_mode=false;
	QString fn2;

	printf("offest: %f, %f, %f\n", (float)offset_x, (float)offset_y, (float)offset_z);
	printf("start: %f, %f, %f\n", 0.0, 0.0, 0.0);
	printf("end: %f, %f, %f\n", (float)volobj->width-volobj->x_resize, (float)volobj->height-volobj->y_resize, (float)volobj->depth-volobj->z_resize);

//	omeroLoader.getImagePlane(z,c,t, glwindow->glvoxel->volobject.texture3d);

	int index_small;
	float x2, y2, z2;
	float r,g,b,a;

	int helper=0;
	int greyscale=0;

	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Loading OMERO Stack...", "Cancel", volobj->depth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
		progressbar->raise();
	}

	for(int i=0; i<3*256; i++) volobj->histogram[i]=0.0;
	
	unsigned char* slice0 = NULL;
	unsigned char* slice1 = NULL;

	z2=offset_z;
	for(float z=0.0; z<volobj->depth; z+=volobj->z_resize, z2++)
	{
		if(showGUI)
		{
			//update our progress bar
			progressbar->setProgress(floor(z));
			if (progressbar->wasCancelled())
				break;
		}
		else progress(z, volobj->texdepth);

		//workout our filename
		//read_texture reads as RGBA		
		zmin = floor(z);
		zmax = ceil(z);

		if(zmax>volobj->depth-1) zmax = volobj->depth-1;

		//printf("z: %f, zmin: %f, zmax: %f, z2: %f\n", z, zmin, zmax, z2);

		zinterp = z-zmin;
		zneginterp = (1.0-zinterp);

		/*fn = dirname;
		fn += strList[(volobj->depth-1)-zmin].ascii();
		img.load(fn, ext);*/

		if(slice0!=NULL) delete[] slice0;
		slice0 = omeroLoader->getImagePlane((volobj->depth-1)-zmin, 0);
	
		//printf("filename0: %s\n", fn.latin1());

		//if(zmin==zmax)
		{
			xstart = 0.0;
			xend = volobj->width;

			ystart = 0.0;
			yend = volobj->height;

			trilinearinterpolation_mode = false;
		}
		/*else
		{
			xstart = volobj->x_resize;
			xend = volobj->width-volobj->x_resize;

			ystart = volobj->y_resize;
			yend = volobj->height-volobj->y_resize;

			trilinearinterpolation_mode = true;

			if(slice1!=NULL) delete[] slice1;
			slice1 = omeroLoader->getImagePlane((volobj->depth-1)-zmax, 0);
		}*/


		//if(omeroLoader->xdim==imgw && omeroLoader->ydim==imgh)
		{
			y2=offset_y;
			for(float y=ystart; y<yend; y+=volobj->y_resize, y2++)
			{
				ymin = floor(y);
				ymax = ceil(y);
				//if(ymax>volobj->height-1) ymax = volobj->height-1;
				yinterp = y-ymin;
				yneginterp = (1.0-yinterp);
	
//				printf("y: %f, ymin: %f, ymax: %f\n", y, ymin, ymax);

				x2=offset_x;
				for(float x=xstart; x<xend; x+=volobj->x_resize, x2++)
				{
					index_small = 3*(((((int)z2*(int)res_y)+(int)y2)*(int)res_x)+(int)x2);
					//printf("xyz2: %d, %d, %d\n", (int) x2, (int)y2, (int) z2);

					//if(trilinearinterpolation_mode==false)
					{
						//-------------------------------
						//Nearest Neighbour resampling
						//-------------------------------

						/*int x22 = floor(x);
						int y22 = floor(y);

						double xdecimal = x-(double)x22;
						double ydecimal = y-(double)y22;

						int x33, y33;
						if(xdecimal>0.5) x33 = x22 + 1;
						else x33 = x22;
						if(ydecimal>0.5) y33 = y22 + 1;
						else y33 = y22;

						//printf("x: %f, y: %f, x22: %d, y22: %d, x33: %d, y33: %d\n", x, y, x22, y22, x33, y33);

						pixel = img.pixel(x33, y33);*/
						//printf("x: %f, y: %f\n", x, y);

						pixel = qRgb( slice0[3*((int)x*volobj->height+(int)y)+0], slice0[3*((int)x*volobj->height+(int)y)+1], slice0[3*((int)x*volobj->height+(int)y)+2]);

						r = qRed(pixel);
						g = qGreen(pixel);
						b = qBlue(pixel);
						a = qGray(pixel);
					}
					/*else
					{

						xmin = floor(x);
						xmax = ceil(x);
						//if(xmax>volobj->width-1) xmax = volobj->width-1;
						xinterp = x-xmin;
						xneginterp = (1.0-xinterp);

						//printf("min: %d, %d, %d\n", xmin, ymin, zmin);
						//printf("max: %d, %d, %d\n", xmax, ymax, zmax);
						//printf("x: %f, xmin: %f, xmax: %f\n", x, xmin, xmax);

						//-------------------------------
						//Tri-Linear Resampling
						//-------------------------------

						//printf("Reading pixels\n");

						pixel000 = qRgb(slice0[3*((int)xmin*volobj->height+(int)ymin)+0], slice0[3*((int)xmin*volobj->height+(int)ymin)+1], slice0[3*((int)xmin*volobj->height+(int)ymin)+2]);
						pixel100 = qRgb(slice0[3*((int)xmax*volobj->height+(int)ymin)+0], slice0[3*((int)xmax*volobj->height+(int)ymin)+1], slice0[3*((int)xmax*volobj->height+(int)ymin)+2]);
						pixel010 = qRgb(slice0[3*((int)xmin*volobj->height+(int)ymax)+0], slice0[3*((int)xmin*volobj->height+(int)ymax)+1], slice0[3*((int)xmin*volobj->height+(int)ymax)+2]);
						pixel001 = qRgb(slice1[3*((int)xmin*volobj->height+(int)ymin)+0], slice1[3*((int)xmin*volobj->height+(int)ymin)+1], slice1[3*((int)xmin*volobj->height+(int)ymin)+2]);
						pixel110 = qRgb(slice0[3*((int)xmax*volobj->height+(int)ymax)+0], slice0[3*((int)xmax*volobj->height+(int)ymax)+1], slice0[3*((int)xmax*volobj->height+(int)ymax)+2]);
						pixel101 = qRgb(slice1[3*((int)xmax*volobj->height+(int)ymin)+0], slice1[3*((int)xmax*volobj->height+(int)ymin)+1], slice1[3*((int)xmax*volobj->height+(int)ymin)+2]);
						pixel011 = qRgb(slice1[3*((int)xmin*volobj->height+(int)ymax)+0], slice1[3*((int)xmin*volobj->height+(int)ymax)+1], slice1[3*((int)xmin*volobj->height+(int)ymax)+2]);
						pixel111 = qRgb(slice1[3*((int)xmax*volobj->height+(int)ymax)+0], slice1[3*((int)xmax*volobj->height+(int)ymax)+1], slice1[3*((int)xmax*volobj->height+(int)ymax)+2]);

						r = (float)qRed(pixel000)*(xneginterp*yneginterp*zneginterp)
						  + (float)qRed(pixel100)*(xinterp*yneginterp*zneginterp)
						  + (float)qRed(pixel010)*(xneginterp*yinterp*zneginterp)
						  + (float)qRed(pixel001)*(xneginterp*yneginterp*zinterp)
						  + (float)qRed(pixel110)*(xinterp*yinterp*zneginterp)
						  + (float)qRed(pixel101)*(xinterp*yneginterp*zinterp)
						  + (float)qRed(pixel011)*(xneginterp*yinterp*zinterp)
						  + (float)qRed(pixel111)*(xinterp*yinterp*zinterp);

						g = (float)qGreen(pixel000)*(xneginterp*yneginterp*zneginterp)
						  + (float)qGreen(pixel100)*(xinterp*yneginterp*zneginterp)
						  + (float)qGreen(pixel010)*(xneginterp*yinterp*zneginterp)
						  + (float)qGreen(pixel001)*(xneginterp*yneginterp*zinterp)
						  + (float)qGreen(pixel110)*(xinterp*yinterp*zneginterp)
						  + (float)qGreen(pixel101)*(xinterp*yneginterp*zinterp)
						  + (float)qGreen(pixel011)*(xneginterp*yinterp*zinterp)
						  + (float)qGreen(pixel111)*(xinterp*yinterp*zinterp);

						b = (float)qBlue(pixel000)*(xneginterp*yneginterp*zneginterp)
						  + (float)qBlue(pixel100)*(xinterp*yneginterp*zneginterp)
						  + (float)qBlue(pixel010)*(xneginterp*yinterp*zneginterp)
						  + (float)qBlue(pixel001)*(xneginterp*yneginterp*zinterp)
						  + (float)qBlue(pixel110)*(xinterp*yinterp*zneginterp)
						  + (float)qBlue(pixel101)*(xinterp*yneginterp*zinterp)
						  + (float)qBlue(pixel011)*(xneginterp*yinterp*zinterp)
						  + (float)qBlue(pixel111)*(xinterp*yinterp*zinterp);

						a = (float)qGray(pixel000)*(xneginterp*yneginterp*zneginterp)
						  + (float)qGray(pixel100)*(xinterp*yneginterp*zneginterp)
						  + (float)qGray(pixel010)*(xneginterp*yinterp*zneginterp)
						  + (float)qGray(pixel001)*(xneginterp*yneginterp*zinterp)
						  + (float)qGray(pixel110)*(xinterp*yinterp*zneginterp)
						  + (float)qGray(pixel101)*(xinterp*yneginterp*zinterp)
						  + (float)qGray(pixel011)*(xneginterp*yinterp*zinterp)
						  + (float)(pixel111)*(xinterp*yinterp*zinterp);
					}*/

					//printf("rgb: %f, %f, %f\n", r, g, b);

					if(r==g && r==b && b==g) greyscale++;
					helper++;

					if(r!=0) red = true;
					if(g!=0) green = true;
					if(b!=0) blue = true;
					//printf("Setting pixels\n");

					//printf("index_small: %d - %d\n", index_small, size);

					//printf("mode: %d\n", mode);
					volobj->texture3d[index_small+0] = GLubyte(r);
					volobj->texture3d[index_small+1] = GLubyte(g); 
					volobj->texture3d[index_small+2] = GLubyte(b);
					volobj->histogram[(3*int(r))+0]++;
					volobj->histogram[(3*int(g))+1]++;
					volobj->histogram[(3*int(b))+2]++;
					//printf("done\n");
				}
			}
		}
	}
	
	if(showGUI)
	{
		delete progressbar;
		progressbar=NULL;
	}
	else progress(volobj->depth, volobj->depth);

	if(slice0!=NULL) delete[] slice0;
	if(slice1!=NULL) delete[] slice1;

	//set up our texture info
	volobj->width = volobj->texwidth = res_x;
	volobj->height =volobj->texheight = res_y;
	volobj->depth =volobj->texdepth = res_z;

	pad(1);

	if(greyscale==helper) greyscale=1;
	else greyscale=0;

//	cout<<"(r,g,b) "<< red<<" "<<green<<" "<<blue<<endl;
//	cout<<"greyscale "<<greyscale<<endl;

	if(greyscale)
	{
		volobj->is_greyscale = 1;
		volobj->has_red = 1;
		volobj->has_green = 0;
		volobj->has_blue = 0;
	}
	else
	{
		volobj->is_greyscale = 0;
		volobj->has_red = red;
		volobj->has_green = green;
		volobj->has_blue = blue;
	}

	float x_resize, y_resize, z_resize;
	x_resize = volobj->width/(float) volobj->maxsizeX;
	y_resize = volobj->height/(float) volobj->maxsizeY;
	z_resize = volobj->depth/(float) volobj->maxsizeZ;

	if(x_resize<1.0) x_resize = 1;
	if(y_resize<1.0) y_resize = 1;
	if(z_resize<1.0) z_resize = 1;

	int res_x = ((float)volobj->width/x_resize)+0.5; 
	int res_y = ((float)volobj->height/y_resize)+0.5;
	int res_z = ((float)volobj->depth/z_resize)+0.5; 

	float maxres1 = 0.0;
	maxres1 = max((float)maxres1, (float)res_x);
	maxres1 = max((float)maxres1, (float)res_y);
	maxres1 = max((float)maxres1, (float)res_z);
	volobj->maxres = maxres1;

//	printf("(r,b,g): %d, %d, %d\n", (int)volobj->has_red, (int)volobj->has_green, (int)volobj->has_blue);
//	printf("greyscale: %d\n", (int) volobj->is_greyscale);
}
#endif

#ifdef LIBTIFF_SUPPORT

void VolIO::load_TIFF(QString fn, int mode)
{
	QString dirname = fn;
	//remove the filename of first file to get only dirname
	int pos = dirname.findRev(QString("/").latin1(), -1, false);
	dirname.replace(pos+1, dirname.length()-pos+1, "");

	if(volobj->is16bit)
	{
		for(int i=0; i<volobj->voldatasize; i++)
			delete[] volobj->voldata[i];
		
		if(volobj->voldata) delete volobj->voldata;
		volobj->voldata = NULL;		
	}

	TIFF* tifobject = TIFFOpen(fn, "r");
	if (!tifobject) return;

	int dircount = 0;
	do 
	{
	    dircount++;
	}
	while (TIFFReadDirectory(tifobject));
	TIFFClose(tifobject);

	//printf("default image: %s\n", dirname.latin1());
	int filenamemode=4;
	int greyscale, helper;
	greyscale = helper = 0;
	
	if(dircount>1)
	{
		TIFF* tifobject = TIFFOpen(fn, "r");
		printf("Loading multi-page TIFF data\n");
		printf("Tiff files: %d\n", dircount);

		if (tifobject) 
		{
			uint32* raster32bit=NULL;
			uint32 w, h;
			uint16 datatype, bpp, unitres, spp;
			float unitresX = 0.0;
			float unitresY = 0.0;
			size_t npixels;
			char* descp;

			TIFFGetField(tifobject, TIFFTAG_IMAGEWIDTH, &w);
			TIFFGetField(tifobject, TIFFTAG_IMAGELENGTH, &h);
			TIFFGetField(tifobject, TIFFTAG_BITSPERSAMPLE, &bpp);
			TIFFGetField(tifobject, TIFFTAG_XRESOLUTION, &unitresX);
			TIFFGetField(tifobject, TIFFTAG_YRESOLUTION, &unitresY);
			TIFFGetField(tifobject, TIFFTAG_RESOLUTIONUNIT, &unitres);
			TIFFGetField(tifobject, TIFFTAG_IMAGEDESCRIPTION, &descp);
			TIFFGetField(tifobject, TIFFTAG_SAMPLESPERPIXEL, &spp);
			QString description = descp;
			
			printf("--------------------------\n");
			printf("Tiff Info:\n");
			printf("%w:%d h:%d\n", (int)w, (int)h);
			printf("bpp: %d\n", (int) bpp);
			printf("spp: %d\n", (int) spp);
			printf("unitresX: %f\n", (float)unitresX);
			printf("unitresY: %f\n", (float)unitresY);
			printf("unitres: %f\n", (float)unitres);
			printf("description: \n%s\n", description.latin1());
			printf("--------------------------\n");
			
			set_metadata((float)unitresX, (float)unitresY, description);

			if(w==0 && h==0)
			{
				QMessageBox* mb = new QMessageBox( "WARNING",
						"Could not open a file in [VOLIO.cppload_PNG()] \n",QMessageBox::Warning,
						QMessageBox::Ok | QMessageBox::Default,0,0 );

				mb->exec();
				return;
			}

			//set up our texture info
			volobj->width = volobj->texwidth = w;
			volobj->height=volobj->texheight = h;
			volobj->depth =volobj->texdepth = dircount; 
				
			uint32 index_3D, index_small;
			float x2, y2, z2;
			int r,g,b,a;
			x2 = offset_x;
			y2 = offset_y;
			z2 = offset_z;
			
			float x,y,z;
			
			//==============================================================
			eval_resize_dimensions();
			
			int size = res_x*res_y*res_z*3;

			//lets have some texture memory
			if(volobj->texture3d==0 || mode==4 || mode==1) 
			{
				if(volobj->texture3d) delete[] volobj->texture3d;
				volobj->texture3d = NULL;

				//lets have some memory for our texture data
				volobj->texture3d = (GLubyte *) malloc(size * sizeof(GLubyte));

				//check if we managed to allocate some memory
				if(volobj->texture3d == NULL)
				{
					QMessageBox* mb = new QMessageBox( "ERROR",
							"Failed to allocate memory in [VOLIO.cpp load_TIF()] \n",QMessageBox::Warning,
							QMessageBox::Ok | QMessageBox::Default,0,0 );

					mb->exec();
					//exit(1);
					return;
				}

				//initialise everything to zero
				for(int ii=0; ii<size; ii++) volobj->texture3d[ii] = 0;
			}		
					
			if(mode==4 || mode==5) for(int i=0; i<3*256; i++) volobj->histogram[i]=0.0;

			if(showGUI)
			{
				progressbar = new Q3ProgressDialog("Loading multi-page TIFF file...", "Cancel", volobj->depth, 0, "progress", TRUE);		
				progressbar->setCaption("Please Wait");
				progressbar->raise();
			}

    		if(bpp==8)
			{				
				//now lets allocate some memory for our raw tiff...
				npixels = w*h;
				raster32bit = (uint32*) _TIFFmalloc(npixels * sizeof (uint32));
				if(raster32bit==NULL)
				{
					printf("ERROR allocating memory, raster32bit \n");
				}
				
				//DO SOME STUFF HERE WITH THE IMAGE.
				for(float z=0; z<volobj->depth; z+=volobj->z_resize)
				{
					if(showGUI)
					{
						//update our progress bar
						progressbar->setProgress((int)z);
						if (progressbar->wasCancelled()) break;
					}
					else progress(z, volobj->depth);

					//tifobject = TIFFOpen(fn.latin1(), "r");
					TIFFSetDirectory(tifobject, z);		
					TIFFReadRGBAImage(tifobject, w, h, raster32bit, 0);

					for(float x=0; x<volobj->width; x+=volobj->x_resize)
					{
						for(float y=0; y<volobj->height; y+=volobj->y_resize)
						{
							uint32 pixel = (uint32) raster32bit[(int)(x+0.5)+(int)(((volobj->height-1)-y)+0.5)*volobj->width];

							if(x2>=res_x) x2=res_x-1;
							if(y2>=res_y) y2=res_y-1;
							if(z2>=res_z) z2=res_z-1;

							index_small = ((((((int)(z2))*(int)(res_y))+((int)(y2)))*(int)(res_x))+((int)(x2)));

							r = (uint8) TIFFGetR(pixel);
							g = (uint8) TIFFGetG(pixel);
							b = (uint8) TIFFGetB(pixel);
							a = (uint8) TIFFGetR(pixel);

							if(r==g && r==b && b==g) greyscale++;

							helper++;

							if(r!=0) red = true;
							if(g!=0) green = true;
							if(b!=0) blue = true;

							if(mode==1)
							{
								volobj->texture3d[3*index_small+0] = GLubyte(r);
								volobj->histogram[(3*int(r))+0]++;
							}
							if(mode==2)
							{
								volobj->texture3d[3*index_small+1] = GLubyte(g); 
								volobj->histogram[(3*int(g))+1]++;
							}
							if(mode==3)
							{
								volobj->texture3d[3*index_small+2] = GLubyte(b); 
								volobj->histogram[(3*int(b))+2]++;
							}				
							if(mode==4)
							{
								volobj->texture3d[3*index_small+0] = GLubyte(r);
								volobj->texture3d[3*index_small+1] = GLubyte(g); 
								volobj->texture3d[3*index_small+2] = GLubyte(b); 
								volobj->histogram[(3*int(r))+0]++;
								volobj->histogram[(3*int(g))+1]++;
								volobj->histogram[(3*int(b))+2]++;
							}
							if(mode==5)
							{
								volobj->texture3d[3*index_small+0] =
								volobj->texture3d[3*index_small+1] =
								volobj->texture3d[3*index_small+2] = GLubyte(a);
								volobj->histogram[(3*int(a))+0]++;
								volobj->histogram[(3*int(a))+1]++;
								volobj->histogram[(3*int(a))+2]++;
							}								
							x2++;
						}
						y2++;
						x2=0;
					}
					z2++;
					y2=0;
				}
				_TIFFfree(raster32bit);
			}
			else if(bpp==16)
			{
					volobj->is16bit=true;

					for(int i=0; i<65536; i++) volobj->histogram16bit[i] = 0.0;
					
					if(volobj->voldata!=NULL) 
					{
						printf("De-allocating 16bit memory\n");
						
						for(int i=0; i<volobj->voldatasize; i++)
							delete[] volobj->voldata[i];
						
						delete volobj->voldata;
						volobj->voldata = NULL;	
					}			

					printf("Trying to allocate 16bit memory\n");				

					volobj->voldatasize = res_z;
					int size2 = res_x*res_y;

					volobj->voldata = new uint16*[res_z];
					for(int i=0; i<res_z; i++)
						volobj->voldata[i] = new uint16 [size2];
				
					if(volobj->voldata==NULL)
					{
						printf("Error allocating 16bit memory\n");
						return;
					}
					else
					{
						printf("Memory allocated OK\n");
					}
					
					//DO SOME STUFF HERE WITH THE IMAGE.
					for(float z=0; z<volobj->depth; z+=volobj->z_resize)
					{
						if(showGUI)
						{
							//update our progress bar
							progressbar->setProgress((int)z);
							if (progressbar->wasCancelled())
								break;
						}
						else progress(z, volobj->depth);

						TIFFSetDirectory(tifobject, z);

						uint32 linesize = TIFFScanlineSize(tifobject);
						char *buf = (char*) _TIFFmalloc(linesize * volobj->height);
						for (uint32 itertiff = 0; itertiff < (uint32)volobj->height; itertiff++)
						{							
							TIFFReadScanline(tifobject, &buf[itertiff * linesize], itertiff, 0);
						}
						
						for(float y=0; y<volobj->height; y+=volobj->y_resize)
						{
							for(float x=0; x<volobj->width; x+=volobj->x_resize)
							{
								//QRgb pixel = pm.pixel(int(x+0.5),int(y+0.5));
								uint16 pixel = ((uint16*) buf)[(int)(x+0.5)+((int)(y+0.5)*volobj->width)];
								r = g = b = a = int(((pixel/65535.0)*255.0)+0.5);
								
								//if(r>255) printf("OHNOH!\n");
								//if(r<0) printf("OHNOH!\n");							
								//printf("pixel-16bit: %d \n", pixel);
	//							printf("pixel-8bit: %d \n\n", int(((pixel/65535.0)*255.0)+0.5));
	//							printf("r: %d\n", r);
								//printf("r: %d g: %d b: %d\n", r,g,b);

								if(x2>=res_x) x2=res_x-1;
								if(y2>=res_y) y2=res_y-1;
								if(z2>=res_z) z2=res_z-1;

								index_small = (((z2*res_y)+y2)*res_x)+x2;
								int index2d = (y2*res_x)+x2;
								
								volobj->voldata[(int)(z2)][index2d] = pixel;
								//volobj->voldata[(int)(z2)][3*index2d+1] = pixel;
								//volobj->voldata[(int)(z2)][3*index2d+2] = pixel;
								
								if(r==g && r==b && b==g) greyscale++;

								helper++;

								if(r!=0) red = true;
								if(g!=0) green = true;
								if(b!=0) blue = true;

								volobj->histogram16bit[(int)pixel]++;
								volobj->histogram[(3*int(a))+0]++;
								volobj->histogram[(3*int(a))+1]++;
								volobj->histogram[(3*int(a))+2]++;

								//printf("int: %d\n", (int)pixel);

								if(mode==1)
								{
									volobj->texture3d[3*index_small+0] = a;
									//volobj->histogram[(3*int(a))+0]++;
								}
								else if(mode==2)
								{
									volobj->texture3d[3*index_small+1] = a; 
									//volobj->histogram[(3*int(a))+1]++;
								}
								else if(mode==3)
								{
									volobj->texture3d[3*index_small+2] = a; 
									//volobj->histogram[(3*int(a))+2]++;					
								}				
								else if(mode==4)
								{
									volobj->texture3d[3*index_small+0] = r;
									volobj->texture3d[3*index_small+1] = g; 
									volobj->texture3d[3*index_small+2] = b; 
									//volobj->histogram[(3*int(r))+0]++;
									//volobj->histogram[(3*int(g))+1]++;
									//volobj->histogram[(3*int(b))+2]++;
								}
								else if(mode==5)
								{
									volobj->texture3d[3*index_small+0] =
									volobj->texture3d[3*index_small+1] =
									volobj->texture3d[3*index_small+2] = a;
								}								
								y2++;
							}
							x2++;
							y2=0;
						}
						z2++;
						x2=0;
						_TIFFfree(buf);
				}
				_TIFFfree(raster32bit);
			}
		}

		TIFFClose(tifobject);
		
		if(showGUI)
		{
			delete progressbar;
			progressbar=NULL;
		}
		else progress(volobj->depth, volobj->depth);
		//return;
	}
	else
	{
		//set our opt data directory
		QDir d;
		d.setSorting(QDir::Name);
		d.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
		d.setNameFilter("*.png *.bmp *.jpg *.tif");
		d.cd(dirname);

		//string list of our files in the directory
		QStringList strList;
		strList = d.entryList();

		printf("LOADING: number of files found: %d\n", d.count());

		QStringList discarded_filenames;
		QString tempfn;
		for(int i=0; i<d.count(); i++)
		{
			tempfn = d[i].ascii();

			if(tempfn.contains("_spr", false))
			{
				discarded_filenames += tempfn;
				//break;
			}
		}
	
		for(int i=0; i<discarded_filenames.size(); i++)
		{
			strList.remove(discarded_filenames.at(i));
		}

		volobj->file_name = dirname;

		if(d.count()==0)
		{
			QMessageBox* mb = new QMessageBox( "WARNING",
					"No files found in directory in [VOLIO.cpp load_PNG()] \n",QMessageBox::Warning,
					QMessageBox::Ok | QMessageBox::Default,0,0 );

			mb->exec();
			//exit(1);
			return;
		}

		//workout our 1st filename
		QString fn = dirname;
		fn += strList[0].ascii();

		TIFF* tifobject = TIFFOpen(fn, "r");

		if (tifobject) 
		{
			uint32* raster32bit=NULL;
			uint32 w, h;
			uint16 datatype, bpp;
			size_t npixels;
		
			TIFFGetField(tifobject, TIFFTAG_IMAGEWIDTH, &w);
			TIFFGetField(tifobject, TIFFTAG_IMAGELENGTH, &h);
			TIFFGetField(tifobject, TIFFTAG_BITSPERSAMPLE, &bpp);

			printf("--------------------------\n");
			printf("Tiff Info:\n");
			printf("%w:%d h:%d\n", (int)w, (int)h);
			printf("bpp: %d\n", (int) bpp);
			printf("--------------------------\n");
		
			/*TIFFGetField(tifobject, TIFFTAG_DATATYPE, &datatype);
			if(datatype==0)
				printf("TIFF_NOTYPE %d\n", datatype);
			else if(datatype=1)
				printf("TIFF_BYTE %d\n", datatype);
			else if(datatype==2)
				printf("TIFF_ASCII %d\n", datatype);
			else if(datatype==3)
				printf("TIFF_SHORT %d\n", datatype);
			else if(datatype==4)
				printf("TIFF_LONG \n", datatype);
			else if(datatype==5)
				printf("TIFF_RATIONAL \n", datatype);
			else if(datatype==6)
				printf("TIFF_SBYTE %d\n", datatype);
			else if(datatype==7)
				printf("TIFF_UNDEFINED %d\n", datatype);
			else if(datatype==8)
				printf("TIFF_SSHORT %d\n", datatype);
			else if(datatype==9)
				printf("TIFF_SLONG %d\n", datatype);
			else if(datatype==10)
				printf("TIFF_SRATIONAL %d\n", datatype);
			else if(datatype==11)
				printf("TIFF_FLOAT %d\n", datatype);
			else if(datatype==12)
				printf("TIFF_DOUBLE %d\n", datatype);
			else if(datatype==13)
				printf("TIFF_IFD %d\n", datatype);*/
		

			if(w==0 && h==0)
			{
				QMessageBox* mb = new QMessageBox( "WARNING",
						"Could not open a file in [VOLIO.cppload_PNG()] \n",QMessageBox::Warning,
						QMessageBox::Ok | QMessageBox::Default,0,0 );

				mb->exec();
				return;
			}

			//set up our texture info
			volobj->width = volobj->texwidth = w;
			volobj->height =volobj->texheight = h;
			volobj->depth =volobj->texdepth = strList.count(); 
			
			uint32 index_3D, index_small;
			float x2, y2, z2;
			int r,g,b,a;
			x2 = offset_x;
			y2 = offset_y;
			z2 = offset_z;
		
			float x,y,z;
		
			//==============================================================
			eval_resize_dimensions();
		
			int size = res_x*res_y*res_z*3;
		
			//lets have some texture memory
			if(volobj->texture3d==0 || mode==4 || mode==1) 
			{
				if(volobj->texture3d) delete[] volobj->texture3d;
				volobj->texture3d = NULL;

				//lets have some memory for our texture data
				volobj->texture3d = (GLubyte *) malloc(size * sizeof(GLubyte));

				//check if we managed to allocate some memory
				if(volobj->texture3d == NULL)
				{
					QMessageBox* mb = new QMessageBox( "ERROR",
							"Failed to allocate memory in [VOLIO.cpp load_PNG()] \n",QMessageBox::Warning,
							QMessageBox::Ok | QMessageBox::Default,0,0 );

					mb->exec();
					//exit(1);
					return;
				}

				//initialise everything to zero
				for(int ii=0; ii<size; ii++) volobj->texture3d[ii] = 0;
			}		
				
			if(mode==4 || mode==5) for(int i=0; i<3*256; i++) volobj->histogram[i]=0.0;

			if(showGUI)
			{
				progressbar = new Q3ProgressDialog("Loading Image Stack...", "Cancel", volobj->depth, 0, "progress", TRUE);		
				progressbar->setCaption("Please Wait");
				progressbar->raise();
			}

    		if(bpp==8)
			{				
				//now lets allocate some memory for our raw tiff...
				npixels = w*h;
				raster32bit = (uint32*) _TIFFmalloc(npixels * sizeof (uint32));
				if(raster32bit==NULL)
				{
					printf("ERROR allocating memory, raster32bit \n");
				}
			
				//DO SOME STUFF HERE WITH THE IMAGE.
				for(float z=0; z<volobj->depth; z+=volobj->z_resize)
				{
					if(showGUI)
					{
						//update our progress bar
						progressbar->setProgress((int)z);
						if (progressbar->wasCancelled())
							break;
					}
					else progress(z, volobj->depth);

					fn = dirname + strList[(int) z].ascii();
				
					tifobject = TIFFOpen(fn.latin1(), "r");
					TIFFReadRGBAImage(tifobject, w, h, raster32bit, 1);
				
					for(float x=0; x<volobj->width; x+=volobj->x_resize)
					{
						for(float y=0; y<volobj->height; y+=volobj->y_resize)
						{
							//QRgb pixel = pm.pixel(int(x+0.5),int(y+0.5));
							uint32 pixel = (uint32) raster32bit[(int)(y+0.5)+(int)(((volobj->width-1)-x)+0.5)*volobj->height];
						
							if(x2>=res_x) x2=res_x-1;
							if(y2>=res_y) y2=res_y-1;
							if(z2>=res_z) z2=res_z-1;

							index_small = ((((((int)(z2))*(int)(res_y))+((int)(y2)))*(int)(res_x))+((int)(x2)));
							//index_small = (((((x2)*res_y)+(z2))*res_x)+(y2));

							//printf("z: %d, %d, %d\n", (int)(res_z-z2), (int)(z2), (int)(res_z));
							//printf("index: %d\n", index_small);

							r = (uint8) TIFFGetR(pixel);
							g = (uint8) TIFFGetG(pixel);
							b = (uint8) TIFFGetB(pixel);
							a = (uint8) TIFFGetR(pixel);

							if(r==g && r==b && b==g) greyscale++;

							helper++;

							if(r!=0) red = true;
							if(g!=0) green = true;
							if(b!=0) blue = true;

							if(mode==1)
							{
								volobj->texture3d[3*index_small+0] = GLubyte(r);
								volobj->histogram[(3*int(r))+0]++;
							}
							if(mode==2)
							{
								volobj->texture3d[3*index_small+1] = GLubyte(g); 
								volobj->histogram[(3*int(g))+1]++;
							}
							if(mode==3)
							{
								volobj->texture3d[3*index_small+2] = GLubyte(b); 
								volobj->histogram[(3*int(b))+2]++;
							}				
							if(mode==4)
							{
								volobj->texture3d[3*index_small+0] = GLubyte(r);
								volobj->texture3d[3*index_small+1] = GLubyte(g); 
								volobj->texture3d[3*index_small+2] = GLubyte(b); 
								volobj->histogram[(3*int(r))+0]++;
								volobj->histogram[(3*int(g))+1]++;
								volobj->histogram[(3*int(b))+2]++;
							}
							if(mode==5)
							{
								volobj->texture3d[3*index_small+0] =
								volobj->texture3d[3*index_small+1] =
								volobj->texture3d[3*index_small+2] = GLubyte(a);
								volobj->histogram[(3*int(a))+0]++;
								volobj->histogram[(3*int(a))+1]++;
								volobj->histogram[(3*int(a))+2]++;
							}								
							x2++;
						}
						y2++;
						x2=0;
					}
					z2++;
					y2=0;
					TIFFClose(tifobject);
				}
				_TIFFfree(raster32bit);
			}
			else if(bpp==16)
			{
					volobj->is16bit=true;

					for(int i=0; i<65536; i++) volobj->histogram16bit[i] = 0.0;
				
					if(volobj->voldata!=NULL) 
					{
						printf("De-allocating 16bit memory\n");
					
						for(int i=0; i<volobj->voldatasize; i++)
							delete[] volobj->voldata[i];
					
						delete volobj->voldata;
						volobj->voldata = NULL;	
					}			

					printf("Trying to allocate 16bit memory\n");				

					volobj->voldatasize = res_z;
					int size2 = res_x*res_y;

					volobj->voldata = new uint16*[res_z];
					for(int i=0; i<res_z; i++)
					{
						volobj->voldata[i] = new uint16 [size2];
					
						for(int j=0; j<size2; j++)
							volobj->voldata[i][j]=0;
					}

					if(volobj->voldata==NULL)
					{
						printf("Error allocating 16bit memory\n");
						return;
					}
					else
					{
						printf("Memory allocated OK\n");
					}
				
					//DO SOME STUFF HERE WITH THE IMAGE.
					for(float z=0; z<volobj->depth; z+=volobj->z_resize)
					{
						if(showGUI)
						{
							//update our progress bar
							progressbar->setProgress((int)z);
							if (progressbar->wasCancelled())
								break;
						}
						else progress(z, volobj->depth);

						fn = dirname + strList[(int) z].ascii();
					
						tifobject = TIFFOpen(fn.latin1(), "r");

						uint32 linesize = TIFFScanlineSize(tifobject);
						char *buf = (char*) _TIFFmalloc(linesize * volobj->height);

						for (uint32 itertiff = 0; itertiff < (uint32)volobj->height; itertiff++)
						{
						
	//						printf("TIFFReadScanline: %d\n", TIFFReadScanline(tifobject, buf, itertiff, 0));
							TIFFReadScanline(tifobject, &buf[itertiff * linesize], itertiff, 0);
	
							//&raster32bit[itertiff * linesize] = (tdata_t) buf[itertiff];
						
			/*				for(uint32 itertiff2=0; itertiff2<volobj->width; itertiff2++)
							{
								int tempindex = (itertiff2 * volobj->width)+ (int) (x+0.5);
								raster32bit[tempindex] = 65365;//(uint32) buf[itertiff2];
							}*/
						}
						//_TIFFfree(buf);
					
	//						&raster32bit[itertiff * linesize]
						//TIFFReadRGBAImage(tifobject, w, h, raster32bit, 1);

					
						for(float y=0; y<volobj->height; y+=volobj->y_resize)
						{
							for(float x=0; x<volobj->width; x+=volobj->x_resize)
							{
								//QRgb pixel = pm.pixel(int(x+0.5),int(y+0.5));
								uint16 pixel = ((uint16*) buf)[(int)(x+0.5)+((int)(y+0.5)*volobj->width)];
								r = g = b = a = int(((pixel/65535.0)*255.0)+0.5);
							
								//if(r>255) printf("OHNOH!\n");
								//if(r<0) printf("OHNOH!\n");							
								//printf("pixel-16bit: %d \n", pixel);
	//							printf("pixel-8bit: %d \n\n", int(((pixel/65535.0)*255.0)+0.5));
	//							printf("r: %d\n", r);
								//printf("r: %d g: %d b: %d\n", r,g,b);

								if(x2>=res_x) x2=res_x-1;
								if(y2>=res_y) y2=res_y-1;
								if(z2>=res_z) z2=res_z-1;

								index_small = (((z2*res_y)+y2)*res_x)+x2;
								int index2d = (y2*res_x)+x2;
							
								volobj->voldata[(int)(z2)][index2d] = pixel;
								//volobj->voldata[(int)(z2)][3*index2d+1] = pixel;
								//volobj->voldata[(int)(z2)][3*index2d+2] = pixel;
							
								if(r==g && r==b && b==g) greyscale++;

								helper++;

								if(r!=0) red = true;
								if(g!=0) green = true;
								if(b!=0) blue = true;

								volobj->histogram16bit[(int)pixel]++;
								volobj->histogram[(3*int(a))+0]++;
								volobj->histogram[(3*int(a))+1]++;
								volobj->histogram[(3*int(a))+2]++;

								//printf("int: %d\n", (int)pixel);

								if(mode==1)
								{
									volobj->texture3d[3*index_small+0] = a;
									//volobj->histogram[(3*int(a))+0]++;
								}
								else if(mode==2)
								{
									volobj->texture3d[3*index_small+1] = a; 
									//volobj->histogram[(3*int(a))+1]++;
								}
								else if(mode==3)
								{
									volobj->texture3d[3*index_small+2] = a; 
									//volobj->histogram[(3*int(a))+2]++;					
								}				
								else if(mode==4)
								{
									volobj->texture3d[3*index_small+0] = r;
									volobj->texture3d[3*index_small+1] = g; 
									volobj->texture3d[3*index_small+2] = b; 
									//volobj->histogram[(3*int(r))+0]++;
									//volobj->histogram[(3*int(g))+1]++;
									//volobj->histogram[(3*int(b))+2]++;
								}
								else if(mode==5)
								{
									volobj->texture3d[3*index_small+0] =
									volobj->texture3d[3*index_small+1] =
									volobj->texture3d[3*index_small+2] = a;
								}								
								y2++;
							}
							x2++;
							y2=0;
						}
						z2++;
						x2=0;
						_TIFFfree(buf);
					//}
					TIFFClose(tifobject);
				}
				_TIFFfree(raster32bit);
			}
		}
	}
	printf("Loading TIFF data\n");
	
	if(showGUI)
	{
		delete progressbar;
		progressbar=NULL;
	}
	else progress(volobj->depth, volobj->depth);

	//set up our texture info
	volobj->width = volobj->texwidth = res_x;
	volobj->height =volobj->texheight = res_y;
	volobj->depth =volobj->texdepth = res_z;

	pad(1);
	
	if(greyscale==helper) greyscale=1;
	else greyscale=0;

	if(greyscale && mode==4)
	{
		volobj->is_greyscale = 1;
		volobj->has_red = 0;
		volobj->has_green = 0;
		volobj->has_blue = 0;
	}
	else if(mode==1)
	{
		volobj->is_greyscale = 0;
		volobj->has_red = red;
		volobj->has_green = 0;
		volobj->has_blue = 0;
	}
	else if(mode==2)
	{
		volobj->is_greyscale = 0;
		volobj->has_green = green;
	}
	else if(mode==3)
	{
		volobj->is_greyscale = 0;
		volobj->has_blue = blue;
	}
	else
	{
		volobj->is_greyscale = 0;
		volobj->has_red = red;
		volobj->has_green = green;
		volobj->has_blue = blue;
	}

	printf("(r,b,g): %d, %d, %d\n", (int)volobj->has_red, (int)volobj->has_green, (int)volobj->has_blue);
	printf("greyscale: %d\n", (int) volobj->is_greyscale);	
}
#endif

void VolIO::load_PNG(QString fn, int mode)//QImage frame
{

#ifdef LIBTIFF_SUPPORT
	volobj->is16bit=false;
#endif

	QString tiff_filename = fn;
	QString dirname = fn;
	//remove the filename of first file to get only dirname
	int pos = dirname.findRev(QString("/").latin1(), -1, false);
	dirname.replace(pos+1, dirname.length()-pos+1, "");

	red=green=blue=false;
	
	load_xyzspacing(dirname+"voxelspacing.txt");
	load_xyzscale(dirname+"voxelscale.txt");
	load_xyzscale_fromlogfile(dirname);
	
	//set our opt data directory
	QDir d;
	d.setSorting(QDir::Name);
	d.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
	d.setNameFilter("*.png *.bmp *.jpg *.tif");
	d.cd(dirname);

	//string list of our files in the directory
	QStringList strList;
	strList = d.entryList();

	QString tempfn;
	for(int i=0; i<d.count(); i++)
	{
		tempfn = d[i].ascii();

		if(tempfn.contains("_spr", false))
		{
			break;
		}

		tempfn = "";
	}

	if(tempfn!="") strList.remove(tempfn);

	//workout our 1st filename
	fn = dirname;
	fn += strList[strList.size()/2.0].ascii();

	volobj->file_name = dirname;

	if(d.count()==0)
	{
		QMessageBox* mb = new QMessageBox( "WARNING",
				"No files found in directory in [VOLIO.cpp load_PNG()] \n",QMessageBox::Warning,
				QMessageBox::Ok | QMessageBox::Default,0,0 );

		mb->exec();
		//exit(1);
		return;
	}

	printf("LOADING: Found %d images to load\n", d.count());

	int filenamemode=0;

	if(fn.contains(".bmp", false)) filenamemode=1;
	else if(fn.contains(".png", false)) filenamemode=2;
	else if(fn.contains(".jpg", false)) filenamemode=3;
#ifdef LIBTIFF_SUPPORT
	else if(fn.contains(".tif", false))
	{
		filenamemode=4;
		load_TIFF(tiff_filename, mode);
		return;
	}
#else
	else if(fn.contains(".tif", false)) filenamemode=4;
#endif
	else filenamemode = -1;

	//load in our first frame
	QImage img, img2;
	if(filenamemode==1) img.load(fn, "BMP");
	else if(filenamemode==2) img.load(fn, "PNG");
	else if(filenamemode==3) img.load(fn, "JPG");
	else if(filenamemode==4) img.load(fn, "TIFF");

	//printf("fn: %s\n", fn.latin1());
	//printf("filenamemode: %d\n", (int)filenamemode);
	
	int imgw = img.width();
	int imgh = img.height();

	printf("LOADING: drop img: %d x %d\n", imgw, imgh);
 
	if(img.width()==0 && img.height()==0)
	{
		//error opening file
//		printf("ERROR: Loading volume slices...  [VOLIO.cpp\load_PNG()]\n");
//		system("pause");
//		exit(1);
		QMessageBox* mb = new QMessageBox( "WARNING",
				"Could not open a file in [VOLIO.cppload_PNG()] \n",QMessageBox::Warning,
				QMessageBox::Ok | QMessageBox::Default,0,0 );

		mb->exec();
		//exit(1);
		return;
	}

	//set up our texture info
	volobj->width = img.width();
	volobj->height = img.height();
	volobj->depth = strList.count(); 

	img.reset();

	//==============================================================
	eval_resize_dimensions();
	int size = res_x*res_y*res_z*3;

	//if(volobj->has_red || volobj->has_green || volobj->has_blue)
	if(mode==2 || mode==3)
	{
		if(res_x!=volobj->texwidth || res_y != volobj->texheight || res_z !=volobj->texdepth)
		{
			QMessageBox* mb = new QMessageBox( "WARNING",
					"Could not append the data channel.\nData channel spatial dimensions differs from the current volume. \n",QMessageBox::Warning,
					QMessageBox::Ok | QMessageBox::Default,0,0 );

			mb->exec();
			//exit(1);
			return;
		}
	}

	volobj->texwidth = res_x;
	volobj->texheight = res_y;
	volobj->texdepth = res_z;

	//printf("DROP MODE: %d\n", mode);
	//printf("Volume XY: %d, %d\n", volobj->texwidth, volobj->texheight);
	//printf("Image XY: %d, %d\n", (int)drop_imgresizeX, (int)drop_imgresizeY);

	//lets have some memory
	//lets be clean
	if(volobj->texture3d==0 || mode==4 || mode==1) 
	{
		if(volobj->texture3d) delete volobj->texture3d;
		volobj->texture3d = NULL;

		//lets have some memory for our texture data
		volobj->texture3d = (GLubyte *) malloc(size * sizeof(GLubyte));

		//check if we managed to allocate some memory
 		if(volobj->texture3d == NULL)
		{
			QMessageBox* mb = new QMessageBox( "ERROR",
					"Failed to allocate memory in [VOLIO.cpp load_PNG()] \n",QMessageBox::Warning,
					QMessageBox::Ok | QMessageBox::Default,0,0 );

			mb->exec();
			//exit(1);
			return;
		}

		//initialise everything to zero
		for(int ii=0; ii<size; ii++) volobj->texture3d[ii] = 0;
	}

	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Loading PNG/BMP/JPG/TIFF Stack...", "Cancel", volobj->depth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
		progressbar->raise();
	}

	//printf("LOADING: RES_X: %d, %d, %d\n", res_x, res_y, res_z);	

	int index_small;
	float x2, y2, z2;
	float r,g,b,a;

	int helper=0;
	int greyscale=0;

	if(mode==4 || mode==5) for(int i=0; i<3*256; i++) volobj->histogram[i]=0.0;
	
	//printf("resize iter: %f, %f, %f\n", volobj->x_resize, volobj->y_resize, volobj->z_resize);

	QString ext;	
	if(filenamemode==1) ext = "BMP";
	else if(filenamemode==2) ext = "PNG";
	else if(filenamemode==3) ext = "JPG";
	else if(filenamemode==4) ext = "TIFF";
	//else return;

	float xmin;
	float xmax;
	float xinterp;
	float xneginterp;
	float ymin;
	float ymax;
	float yinterp;
	float yneginterp;
	float zmin;
	float zmax;
	float zinterp;
	float zneginterp;
	QRgb pixel000, pixel100, pixel010, pixel001, pixel110, pixel101, pixel011, pixel111;
	QRgb pixel;

	float xstart, xend;
	float ystart, yend;

	bool trilinearinterpolation_mode=false;
	QString fn2;

	printf("LOADING: offest: %f, %f, %f\n", (float)offset_x, (float)offset_y, (float)offset_z);
	printf("LOADING: start: %f, %f, %f\n", 0.0, 0.0, 0.0);
	printf("LOADING: end: %f, %f, %f\n", (float)volobj->width-volobj->x_resize, (float)volobj->height-volobj->y_resize, (float)volobj->depth-volobj->z_resize);
	printf("LOADING: steps: %f, %f, %f\n", volobj->x_resize, volobj->y_resize, volobj->z_resize);
	printf("LOADING: size: %d, %d, %d\n", volobj->width, volobj->height, volobj->depth);

	z2=offset_z;
	for(float z=0.0; z<volobj->depth-volobj->z_resize; z+=volobj->z_resize, z2++)
	{
		if(showGUI)
		{
			//update our progress bar
			progressbar->setProgress(z);
			if (progressbar->wasCancelled())
			    break;
		}
		else progress(z, volobj->depth);

		//workout our filename
		//read_texture reads as RGBA		
		zmin = floor(z);
		zmax = ceil(z);

		if(zmax>volobj->depth-1) zmax = volobj->depth-1;

		//printf("z: %f, zmin: %f, zmax: %f, z2: %f\n", z, zmin, zmax, z2);

		zinterp = z-zmin;
		zneginterp = (1.0-zinterp);

		fn = dirname;
		fn += strList[zmin].ascii();
		img.load(fn, ext);
	

		//printf("filename0: %s\n", fn.latin1());

		/*if(zmin==zmax)
		{
			xstart = 0.0;
			xend = volobj->width;

			ystart = 0.0;
			yend = volobj->height;

			trilinearinterpolation_mode = false;
			//printf("no tri\n");
		}*/
		//else
		{
			xstart = volobj->x_resize;
			xend = volobj->width-volobj->x_resize;

			ystart = volobj->y_resize;
			yend = volobj->height-volobj->y_resize;

			trilinearinterpolation_mode = true;
			fn2 = dirname;
			fn2 += strList[zmax].ascii();
			img2.load(fn2, ext);
			//printf("tri\n");
		}

		//printf("filename1: %s\n\n", fn2.latin1());

		/*if(img.format()!=QImage::Format_RGB32 || img.format()!=QImage::Format_ARGB32)
		{
			img = img.convertToFormat(QImage::Format_RGB32);
		}*/

	 	if(img.width()==imgw && img.height()==imgh)
		{
			y2=offset_y;
			for(float y=ystart; y<yend-volobj->y_resize; y+=volobj->y_resize, y2++)
			{

				ymin = floor(y);
				ymax = ceil(y);
				if(ymax>volobj->height-1) ymax = volobj->height-1;
				yinterp = y-ymin;
				yneginterp = (1.0-yinterp);
	
//				printf("y: %f, ymin: %f, ymax: %f\n", y, ymin, ymax);

				x2=offset_x;
				for(float x=xstart; x<xend-volobj->x_resize; x+=volobj->x_resize, x2++)
				{
					//printf("x: %f\n", x);
					//printf("y: %f\n", y);
					//printf("z: %f\n", z);
					index_small = 3*(((((int)z2*(int)res_y)+(int)y2)*(int)res_x)+(int)x2);
					//printf("xyz2: %d, %d, %d\n", (int) x2, (int)y2, (int) z2);

					/*if(trilinearinterpolation_mode==false)
					{
						//-------------------------------
						//Nearest Neighbour resampling
						//-------------------------------
						int x22 = floor(x);
						int y22 = floor(y);

						double xdecimal = x-(double)x22;
						double ydecimal = y-(double)y22;

						int x33, y33;
						if(xdecimal>0.5) x33 = x22 + 1;
						else x33 = x22;
						if(ydecimal>0.5) y33 = y22 + 1;
						else y33 = y22;

						//printf("x: %f, y: %f, x22: %d, y22: %d, x33: %d, y33: %d\n", x, y, x22, y22, x33, y33);

						pixel = img.pixel(x33, y33);
						//printf("x: %f, y: %f\n", x, y);

						//pixel = img.pixel((int)x, (int)y);
						r = qRed(pixel);
						g = qGreen(pixel);
						b = qBlue(pixel);
						a = qGray(pixel);
					}
					else*/
					{

						xmin = floor(x);
						xmax = ceil(x);
						if(xmax>volobj->width-1) xmax = volobj->width-1;
						xinterp = x-xmin;
						xneginterp = (1.0-xinterp);

						//printf("min: %d, %d, %d\n", xmin, ymin, zmin);
						//printf("max: %d, %d, %d\n", xmax, ymax, zmax);
						//printf("x: %f, xmin: %f, xmax: %f\n", x, xmin, xmax);

						//-------------------------------
						//Tri-Linear Resampling
						//-------------------------------

						//printf("Reading pixels\n");

						pixel000 = img.pixel((int)xmin, (int)ymin);
						pixel100 = img.pixel((int)xmax, (int)ymin);
						pixel010 = img.pixel((int)xmin, (int)ymax);
						pixel001 = img2.pixel((int)xmin, (int)ymin);
						pixel110 = img.pixel((int)xmax, (int)ymax);
						pixel101 = img2.pixel((int)xmax, (int)ymin);
						pixel011 = img2.pixel((int)xmin, (int)ymax);
						pixel111 = img2.pixel((int)xmax, (int)ymax);

						r = (float)qRed(pixel000)*(xneginterp*yneginterp*zneginterp)
						  + (float)qRed(pixel100)*(xinterp*yneginterp*zneginterp)
						  + (float)qRed(pixel010)*(xneginterp*yinterp*zneginterp)
						  + (float)qRed(pixel001)*(xneginterp*yneginterp*zinterp)
						  + (float)qRed(pixel110)*(xinterp*yinterp*zneginterp)
						  + (float)qRed(pixel101)*(xinterp*yneginterp*zinterp)
						  + (float)qRed(pixel011)*(xneginterp*yinterp*zinterp)
						  + (float)qRed(pixel111)*(xinterp*yinterp*zinterp);

						g = (float)qGreen(pixel000)*(xneginterp*yneginterp*zneginterp)
						  + (float)qGreen(pixel100)*(xinterp*yneginterp*zneginterp)
						  + (float)qGreen(pixel010)*(xneginterp*yinterp*zneginterp)
						  + (float)qGreen(pixel001)*(xneginterp*yneginterp*zinterp)
						  + (float)qGreen(pixel110)*(xinterp*yinterp*zneginterp)
						  + (float)qGreen(pixel101)*(xinterp*yneginterp*zinterp)
						  + (float)qGreen(pixel011)*(xneginterp*yinterp*zinterp)
						  + (float)qGreen(pixel111)*(xinterp*yinterp*zinterp);

						b = (float)qBlue(pixel000)*(xneginterp*yneginterp*zneginterp)
						  + (float)qBlue(pixel100)*(xinterp*yneginterp*zneginterp)
						  + (float)qBlue(pixel010)*(xneginterp*yinterp*zneginterp)
						  + (float)qBlue(pixel001)*(xneginterp*yneginterp*zinterp)
						  + (float)qBlue(pixel110)*(xinterp*yinterp*zneginterp)
						  + (float)qBlue(pixel101)*(xinterp*yneginterp*zinterp)
						  + (float)qBlue(pixel011)*(xneginterp*yinterp*zinterp)
						  + (float)qBlue(pixel111)*(xinterp*yinterp*zinterp);

						a = (float)qGray(pixel000)*(xneginterp*yneginterp*zneginterp)
						  + (float)qGray(pixel100)*(xinterp*yneginterp*zneginterp)
						  + (float)qGray(pixel010)*(xneginterp*yinterp*zneginterp)
						  + (float)qGray(pixel001)*(xneginterp*yneginterp*zinterp)
						  + (float)qGray(pixel110)*(xinterp*yinterp*zneginterp)
						  + (float)qGray(pixel101)*(xinterp*yneginterp*zinterp)
						  + (float)qGray(pixel011)*(xneginterp*yinterp*zinterp)
						  + (float)qGray(pixel111)*(xinterp*yinterp*zinterp);
					}

				//	printf("rgb: %f, %f, %f\n", r, g, b);

					if(r==g && r==b && b==g) greyscale++;
					helper++;

					if(r!=0) red = true;
					if(g!=0) green = true;
					if(b!=0) blue = true;
					//printf("Setting pixels\n");

					//printf("index_small: %d - %d\n", index_small, size);

					//printf("mode: %d\n", mode);
					if(mode==1)
					{
						volobj->texture3d[index_small+0] = GLubyte(r);
						volobj->histogram[(3*int(r))+0]++;
					}
					else if(mode==2)
					{
						volobj->texture3d[index_small+1] = GLubyte(g); 
						volobj->histogram[(3*int(g))+1]++;
					}
					else if(mode==3)
					{
						volobj->texture3d[index_small+2] = GLubyte(b); 
						volobj->histogram[(3*int(b))+2]++;
					}				
					else if(mode==4)
					{
						volobj->texture3d[index_small+0] = GLubyte(r);
						volobj->texture3d[index_small+1] = GLubyte(g); 
						volobj->texture3d[index_small+2] = GLubyte(b);
						volobj->histogram[(3*int(r))+0]++;
						volobj->histogram[(3*int(g))+1]++;
						volobj->histogram[(3*int(b))+2]++;
					}
					else if(mode==5)
					{
						volobj->texture3d[index_small+0] =
						volobj->texture3d[index_small+1] =
						volobj->texture3d[index_small+2] = GLubyte(r);
						volobj->histogram[(3*int(a))+0]++;
						volobj->histogram[(3*int(a))+1]++;
						volobj->histogram[(3*int(a))+2]++;
					}

					//printf("done\n");
				}
			}
		}
	}

	if(showGUI)
	{
		progressbar->setProgress(volobj->depth);
		delete progressbar;
		progressbar=NULL;
	}
	else progress(volobj->depth, volobj->depth);

	//set up our texture info
	volobj->width = volobj->texwidth = res_x;
	volobj->height =volobj->texheight = res_y;
	volobj->depth =volobj->texdepth = res_z;

	pad(1);

	if(greyscale==helper) greyscale=1;
	else greyscale=0;

	//cout<<"LOADING: (r,g,b) "<< red<<" "<<green<<" "<<blue<<endl;
	//cout<<"LOADING: greyscale "<<greyscale<<endl;

	if(greyscale && mode==4)
	{
		volobj->is_greyscale = 1;
		volobj->has_red = 0;
		volobj->has_green = 0;
		volobj->has_blue = 0;
	}
	else if(mode==1)
	{
		volobj->is_greyscale = 0;
		volobj->has_red = red;
		volobj->has_green = 0;
		volobj->has_blue = 0;
	}
	else if(mode==2)
	{
		volobj->is_greyscale = 0;
		volobj->has_green = green;
	}
	else if(mode==3)
	{
		volobj->is_greyscale = 0;
		volobj->has_blue = blue;
	}
	else
	{
		volobj->is_greyscale = 0;
		volobj->has_red = red;
		volobj->has_green = green;
		volobj->has_blue = blue;
	}

	//printf("(r,b,g): %d, %d, %d\n", (int)volobj->has_red, (int)volobj->has_green, (int)volobj->has_blue);
	//printf("greyscale: %d\n", (int) volobj->is_greyscale);
}
void VolIO::pad(int pad_size)
{
	//return;

	int x, y, z, index_3D;

	//ZX PLANE
	for (z =0; z<volobj->texdepth; z++)
	{	
		for(x=0; x<volobj->texwidth; x++)
		{
			for(y=0; y<pad_size; y++)
			{
				index_3D = ((((((volobj->texdepth-1)-z)*volobj->texheight)+y)*volobj->texwidth)+x);
			
				volobj->texture3d[3*index_3D+0] = 0;
				volobj->texture3d[3*index_3D+1] = 0;
				volobj->texture3d[3*index_3D+2] = 0;
			}
			for(y=volobj->texheight-pad_size; y<volobj->texheight; y++)
			{
				index_3D = ((((((volobj->texdepth-1)-z)*volobj->texheight)+y)*volobj->texwidth)+x);
			
				volobj->texture3d[3*index_3D+0] = 0;
				volobj->texture3d[3*index_3D+1] = 0;
				volobj->texture3d[3*index_3D+2] = 0;
			}
		}
	}
	//XY PLANE
	for (x =0; x<volobj->texwidth; x++)
	{	
		for(y=0; y<volobj->texheight; y++)
		{
			for(z=0; z<pad_size; z++)
			{
				index_3D = ((((((volobj->texdepth-1)-z)*volobj->texheight)+y)*volobj->texwidth)+x);
			
				volobj->texture3d[3*index_3D+0] = 0;
				volobj->texture3d[3*index_3D+1] = 0;
				volobj->texture3d[3*index_3D+2] = 0;
			}
			for(z=volobj->texdepth-pad_size; z<volobj->texdepth; z++)
			{
				index_3D = ((((((volobj->texdepth-1)-z)*volobj->texheight)+y)*volobj->texwidth)+x);
			
				volobj->texture3d[3*index_3D+0] = 0;
				volobj->texture3d[3*index_3D+1] = 0;
				volobj->texture3d[3*index_3D+2] = 0;
			}
		}
	}
	//ZY PLANE
	for (z =0; z<volobj->texdepth; z++)
	{	
		for(y=0; y<volobj->texheight; y++)
		{
			for(x=0; x<pad_size; x++)
			{
				index_3D = ((((((volobj->texdepth-1)-z)*volobj->texheight)+y)*volobj->texwidth)+x);
			
				volobj->texture3d[3*index_3D+0] = 0;
				volobj->texture3d[3*index_3D+1] = 0;
				volobj->texture3d[3*index_3D+2] = 0;
			}
			for(x=volobj->texwidth-pad_size; x<volobj->texwidth; x++)
			{
				index_3D = ((((((volobj->texdepth-1)-z)*volobj->texheight)+y)*volobj->texwidth)+x);
			
				volobj->texture3d[3*index_3D+0] = 0;
				volobj->texture3d[3*index_3D+1] = 0;
				volobj->texture3d[3*index_3D+2] = 0;
			}
		}
	}
}
int VolIO::makepower2(int val)
{
   int newval=val;

	if(val<=8) 
		newval = 8;
	else if(val<=16) 
		newval = 16;
	else if(val<=32) 
		newval = 32;
	else if(val<=64) 
		newval = 64;
	else if(val<=128)
		newval = 128;
	else if(val<=256) 
		newval = 256;
	else if(val<=512)
		newval = 512;
	else if(val<=1024)
		newval = 1024;
	else if(val<=2048)
		newval = 2048;
	return newval;
}
void VolIO::resize(void)
{
	printf("RESIZING\n");

	//if data dimensions are already under maxsize, do nothing
	if(volobj->width<=volobj->maxsizeX && volobj->height<=volobj->maxsizeY && volobj->depth<=volobj->maxsizeZ ) return;

	//float x_resize, y_resize, z_resize;
	volobj->x_resize = volobj->width/(float) volobj->maxsizeX;
	volobj->y_resize = volobj->height/(float) volobj->maxsizeY;
	volobj->z_resize = volobj->depth/(float) volobj->maxsizeZ;

	if(volobj->x_resize<1.0) volobj->x_resize = 1;
	if(volobj->y_resize<1.0) volobj->y_resize = 1;
	if(volobj->z_resize<1.0) volobj->z_resize = 1;

	float maxresize = 0;
	maxresize = max((float)maxresize, (float)volobj->x_resize);
	maxresize = max((float)maxresize, (float)volobj->y_resize);
	maxresize = max((float)maxresize, (float)volobj->z_resize);
	volobj->x_resize = volobj->y_resize = volobj->z_resize = maxresize;

	res_x = ((float)volobj->width/volobj->x_resize)+0.5; 
	res_y = ((float)volobj->height/volobj->y_resize)+0.5;
	res_z = ((float)volobj->depth/volobj->z_resize)+0.5; 

	float maxres1 = 0.0;
	maxres1 = max((float)maxres1, (float)res_x);
	maxres1 = max((float)maxres1, (float)res_y);
	maxres1 = max((float)maxres1, (float)res_z);
	volobj->maxres = maxres1;

	offset_x = ((makepower2(maxres1)-res_x)/2.0);
	offset_y = ((makepower2(maxres1)-res_y)/2.0); 
	offset_z = ((makepower2(maxres1)-res_z)/2.0); 

	res_x = makepower2(maxres1);
	res_y = makepower2(maxres1);
	res_z = makepower2(maxres1);

	printf("Original Dimensions: %d, %d, %d\n", volobj->width, volobj->height, volobj->depth);
	printf("Resized Dimensions: %d, %d, %d\n", res_x, res_y, res_z);
	printf("Resize Factor: %f, %f, %f\n", volobj->x_resize, volobj->y_resize, volobj->z_resize);
	printf("Offsets: %d, %d, %d\n", offset_x, offset_y, offset_z);

	int size = res_x*res_y*res_z*3;

	//lets have some memory
	unsigned char* temp_storage = NULL;
	temp_storage = (GLubyte *)malloc(size*sizeof(GLubyte));
	
	//initialise everything to zero
	for(int ii=0; ii<size; ii++) temp_storage[ii] = 0;

	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Resizing Volume...", "Cancel", volobj->depth, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
	}

	int index_big, index_small;
	float x2, y2, z2;
	x2 = offset_x;
	y2 = offset_y;
	z2 = offset_z;
	for(float z=0; z<volobj->depth; z+=volobj->z_resize)
	{
		if(showGUI)
		{
			progressbar->setProgress(z);
			if (progressbar->wasCancelled())
				break;
		}
		else progress(z, volobj->depth);

		for(float y=0; y<volobj->height; y+=volobj->y_resize)
		{
			for(float x=0; x<volobj->width; x+=volobj->x_resize)
			{
				if(x2>=res_x) x2=res_x-1;
				if(y2>=res_y) y2=res_y-1;
				if(z2>=res_z) z2=res_z-1;

				index_big =   ((( ((int)(z+0.5)*volobj->height) + (int)(y+0.5)) * volobj->width) + (int)(x+0.5));
				index_small = ((( (z2*res_y) + y2) * res_x) + x2);

				temp_storage[3*index_small+0] = volobj->texture3d[3*index_big+0];
				temp_storage[3*index_small+1] = volobj->texture3d[3*index_big+1];
				temp_storage[3*index_small+2] = volobj->texture3d[3*index_big+2];
				
				y2++;
			}
			x2++;
			y2=0;
		}
		z2++;
		x2=0;
	}

	if(showGUI)
	{
		delete progressbar;
		progressbar=NULL;
	}
	else progress(volobj->depth, volobj->depth);

	delete[] volobj->texture3d;

	//lets have some memory
	volobj->texture3d = (GLubyte *)malloc(size*sizeof(GLubyte));

	for(int i=0; i<size; i++) volobj->texture3d[i] = temp_storage[i];

	delete[] temp_storage;

	//set up our texture info
	volobj->width = volobj->texwidth = res_x;
	volobj->height =volobj->texheight = res_y;
	volobj->depth =volobj->texdepth = res_z; 
}

//EXPERIMENTAL CODE
void VolIO::init_subimageloading(QString dirname, int mode)
{
	volobj->file_name = dirname;
	load_xyzspacing(dirname+"voxelspacing.txt");
	load_xyzscale(dirname+"voxelscale.txt");
	load_xyzscale_fromlogfile(dirname);

	red=green=blue=false;
//	volobj->is16bit=false;
	
	//set our opt data directory
	QDir d;
	d.setSorting(QDir::Name);
	d.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
	d.setNameFilter("*.png *.bmp *.jpg *.tif");
	d.cd(dirname);
			
	//string list of our files in the directory
	QStringList strList;
	strList = d.entryList();

	QString tempfn;
	for(int i=0; i<d.count(); i++)
	{
		tempfn = d[i].ascii();

		if(tempfn.contains("_spr", false))
		{
			break;
		}

		tempfn = "";
	}

	if(tempfn!="") strList.remove(tempfn);

	volobj->file_name = dirname;

	if(d.count()==0)
	{
		QMessageBox* mb = new QMessageBox( "WARNING",
				"No files found in directory in [VOLIO.cpp load_PNG()] \n",QMessageBox::Warning,
				QMessageBox::Ok | QMessageBox::Default,0,0 );

		mb->exec();
		//exit(1);
		return;
	}

	//workout our 1st filename
	QString fn = dirname;
	fn += strList[0].ascii();

	int filenamemode=0;

	if(fn.contains(".bmp", false)) filenamemode=1;
	else if(fn.contains(".png", false)) filenamemode=2;
	else if(fn.contains(".jpg", false)) filenamemode=3;
	else if(fn.contains(".tif", false)) filenamemode=4;
	else filenamemode = -1;

	//load in our first frame
	QImage pm;
	if(filenamemode==1) pm.load(fn, "BMP");
	else if(filenamemode==2) pm.load(fn, "PNG");
	else if(filenamemode==3) pm.load(fn, "JPG");

	//printf("fn: %s\n", fn.latin1());
	//printf("filenamemode: %d\n", (int)filenamemode);
	
	if(pm.width()==0 && pm.height()==0)
	{
		//error opening file
//		printf("ERROR: Loading volume slices...  [VOLIO.cpp\load_PNG()]\n");
//		system("pause");
//		exit(1);
		QMessageBox* mb = new QMessageBox( "WARNING",
				"Could not open a file in [VOLIO.cpp load_PNG()] \n",QMessageBox::Warning,
				QMessageBox::Ok | QMessageBox::Default,0,0 );

		mb->exec();
		//exit(1);
		return;
	}

	if(mode==4 || mode==5) for(int i=0; i<3*256; i++) volobj->histogram[i]=0.0;

	//set up our texture info
	res_x = volobj->width = volobj->texwidth = pm.width();
	res_y = volobj->height =volobj->texheight = pm.height();
	res_z = volobj->depth =volobj->texdepth = strList.count();

	volobj->x_resize = 1.0;
	volobj->y_resize = 1.0;
	volobj->z_resize = 1.0;

	volobj->maxres = volobj->width;
	volobj->maxres = max(volobj->height, volobj->maxres);
	volobj->maxres = max(volobj->depth, volobj->maxres);

	offset_x = offset_y = offset_z = 0;

	volobj->is_greyscale = true;
	volobj->has_red = true;
	volobj->has_green = false;
	volobj->has_blue = false;
}
QStringList VolIO::load_subimage_strngList(void)
{
	QString dirname = volobj->file_name;
	load_xyzspacing(dirname+"voxelspacing.txt");
	load_xyzscale(dirname+"voxelscale.txt");
	load_xyzscale_fromlogfile(dirname);

	//printf("Loading dir: %s\n", dirname.latin1());

	QDir d;
	d.setSorting(QDir::Name);
	d.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );
	d.setNameFilter("*.png *.bmp *.jpg *.tif");
	d.cd(dirname);
			
	//string list of our files in the directory
	QStringList strList;
	strList = d.entryList();

	QString tempfn;
	for(int i=0; i<d.count(); i++)
	{
		tempfn = d[i].ascii();

		if(tempfn.contains("_spr", false))
		{
			break;
		}

		tempfn = "";
	}

	if(tempfn!="") strList.remove(tempfn);
	//workout our 1st filename
	QString fn = dirname;
	fn += strList[0].ascii();

	int filenamemode=0;

	if(fn.contains(".bmp", false)) filenamemode=1;
	else if(fn.contains(".png", false)) filenamemode=2;
	else if(fn.contains(".jpg", false)) filenamemode=3;
	else if(fn.contains(".tif", false)) filenamemode=4;
	else filenamemode = -1;

	//load in our first frame
	QImage pm;
	if(filenamemode==1) pm.load(fn, "BMP");
	else if(filenamemode==2) pm.load(fn, "PNG");
	else if(filenamemode==3) pm.load(fn, "JPG");
	else if(filenamemode==4)
	{
		return strList;
	}
	
	//printf("fn: %s\n", fn.latin1());
	//printf("filenamemode: %d\n", (int)filenamemode);
	
	if(pm.width()==0 && pm.height()==0)
	{
		//error opening file
//		printf("ERROR: Loading volume slices...  [VOLIO.cpp\load_PNG()]\n");
//		system("pause");
//		exit(1);
		QMessageBox* mb = new QMessageBox( "WARNING",
				"Could not open a file in [VOLIO.cppload_PNG()] \n",QMessageBox::Warning,
				QMessageBox::Ok | QMessageBox::Default,0,0 );

		mb->exec();
		//exit(1);
		return strList;
	}

	//set up our texture info
	volobj->width = volobj->texwidth = pm.width();
	volobj->height =volobj->texheight = pm.height();
	volobj->depth =volobj->texdepth = strList.count(); 

	return strList;
}
void VolIO::pad_subimage(int pad_size, int xstart, int ystart, int zstart, int brickresX, int brickresY, int brickresZ, unsigned char* data)
{
	return;
	int x,y,z, index_3D;

	int xend = xstart+brickresX;
	int yend = ystart+brickresY; 
	int zend = zstart+brickresZ;

	//ZX PLANE
	for (z =zstart; z<zend; z++)
	{	
		for(x=xstart; x<xend; x++)
		{
			for(y=ystart; y<ystart+pad_size; y++)
			{
				index_3D = (((((z-zstart)*brickresY)+(y-ystart))*brickresX)+(x-xstart));
			
				volobj->texture3d[3*index_3D+0] = 0;
				volobj->texture3d[3*index_3D+1] = 0;
				volobj->texture3d[3*index_3D+2] = 0;
			}
			for(y=yend-pad_size; y<yend; y++)
			{
				index_3D = (((((z-zstart)*brickresY)+(y-ystart))*brickresX)+(x-xstart));
			
				volobj->texture3d[3*index_3D+0] = 0;
				volobj->texture3d[3*index_3D+1] = 0;
				volobj->texture3d[3*index_3D+2] = 0;
			}
		}
	}
	//XY PLANE
	for (x =xstart; x<xend; x++)
	{	
		for(y=ystart; y<yend; y++)
		{
			for(z=zstart; z<zstart+pad_size; z++)
			{
				index_3D = (((((z-zstart)*brickresY)+(y-ystart))*brickresX)+(x-xstart));
			
				volobj->texture3d[3*index_3D+0] = 0;
				volobj->texture3d[3*index_3D+1] = 0;
				volobj->texture3d[3*index_3D+2] = 0;
			}
			for(z=zend-pad_size; z<zend; z++)
			{
				index_3D = (((((z-zstart)*brickresY)+(y-ystart))*brickresX)+(x-xstart));
			
				volobj->texture3d[3*index_3D+0] = 0;
				volobj->texture3d[3*index_3D+1] = 0;
				volobj->texture3d[3*index_3D+2] = 0;
			}
		}
	}
	//ZY PLANE
	for (z =zstart; z<zend; z++)
	{	
		for(y=ystart; y<yend; y++)
		{
			for(x=xstart; x<xstart+pad_size; x++)
			{
				index_3D = (((((z-zstart)*brickresY)+(y-ystart))*brickresX)+(x-xstart));
			
				volobj->texture3d[3*index_3D+0] = 0;
				volobj->texture3d[3*index_3D+1] = 0;
				volobj->texture3d[3*index_3D+2] = 0;
			}
			for(x=xstart-pad_size; x<xend; x++)
			{
				index_3D = (((((z-zstart)*brickresY)+(y-ystart))*brickresX)+(x-xstart));
			
				volobj->texture3d[3*index_3D+0] = 0;
				volobj->texture3d[3*index_3D+1] = 0;
				volobj->texture3d[3*index_3D+2] = 0;
			}
		}
	}

}
bool VolIO::load_subimage(QStringList strList, int xstart, int ystart, int zstart, int brickresX, int brickresY, int brickresZ, unsigned char* data)
{
	int xend = xstart+brickresX;
	int yend = ystart+brickresY; 
	int zend = zstart+brickresZ;
	QString dirname = volobj->file_name;

	QRgb pixel;
	QString fn;
	int index_small;
	QImage pm;

	//if(volobj->texture3d_brick!=0) delete volobj->texture3d_brick;
	//volobj->texture3d_brick = new unsigned char[brickresX*brickresY*brickresZ];
	
	/*progressbar = new Q3ProgressDialog("Loading PNG/BMP/JPG/TIFF sub-Image...", "Cancel", zend-zstart, 0, "progress", TRUE);		
	progressbar->setCaption("Please Wait");
	progressbar->raise();*/

	bool isempty=true;

	int r,g,b,a;
	for(float z=zstart; z<zend; z++)
	{
		//update our progress bar
        /*progressbar->setProgress(z-zstart);
        if (progressbar->wasCancelled())
            break;*/

		//if(z>strList.count()-1) z = strList.count()-1;

		//workout our filename
		fn = dirname;
		fn += strList[z].ascii(); //(strList.count()-1)-

		//printf("fn: %s\n", fn.latin1());

		//read_texture reads as RGBA		
		//if(filenamemode==1) pm.load(fn, "BMP");
		//if(filenamemode==2) 
		pm.load(fn, "PNG");
		//if(filenamemode==3) pm.load(fn, "JPG");
		//if(filenamemode==4)pm.load(fn, "TIFF");
		
//		printf("w: %d, h: %d \n", pm.width(), pm.height());
		for(float x=xstart; x<xend; x++)
		{
			for(float y=ystart; y<yend; y++)
			{
				//printf("%d, %d\n", int(x+0.5), int(y+0.5));
				/*if(x>pm.width()-1)  x = pm.width()-1;
				if(y>pm.height()-1) y = pm.height()-1;
				if(x<0) x=0;
				if(y<0) y=0;*/

				//make sure we round up to nearest int for x,y
				pixel = pm.pixel(int(x),int(y));
				
				index_small = (((((z-zstart)*brickresY)+(y-ystart))*brickresX)+(x-xstart));

				r = qGray(pixel);
				/*g = qGreen(pixel);
				b = qBlue(pixel);
				a = qGray(pixel);*/

				if(r!=0) isempty = false;
				/*if(g!=0) green = true;
				if(b!=0) blue = true;*/

				/*if(x==ystart) r = 0;
				if(y==xstart) r = 0;
				if(z==zstart) r = 0;

				if(x==xend-1) r = 0;
				if(y==yend-1) r = 0;
				if(z==zend-1) r = 0;*/

				//printf("voxel: %d\n", (int) r);
				data[index_small] = GLubyte(r);
				//volobj->texture3d_brick[index_small] = GLubyte(r);	
			}
		}

		//pm.reset();
	}

	//pad_subimage(1, xstart, ystart, zstart, brickresX, brickresY, brickresZ, data);
	//delete progressbar;
	//progressbar=NULL;

	return isempty;
}

void VolIO::load_sliceThread(const QtThreadedSliceLoader &threadloader)
{
//		printf("fn: %s\n", fn.latin1());
	QImage pm;

	//read_texture reads as RGBA		
	pm.load(threadloader.fn, threadloader.ext);
	
	int index_3D, index_small;
	int r,g,b,a;

	int x3 = threadloader.x2;
	int y3 = threadloader.y2;
	int z3 = threadloader.z2; 

//		printf("w: %d, h: %d \n", pm.width(), pm.height());
	for(float x=0; x<threadloader.volobj->width-1; x+=threadloader.volobj->x_resize)
	{
		for(float y=0; y<threadloader.volobj->height-1; y+=threadloader.volobj->y_resize)
		{
			//printf("%d, %d\n", int(x+0.5), int(y+0.5));

			//make sure we round up to nearest int for x,y
			QRgb pixel = pm.pixel(int(x+0.5),int(y+0.5));
			
			if(x3>=threadloader.volobj->resized_texwidth) x3=threadloader.volobj->resized_texwidth-1;
			if(y3>=threadloader.volobj->resized_texheight) y3=threadloader.volobj->resized_texheight-1;
			if(z3>=threadloader.volobj->resized_texdepth) z3=threadloader.volobj->resized_texdepth-1;

			index_small = (((((z3)*threadloader.volobj->resized_texheight)+(y3))*threadloader.volobj->resized_texwidth)+(x3));

			r = qRed(pixel);
			g = qGreen(pixel);
			b = qBlue(pixel);
			a = qGray(pixel);

			//if(r!=0 || g!=0 || b!=0)
			{
				if(threadloader.mode==4)
				{
					threadloader.volobj->texture3d[3*index_small+0] = GLubyte(r);
					threadloader.volobj->texture3d[3*index_small+1] = GLubyte(g); 
					threadloader.volobj->texture3d[3*index_small+2] = GLubyte(b); 
					threadloader.volobj->histogram[(3*int(r))+0]++;
					threadloader.volobj->histogram[(3*int(g))+1]++;
					threadloader.volobj->histogram[(3*int(b))+2]++;
				}
				else if(threadloader.mode==1)
				{
					threadloader.volobj->texture3d[3*index_small+0] = GLubyte(r);
					threadloader.volobj->histogram[(3*int(r))+0]++;
				}
				else if(threadloader.mode==2)
				{
					threadloader.volobj->texture3d[3*index_small+1] = GLubyte(g); 
					threadloader.volobj->histogram[(3*int(g))+1]++;
				}
				else if(threadloader.mode==3)
				{
					threadloader.volobj->texture3d[3*index_small+2] = GLubyte(b); 
					threadloader.volobj->histogram[(3*int(b))+2]++;
				}				
			}

			y3++;
		}
		x3++;
		y3=0;
	}

	//return threadloader;
}
QtThreadedSliceLoader runme(QtThreadedSliceLoader threadloader)
{
	threadloader.run();
	return threadloader;
}

void VolIO::set_metadata(float x, float y, QString descp)
{
	istringstream  input(descp.toStdString());
	Tokenizer token(input);
	string label;
	float z_val = -1;
	bool found_meta=false;

	//until oef is reached step through each token
	while(!input.eof())
	{
		//get the first string lable
		label = token.sval();

		if(token.ttype()==TT_WORD)
		{
			if(label=="spacing")
			{
				token.nextToken();
				token.nextToken();
				if(token.ttype()==TT_NUMBER)
				{
					found_meta=true;
					z_val = token.nval();
					printf("val: %f\n", z_val);
				}
			}
			else
			{
				//cout<<"WARNING: Unrecognized Label..."<<endl;
				token.nextToken();
			}
		}
		else token.nextToken();
	}

	if(z_val==-1) z_val = 1.0;
	if(x==0.0) x=1.0;
	if(y==0.0) y=1.0;

	if(found_meta)
	{
		volobj->xscale = 1.0/x;
		volobj->yscale = 1.0/y;
		volobj->zscale = z_val;
	
		volobj->xsize = 1.0/x;
		volobj->ysize = 1.0/y;
		volobj->zsize = z_val;
	}
}