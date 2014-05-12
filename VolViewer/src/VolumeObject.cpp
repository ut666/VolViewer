/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "VolumeObject.h"

VolumeObject::VolumeObject()
{
	maxres=0;
	maxsizeX=0;
	maxsizeY=0;
	maxsizeZ=0;
	file_type = -1;
	file_name = "";
	texture3d = 0;

	x_resize = y_resize = z_resize = 1.0;

	width = height = depth = 
	texwidth = texheight = texdepth = 1;

	for(int i=0; i<256*3; i++)
		histogram[i] = 0;

	is_greyscale = has_red = has_green = has_blue = 0;

	maxres = 1.0;
	xsize = ysize = zsize = 1.0;
	maxsize = 1.0;

#ifdef LIBTIFF_SUPPORT
	is16bit = false;
	voldata=NULL;
	voldatasize=1;
#endif
}
VolumeObject::~VolumeObject()
{
	if(texture3d) delete texture3d;
}
void VolumeObject::resetBoundingBox(void)
{
	boundingboxMin = Vector(0,0,0);
	boundingboxMax = Vector(texwidth, texheight, texdepth);

}
void VolumeObject::compute_fastboundingbox_channel(Vector centre, int channel, int window_size, float sizefactor)
{
	int maxX, minX, maxY, minY, maxZ, minZ;

	maxX = centre.x + sizefactor + window_size;
	maxY = centre.y + sizefactor + window_size;
	maxZ = centre.z + sizefactor + window_size;

	minX = centre.x - sizefactor - window_size;
	minY = centre.y - sizefactor - window_size;
	minZ = centre.z - sizefactor - window_size;

	if(maxX>texwidth-1) maxX=texwidth-1;
	if(maxY>texheight-1) maxY=texheight-1;
	if(maxZ>texdepth-1) maxZ=texdepth-1;
	if(minX<0) minX=0;
	if(minY<0) minY=0;
	if(minZ<0) minZ=0;

	boundingboxSize.x = ((maxX)-(minX-1));
	boundingboxSize.y = ((maxY)-(minY-1));
	boundingboxSize.z = ((maxZ)-(minZ-1));
	boundingboxCentre.x = 0.0; //-(((float)boundingboxSize.x)/2.0);
	boundingboxCentre.y = 0.0; //-(((float)boundingboxSize.y)/2.0);
	boundingboxCentre.z = 0.0; //-(((float)boundingboxSize.z)/2.0);
	boundingboxMin = Vector(minX, minY, minZ);
	boundingboxMax = Vector(maxX, maxY, maxZ);

}
void VolumeObject::compute_boundingbox_channel_bounded(int channel, int window_size)
{
	if(!texture3d) return;

	/*progressbar = new Q3ProgressDialog("Computing Volume Bounding Box...", "Cancel", 2, 0, "progress", TRUE);		
	progressbar->setCaption("Please Wait");
	progressbar->setMinimumDuration(100);*/

	//printf("BOUNDINGBOX:\n");
	int maxX, minX, maxY, minY, maxZ, minZ;
	maxX = maxY = maxZ = -9999999999;
	minX = minY = minZ =  9999999999;
	
	int ziter = 1;//((float)texdepth/32.0);
	int yiter = 1;//((float)texheight/32.0);
	int xiter = 1;//((float)texwidth/32.0);

	//printf("Bounding box steps: %d, %d, %d\n", xiter, yiter, ziter);

	int index;
	for(int z = boundingboxMin.z-window_size; z <boundingboxMax.z+window_size; z++)
	{
		for(int x = boundingboxMin.x-window_size; x <boundingboxMax.x+window_size; x++)
		{
			for(int y = boundingboxMin.y-window_size; y <boundingboxMax.y+window_size; y++)
			{
				//index = ((((z*texheight)+y)*texwidth)+x);
				index = get_index1D(x,y,z);

				if(texture3d[3*index+channel])
				{
					if(x<minX) minX = x;
					if(y<minY) minY = y;
					if(z<minZ) minZ = z;

					if(x>maxX) maxX = x;
					if(y>maxY) maxY = y;
					if(z>maxZ) maxZ = z;
				}
			}
		}
	}

	if(maxX==-9999999999) maxX=texwidth-1;
	if(maxY==-9999999999) maxY=texheight-1;
	if(maxZ==-9999999999) maxZ=texdepth-1;

	if(minX==9999999999) minX=0;
	if(minY==9999999999) minY=0;
	if(minZ==9999999999) minZ=0;

	//printf("max: %d, %d, %d\n", maxX, maxY, maxZ);
	//printf("min: %d, %d, %d\n", minX, minY, minZ);

	maxX += (float)window_size/2.0;
	maxY += (float)window_size/2.0;
	maxZ += (float)window_size/2.0;
	minX -= (float)window_size/2.0;
	minY -= (float)window_size/2.0;
	minZ -= (float)window_size/2.0;

	maxX += xiter;
	maxY += yiter;
	maxZ += ziter;

	minX -= xiter;
	minY -= yiter;
	minZ -= ziter;	
	
	if(maxX>texwidth-1) maxX=texwidth-1;
	if(maxY>texheight-1) maxY=texheight-1;
	if(maxZ>texdepth-1) maxZ=texdepth-1;
	if(minX<0) minX=0;
	if(minY<0) minY=0;
	if(minZ<0) minZ=0;

	/*maxX = texwidth;
	maxY = texheight;
	maxZ = texdepth;
	minX = 0;
	minY = 0;
	minZ = 0;*/

	boundingboxSize.x = ((maxX)-(minX-1));
	boundingboxSize.y = ((maxY)-(minY-1));
	boundingboxSize.z = ((maxZ)-(minZ-1));
	boundingboxCentre.x = 0.0; //-(((float)boundingboxSize.x)/2.0);
	boundingboxCentre.y = 0.0; //-(((float)boundingboxSize.y)/2.0);
	boundingboxCentre.z = 0.0; //-(((float)boundingboxSize.z)/2.0);
	boundingboxMin = Vector(minX, minY, minZ);
	boundingboxMax = Vector(maxX, maxY, maxZ);

	printf("BOUNDINGBOX: %d, %d, %d\n", boundingboxSize.x, boundingboxSize.y, boundingboxSize.z);
	printf("BOUNDINGBOX CENTRE: %d, %d, %d\n", boundingboxCentre.x, boundingboxCentre.y, boundingboxCentre.z);

/*
	progressbar->setProgress(2);
	delete progressbar;*/
}
void VolumeObject::compute_boundingbox_channel(int channel, int window_size)
{
	if(!texture3d) return;

	/*progressbar = new Q3ProgressDialog("Computing Volume Bounding Box...", "Cancel", 2, 0, "progress", TRUE);		
	progressbar->setCaption("Please Wait");
	progressbar->setMinimumDuration(100);*/

	//printf("BOUNDINGBOX:\n");
	int maxX, minX, maxY, minY, maxZ, minZ;
	maxX = maxY = maxZ = -9999999999;
	minX = minY = minZ =  9999999999;
	
	int ziter = 1;//((float)texdepth/32.0);
	int yiter = 1;//((float)texheight/32.0);
	int xiter = 1;//((float)texwidth/32.0);

	//printf("Bounding box steps: %d, %d, %d\n", xiter, yiter, ziter);

	int index;
	for(int z = 0; z <texdepth; z+=ziter)
	{
		//progressbar->setProgress(z);
		//if (progressbar->wasCancelled())
		//	break;

		for(int x = 0; x <texwidth; x+=xiter)
		{
			for(int y = 0; y <texheight; y+=yiter)
			{
				index = ((((z*texheight)+y)*texwidth)+x);
				//index = get_index1D(x,y,z);

				if(texture3d[3*index+channel])
				{
					if(x<minX) minX = x;
					if(y<minY) minY = y;
					if(z<minZ) minZ = z;

					if(x>maxX) maxX = x;
					if(y>maxY) maxY = y;
					if(z>maxZ) maxZ = z;
				}
			}
		}
	}

	if(maxX==-9999999999) maxX=texwidth-1;
	if(maxY==-9999999999) maxY=texheight-1;
	if(maxZ==-9999999999) maxZ=texdepth-1;

	if(minX==9999999999) minX=0;
	if(minY==9999999999) minY=0;
	if(minZ==9999999999) minZ=0;

	//printf("max: %d, %d, %d\n", maxX, maxY, maxZ);
	//printf("min: %d, %d, %d\n", minX, minY, minZ);

	maxX += (float)window_size/2.0;
	maxY += (float)window_size/2.0;
	maxZ += (float)window_size/2.0;
	minX -= (float)window_size/2.0;
	minY -= (float)window_size/2.0;
	minZ -= (float)window_size/2.0;

	maxX += xiter;
	maxY += yiter;
	maxZ += ziter;

	minX -= xiter;
	minY -= yiter;
	minZ -= ziter;	
	
	if(maxX>texwidth-1) maxX=texwidth-1;
	if(maxY>texheight-1) maxY=texheight-1;
	if(maxZ>texdepth-1) maxZ=texdepth-1;
	if(minX<0) minX=0;
	if(minY<0) minY=0;
	if(minZ<0) minZ=0;

	/*maxX = texwidth;
	maxY = texheight;
	maxZ = texdepth;
	minX = 0;
	minY = 0;
	minZ = 0;*/

	boundingboxSize.x = ((maxX)-(minX-1));
	boundingboxSize.y = ((maxY)-(minY-1));
	boundingboxSize.z = ((maxZ)-(minZ-1));
	boundingboxCentre.x = 0.0; //-(((float)boundingboxSize.x)/2.0);
	boundingboxCentre.y = 0.0; //-(((float)boundingboxSize.y)/2.0);
	boundingboxCentre.z = 0.0; //-(((float)boundingboxSize.z)/2.0);
	boundingboxMin = Vector(minX, minY, minZ);
	boundingboxMax = Vector(maxX, maxY, maxZ);
/*
	progressbar->setProgress(2);
	delete progressbar;*/
}
void VolumeObject::create_volume(void)
{
	if(texture3d) delete texture3d;
	texture3d = NULL;
	
	width = texwidth = 256;
	height = texheight = 256;
	depth = texdepth = 256;
	
	int size = 3 * texwidth * texheight * texdepth;

	//lets have some memory for our texture data
	texture3d = new unsigned char[3*size];

	Vector centre = Vector(42,65,200);
	Vector width = Vector(8,8,8);
	float border = 32;

	is_greyscale = false;
	has_red = true;
	has_green = true;
	has_blue = true;

	for(int x=0; x<texwidth; x++)
	{
		for(int y=0; y<texheight; y++)
		{
			for(int z=0; z<texdepth; z++)
			{
				int index1D = get_index1D(x,y,z);

				texture3d[3*index1D+0] = 0;
				texture3d[3*index1D+1] = 0;
				texture3d[3*index1D+2] = 0;

				if(x<border || x>texwidth-border)
					if(y<border || y>texheight-border)
						if(z<border || z>texdepth-border)
						{
							texture3d[3*index1D+0] = 255;
							texture3d[3*index1D+1] = 255;
							texture3d[3*index1D+2] = 255;
						}

				if(x>centre.x-width.x && x<centre.x+width.x)
				{
					if(y>centre.y-width.y && y<centre.y+width.y)
					{
						if(z>centre.z-width.z && z<centre.z+width.z)
						{
							texture3d[3*index1D+0] = 255;
							texture3d[3*index1D+1] = 255;
							texture3d[3*index1D+2] = 255;
						}
					}
				}
			}
		}
	}
}
void VolumeObject::compute_boundingbox(void)
{
	if(!texture3d) return;

	progressbar = new Q3ProgressDialog("Computing Volume Bounding Box...", "Cancel", 2, 0, "progress", TRUE);		
	progressbar->setCaption("Please Wait");
	progressbar->setMinimumDuration(100);

	//printf("BOUNDINGBOX:\n");
	int maxX, minX, maxY, minY, maxZ, minZ;
	
	maxX = maxY = maxZ = -8192;
	minX = minY = minZ = 8192;
	int  r,g,b, index;
	for(int z = 0; z <texdepth; z++)
	{
		//progressbar->setProgress(z);
		//if (progressbar->wasCancelled())
		//	break;

		for(int x = 0; x <texwidth; x++)
		{
			for(int y = 0; y <texheight; y++)
			{
				index = ((((z*texheight)+y)*texwidth)+x);

				//r=g=b=0;
				//if(has_red) 
					r = texture3d[3*index+0];
				//if(has_green) 
					g = texture3d[3*index+1];
				//if(has_blue) 
					b = texture3d[3*index+2];
				
				//printf("has: %d, %d, %d\n",(int)has_red,(int)has_green,(int)has_blue);
				if(r+g+b!=0)
				{
					if(x<minX) minX = x;
					if(y<minY) minY = y;
					if(z<minZ) minZ = z;

					if(x>maxX) maxX = x;
					if(y>maxY) maxY = y;
					if(z>maxZ) maxZ = z;
				}
				/*else
				{
					printf("vox: %d, %d, %d\n", r,g,b);
				}*/
			}
		}
	}
	/*
	maxX += 2;
	maxY += 2;
	maxZ += 2;

	minX -= 2;
	minY -= 2;
	minZ -= 2;
	*/
	if(minX<0) minX=0;
	if(minY<0) minY=0;
	if(minZ<0) minZ=0;
	if(maxX>texwidth-1) maxX=texwidth-1;
	if(maxY>texheight-1) maxY=texheight-1;
	if(maxZ>texdepth-1) maxZ=texdepth-1;

/*	maxX = texwidth;
	maxY = texheight;
	maxZ = texdepth;
	minX = 0;
	minY = 0;
	minZ = 0;
*/
	boundingboxSize.x = ((maxX)-(minX));
	boundingboxSize.y = ((maxY)-(minY));
	boundingboxSize.z = ((maxZ)-(minZ));
	boundingboxCentre.x = -(((float)texwidth-1)/2.0)+(minX+(((float)boundingboxSize.x)/2.0));
	boundingboxCentre.y = -(((float)texheight-1)/2.0)+(minY+(((float)boundingboxSize.y)/2.0));
	boundingboxCentre.z = -(((float)texdepth-1)/2.0)+(minZ+(((float)boundingboxSize.z)/2.0));
	boundingboxMin = Vector(minX, minY, minZ);
	boundingboxMax = Vector(maxX, maxY, maxZ);

	progressbar->setProgress(2);
	delete progressbar;
/*	
	printf("min: %d, %d, %d\n", minX, minY, minZ);
	printf("max: %d, %d, %d\n", maxX, maxY, maxZ);
	printf("boundingboxMin: %f %f %f\n", boundingboxMin.x, boundingboxMin.y, boundingboxMin.z);	
	printf("boundingboxMax: %f %f %f\n", boundingboxMax.x, boundingboxMax.y, boundingboxMax.z);
	printf("boundingboxSize: %f %f %f\n", boundingboxSize.x, boundingboxSize.y, boundingboxSize.z);
	printf("boundingboxCentre: %f %f %f\n", boundingboxCentre.x, boundingboxCentre.y, boundingboxCentre.z);
*/
}
int VolumeObject::get_index1DinBoundingBox(int x, int y, int z)
{
	/*if(zval>volobj->boundingboxMax.z-1) zval = volobj->boundingboxMax.z-1;
	if(yval>volobj->boundingboxMax.y-1) yval = volobj->boundingboxMax.y-1;
	if(xval>volobj->boundingboxMax.x-1) xval = volobj->boundingboxMax.x-1;

	if(zval<volobj->boundingboxMin.z) zval = volobj->boundingboxMin.z;
	if(yval<volobj->boundingboxMin.y) yval = volobj->boundingboxMin.y;
	if(xval<volobj->boundingboxMin.x) xval = volobj->boundingboxMin.x;
							
	index = 3*(((((zval-volobj->boundingboxMin.z)*height)+(yval-volobj->boundingboxMin.y))*width)+(xval-volobj->boundingboxMin.x));
	*/

	if(z>boundingboxMax.z-1) z = boundingboxMax.z-1;
	if(y>boundingboxMax.y-1) y = boundingboxMax.y-1;
	if(x>boundingboxMax.x-1) x = boundingboxMax.x-1;

	if(z<boundingboxMin.z) z = boundingboxMin.z;
	if(y<boundingboxMin.y) y = boundingboxMin.y;
	if(x<boundingboxMin.x) x = boundingboxMin.x;
									
	int index = (((((z-boundingboxMin.z)*(boundingboxMax.y-boundingboxMin.y+1))+(y-boundingboxMin.y))*(boundingboxMax.x-boundingboxMin.x+1))+(x-boundingboxMin.x));
	return index;
}
int VolumeObject::get_index1D(int x, int y, int z)
{
	if(x>texwidth-1) x = texwidth-1;
	if(y>texheight-1) y = texheight-1;
	if(z>texdepth-1) z = texdepth-1;
	if(x<0) x=0;
	if(y<0) y=0;
	if(z<0) z=0;

	int index1d = ((((z*texheight)+(y))*texwidth)+(x));
	return index1d;
}
Vector VolumeObject::get_index3D(int index1d)
{
	double temp = ((double)index1d/(double)texheight);
	temp /= (double)texwidth;
	int z = temp;

	double temp2 = z*texheight*texwidth;
	int y = (index1d-temp2)/texwidth;

	int x = index1d-((z*texheight)+y)*texwidth;

	//printf("x,y,z: %d, %d, %d\n", x,y,z);

	return Vector(x,y,z);
}
void VolumeObject::resize(float factor)
{
	printf("RESIZING\n");

	float x_resize, y_resize, z_resize;
	x_resize = (float) factor; //volobj->width/(float) volobj->maxsizeX;
	y_resize = (float) factor; //volobj->height/(float) volobj->maxsizeY;
	z_resize = (float) factor; //volobj->depth/(float) volobj->maxsizeZ;

	float maxresize = 0;
	if(x_resize>maxresize) maxresize = x_resize;
	if(y_resize>maxresize) maxresize = y_resize;
	if(z_resize>maxresize) maxresize = z_resize;
	x_resize = y_resize = z_resize = maxresize;

	int res_x = ((float)width/x_resize)+0.5; 
	int res_y = ((float)height/y_resize)+0.5;
	int res_z = ((float)depth/z_resize)+0.5; 

	float maxres1 = 0.0;
	if(res_x>maxres1) maxres1 = res_x;
	if(res_y>maxres1) maxres1 = res_y;
	if(res_z>maxres1) maxres1 = res_z;
	maxres = maxres1;

	int offset_x = ((makepower2(maxres1)-res_x)/2.0);
	int offset_y = ((makepower2(maxres1)-res_y)/2.0); 
	int offset_z = ((makepower2(maxres1)-res_z)/2.0); 

	res_x = makepower2(maxres1);
	res_y = makepower2(maxres1);
	res_z = makepower2(maxres1);

	printf("\tOriginal Dimensions: %d, %d, %d\n", width, height, depth);
	printf("\tResized Dimensions: %d, %d, %d\n", res_x, res_y, res_z);
	printf("\tResize Factor: %f, %f, %f\n", x_resize, y_resize, z_resize);
	printf("\tOffsets: %d, %d, %d\n", offset_x, offset_y, offset_z);

	int size = res_x*res_y*res_z*3;

	//lets have some memory
	unsigned char* temp_storage = NULL;
	temp_storage = (unsigned char *)malloc(size*sizeof(unsigned char));
	
	//initialise everything to zero
	for(int ii=0; ii<size; ii++) temp_storage[ii] = 0;

	progressbar = new Q3ProgressDialog("Resizing Volume...", "Cancel", depth, 0, "progress", TRUE);		
	progressbar->setCaption("Please Wait");

	int index_big, index_small;
	float x2, y2, z2;
	x2 = offset_x;
	y2 = offset_y;
	z2 = offset_z;
	for(float z=0; z<depth; z+=z_resize)
	{
        progressbar->setProgress(z);
        if (progressbar->wasCancelled())
            break;

		for(float y=0; y<height; y+=y_resize)
		{
			for(float x=0; x<width; x+=x_resize)
			{
				if(x2>=res_x) x2=res_x-1;
				if(y2>=res_y) y2=res_y-1;
				if(z2>=res_z) z2=res_z-1;

				index_big =   ((( ((int)(z+0.5)*height) + (int)(y+0.5)) * width) + (int)(x+0.5));
				index_small = ((( (z2*res_y) + y2) * res_x) + x2);

				temp_storage[3*index_small+0] = texture3d[3*index_big+0];
				temp_storage[3*index_small+1] = texture3d[3*index_big+1];
				temp_storage[3*index_small+2] = texture3d[3*index_big+2];
				
				y2++;
			}
			x2++;
			y2=0;
		}
		z2++;
		x2=0;
	}

    progressbar->setProgress(depth);
	delete progressbar;
	progressbar=NULL;

	delete[] texture3d;

	//lets have some memory
	texture3d = (unsigned char *)malloc(size*sizeof(unsigned char));

	for(int i=0; i<size; i++) texture3d[i] = temp_storage[i];

	delete[] temp_storage;

	//set up our texture info
	width =  texwidth = res_x;
	height = texheight = res_y;
	depth = texdepth = res_z; 
}
int VolumeObject::makepower2(int val)
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
	else if(val<=96) 
		newval = 96;
	else if(val<=128)
		newval = 128;
	else if(val<=256) 
		newval = 256;
	else if(val<=384) 
		newval = 384;
	else if(val<=512)
		newval = 512;
	else if(val<=768)
		newval = 768;
	else if(val<=1024)
		newval = 1024;

	return newval;
}
const VolumeObject& VolumeObject::operator= (const VolumeObject& right)
{
	maxres=right.maxres;
	maxsizeX=right.maxsizeX;
	maxsizeY=right.maxsizeY;
	maxsizeZ=right.maxsizeZ;
	
	is_greyscale = right.is_greyscale;
	has_red = right.has_red;
	has_green = right.has_green;
	has_blue = right.has_blue;

	xsize = right.xsize;
	ysize = right.ysize;
	zsize = right.zsize;

	file_type = right.file_type;
	file_name = right.file_name;

	width = right.width;
	height = right.height;
	depth = right.depth;

	texwidth = right.texwidth;
	texheight = right.texheight;
	texdepth = right.texdepth;

	//nb 4 channels taken as default
	int size = texwidth*texheight*texdepth*3;

	//delete old, and allocate new memory
	if(texture3d) delete[] texture3d;
	texture3d = new unsigned char[size];

	for(int i=0; i<size; i++)
		texture3d[i] = right.texture3d[i];

	for(int ii=0; ii<256*3; ii++)
		histogram[ii] = right.histogram[ii];

	return *this;
}
Vector VolumeObject::worldspacetoimagespace(Vector v)
{
	Vector imgspace;

	v.x /= (xsize/maxsize);
	v.y /= (ysize/maxsize);
	v.z /= (zsize/maxsize);
	imgspace.x = v.x + (float)texwidth/2.0;
	imgspace.y = v.y + (float)texheight/2.0;
	imgspace.z = v.z + (float)texdepth/2.0;
	
	return imgspace;
}
Vector VolumeObject::imagespacetoworldspace(Vector v)
{
	Vector worldspace;

	worldspace.x = v.x - (float)(texwidth)/2.0;
	worldspace.y = v.y - (float)(texheight)/2.0;
	worldspace.z = v.z - (float)(texdepth)/2.0;
	worldspace.x *= (xsize/maxsize);
	worldspace.y *= (ysize/maxsize);
	worldspace.z *= (zsize/maxsize);

	return worldspace;
}
int VolumeObject::get_value(int x, int y, int z, int c)
{
	int index = get_index1D(x,y,z);
	int value = texture3d[3*index+c];
	return value;
}
int VolumeObject::get_value(int index1d, int c)
{
	int value = texture3d[3*index1d+c];
	return value;
}
void VolumeObject::set_value(int x, int y, int z, int c, int value)
{
	int index = get_index1D(x,y,z);
	texture3d[3*index+c] = value;
}
void VolumeObject::set_value(int index1d, int c, int value)
{
	texture3d[3*index1d+c] = value;
}
