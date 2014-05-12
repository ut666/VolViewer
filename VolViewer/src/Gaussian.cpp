/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "Gaussian.h"

Gaussian::Gaussian()
{
	sigma = 0.0;
	half_width = 0.0;
}

Gaussian::~Gaussian()
{

}

void Gaussian::clear_all(void)
{
	//3d
	for(int nn=0; nn<mask3d.size(); nn++)
		for(int nnn=0; nnn<mask3d[nn].size(); nnn++)
		{
			mask3d[nn][nnn].clear();
		}		
	
	for(int nnn=0; nnn<mask3d.size(); nnn++)
	{
		mask3d[nnn].clear();
	}
	mask3d.clear();
	
	//2d
	for(int nnn=0; nnn<mask2d.size(); nnn++)
	{
		mask2d[nnn].clear();
	}
	mask2d.clear();	
	
	//1d
	mask1d.clear();	
}

double Gaussian::gaussian1d(double x)
{
	double result;

	result = (1.0/(sqrt(TWOPI*sigma))) * exp(-((x)*(x)) / (2*sigma*sigma));

	return result;
}

double Gaussian::gaussian2d(double x, double y)
{
	double result;

	result = (1.0/(sqrt(TWOPI*sigma)))*exp(-((x)*(x)+(y)*(y))/(2*sigma*sigma));

	return result;
}

double Gaussian::gaussian3d(double x, double y, double z)
{
	double result;

	result = (1.0/(sqrt(TWOPI*sigma)))*exp(-((x)*(x)+(y)*(y)+(z)*(z))/(2*sigma*sigma));

	return result;
}

void Gaussian::eval_half_width(void)
{
	//this really is the full width at half height
   half_width = 2.0*sqrt(2.0*log(2.0))*sigma;
}

void Gaussian::eval_sigma(int r)
{
	size = 2*r+1;
	sigma = (2*(double)(r)+1)/4.9;
}
const vector<float> Gaussian::gaussianmask1Df(float s, float size)
{
	//allocate some storage for our 1D mask
	fmask1d.resize((int)size);

	//set our gaussian values
	float sum = 0;
	float center=ceil(size/2.0);
	center--;

	for(float i=0; i<size; i++)
	{
			float x = i-center;

			fmask1d[(int)i] = (float)gaussian1d((double)x);
			sum += fmask1d[i];
	}

	//normalise
	for(int j=0; j<fmask1d.size(); j++)
		fmask1d[j] /=sum;

	//return the mask
	return fmask1d;
}
const vector<double> Gaussian::gaussianmask1D(double s, double size)
{
	//allocate some storage for our 1D mask
	mask1d.resize((int)size);

	//set our gaussian values
	double sum = 0;
	double center=ceil(size/2.0);
	center--;

	for(double i=0; i<size; i++)
	{
			double x = i-center;

			mask1d[(int)i] = gaussian1d(x);
			sum += mask1d[i];
	}

	//normalise
	for(int j=0; j<mask1d.size(); j++)
		mask1d[j] /=sum;

	//return the mask
	return mask1d;
}

const vector< vector<double> > Gaussian::gaussianmask2D(double s, double size)
{
	sigma = s;

	//allocate some storage for our 1D mask
	mask2d.resize((int)size);
	for(int n=0; n<mask2d.size(); n++)
		mask2d[n].resize((int)size);

	//set our gaussian values
	double sum = 0;
	double center=ceil(size/2.0);
	center--;

	for(double i=0; i<size; i++)
	{
		for(double j=0; j<size; j++)
		{
			double x = i-center;
			double y = j-center;

			mask2d[(int)i][(int)j] = gaussian2d(x,y);
			sum += mask2d[(int)i][(int)j];
		}
	}

	//normalise
	for(int l=0; l<(int)size; l++)
		for(int k=0; k<(int)size; k++)
			mask2d[l][k] /=sum;

	//return the mask
	return mask2d;
}


