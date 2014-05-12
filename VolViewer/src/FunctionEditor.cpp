/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */

#include "FunctionEditor.h"

FunctionEditor::FunctionEditor()
{
 	selected = -1;
	delete_event = 0;

	delete_zone = 0.0;
	repel_factor = 0.0;

	x_min = 0.0;
	x_max = 0.0;
	size = 0.0;
}
FunctionEditor::~FunctionEditor()
{
	clear();
}
void FunctionEditor::clear(void)
{
	bs.clear();
}
void FunctionEditor::init(float _size, int border, int offset)
{	
	size = _size;
	delete_zone = border;
	repel_factor = offset;

    bs.init(4, (int)_size, 2);

	bs.control_points[0].x=border+0.0;			bs.control_points[0].y=border+_size;			bs.control_points[0].z=0.0;
	bs.control_points[1].x=border+(_size*0.25);  bs.control_points[1].y=border+(_size*0.75);  bs.control_points[1].z=0.0;
	bs.control_points[2].x=border+(_size*0.75);  bs.control_points[2].y=border+(_size*0.25);  bs.control_points[2].z=0.0;
	bs.control_points[3].x=border+size;			bs.control_points[3].y=border+0.0;			bs.control_points[3].z=0.0;
 
	/*bs.control_points_colours[0] = Vector(border+_size);
	bs.control_points_colours[1] = Vector(border+(_size*0.75));
	bs.control_points_colours[2] = Vector(border+(_size*0.25));
	bs.control_points_colours[3] = Vector(border+0.0);*/

	bs.compute();
}
void FunctionEditor::reset()
{
	bs.init(4, size, 2);

	bs.control_points[0].x=delete_zone+0.0;			bs.control_points[0].y=delete_zone+size;			bs.control_points[0].z=delete_zone;
	bs.control_points[1].x=delete_zone+(size*0.25);  bs.control_points[1].y=delete_zone+(size*0.75);  bs.control_points[1].z=0.0;
	bs.control_points[2].x=delete_zone+(size*0.75);  bs.control_points[2].y=delete_zone+(size*0.25);  bs.control_points[2].z=0.0;
	bs.control_points[3].x=delete_zone+size;			bs.control_points[3].y=delete_zone+0.0;			bs.control_points[3].z=0.0;
 
	/*bs.control_points_colours[0] = Vector(delete_zone+size);
	bs.control_points_colours[1] = Vector(delete_zone+(size*0.75));
	bs.control_points_colours[2] = Vector(delete_zone+(size*0.25));
	bs.control_points_colours[3] = Vector(delete_zone+0.0);*/

	bs.compute();
}
void FunctionEditor::addPoint(int x, int y)
{
	//constrain points to (0,0) -> (1,1) box
	if(x<delete_zone || x>size+delete_zone || y<delete_zone || y>size+delete_zone) return;
	
	Vector p;
	p.x = x;
	p.y = y;
	p.z = 0;

	bs.control_points.push_back(p);
	bs.num_control_points++;

	//bs.control_points_colours.push_back(Vector(y));

	//sort our points according to their x value
	quicksortPoints(0, bs.control_points.size()-1);

	//make sure we are not allowed to leave function space
	constrain_in_bb();
	constrain_in_neighbours();

	bs.compute();

	//find our inserted point
	for(int i=0; i<bs.control_points.size(); i++)
	{
		if(p.x==bs.control_points[i].x) selected = i;
	}
}
void FunctionEditor::deletePoint(void)
{
	//make sure we have more than 4 points
	if(bs.control_points.size()<5)
	{
		selected=-1;
		return;
	}
	
	/*if(selected>0 && selected<bs.control_points.size())
	{
		bs.control_points.erase(bs.control_points.begin()+selected);
		//bs.control_points_colours.erase(bs.control_points_colours.begin()+selected);
	}*/

	int j=0;
	vector<Vector>::iterator i;

	for(i=bs.control_points.begin(); i!=bs.control_points.end(); i++)
	{
		if(j==selected) 
		{
			//make sure we dont delete our start and end points
			if(j!=0 && j!=bs.control_points.size())
			{
				bs.control_points.erase(i);
				//bs.control_points_colours.erase(i);
				bs.num_control_points--;
				break;
			}
		}

		j++;
	}
	
	//make sure we are allowed to leave function space
	constrain_in_bb();
	constrain_in_neighbours();

	bs.compute();
	selected=-1;
}
void FunctionEditor::movePoint(int x, int y)
{
	//if we have a delete event move v (outside grey)
	if(delete_event==1)
	{
		if(x<x_min) x = x_min+repel_factor;
		if(x>x_max) x = x_max-repel_factor;

		v.x = x;
		v.y = y;
	}
	//else move cp normally, (inside box)
	else
	{
		bs.control_points[selected].x = x;
		bs.control_points[selected].y = y;
		//bs.control_points_colours[selected]=Vector(y);
	}

	//make sure we are allowed to leave function space
	constrain_in_bb();
	constrain_in_neighbours();

	//compute our curve
	bs.compute();
}

