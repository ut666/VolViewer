/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "Floodfill.h"

Floodfill::Floodfill()
{
	volobj = NULL;
}
Floodfill::~Floodfill()
{
	
}
void Floodfill::progress(int i, int size)
{
	float prog = ( (float)(i+1)/(float)(size) )*100.0f;
	printf("\r\r Progress = %.2f", prog);
}
void Floodfill::init(int w, int h, int d, int visitedmode)
{
	width = w;
	height = h;
	depth = d;
	
	maxrecursiondepth = 4;

	if(visitedmode==VISITEDARRAY)
	{
		visited_voxels = new bool**[w];
		for(int i=0; i<w; i++)
		{
			visited_voxels[i] = new bool*[h];
			for(int j=0; j<h; j++)
			{
				visited_voxels[i][j] = new bool[d];
			}
		}
		
		init_visited();
	}

	float size = 1.0;
	float resX = volobj->texwidth;
	float resY = volobj->texheight;
	float resZ = volobj->texdepth;
	float maxres = 0.0;
	
	maxres = max((float)maxres, (float)resX);
	maxres = max((float)maxres, (float)resY);
	maxres = max((float)maxres, (float)resZ);
	
	float maxressize =  max((float)volobj->xsize, (float)volobj->ysize);
	maxressize =  max((float)maxressize, (float)volobj->zsize);
	
	float maxsize =  max((float)volobj->xsize, (float)volobj->ysize);
	maxsize =  max((float)maxsize, (float)volobj->zsize);
	
	scaleX = (volobj->xsize/maxsize);//(maxsize/resX);
	scaleY = (volobj->ysize/maxsize);//(maxsize/resY);
	scaleZ = (volobj->zsize/maxsize);//(maxsize/resZ);
}
void Floodfill::delete_visited(void)
{
	printf("Deleting visited...\n");
	for(int i=0; i<width; i++)
	{
		for(int j=0; j<height; j++)
		{
			delete visited_voxels[i][j];
		}
	}
	for(int i=0; i<width; i++)
	{
		delete visited_voxels[i];
	}

	delete visited_voxels;
	visited_voxels = NULL;
	printf("done...\n");
}
void Floodfill::init_visited(void)
{
	for(int i=0; i<width; i++)
	{
		for(int j=0; j<height; j++)
		{
			for(int k=0; k<depth; k++)
			{
				visited_voxels[i][j][k] = false;
			}
		}
	}
}
Point3D Floodfill::get3Dindex(int index)
{
	Point3D p;
	int tempindex;
	
	//for each x
	for (int x =0; x < width; x++)
	{
		//for each y
		for (int y =0; y < height; y++)
		{
			for (int z = 0; z < depth; z++)
			{
				p = Point3D(x,y,z);
				tempindex = get1Dindex(p);
				
				if(tempindex==index) return p;
			}
		}
	}

	p.x = p.y = p.z = -1;
	return p;
}
int Floodfill::get1Dindex(Point3D p)
{
 	int index1d = (((((p.z)*height)+(p.y))*width)+(p.x));
	return index1d;
}
int Floodfill::getValue(Point3D p, bool red, bool green, bool blue)
{
	int index = get1Dindex(p);
	
	float value;
	float helper =0;
	if(red)
	{
		value = volobj->texture3d[3*index+0];
		helper++;
	}
	if(green)
	{
		value = volobj->texture3d[3*index+1];
		helper++;
	}
	if(blue)
	{
		value = volobj->texture3d[3*index+2];
		helper++;
	}
	
	value /= helper;
	return (int)(value);
}
bool Floodfill::apply_rule(int x0, int y0, int z0, int x1, int y1, int z1, int rule)
{
	Point3D p0 = Point3D(x0,y0,z0);
	Point3D p1 = Point3D(x1,y1,z1);
	
	int index0 = get1Dindex(p0);
	
	Vector rgb0, rgb1;
	Vector yellow = Vector(255,255,0);
	
	if(rule==COLOUR_RULE)
	{
		rgb0.x = getValue(p0, true, false, false);
		//rgb0.y = getValue(p0, false, true, false);
		//rgb0.z = getValue(p0, false, false, true);
		rgb1.x = getValue(p1, true, false, false);
		//rgb1.y = getValue(p1, false, true, false);
		//rgb1.z = getValue(p1, false, false, true);

		if(rgb0.x==rgb1.x) return true;
		else return false;
	}
	else if(rule==RCOLOURWINDOW_RULE)
	{
		rgb0.x = getValue(p0, true, false, false);
		rgb1.x = getValue(p1, true, false, false);

		if( rgb1.x<(rgb0.x+WINDOW_SIZE) && rgb1.x>(rgb0.x-WINDOW_SIZE) ) return true;
		else return false;
	}
	else if(rule==GCOLOURWINDOW_RULE)
	{
		rgb0.y = getValue(p0, false, true, false);
		rgb1.y = getValue(p1, false, true, false);

		if( rgb1.y<(rgb0.y+WINDOW_SIZE) && rgb1.y>(rgb0.y-WINDOW_SIZE) )return true;
		else return false;
	}
	else if(rule==BCOLOURWINDOW_RULE)
	{
		rgb0.z = getValue(p0, false, false, true);
		rgb1.z = getValue(p1, false, false, true);

		if( rgb1.z<(rgb0.z+WINDOW_SIZE) && rgb1.z>(rgb0.z-WINDOW_SIZE) ) return true;
		else return false;
	}
	else if(rule==RCOLOUR_RULE)
	{
		rgb0.x = getValue(p0, true, false, false);
		rgb1.x = getValue(p1, true, false, false);

		if(rgb0.x==rgb1.x) return true;
		else return false;
	}
	else if(rule==GCOLOUR_RULE)
	{
		rgb0.y = getValue(p0, false, true, false);
		rgb1.y = getValue(p1, false, true, false);

		if(rgb0.y==rgb1.y) return true;
		else return false;
	}
	else if(rule==BCOLOUR_RULE)
	{
		rgb0.z = getValue(p0, false, false, true);
		rgb1.z = getValue(p1, false, false, true);

		if(rgb0.z==rgb1.z) return true;
		else return false;
	}
	else if(rule==FILL_RULE)
	{
		rgb0.x = getValue(p0, true, false, false);
		rgb0.y = getValue(p0, false, true, false);
		rgb0.z = getValue(p0, false, false, true);
		rgb1.x = getValue(p1, true, false, false);
		rgb1.y = getValue(p1, false, true, false);
		rgb1.z = getValue(p1, false, false, true);

		if(rgb0!=rgb1)
		{
			int region0=-1;
			int region1=-1;
		
			for(int i=0; i<region_colours_tokeep.size(); i++)
			{
				if(rgb0==region_colours_tokeep[i])
				{
					//printf("found region0\n");
					region0 = i;
				}
				if(rgb1==region_colours_tokeep[i])
				{
					//printf("found region1\n");
					region1 = i;
				}
			}
				
            /*if(region0!=-1 && region1!=-1 && region0!=region1 && rgb0 != 0 && rgb1 != 0) //  && rgb0 != 0 && rgb1 != 0  && rgb0 != yellow && rgb1 != yellow
			{
				//printf("FOUND EDGE\n");
				edges[0][region0][region1] = true;
				edges[0][region1][region0] = true;
		
                volobj->texture3d[3*index0+0] = 255;
				volobj->texture3d[3*index0+1] = 255; 
                volobj->texture3d[3*index0+2] = 0;
			}*/
			
			return true;
		}
		else return false;
	}
	
	return false;
}