const vector< vector< vector<double> > > Gaussian::gaussianmask3D(double s, double size)
{
	sigma = s;

	//allocate some storage for our 1D mask

	mask3d.resize((int)size);
	for(int n=0; n<mask3d.size(); n++)
		mask3d[n].resize((int)size);

	for(int nn=0; nn<mask3d.size(); nn++)
		for(int nnn=0; nnn<mask3d.size(); nnn++)
		mask3d[nn][nnn].resize((int)size);

	//set our gaussian values
	double sum = 0;
	double center=ceil(size/2.0);
	center--;

	for(double i=0; i<mask3d.size(); i++)
	{
		for(double j=0; j<mask3d.size(); j++)
		{
			for(double k=0; k<mask3d.size(); k++)
			{
				double x = i-center;
				double y = j-center;
				double z = k-center;

				mask3d[(int)i][(int)j][(int)k] = gaussian3d(x,y,z);
				sum += mask3d[(int)i][(int)j][(int)k];
			}
		}
	}

	//normalise
	for(int l=0; l<(int)size; l++)
		for(int m=0; m<(int)size; m++)
			for(int n=0; n<(int)size; n++)
				mask3d[l][m][n] /=sum;

	//return the mask
	return mask3d;
}
void Gaussian::display1d(void)
{
   double i;

   glColor3f(1.0,1.0,1.0);
   glBegin(GL_LINE_STRIP);
   for(i=-3.0*sigma; i<3.0*sigma; i+=0.1)
   {			
	   glVertex3f(i, gaussian1d(i), 0);
   }
   glEnd();

   glBegin(GL_POINTS);
   for(i=-3.0*sigma; i<3.0*sigma; i+=0.1)
   {
	   glVertex3f(i, gaussian1d(i), 0);
	}
   glEnd();

   glColor3f(0.0,1.0,0.0);
   glBegin(GL_LINES);
		glVertex3f(-3.0*sigma, 0, 0);
		glVertex3f(3.0*sigma, 0, 0);
   glEnd();
   
}
void Gaussian::display2d(void)
{
   double j;
 
   glColor3f(1.0,1.0,1.0);
   for(double i=-3.0*sigma; i<3.0*sigma; i+=0.1)
   {
	   glBegin(GL_LINE_STRIP);
	   for(j=-3.0*sigma; j<3.0*sigma; j+=0.1)
	   {		    
			glVertex3f(i,j,gaussian2d(i,j));
	   }
	   glEnd();

	   glBegin(GL_POINTS);
	   for(j=-3.0*sigma; j<3.0*sigma; j+=0.1)
	   {		    
			glVertex3f(i,j,gaussian2d(i,j));
	   }
	   glEnd();
   }

   glColor3f(0.0,1.0,0.0);
   glBegin(GL_LINES);
		glVertex3f(-3.0*sigma, 3.0*sigma, 0);
		glVertex3f(3.0*sigma, 3.0*sigma, 0);

		glVertex3f(-3.0*sigma, -3.0*sigma, 0);
		glVertex3f(3.0*sigma, -3.0*sigma, 0);

		glVertex3f(-3.0*sigma, -3.0*sigma, 0);
		glVertex3f(-3.0*sigma, 3.0*sigma, 0);

		glVertex3f(3.0*sigma, -3.0*sigma, 0);
		glVertex3f(3.0*sigma, 3.0*sigma, 0);
   glEnd();

}

void Gaussian::display3d(void)
{
   glColor3f(1.0,1.0,1.0);
   glBegin(GL_POINTS);
   for(double i=-3.0*sigma; i<3.0*sigma; i+=0.25)
	   for(double j=-3.0*sigma; j<3.0*sigma; j+=0.25)
		   for(double k=-3.0*sigma; k<3.0*sigma; k+=0.25)
		   {		    
				glColor3f(gaussian3d(i,j,k)*10, gaussian3d(i,j,k)*10, gaussian3d(i,j,k)*10);
				glVertex3f(i,j, k);
		   }
   glEnd();

   glColor3f(0.0,1.0,0.0);
   glBegin(GL_LINES);
		glVertex3f(-3.0*sigma, 3.0*sigma, 0);
		glVertex3f(3.0*sigma, 3.0*sigma, 0);

		glVertex3f(-3.0*sigma, -3.0*sigma, 0);
		glVertex3f(3.0*sigma, -3.0*sigma, 0);

		glVertex3f(-3.0*sigma, -3.0*sigma, 0);
		glVertex3f(-3.0*sigma, 3.0*sigma, 0);

		glVertex3f(3.0*sigma, -3.0*sigma, 0);
		glVertex3f(3.0*sigma, 3.0*sigma, 0);
   glEnd();
   
}