void FunctionEditor::constrain_in_bb(void)
{
	//if not in delete mode and something is selected
	if(selected!=-1)
	{
		//constrain start and end point at x(0) x(1) of function box
		bs.control_points[0].x = (float)delete_zone;
		bs.control_points[bs.control_points.size()-1].x = (float)delete_zone+size;

		//do not allow min 4 points to be moved to delete area (outside grey area)
		if(bs.control_points.size()==4)
		{
				//constrain points to (0,0) -> (1,1) box
				if(bs.control_points[selected].x<delete_zone) bs.control_points[selected].x= delete_zone;
				if(bs.control_points[selected].x>(size+delete_zone)) bs.control_points[selected].x= size+delete_zone;

				if(bs.control_points[selected].y<delete_zone) bs.control_points[selected].y= delete_zone;
				if(bs.control_points[selected].y>(size+delete_zone)) bs.control_points[selected].y= size+delete_zone;
		}
		//if we have more than four points these can be moved to delete area
		else
		{		
			//are we still in the delete buffer zone (grey area)
			if((bs.control_points[selected].y>(delete_zone/2))&&(bs.control_points[selected].y<(size+delete_zone+(delete_zone/2))))
			{		
				//constrain points to (0,0) -> (1,1) box
				if(bs.control_points[selected].y<delete_zone) bs.control_points[selected].y= delete_zone;
				if(bs.control_points[selected].y>size+delete_zone) bs.control_points[selected].y= size+delete_zone;
			}
			//if we moved outside delete buffer (grey area) produce a delete_event
			else
			{			
				//dont allow deletion of end points...
				if(selected==0 ||  selected==bs.control_points.size()-1)
				{
					if(bs.control_points[selected].y<delete_zone) bs.control_points[selected].y= delete_zone;
					if(bs.control_points[selected].y>size+delete_zone) bs.control_points[selected].y= size+delete_zone;
					return;
				}
				
				
				//save the point we want to delete, in case we dont really...
				v = bs.control_points[selected];

				x_min = bs.control_points[selected-1].x;
				x_max = bs.control_points[selected+1].x;

				//delete point from control po\ints (allows for delete preview)
				deletePoint();

				//generate the event
				delete_event=1;

				//select none of our cp
				selected =-1;
			}
		}
	}

	//if in delete mode
	if(delete_event==1)
	{
		//if our temp point is back to the delete buffer zone,
		//we dont want to delete it, so add it back
		if( (v.y>(delete_zone/2))&&(v.y<(size+delete_zone+(delete_zone/2))) )
		{
			//add the point back
			bs.control_points.push_back(v);
			bs.num_control_points++;

			//sort our points according to their x value
			quicksortPoints(0, bs.control_points.size()-1);			

			//find our inserted point
			for(int i=0; i<bs.control_points.size(); i++)
			{
				if(v.x==bs.control_points[i].x) selected = i;
			}

			//compute the new bspline
			bs.compute();

			//end delete event
			delete_event = 0;
		}
	}
}
void FunctionEditor::constrain_in_neighbours(void)
{
	//since start and end points are x constraint we dont need to check them
	if(selected!=0 && selected!=bs.control_points.size()-1 && selected!=-1)
	{
		//if we are smaller then our left neighbour constrain
		if(bs.control_points[selected].x < bs.control_points[selected-1].x)
		{
			bs.control_points[selected].x = bs.control_points[selected-1].x+repel_factor;
		}
		//if we are bigger then our right neighbour constrain
		if(bs.control_points[selected].x> bs.control_points[selected+1].x)
		{
			bs.control_points[selected].x = bs.control_points[selected+1].x-repel_factor;
		}

	}
}

int FunctionEditor::selectPoint(int x, int y)
{
	//check for collision
	for(int i=0; i<bs.control_points.size(); i++)
	{
		if(x>=bs.control_points[i].x-repel_factor && x<=bs.control_points[i].x+repel_factor)
		{
			if(y>=bs.control_points[i].y-repel_factor && y<=bs.control_points[i].y+repel_factor)
			{
				selected = i;
				return 1;
			}
		}
	}

	return 0;
}