bool Floodfill::isVisited(int x, int y, int z, int mode)
{
	if(mode==VISITEDARRAY)
	{
		return visited_voxels[x][y][z];
	}
	else if(mode==VISITEDDIRECT)
	{
		Point3D p = Point3D(x,y,z);
		int index = get1Dindex(p);

		if(volobj->texture3d[(3*index)+DST_CHANNEL]==255) return true;
		else return false;
	}

	return false;
}
void Floodfill::markVisited(int x, int y, int z, int mode)
{
	if(mode==VISITEDARRAY)
	{
		visited_voxels[x][y][z]=true;
	}
	else if(mode==VISITEDDIRECT)
	{
		Point3D p = Point3D(x,y,z);
		int index = get1Dindex(p);

		volobj->texture3d[(3*index)+DST_CHANNEL]=255;
	}
}
void Floodfill::markUnVisited(int x, int y, int z, int mode)
{
	if(mode==VISITEDARRAY)
	{
		visited_voxels[x][y][z]=false;
	}
	else if(mode==VISITEDDIRECT)
	{
		Point3D p = Point3D(x,y,z);
		int index = get1Dindex(p);

		volobj->texture3d[3*index+DST_CHANNEL]=0;
	}
}

//search (6) or (26) connected neighborhood for similiar voxel as starting point
void Floodfill::search_neighborhood(int x, int y, int z, vector<Point3D>* stack, int rule, int connectivity, int visitmode)
{
	//resolution of steps
	int steps = 1;

	//setup our searh indices
	int indexNorth = y + steps;
	int indexSouth = y - steps;
	int indexEast  = x + steps;
	int indexWest  = x - steps;
	int indexUp	   = z + steps;
	int indexDown  = z - steps;

	//helper
	Point3D temp;

	// look around for connected voxels marked as data (ie: equal to -1)
	
	// 6 CONNECTED
	//-----------------

	//EAST
	if(indexEast < width ) //if we aint out of bounds
	{
		//if the voxel has not been visited
		if(isVisited(indexEast, y, z, visitmode)==false)
		{
			if(apply_rule(x,y,z, indexEast,y,z, rule))
			{
				//then voxel is in the same region
				temp.x = indexEast; temp.y = y; temp.z = z;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				markVisited(indexEast, y, z, visitmode);
			}
		}
	}

	//WEST
	if(indexWest >= 0 ) //if we aint out of bounds
	{
		//if the voxel has not been visited
		if(isVisited(indexWest, y, z, visitmode)==false)
		{
			if( apply_rule(x,y,z, indexWest,y,z, rule))
			{
				//then voxel is in the same region
				temp.x = indexWest; temp.y = y; temp.z = z;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				markVisited(indexWest, y, z, visitmode);
			}
		}
	}

	//NORTH
	if(indexNorth < height ) //if we aint out of bounds
	{
		//if the voxel has not been visited
		if(isVisited(x, indexNorth, z, visitmode)==false)
		{
			if(apply_rule(x,y,z, x,indexNorth,z, rule))
			{
				//then voxel is in the same region
				temp.x = x; temp.y = indexNorth; temp.z = z;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				markVisited(x, indexNorth, z, visitmode);
			}
		}
	}

	//SOUTH
	if(indexSouth >= 0) //if we aint out of bounds
	{
		//if the voxel has not been visited
		if(isVisited(x, indexSouth, z, visitmode)==false)
		{
			if(apply_rule(x,y,z, x,indexSouth,z, rule))
			{
				//then voxel is in the same region
				temp.x = x; temp.y = indexSouth; temp.z = z;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				markVisited(x, indexSouth, z, visitmode);
			}
		}
	}

	//UP
	if(indexUp < depth ) //if we aint out of bounds
	{
		//if the voxel has not been visited
		if(isVisited(x, y, indexUp, visitmode)==false)
		{
			if(apply_rule(x,y,z, x,y,indexUp, rule))
			{
				//then voxel is in the same region
				temp.x = x; temp.y = y; temp.z = indexUp;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				markVisited(x, y, indexUp, visitmode);
			}
		}
	}

	//DOWN
	if(indexDown >= 0) //if we aint out of bounds
	{
		//if the voxel has not been visited
		if(isVisited(x, y, indexDown, visitmode)==false)
		{
			if(apply_rule(x,y,z, x,y,indexDown, rule))
			{
				//then voxel is in the same region
				temp.x = x; temp.y = y; temp.z = indexDown;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				markVisited(x, y, indexDown, visitmode);
			}
		}
	}
	/*
	if(connectivity==CONNECT26) //CONNECT26
	{
		// 26 CONNECTED (20 additional cases)
		//----------------------------------

		//NORTH WEST
		if(indexNorth < height && indexWest >= 0) //if we aint out of bounds
		{
			//if the voxel has not been visited
			if(visited_voxels[indexWest][indexNorth][z]==false)
			if(apply_rule(x,y,z, indexWest,indexNorth,z, rule))
			{
				//then voxel is in the same region
				temp.x = indexWest;
				temp.y = indexNorth;
				temp.z = z;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				visited_voxels[indexWest][indexNorth][z] = true;
			}
		}

		//SOUTH WEST
		if(indexSouth >= 0 && indexWest >= 0) //if we aint out of bounds
		{
			//if the voxel has not been visited
			if(visited_voxels[indexWest][indexSouth][z]==false)
			if(apply_rule(x,y,z, indexWest,indexSouth,z, rule))
			{
				//then voxel is in the same region
				temp.x = indexWest;
				temp.y = indexSouth;
				temp.z = z;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				visited_voxels[indexWest][indexSouth][z] = true;
			}
		}
		
		//NORTH EAST
		if(indexNorth < height && indexEast < width) //if we aint out of bounds
		{
			//if the voxel has not been visited
			if(visited_voxels[indexEast][indexNorth][z]==false)
			if(apply_rule(x,y,z, indexEast,indexNorth,z, rule))
			{
				//then voxel is in the same region
				temp.x = indexEast;
				temp.y = indexNorth;
				temp.z = z;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				visited_voxels[indexEast][indexNorth][z] = true;
			}
		}

		//SOUTH EAST
		if(indexSouth >= 0 && indexEast < width) //if we aint out of bounds
		{
			//if the voxel has not been visited
			if(visited_voxels[indexEast][indexSouth][z]==false)
			if(apply_rule(x,y,z, indexEast,indexSouth,z, rule))
			{
				//then voxel is in the same region
				temp.x = indexEast;
				temp.y = indexSouth;
				temp.z = z;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				visited_voxels[indexEast][indexSouth][z] = true;
			}
		}
		
		//-------------------------------------------------------------------------

		//UP  NORTH
		if(indexUp < depth && indexNorth < height) //if we aint out of bounds
		{
			//if the voxel has not been visited
			if(visited_voxels[x][indexNorth][indexUp]==false)
			if(apply_rule(x,y,z, x,indexNorth,indexUp, rule))
			{
				//then voxel is in the same region
				temp.x = x;
				temp.y = indexNorth;
				temp.z = indexUp;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				visited_voxels[x][indexNorth][indexUp] = true;
			}
		}
		//UP SOUTH
		if(indexUp < depth && indexSouth >= 0) //if we aint out of bounds
		{
			//if the voxel has not been visited
			if(visited_voxels[x][indexSouth][indexUp]==false)
			if(apply_rule(x,y,z, x,indexSouth,indexUp, rule))
			{
				//then voxel is in the same region
				temp.x = x;
				temp.y = indexSouth;
				temp.z = indexUp;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				visited_voxels[x][indexSouth][indexUp] = true;
			}
		}
		//UP EAST
		if(indexUp < depth && indexEast < width) //if we aint out of bounds
		{
			//if the voxel has not been visited
			if(visited_voxels[indexEast][y][indexUp]==false)
			if(apply_rule(x,y,z, indexEast,y,indexUp, rule))
			{
				//then voxel is in the same region
				temp.x = indexEast;
				temp.y = y;
				temp.z = indexUp;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				visited_voxels[indexEast][y][indexUp] = true;
			}
		}
		//UP WEST
		if(indexUp < depth && indexWest >= 0) //if we aint out of bounds
		{
			//if the voxel has not been visited
			if(visited_voxels[indexWest][y][indexUp]==false)
			if(apply_rule(x,y,z, indexWest,y,indexUp, rule))
			{
				//then voxel is in the same region
				temp.x = indexWest;
				temp.y = y;
				temp.z = indexUp;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				visited_voxels[indexWest][y][indexUp] = true;
			}
		}
		//UP SOUTH WEST
		if(indexUp < depth && indexSouth >= 0 && indexWest >= 0) //if we aint out of bounds
		{
			//if the voxel has not been visited
			if(visited_voxels[indexWest][indexSouth][indexUp]==false)
			if(apply_rule(x,y,z, indexWest,indexSouth,indexUp, rule))
			{
				//then voxel is in the same region
				temp.x = indexWest;
				temp.y = indexSouth;
				temp.z = indexUp;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				visited_voxels[indexWest][indexSouth][indexUp] = true;
			}
		}
		//UP SOUTH EAST
		if(indexUp < depth && indexSouth >= 0 && indexEast < width) //if we aint out of bounds
		{
			//if the voxel has not been visited
			if(visited_voxels[indexEast][indexSouth][indexUp]==false)
			if(apply_rule(x,y,z, indexEast,indexSouth,indexUp, rule))
			{
				//then voxel is in the same region
				temp.x = indexEast;
				temp.y = indexSouth;
				temp.z = indexUp;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				visited_voxels[indexEast][indexSouth][indexUp] = true;
			}
		}
		//UP NORTH EAST
		if(indexUp < depth && indexNorth < height && indexEast < width) //if we aint out of bounds
		{
			//if the voxel has not been visited
			if(visited_voxels[indexEast][indexNorth][indexUp]==false)
			if(apply_rule(x,y,z, indexEast,indexNorth,indexUp, rule))
			{
				//then voxel is in the same region
				temp.x = indexEast;
				temp.y = indexNorth;
				temp.z = indexUp;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				visited_voxels[indexEast][indexNorth][indexUp] = true;
			}
		}
		//UP NORTH WEST
		if(indexUp < depth && indexNorth < height && indexWest >= 0) //if we aint out of bounds
		{
			//if the voxel has not been visited
			if(visited_voxels[indexWest][indexNorth][indexUp]==false)
			if(apply_rule(x,y,z, indexWest,indexNorth,indexUp, rule))
			{
				//then voxel is in the same region
				temp.x = indexWest;
				temp.y = indexNorth;
				temp.z = indexUp;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				visited_voxels[indexWest][indexNorth][indexUp] = true;
			}
		}
		//-------------------------------------------------------------------------

		//DOWN NORTH
		if(indexDown >= 0 && indexNorth < height) //if we aint out of bounds
		{
			//if the voxel has not been visited
			if(visited_voxels[x][indexNorth][indexDown]==false)
			if(apply_rule(x,y,z, x,indexNorth,indexDown, rule))
			{
				//then voxel is in the same region
				temp.x = x;
				temp.y = indexNorth;
				temp.z = indexDown;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				visited_voxels[x][indexNorth][indexDown] = true;
			}
		}
		//DOWN SOUTH
		if(indexDown >= 0 && indexSouth >= 0) //if we aint out of bounds
		{
			//if the voxel has not been visited
			if(visited_voxels[x][indexSouth][indexDown]==false)
			if(apply_rule(x,y,z, x,indexSouth,indexDown, rule))
			{
				//then voxel is in the same region
				temp.x = x;
				temp.y = indexSouth;
				temp.z = indexDown;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				visited_voxels[x][indexSouth][indexDown] = true;
			}
		}
		//DOWN WEST
		if(indexDown >= 0 && indexWest >=0) //if we aint out of bounds
		{
			//if the voxel has not been visited
			if(visited_voxels[indexWest][y][indexDown]==false)
			if(apply_rule(x,y,z, indexWest,y,indexDown, rule))
			{
				//then voxel is in the same region
				temp.x = indexWest;
				temp.y = y;
				temp.z = indexDown;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				visited_voxels[indexWest][y][indexDown] = true;
			}
		}
		//DOWN EAST
		if(indexDown >= 0 && indexEast < width) //if we aint out of bounds
		{
			//if the voxel has not been visited
			if(visited_voxels[indexEast][y][indexDown]==false)
			if(apply_rule(x,y,z, indexEast,y,indexDown, rule))
			{
				//then voxel is in the same region
				temp.x = indexEast;
				temp.y = y;
				temp.z = indexDown;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				visited_voxels[indexEast][y][indexDown] = true;
			}
		}
		//DOWN SOUTH WEST
		if(indexDown >= 0 && indexSouth >= 0 && indexWest >=0) //if we aint out of bounds
		{
			//if the voxel has not been visited
			if(visited_voxels[indexWest][indexSouth][indexDown]==false)
			if(apply_rule(x,y,z, indexWest,indexSouth,indexDown, rule))
			{
				//then voxel is in the same region
				temp.x = indexWest;
				temp.y = indexSouth;
				temp.z = indexDown;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				visited_voxels[indexWest][indexSouth][indexDown] = true;
			}
		}
		//DOWN SOUTH EAST
		if(indexDown >= 0 && indexSouth >= 0 && indexEast < width) //if we aint out of bounds
		{
			//if the voxel has not been visited
			if(visited_voxels[indexEast][indexSouth][indexDown]==false)
			if(apply_rule(x,y,z, indexEast,indexSouth,indexDown, rule))
			{
				//then voxel is in the same region
				temp.x = indexEast;
				temp.y = indexSouth;
				temp.z = indexDown;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				visited_voxels[indexEast][indexSouth][indexDown] = true;
			}
		}
		//DOWN NORTH WEST
		if(indexDown >= 0 && indexNorth < height && indexWest >=0) //if we aint out of bounds
		{
			//if the voxel has not been visited
			if(visited_voxels[indexWest][indexNorth][indexDown]==false)
			if(apply_rule(x,y,z, indexWest,indexNorth,indexDown, rule))
			{
				//then voxel is in the same region
				temp.x = indexWest;
				temp.y = indexNorth;
				temp.z = indexDown;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				visited_voxels[indexWest][indexNorth][indexDown] = true;
			}
		}
		//DOWN NORTH EAST
		if(indexDown >= 0 && indexNorth < height && indexEast < width) //if we aint out of bounds
		{
			//if the voxel has not been visited
			if(visited_voxels[indexEast][indexNorth][indexDown]==false)
			if(apply_rule(x,y,z, indexEast,indexNorth,indexDown, rule))
			{
				//then voxel is in the same region
				temp.x = indexEast;
				temp.y = indexNorth;
				temp.z = indexDown;

				//we can add it to the stack
				stack->push_back(temp);

				//and mark the voxel with region info
				visited_voxels[indexEast][indexNorth][indexDown] = true;
			}
		}
	}*/
}
void Floodfill::applyfloodfillseededsmoothed(Point3D seedpoint, int src, int dst)
{
	if(volobj==NULL) return;

	Filter filter;
	filter.volobj = volobj;

	WINDOW_SIZE = 2;
	int GAUSSIAN_SIZE = 1;
	float GAUSSIAN_SIGMA = 2.0;
	int THRESHOLD_SIZE = 100;

	SRC_CHANNEL = src;
	DST_CHANNEL = dst;

	int visitedmode = VISITEDDIRECT;
	int rule = -1;
	if(SRC_CHANNEL==0) rule = RCOLOURWINDOW_RULE;
	else if(SRC_CHANNEL==1) rule = GCOLOURWINDOW_RULE;
	else if(SRC_CHANNEL==2) rule = BCOLOURWINDOW_RULE;

	//printf("channels src: %d, dst: %d\n", SRC_CHANNEL, DST_CHANNEL);

	init(volobj->texwidth, volobj->texheight, volobj->texdepth, visitedmode);

	//empty destination channels
	int size = volobj->texwidth*volobj->texheight*volobj->texdepth;
	for(int i=0; i<size; i++)
	{
		if(SRC_CHANNEL==0)
		{
			volobj->texture3d[3*i+1] = 0;
			volobj->texture3d[3*i+2] = 0;
		}
		if(SRC_CHANNEL==1)
		{
			volobj->texture3d[3*i+0] = 0;
			volobj->texture3d[3*i+1] = 0;
		}
		if(SRC_CHANNEL==1)
		{
			volobj->texture3d[3*i+0] = 0;
			volobj->texture3d[3*i+2] = 0;
		}
	}

	//holds voxel indices of the current region of interest
	vector<Point3D> stack;
	vector<int> curr_region;

	int tempi;
	Point3D temp;
	int x, y, z;
	int recursiondepth=0;
	int index, helper, oldhelper;
	bool stopsearch=false;
	//==================
	//FLOODFILL
	//==================

	//push to stack
	stack.push_back(seedpoint);		

	//while stack not empty
	while(stack.size())
	{		
		//save item and pop off stack
		temp = stack[stack.size()-1];
		tempi = get1Dindex(temp);
		stack.pop_back();
	
		//mark the voxel
		markVisited(temp.x, temp.y, temp.z, visitedmode);
		//curr_region.push_back(tempi);	
	
		//search neighborhood for the next pixel
		if(!stopsearch) search_neighborhood((int)temp.x, (int)temp.y, (int)temp.z, &stack, rule, CONNECT6, visitedmode);							
		
		vector< bool > channels;
		channels.resize(3);
		channels[0]=false;
		channels[1]=true;
		channels[2]=false;

//		printf("helper: %d\n", helper);
//		return;

		recursiondepth++;
		if(recursiondepth>10) stopsearch=true;
	}

	return;
/*
		filter.apply_gaussian_filter_decomp(1, 2.0, 0, channels);
		size = volobj->texwidth*volobj->texheight*volobj->texdepth;
		helper=0;
		for(int i=0; i<size; i++)
		{
			if(volobj->texture3d[3*i+DST_CHANNEL]>(255-WINDOW_SIZE))
			{
				volobj->texture3d[3*i+DST_CHANNEL] = 255;
				helper++;
			}
			else
			{
				volobj->texture3d[3*i+DST_CHANNEL]=0;
			}
		}
*/
	filter.volobj = 0;
}
void Floodfill::applyfloodfillseeded(Point3D seedpoint, int src, int dst, int window)
{
	if(volobj==NULL) return;

	SRC_CHANNEL = src;
	DST_CHANNEL = dst;
	WINDOW_SIZE = window;
	int visitedmode = VISITEDDIRECT;
	int rule = -1;
	if(SRC_CHANNEL==0) rule = RCOLOURWINDOW_RULE;
	else if(SRC_CHANNEL==1) rule = GCOLOURWINDOW_RULE;
	else if(SRC_CHANNEL==2) rule = BCOLOURWINDOW_RULE;

	//printf("channels src: %d, dst: %d\n", SRC_CHANNEL, DST_CHANNEL);

	init(volobj->texwidth, volobj->texheight, volobj->texdepth, visitedmode);

	//empty destination channels
	int size = volobj->texwidth*volobj->texheight*volobj->texdepth;
	for(int i=0; i<size; i++)
	{
		if(SRC_CHANNEL==0)
		{
			volobj->texture3d[3*i+1] = 0;
			volobj->texture3d[3*i+2] = 0;
		}
		if(SRC_CHANNEL==1)
		{
			volobj->texture3d[3*i+0] = 0;
			volobj->texture3d[3*i+1] = 0;
		}
		if(SRC_CHANNEL==1)
		{
			volobj->texture3d[3*i+0] = 0;
			volobj->texture3d[3*i+2] = 0;
		}
	}

	//holds voxel indices of the current region of interest
	vector<Point3D> stack;
	vector<int> curr_region;

	int tempi;
	Point3D temp;
	int x, y, z;
	int recursiondepth=0;
	int index;


	//==================
	//FLOODFILL
	//==================

	//push to stack
	stack.push_back(seedpoint);		

	//while stack not empty
	while(stack.size())
	{		
		//save item and pop off stack
		temp = stack[stack.size()-1];
		tempi = get1Dindex(temp);
		stack.pop_back();
	
		//mark the voxel
		markVisited(temp.x, temp.y, temp.z, visitedmode);
		//curr_region.push_back(tempi);	
	
		//search neighborhood for the next pixel
		search_neighborhood((int)temp.x, (int)temp.y, (int)temp.z, &stack, rule, CONNECT6, visitedmode);							
	}

	volobj->is_greyscale = false;

	if(src==0) volobj->has_red = true;
	if(src==1) volobj->has_green = true;
	if(src==2) volobj->has_blue = true;

	if(dst==0) volobj->has_red = true;
	if(dst==1) volobj->has_green = true;
	if(dst==2) volobj->has_blue = true;	
}
void Floodfill::floodfill(int rule)
{
	if(volobj==NULL) return;
	
	//holds voxel indices of the current region of interest
	vector<Point3D> stack;
	vector<int> curr_region;
	
	Point3D temp;
	int tempi;
	
	int x, y, z;

	regions = 0;
	region_sizes.clear();
	region_colours.clear();
	region_indices.clear();
		
	init(volobj->texwidth, volobj->texheight, volobj->texdepth, VISITEDARRAY);
	
	int border = 0;
	
	printf("Labelling Background\n");
	//floodfillseeded(Point3D(0,0,0), rule);
	printf("done...\n");

	Vector rgb, hsv;
	int recursiondepth=0;
	
	//for each x
	for (x =border; x < width-border; x++)
	{
		progress(x, width-border);

		//for each y
		for (y =border; y < height-border; y++)
		{
			for (z = 0; z < depth; z++)
			{	
				temp.x = x;
				temp.y = y;
				temp.z = z;
				tempi = get1Dindex(temp);

				rgb.x = volobj->texture3d[3*tempi+0];
				rgb.y = volobj->texture3d[3*tempi+1]; 
				rgb.z = volobj->texture3d[3*tempi+2];
						
				//if voxel is data and not visited
				//-----------------------------------
				//we can mark our voxels space with -1 or 0
				//if -1 then it is a voxel we want to apply our flood fill to
				//how we mark which voxels are data is done elsewhere.
				if(isVisited(x,y,z,VISITEDARRAY)==false && rgb!=0.0)
				{
					//we found a region
					regions++;
					
					/*hsv.x = rand()%360;
					hsv.y = (float)(rand()%255)/255.0;
					hsv.z = 1.0;			
					HSVtoRGB(hsv, &rgb);*/

					int cont=1;
					
					while(cont)
					{
						rgb.x = (float)(rand()%255)/255.0;
						rgb.y = 0.0;
						rgb.z = (float)(rand()%255)/255.0;					
						rgb *= 255;
						
						cont = 0;
						
						for(int i=0; i<region_colours.size(); i++)
						{
							if(rgb==region_colours[i]) cont = 1;
						}
					}					
					
					
					//save current position
					temp.x = x;
					temp.y = y;
					temp.z = z;

					recursiondepth=0;
					stack.push_back(temp);	
					curr_region.push_back(get1Dindex(temp));

					//while stack not empty
					while(stack.size())
					{						
						//save item and pop off stack
						temp = stack[stack.size()-1];
						stack.pop_back();
		
						//mark the voxel
						markVisited(temp.x, temp.y, temp.z, VISITEDARRAY);
						curr_region.push_back(get1Dindex(temp));

						//search neighborhood for the next pixel
						search_neighborhood((int)temp.x, (int)temp.y, (int)temp.z, &stack, rule, CONNECT26, VISITEDARRAY);							

						//curr_region.push_back(get1Dindex(temp));
						tempi = get1Dindex(temp);
						volobj->texture3d[3*tempi+0] = rgb.x;
						volobj->texture3d[3*tempi+1] = rgb.y; 
						volobj->texture3d[3*tempi+2] = rgb.z;
						
						recursiondepth++;
					}

					region_colours.push_back(Vector((int)rgb.x, (int)rgb.y, (int)rgb.z));		
					//region_sizes.push_back(recursiondepth);		

					region_indices.push_back(curr_region);		
					curr_region.clear();
				}
				
			}
		}
	}

	//	printf("\n Regions Found: %i \n", regions);
	printf("\n Regions Found: %i \n", regions);
}

