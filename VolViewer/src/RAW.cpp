/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
 #include "RAW.h"

RAW_volume::RAW_volume()
{
	channels = 0;
	WLZ_scale =1;
}

RAW_volume::~RAW_volume()
{
}

void RAW_volume::load_DAT(const std::string &filename)
{
	FILE* infile = fopen(filename.data(), "rt");

	if (infile==NULL)
	{
		printf("ERROR: File does not exist!\n");
		printf("FILENAME: %s\n", filename.data());
		system("pause");
		exit(1);
		//ERROR OPENING FILE
	}

	char buf[81];
	bool done = false;
	
	while (!done)
	{
		fgets(buf, 80, infile);
		if (feof(infile))
		{
			done = true;
			fclose(infile);
		}
		else
		{
		  switch (buf[0])
		  {	      	
			case 'R':
				{
   				  channels=1; //default to only intensity space...
				  if (sscanf(buf, "Resolution: %i %i %i", &res_x, &res_y, &res_z) != 3)
				  {
					break;
				  }
				}
				break;
			case 'C':
				{
				  if (sscanf(buf, "Channels: %i", &channels) != 1)
				  {
					break;
				  }
				}
				break;

			case 'S':
				{
				  if (sscanf(buf, "Scale: %f", &WLZ_scale) != 1)
				  {
					break;
				  }
				}
				break;


			case 'O:':
				{
				  /*if (sscanf(buf, "ObjectModel:: %i %i %i", &res_x, &res_y, &res_z) != 3)
				  {
					break;
				  }*/
				}
				break;
			  default:
				{
					// Ignore lines that we don't recognize.
				}
				break;
		  }
		}
	}
}
void RAW_volume::load_RAW(int z, FILE *f, unsigned char* tex_buffer, int mode, int texy, int texx, double* hist, bool* red, bool* green, bool* blue, bool* greyscale)
{
	//for now only support greyscale and 4 Bytes per pixel
	unsigned char r, g, b, a;
	float rf;
	int index1D;

	int x_offset = (texx-res_x)/2.0;
	int y_offset = (texy-res_y)/2.0;
//	*red=*green=*blue=false;
	
	r =0;
	b =0;
	g =0;
	a =0;

	int checker = 0;
	int helper = 0;

	for (int y=y_offset; y<res_y+y_offset; y++)
	{
		for (int x=x_offset; x<res_x+x_offset; x++)				
		{			
			index1D = 3*((((z*texy)+y)*texx)+x);

			//read binary value
			//& store our voxel value

//			printf("mode: %d\n", (int) mode);

			if(channels==1)
			{
					fread(&a, sizeof(unsigned char), 1, f);
					r =a;
					g =a;
					b =a;
			}
			else if(channels==2)
			{
					fread(&r, sizeof(unsigned char), 1, f);
					fread(&g, sizeof(unsigned char), 1, f);
					b =0;
			}
			else if(channels==3)
			{
					fread(&r, sizeof(unsigned char), 1, f);
					fread(&g, sizeof(unsigned char), 1, f);
					fread(&b, sizeof(unsigned char), 1, f);
			}
			else if(channels==4)
			{
					fread(&r, sizeof(unsigned char), 1, f);
					fread(&g, sizeof(unsigned char), 1, f);
					fread(&b, sizeof(unsigned char), 1, f);
					fread(&a,  sizeof(unsigned char), 1, f);
			}
/*			else if(channels==6)
			{
					fread(&rf, sizeof(float), 1, f);
					rf *= 255;
					r = rf;
					g = rf;
					b = rf;
					a = rf; 
			}
*/
			if(r!=0)	*red = true;
			if(g!=0)	*green = true;
			if(b!=0)	*blue = true;

			if(r==g && r==b && g==b)
			{
				checker++;
			}

			helper++;

			if(mode==1)
			{
				tex_buffer[index1D+0] = GLubyte(r);
				tex_buffer[index1D+1] = 0;
				tex_buffer[index1D+2] = 0;
				//tex_buffer[index1D+3] = GLubyte(a);

				*green=*blue=false;
				
				hist[(3*(int)r)+0]++;
			}
			if(mode==2)
			{
				tex_buffer[index1D+1] = GLubyte(g); 
				//tex_buffer[index1D+3] = GLubyte(max(tex_buffer[index1D+3], a));

				hist[(3*(int)g)+1]++;

			}
			if(mode==3)
			{
				tex_buffer[index1D+2] = GLubyte(b); 
				//tex_buffer[index1D+3] = GLubyte(max(tex_buffer[index1D+3], a));
			
				hist[(3*(int)b)+2]++;
			}	
			if(mode==4)
			{
				tex_buffer[index1D+0] = GLubyte(r);
				tex_buffer[index1D+1] = GLubyte(g);
				tex_buffer[index1D+2] = GLubyte(b);
				//tex_buffer[index1D+3] = GLubyte(a);

				hist[(3*(int)r)+0]++;
				hist[(3*(int)g)+1]++;
				hist[(3*(int)b)+2]++;
			}
		}
	}

	if(checker==helper) *greyscale=true;
	else *greyscale=false;
}

void RAW_volume::save_DAT(const std::string &filename, int x, int y, int z, float scale)
{
	ofstream fout;										// Here we create an ofstream and we will call it "fout", like "cout".
	fout.open(filename.data());								// We call a function from our ofstream object called open().

	WLZ_scale = scale;

	if (fout==NULL)
	{
		//ERROR OPENING FILE
	}
	
	fout<<"Resolution: "<<x<<" "<<y<<" "<<z<<endl;
	fout<<"Channels: 4"<<endl;
	fout<<"Format: UCHAR"<<endl;
	fout<<"Scale: "<<WLZ_scale<<endl<<endl;
	
	fout.close();										// We must always close the file after we are done with it.  It frees memory and make's it so we can access it again.
}

void RAW_volume::save_RAW(FILE *f, unsigned char* vol_buffer, int resx, int resy, int z)
{
	//for now only support greyscale and 4 Bytes per pixel
	unsigned char voxel;
	int index1D;

	for (int y =0; y<resy; y++)
	{
		for (int x =0; x<resx; x++)				
		{
			index1D = 3*((((z*resy)+y)*resx)+x);
			
			//read from memory and write to disk
			voxel = (unsigned char)(vol_buffer[index1D+0]);
			fwrite(&voxel, sizeof(unsigned char), 1, f);
			voxel = (unsigned char)(vol_buffer[index1D+1]);
			fwrite(&voxel, sizeof(unsigned char), 1, f);
			voxel = (unsigned char)(vol_buffer[index1D+2]);
			fwrite(&voxel, sizeof(unsigned char), 1, f);
			voxel = (unsigned char)(vol_buffer[index1D+3]);
			fwrite(&voxel, sizeof(unsigned char), 1, f);
		}
	}
}

