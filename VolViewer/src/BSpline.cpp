/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "BSpline.h"

BSpline::BSpline()
{
	linear=false;
}


BSpline::~BSpline()
{
	clear();
}
void BSpline::clear(void)
{
	curve_points.clear();
	control_points.clear();
	control_points_colours.clear();
	u.clear();
}
	
void BSpline::init(int ncontp, int ncurvp, int d)
{
	poly_degree = d+1;
	num_control_points = ncontp-1;
	num_curve_points = ncurvp;

	control_points.resize(num_control_points+1);
	control_points_colours.resize(num_control_points+1);
}

void BSpline::compute(void)
{
	curve_points.clear();
	curve_points.resize(num_curve_points);

	if(linear==true && num_curve_points==256)
	{
		Vector p;
		float interp;

		for (int index=0; index<curve_points.size(); index++)
		{
			for (int j=0; j<control_points.size()-1; j++)
			{
				if(index>=control_points[j].x && index<control_points[j+1].x)
				{
					interp = (float)(index-control_points[j].x)/(float)(control_points[j+1].x-control_points[j].x);

					p.x = index;
					p.y = (1.0-interp)*control_points[j].y + (interp)*control_points[j+1].y;
					p.z = 0.0;
					curve_points[index]=p;
				}
			}	
		}
	}
	else
	{
		u.clear();
		u.resize(num_control_points+poly_degree+1);

		compute_intervals();
		
		double increment=(double) (num_control_points-poly_degree+2)/(num_curve_points-1);  // how much parameter goes up each time
		double interval=0;

		for (int index=0; index<num_curve_points; index++)
		{
			compute_point(interval, &curve_points[index]);

			// increment our parameter
			interval=interval+increment;  
		}

		u.clear();
	}
}

double BSpline::blend(int k, int t, double v)  // calculate the blending value
{
  double value;

  if (t==1)			// base case for the recursion
  {
    if ((u[k]<=v) && (v<u[k+1]))
	{
      value=1;
    }
		else
		{
		  value=0;
		}
  }
	  else
	  {
		if ((u[k+t-1]==u[k]) && (u[k+t]==u[k+1]))  // check for divide by zero
		{
		  value = 0;
		}
			else
			{
				if (u[k+t-1]==u[k]) // if a term's denominator is zero,use just the other
				{
					value = (u[k+t] - v) / (u[k+t] - u[k+1]) * blend(k+1, t-1, v);
				}
					else
					{
						if (u[k+t]==u[k+1])
						{
							value = (v - u[k]) / (u[k+t-1] - u[k]) * blend(k, t-1, v);
						}
							else
							{
							  value = (v - u[k]) / (u[k+t-1] - u[k]) * blend(k, t-1, v) +
								  (u[k+t] - v) / (u[k+t] - u[k+1]) * blend(k+1, t-1, v);
							}
					}
			}
	  }

  return value;
}

void BSpline::compute_intervals(void)   // figure out the knots
{
  int j;

  for (j=0; j<=num_control_points+poly_degree; j++)
  {
    if (j<poly_degree)
	{	
		u[j]=0;
    }
	else
	{
		if ((poly_degree<=j) && (j<=num_control_points))
		{
		  u[j]=j-poly_degree+1;
		}
		 else
			{
				if (j>poly_degree)
				{
				  u[j]=num_control_points-poly_degree+2;  // if num_control_points-poly_degree=-2 then we're screwed, everything goes to 0
				}
			}
	}
  }
}

void BSpline::compute_point(double v, Vector* output)
{
	int k;
	double temp;

	for (k=0; k<control_points.size(); k++)
	{
		// same blend is used for each dimension coordinate
		temp = blend(k,poly_degree, v);
		output->x = output->x + control_points[k].x * temp;
		output->y = output->y + control_points[k].y * temp;
		output->z = output->z + control_points[k].z * temp;
	}
}
const BSpline& BSpline::operator= (const BSpline& right)
{
	clear();

	control_points.resize(right.control_points.size());
	control_points_colours.resize(right.control_points_colours.size());

	curve_points.resize(right.curve_points.size());
	u.resize(right.u.size());
	
	int i;

	for(i=0; i<right.control_points.size(); i++)
		control_points[i] = right.control_points[i];

	for(i=0; i<right.control_points_colours.size(); i++)
		control_points_colours[i] = right.control_points_colours[i];

	for(i=0; i<right.curve_points.size(); i++)
		curve_points[i] = right.curve_points[i];

	for(i=0; i<right.u.size(); i++)
		u[i] = right.u[i];

	num_control_points = right.num_control_points;
	num_curve_points = right.num_curve_points;
	poly_degree = right.poly_degree;
	
	linear = right.linear;

	return *this;
}