//Quicksort
void FunctionEditor::quicksortPoints(int lo, int hi)
{
	int left, right;
	float median;
	Vector temp;

	if( hi > lo ) /* i.e. at least 2 elements, then */
	{ 
	  left=lo; 
	  right=hi;

	  median=bs.control_points[lo].x;  /* NB. just an estimate! */

	  while(right >= left) /* partition a[lo..hi] */
	  { 
		 while(bs.control_points[left].x < median) left++;
		 while(bs.control_points[right].x > median) right--;

		 if(left > right) break;

		 //swap:
		 temp=bs.control_points[left]; 
		 bs.control_points[left]=bs.control_points[right]; 
		 bs.control_points[right]=temp;
		
		 //temp = bs.control_points_colours[left];
		 //bs.control_points_colours[left]=bs.control_points_colours[right]; 
		// bs.control_points_colours[right]=temp;

		 left++; 
		 right--;
	  }

	  quicksortPoints(lo, right);// divide and conquer
	  quicksortPoints(left,  hi);
	}
}
void FunctionEditor::display(void)
{
//	glScalef((float)width/scale, (float)height/scale, 0);
//	glTranslatef(0.5, 0.5, 0);
	glColor3f(0.3f, 0.3f, 0.3f);

	glColor3f(0.2f, 0.2f, 0.2f);
	glBegin(GL_QUADS);
		glVertex3f(0.0-delete_zone, 0.0-delete_zone, 0.0);
		glVertex3f(1.0+delete_zone, 0.0-delete_zone, 0.0);
		glVertex3f(1.0+delete_zone, 1.0+delete_zone, 0.0);
		glVertex3f(0.0-delete_zone, 1.0+delete_zone, 0.0);
	glEnd();

	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(1.0, 0.0, 0.0);
		glVertex3f(1.0, 1.0, 0.0);
		glVertex3f(0.0, 1.0, 0.0);
	glEnd();

	glColor3f(0.3f, 0.3f, 0.3f);
	float grid_steps = 1.0f/10.0f;
	float grid_step=0;
	for(int ii=0; ii<10; ii++)
	{
		glBegin(GL_LINE_STRIP);
		glVertex3f(grid_step,0.0, 0.0);
		glVertex3f(grid_step,1.0, 0.0);
		glEnd();
		glBegin(GL_LINE_STRIP);
		glVertex3f(0.0,grid_step, 0.0);
		glVertex3f(1.0,grid_step, 0.0);
		glEnd();

		grid_step+=grid_steps;
	}

	glBegin(GL_POINTS);
	for(int i=0; i<bs.control_points.size(); i++)
	{		 
		if(i==selected)	glColor3f(1.0, 0.0, 0.0);
		else glColor3f(1.0, 1.0, 0.0);

		glVertex3fv(&bs.control_points[i].x);
	}
	glEnd();

	if(delete_event==1)
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POINTS);
		glVertex3fv(&v.x);
		glEnd();
	}


	glColor3f(0.3f, 0.3f, 1.0f);
	glBegin(GL_LINES);
	for(int ij=0; ij<bs.control_points.size(); ij++)
	{		 
		glVertex3fv(&bs.control_points[ij].x);
	}
	glEnd();

	glColor3f(0.0, 1.0, 0.0);
	glBegin(GL_LINE_STRIP);
	for(int j=0; j<bs.curve_points.size(); j++)
		glVertex3fv(&bs.curve_points[j].x);
	glEnd();

	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_LOOP);
		glVertex3f(0.0, 0.0, 0.0);
		glVertex3f(1.0, 0.0, 0.0);
		glVertex3f(1.0, 1.0, 0.0);
		glVertex3f(0.0, 1.0, 0.0);
	glEnd();
}
void FunctionEditor::resize(int w, int h)
{
//	width = w;
//	height = h;
}

const FunctionEditor& FunctionEditor::operator= (const FunctionEditor& right)
{

	//temp vector for delete_event
	v = right.v;
	x_min = right.x_min;
	x_max = right.x_max;

	size = right.size;

	//a delete event
	delete_event = right.delete_event;

	//which cp is selected
	selected = right.selected;

	//our delete zone size
	delete_zone = right.delete_zone;
	//our repultion factor
	repel_factor = right.repel_factor;

	//the bspline
	bs = right.bs;
	
	return *this;
}