void Floodfill::apply_regiothreshold(void)
{
	printf("Applying Region Threshold...\n");

	int maxindex=-1;
	int maxsize=(float)(width*height*depth)/2.0f;
	int minsize=50*50*50;

	int index=0;
	int regions_tokeep=0;
	
	vector<bool> regions_todelete;
	Vector rgb, hsv;
		
	for(int i=0; i<region_indices.size(); i++)
	{
		progress(i, region_indices.size());
		//printf("size: %d\n", region_indices[i].size());

		if(region_indices[i].size()>maxsize || region_indices[i].size()<minsize)
		{
			regions_todelete.push_back(true);
			
			for(int j=0; j<region_indices[i].size(); j++)
			{
				maxindex = region_indices[i][j];				
				
				volobj->texture3d[3*maxindex+0] = 0;
				volobj->texture3d[3*maxindex+1] = 0; 
				volobj->texture3d[3*maxindex+2] = 0;
			}
		}
		else
		{
			regions_todelete.push_back(false);
			regions_tokeep++;
			/*//((float)i/(float)region_indices.size())*360.0;
			hsv.x = rand()%360;
			hsv.y = (float)(rand()%255)/255.0;
			hsv.z = 1.0;			
			HSVtoRGB(hsv, &rgb);
			rgb *=
			for(int j=0; j<region_indices[i].size(); j++)
			{
				maxindex = region_indices[i][j];				
				
				volobj->texture3d[3*maxindex+0] = rgb.x*255.0f;
				volobj->texture3d[3*maxindex+1] = rgb.y*255.0f; 
				volobj->texture3d[3*maxindex+2] = rgb.z*255.0f;
			}*/
		}
	}
	
	printf("\n Regions to keep: %i \n", regions_tokeep);
}

void Floodfill::eval_connectivity(void)
{
	if(volobj==NULL) return;
		
	edges.resize(1);
	edges[0].resize(region_colours_tokeep.size());
	
	for(int j=0; j<edges[0].size(); j++)
		edges[0][j].resize(region_colours_tokeep.size());
	
	for(int i=0; i<edges[0].size(); i++)
		for(int j=0; j<edges[0][i].size(); j++)
			edges[0][i][j]=false;

	//holds voxel indices of the current region of interest
	vector< vector<Point3D> > stack;
	vector<Point3D> s;
	
	Point3D temp;
	int tempi;
	int x, y, z;
	int border = 0;
		
	init(volobj->texwidth, volobj->texheight, volobj->texdepth, VISITEDARRAY);
	
	Vector rgb, region_rgb;
	Vector black = Vector(0,0,0);
	Vector yellow = Vector(255,255,0);
	
	int current_regionindex;
	int regionindex;
	
	int recursiondepth=0;
        int maxrecursiondepth = 25;
	int currentlvl=0;
	int go = 1;
	
	//for each x
	for (x =border; x < width-border; x++)
	{
		progress(x, width-border);

		//for each y
		for (y =border; y < height-border; y++)
		{
			for (z = 0; z < depth; z++)
			{
				//save current position
				temp.x = x;
				temp.y = y;
				temp.z = z;

				tempi = get1Dindex(temp);
				
				region_rgb.x = volobj->texture3d[3*tempi+0];
				region_rgb.y = volobj->texture3d[3*tempi+1];
				region_rgb.z = volobj->texture3d[3*tempi+2];
			
				//if voxel is data and not visited
				//-----------------------------------
				//we can mark our voxels space with -1 or 0
				//if -1 then it is a voxel we want to apply our flood fill to
				//how we mark which voxels are data is done elsewhere.
				if(region_rgb!=yellow && region_rgb!=black && isVisited(x,y,z,VISITEDARRAY)==false) // 
				{						
					for(int i=0; i<region_colours_tokeep.size(); i++)
					{
						if(region_rgb==region_colours_tokeep[i])
						{
							current_regionindex = i;
						}
					}
						
					s.push_back(temp);
					stack.push_back(s);
					//printf("regionsize: %d\n", region_sizes[current_regionindex]);				
					//printf("setting maxrecursiondepth to: %d\n", maxrecursiondepth);

					recursiondepth=0;
					currentlvl=0;
					go = 1;
					
					while(go)
					{
						s.clear();
						
						//while stack not empty
						while(stack[currentlvl].size())
						{						
							//save item and pop off stack
							
							//printf("currentlvl: %d, currentlvlstacksize: %d\n", currentlvl, stack[currentlvl].size());
							
							temp = stack[currentlvl][stack[currentlvl].size()-1];
							tempi = get1Dindex(temp);
							stack[currentlvl].pop_back();
					
							//mark the voxel
							markVisited(temp.x, temp.y, temp.z, VISITEDARRAY);
							
/*							rgb.x = volobj->texture3d[3*tempi+0];
							rgb.y = volobj->texture3d[3*tempi+1];
							rgb.z = volobj->texture3d[3*tempi+2];
*/
							volobj->texture3d[3*tempi+0] = region_rgb.x;
							volobj->texture3d[3*tempi+1] = region_rgb.y; 
							volobj->texture3d[3*tempi+2] = region_rgb.z;					

							//search neighborhood for the next pixel
							search_neighborhood((int)temp.x, (int)temp.y, (int)temp.z, &s, FILL_RULE, CONNECT26, VISITEDARRAY);													

							//if it isnt marked as an edge
							/*if(rgb.x!=255.0 && rgb.y!=255.0 && rgb.z !=0.0)
							{
								volobj->texture3d[3*tempi+0] = region_rgb.x;
								volobj->texture3d[3*tempi+1] = region_rgb.y; 
								volobj->texture3d[3*tempi+2] = region_rgb.z;					
							}*/
							
							/*if(region_rgb!=rgb)
							{
								for(int i=0; i<region_colours_tokeep.size(); i++)
								{
									if(rgb==region_colours_tokeep[i])
									{
										printf("FOUND EDGE\n");

										regionindex = i;
										edges[0][current_regionindex][regionindex] = true;
										edges[0][regionindex][current_regionindex] = true;
										
										visited_voxels[(int)temp.x][(int)temp.y][(int)temp.z] = false;
									}
								}
							}*/																				
						}
					
						//printf("recursiondepth: %d, stacksize: %d\n", recursiondepth, s.size());
											
						stack.push_back(s);
						stack[currentlvl].clear();
						currentlvl++;
						
						recursiondepth++;
						if(recursiondepth>=maxrecursiondepth || s.empty())
						{	
								
							/*for(int j=0; j<stack.size(); j++)
							{
								for(int i=0; i<stack[j].size(); i++)
								{
									temp = stack[j][i];
									visited_voxels[(int)temp.x][(int)temp.y][(int)temp.z] = false;
								}
								stack[j].clear();
							}*/
						
							stack.clear();
							
							/*for(int i=0; i<s.size(); i++)
							{
								visited_voxels[(int)s[i].x][(int)s[i].y][(int)s[i].z] = false;
							}*/
							
							s.clear();
							go=0;
							
							//return;
						}
						/*else
						{
							stack[currentlvl].clear();
						}*/
					}
				}
				
			}
		}
	}

	//	printf("\n Regions Found: %i \n", regions);
	printf("\n Regions Found: %i \n", regions);
}

void Floodfill::apply_result(int minsize, int maxsize)
{
		
	printf("Applying Region Threshold...\n");

	int maxindex=-1;
	//int maxsize=(float)(width*height*depth)/8.0f;
	//int minsize=1000;

	int index=0;
	int regions_tokeep=0;
	
	regions_todelete.clear();
	Vector rgb, hsv;

	ofstream fout;
	fout.open("regions.txt");
	for(int i=0; i<region_indices.size(); i++)
		fout<<region_indices[i].size()<<endl;
	fout.close();

	for(int i=0; i<region_indices.size(); i++)
	{
		progress(i, region_indices.size());
		//printf("size: %d\n", region_indices[i].size());

		if(region_indices[i].size()>maxsize || region_indices[i].size()<minsize)
		{
			regions_todelete.push_back(true);
			
			for(int j=0; j<region_indices[i].size(); j++)
			{
				maxindex = region_indices[i][j];				
				
				volobj->texture3d[3*maxindex+0] = 0;
				volobj->texture3d[3*maxindex+1] = 0; 
				volobj->texture3d[3*maxindex+2] = 0;
			}
		}
		else
		{
			regions_todelete.push_back(false);
			regions_tokeep++;
			/*//((float)i/(float)region_indices.size())*360.0;
			hsv.x = rand()%360;
			hsv.y = (float)(rand()%255)/255.0;
			hsv.z = 1.0;			
			HSVtoRGB(hsv, &rgb);
			rgb *=
			for(int j=0; j<region_indices[i].size(); j++)
			{
				maxindex = region_indices[i][j];				
				
				volobj->texture3d[3*maxindex+0] = rgb.x*255.0f;
				volobj->texture3d[3*maxindex+1] = rgb.y*255.0f; 
				volobj->texture3d[3*maxindex+2] = rgb.z*255.0f;
			}*/
		}
	}
	
	printf("\n Regions to keep: %i \n", regions_tokeep);
	/*
	PCA pca;
	vector< PCA > pca_vector;
	Vector p;
	celleigenvectors.clear();
	bool skip_region=false;
	int region_index;
	
	float voxX = 1.0;
	float voxY = 1.0;
	float voxZ = 1.0;	
	
	float unitX = volobj->xscale;
	float unitY = volobj->yscale;
	float unitZ = volobj->zscale;
	
	float maxsize2 =  max(unitX, unitY);
	maxsize2 =  max(maxsize2, unitZ);
	
	//Vector res = Vector((glwindow->glvoxel->volobject.xsize/maxsize), (glwindow->glvoxel->volobject.ysize/maxsize), (glwindow->glvoxel->volobject.zsize/maxsize));
	
	float spacingX = unitX/maxsize2;
	float spacingY = unitY/maxsize2; 
	float spacingZ = unitZ/maxsize2;
	
	float rescaleX = volobj->x_resize;
	float rescaleY = volobj->y_resize;
	float rescaleZ = volobj->z_resize;

	printf("spacing: %f, %f, %f\n", spacingX, spacingY, spacingZ);
	printf("rescale: %f, %f, %f\n", rescaleX, rescaleY, rescaleZ);

	printf("Evaluating PCA data...\n");
		
	for(int i=0; i<region_colours.size(); i++)
		if(regions_todelete[i]==false) region_colours_tokeep.push_back(region_colours[i]);
		
	pca_vector.resize(region_colours.size());

	for (int x=0; x <width; x++)
	{
		progress(x, width);
		
		for (int y=0; y < height; y++)
		{
			for (int z = 0; z < depth; z++)
			{
				maxindex = get1Dindex(Point3D(x,y,z));
				
				rgb.x = volobj->texture3d[3*maxindex+0];
				rgb.y = volobj->texture3d[3*maxindex+1]; 
				rgb.z = volobj->texture3d[3*maxindex+2];

				for(int i=0; i<region_colours_tokeep.size(); i++)
				{
					if(rgb==region_colours_tokeep[i])
					{
						p.x = (float)x-((float)((width-1))/2.0);
						p.y = (float)y-((float)((height-1))/2.0);
						p.z = (float)z-((float)((depth-1))/2.0);

						p.x *= scaleX;
						p.y *= scaleY;
						p.z *= scaleZ;
												
						//printf("p: %f, %f, %f\n", p.x, p.y, p.z);
						pca_vector[i].data.push_back(p);
					}
				}
			}
		}
	}

	for(int i=0; i<pca_vector.size(); i++)
	{
			pca_vector[i].eval_PCA();
			//pca.eval_boundingbox(scaleX,scaleY, scaleZ);
			//pca.eval_scaled_eigensvectors(scaleX,scaleY, scaleZ);
			pca_vector[i].eval_scaled_eigensvectors((float)scaleX, (float)scaleY, (float)scaleZ);

			Vector v;
			for(int j=0; j<pca_vector[i].eigenvectors.size(); j++)
			{	
				cellmeans.push_back(Vector(pca_vector[i].originalmeanx, pca_vector[i].originalmeany, pca_vector[i].originalmeanz));
				celleigenvectors.push_back(pca_vector[i].eigenvectors[j]);
			}
	}
*/
/*
	for(int i=0; i<region_indices.size(); i++)
	{
		progress(i, region_indices.size());
		region_index=i;
	
		if(regions_todelete[i]==true)
		{
			//printf("Skipping region\n");
			//printf("size: %d\n", region_indices[i].size());

		}
		else
		{
			pca.reset();

			//printf("Loading PCA data...\n");
			//printf("size: %d\n", region_indices[i].size());
			
			for (int x=0; x <width; x++)
			{
				//progress(x, width);
				for (int y=0; y < height; y++)
				{
					for (int z = 0; z < depth; z++)
					{					
						maxindex = get1Dindex(Point3D(x,y,z));
						
						rgb.x = volobj->texture3d[3*maxindex+0];
						rgb.y = volobj->texture3d[3*maxindex+1]; 
						rgb.z = volobj->texture3d[3*maxindex+2];	
						
						if(rgb==region_colours[i])
						{
							p.x = (float)x-((float)((width-1))/2.0);
							p.y = (float)y-((float)((height-1))/2.0);
							p.z = (float)z-((float)((depth-1))/2.0);

							p.x *= scaleX;
							p.y *= scaleY;
							p.z *= scaleZ;
													
							//printf("p: %f, %f, %f\n", p.x, p.y, p.z);
							pca.data.push_back(p);
						}
					}
				}
			}
			
			//printf("pca size: %d\n\n", pca.data.size());
			//if(pca.data.size()!=region_indices[i].size()) printf("UHOH\n");
			
			pca.eval_PCA();
			//pca.eval_boundingbox(scaleX,scaleY, scaleZ);
			//pca.eval_scaled_eigensvectors(scaleX,scaleY, scaleZ);
			pca.eval_scaled_eigensvectors((float)scaleX, (float)scaleY, (float)scaleZ);

			Vector v;
			for(int j=0; j<pca.eigenvectors.size(); j++)
			{	
				cellmeans.push_back(Vector(pca.originalmeanx, pca.originalmeany, pca.originalmeanz));
				celleigenvectors.push_back(pca.eigenvectors[j]);
			}
		}
	}
*/
}
