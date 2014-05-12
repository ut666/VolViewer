/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "PCA.h"

PCA::PCA()
{
	meanx = meany = meanz = 0.0;
	variancex = variancey = variancez = 0.0;
	covariance = 0.0;
}
PCA::~PCA()
{
	reset();
}
void PCA::reset(void)
{
	meanx = meany = meanz = 0.0;
	variancex = variancey = variancez = 0.0;
	covariance = 0.0;

	eigenvectors.clear();
	
	data.clear();
}
void PCA::gen_rnd1D(int size)
{
	for(int i=0; i<size; i++)
	{
//		data.push_back(Vector(RANDOM_RANGE(50.0, 100.0), 0.0, 0.0));
	}
}
void PCA::gen_rnd2D(int size)
{
	for(int i=0; i<size; i++)
	{
		for(int j=0; j<size; j++)
		{
			data.push_back(Vector(i, j, 0.0));
		}

	}
}
void PCA::gen_rnd3D(int size)
{
	for(int i=0; i<size; i++)
	{
		for(int j=0; j<size; j++)
		{
			for(int k=0; k<size; k++)
			{
				data.push_back(Vector(i,j,k));
			}
		}
		
	//	data.push_back(Vector(RANDOM_RANGE(0.0, 100.0), RANDOM_RANGE(0.0, 100.0), RANDOM_RANGE(0.0, 100.0))); //i/10.0
	}
}

void PCA::eval_meanX()
{
	double sum_values = 0;

	//sum the values up
	for(int i=0; i<data.size(); i++)
	{
		sum_values +=  data[i].x;
	}

	//divide by the number of values
	meanx = sum_values / (double)(data.size());
}
void PCA::eval_meanY()
{
	double sum_values = 0;

	//sum the values up
	for(int i=0; i<data.size(); i++)
	{
		sum_values +=  data[i].y;
	}

	//divide by the number of values
	meany = sum_values / (double)(data.size());
//s	printf("meany: %f\n", meany);

}
void PCA::eval_meanZ()
{
	double sum_values = 0;

	//sum the values up
	for(int i=0; i<data.size(); i++)
	{
		sum_values +=  data[i].z;
	}

	//divide by the number of values
	meanz = sum_values / (double)(data.size());
}
void PCA::eval_varX()
{
  double VarSum = 0;
  double x;

  // calculate the standard deviation sum
  for (int i=0; i<data.size(); i++) 
  {
    x = data[i].x - meanx;
    VarSum += (x * x);
  }

  //work out the variance
  variancex = VarSum / (double)(data.size()-1);
}
void PCA::eval_varY()
{
  double VarSum = 0;
  double y;

  // calculate the standard deviation sum
  for (int i=0; i<data.size(); i++) 
  {
    y = data[i].y - meany;
    VarSum += (y * y);
  }

  //work out the variance
  variancey = VarSum / (double)(data.size()-1);
}

void PCA::eval_varZ()
{
  double VarSum = 0;
  double z;

  // calculate the standard deviation sum
  for (int i=0; i<data.size(); i++) 
  {
    z = data[i].z - meanz;
    VarSum += (z * z);
  }

  //work out the variance
  variancez = VarSum / (double)(data.size()-1);
}

void PCA::eval_covariance(void)
{
  double x;
  double y;
  double CoVarSum = 0.0;

  for (int i=0; i<data.size(); i++) 
  {
	  x = data[i].x - meanx;
	  y = data[i].y - meany;
	  CoVarSum += x*y;
  }

  covariance = CoVarSum / (double)(data.size()-1);
}

void PCA::eval_covariance_matrix(void)
{
	int i;

	//allocate memory to covariance_matrix
	// covariance_matrix[3][3]
	covariance_matrix.resize(3);	
	for(i=0; i<covariance_matrix.size(); i++)
		covariance_matrix[i].resize(3);

	double x = 0;
	double y = 0;
	double CoVarSumXX = 0.0;
	double CoVarSumXY = 0.0;
	double CoVarSumXZ = 0.0;
	double CoVarSumYX = 0.0;
	double CoVarSumYY = 0.0;
	double CoVarSumYZ = 0.0;
	double CoVarSumZX = 0.0;
	double CoVarSumZY = 0.0;
	double CoVarSumZZ = 0.0;

	eval_meanX();
	eval_meanY();
	eval_meanZ();

	//CoVarMatrix
	for (i=0; i<data.size(); i++) 
	{
	  x = data[i].x - meanx;
	  y = data[i].x - meanx;
	  CoVarSumXX += x*y;

	  x = data[i].x - meanx;
	  y = data[i].y - meany;
	  CoVarSumXY += x*y;

	  x = data[i].x - meanx;
	  y = data[i].z - meanz;
	  CoVarSumXZ += x*y;

	  x = data[i].y - meany;
	  y = data[i].x - meanx;
	  CoVarSumYX += x*y;

	  x = data[i].y - meany;
	  y = data[i].y - meany;
	  CoVarSumYY += x*y;

	  x = data[i].y - meany;
	  y = data[i].z - meanz;
	  CoVarSumYZ += x*y;

	  x = data[i].z - meanz;
	  y = data[i].x - meanx;
	  CoVarSumZX += x*y;

	  x = data[i].z - meanz;
	  y = data[i].y - meany;
	  CoVarSumZY += x*y;

	  x = data[i].z - meanz;
	  y = data[i].z - meanz;
	  CoVarSumZZ += x*y;
	}

	double size = (double)(data.size()-1);

	CoVarSumXX /= size;	CoVarSumXY /= size;	CoVarSumXZ /= size;
	CoVarSumYX /= size;	CoVarSumYY /= size;	CoVarSumYZ /= size;
	CoVarSumZX /= size;	CoVarSumZY /= size;	CoVarSumZZ /= size;

	covariance_matrix[0][0] = CoVarSumXX; 	covariance_matrix[0][1] = CoVarSumXY; 	covariance_matrix[0][2] = CoVarSumXZ;
	covariance_matrix[1][0] = CoVarSumYX; 	covariance_matrix[1][1] = CoVarSumYY; 	covariance_matrix[1][2] = CoVarSumYZ;
	covariance_matrix[2][0] = CoVarSumZX; 	covariance_matrix[2][1] = CoVarSumZY; 	covariance_matrix[2][2] = CoVarSumZZ;
}

void PCA::eval_PCA(void)
{
	if(data.empty())return;
	
	//evaluate the means
	eval_meanX();
	eval_meanY();
	eval_meanZ();
	
	originalmeanx = meanx;
	originalmeany = meany;
	originalmeanz = meanz;

	int i;

	//substract the mean from our data
	for (i=0; i<data.size(); i++) 
	{
		data[i].x -= meanx;
		data[i].y -= meany;
		data[i].z -= meanz;
	}

	//evaluate the covariance matrix
	eval_covariance_matrix();

/*	cout<<covariance_matrix[0][0]<<" "<<covariance_matrix[0][1]<<" "<<covariance_matrix[0][2]<<endl;
	cout<<covariance_matrix[1][0]<<" "<<covariance_matrix[1][1]<<" "<<covariance_matrix[1][2]<<endl;
	cout<<covariance_matrix[2][0]<<" "<<covariance_matrix[2][1]<<" "<<covariance_matrix[2][2]<<endl;
*/
	//evaluate eigenvectors
	eval_eigenvectors();

	//eval_aligned_data();
		
/*	cout<<endl<<endl<<"Mean:"<<endl;
	cout<<"meanX: "<<originalmeanx<<"	meanY: "<<originalmeany<<"	meanZ: "<<originalmeanz<<endl;
	cout<<endl;
	cout<<"Eigenvectors:"<<endl;
	float size;
	for (i=0; i<eigenvectors.size(); i++)
	{		
		cout<<"x: "<<eigenvectors[i].x<<"	y: "<<eigenvectors[i].z<<"	z: "<<eigenvectors[i].z<<"	v: "<<eigenvectors[i].w<<endl;
	}
	cout<<endl;*/

/*	//add the original mean from our data
	for (i=0; i<data.size(); i++) 
	{
		data[i].x += originalmeanx;
		data[i].y += originalmeany;
		data[i].z += originalmeanz;
	}
*/
}
void PCA::eval_scaled_eigensvectors(float xscale, float yscale, float zscale)
{
	if(data.empty())return;
	eval_boundingbox(1,1,1);

	//float eigenxsize = eigenvectors[0].length();
	//float eigenysize = eigenvectors[1].length();
	//float eigenzsize = eigenvectors[2].length();
	
	float xsize = (bounding_box[1].x-bounding_box[4].x);
	float ysize = (bounding_box[1].y-bounding_box[4].y);
	float zsize = (bounding_box[1].z-bounding_box[4].z);

	xsize /= 2.0;
	ysize /= 2.0;
	zsize /= 2.0;
	
	/*
	printf("scales: %f, %f, %f\n", xscale, yscale, zscale);
	printf("sizes: %f, %f, %f\n", xsize, ysize, zsize);
	//printf("eigensizes: %f, %f, %f\n", eigenxsize, eigenysize, eigenzsize);

	xsize *= xscale;
	ysize *= yscale;
	zsize *= zscale;
*/
	//printf("bbsizes: %f, %f, %f\n", xsize, ysize, zsize);
	
	/*vector<float> sizes;
	sizes.push_back(xsize);
	sizes.push_back(ysize);
	sizes.push_back(zsize);
	sort(sizes.begin(), sizes.end());
	xsize = sizes[0];
	ysize = sizes[1];
	zsize = sizes[2];*/

	/*xsize = eigenvectors[0].length();
	ysize = eigenvectors[1].length();
	zsize = eigenvectors[2].length();
	
	printf("eigenveclength: %f, %f, %f\n", xsize, ysize, zsize);*/

	/*xsize = eigenvectors[0].w*xscale;
	ysize = eigenvectors[1].w*yscale;
	zsize = eigenvectors[2].w*zscale;
	
	printf("scale: %f, %f, %f\n", xscale, yscale, zscale);
	printf("eigenvals: %f, %f, %f\n", xsize, ysize, zsize);*/

	for (int i=0; i<eigenvectors.size(); i++)
	{		
		if(i==0)
		{
			//eigenvectors[i].normalize();
			eigenvectors[i].x *= xsize;
			eigenvectors[i].y *= xsize;
			eigenvectors[i].z *= xsize;
		}
		else if(i==1)
		{
			//eigenvectors[i].normalize();
			eigenvectors[i].x *= ysize;
			eigenvectors[i].y *= ysize;
			eigenvectors[i].z *= ysize;			
		}
		else if(i==2)
		{
			//eigenvectors[i].normalize();
			eigenvectors[i].x *= zsize;
			eigenvectors[i].y *= zsize;
			eigenvectors[i].z *= zsize;		
		}		
	}
}
void PCA::eval_eigenvectors(void)
{
	long double	temp_array[3*(3+1)/2];
	long double	temp_vectorarray[9];

	int helper=0;

	temp_array[0] = (long double) covariance_matrix[0][0];
	temp_array[1] = (long double) covariance_matrix[1][0];
	temp_array[2] = (long double) covariance_matrix[1][1];
	temp_array[3] = (long double) covariance_matrix[2][0];
	temp_array[4] = (long double) covariance_matrix[2][1];
	temp_array[5] = (long double) covariance_matrix[2][2];

	eigen(1, 3, temp_array, temp_vectorarray);

	eigenvectors.push_back( Vector(temp_vectorarray[0], temp_vectorarray[1], temp_vectorarray[2], temp_array[0]) );
	eigenvectors.push_back( Vector(temp_vectorarray[3], temp_vectorarray[4], temp_vectorarray[5], temp_array[2]) );
	eigenvectors.push_back( Vector(temp_vectorarray[6], temp_vectorarray[7], temp_vectorarray[8], temp_array[5]) );
}

void PCA::eval_aligned_data(void)
{
	aligned_data.resize(data.size());

    for (int r=0; r<data.size();r++) 
	{
        for (int c=0;c<eigenvectors.size();c++) 
		{
			aligned_data[r][c] = 0.0;

            for (int i=0;i<3;i++) 
			{
				aligned_data[r][c] += data[r][i]*eigenvectors[c][i];
			}
        }
    }

/*	{
	aligned_data2.resize(data.size());
    for (int r=0; r<aligned_data.size();r++) 
	{
        for (int c=0;c<eigenvectors.size();c++) 
		{
			aligned_data2[r][c] = 0.0;

            for (int i=0;i<3;i++) 
			{
				aligned_data2[r][c] += aligned_data[r][i]*eigenvectors[i][c];
			}
        }
    }
	}
*/
}
void PCA::eval_boundingbox(float xscale, float yscale, float zscale)
{
	if(data.empty())return;
	eval_aligned_data();
	
	float mins[3], maxs[3];

	//bounding box stuff
	mins[0] = maxs[0] = aligned_data[0][0];
	mins[1] = maxs[1] = aligned_data[0][1];
	mins[2] = maxs[2] = aligned_data[0][2];

    for (int r=0; r<aligned_data.size();r++) 
	{
		mins[0] = min(mins[0], aligned_data[r][0]);
		mins[1] = min(mins[1], aligned_data[r][1]);
		mins[2] = min(mins[2], aligned_data[r][2]);	
		maxs[0] = max(maxs[0], aligned_data[r][0]);
		maxs[1] = max(maxs[1], aligned_data[r][1]);
		maxs[2] = max(maxs[2], aligned_data[r][2]);	
	}

	float t0 = 0.0;
	vector<Vector> bounding_box_temp;

	//FRONT
	bounding_box_temp.push_back( Vector(maxs[0] + t0, mins[1] - t0, maxs[2] + t0) );
	bounding_box_temp.push_back( Vector(maxs[0] + t0, maxs[1] + t0, maxs[2] + t0) );
	bounding_box_temp.push_back( Vector(mins[0] - t0, maxs[1] + t0, maxs[2] + t0) );
	bounding_box_temp.push_back( Vector(mins[0] - t0, mins[1] - t0, maxs[2] + t0) );

	//BACK
	bounding_box_temp.push_back( Vector(mins[0] - t0, mins[1] - t0, mins[2] - t0) );
	bounding_box_temp.push_back( Vector(mins[0] - t0, maxs[1] + t0, mins[2] - t0) );
	bounding_box_temp.push_back( Vector(maxs[0] + t0, maxs[1] + t0, mins[2] - t0) );
	bounding_box_temp.push_back( Vector(maxs[0] + t0, mins[1] - t0, mins[2] - t0) );
	
	{
	bounding_box.resize(bounding_box_temp.size());
    for (int r=0; r<bounding_box_temp.size();r++) 
	{
		bounding_box[r] = bounding_box_temp[r];
		
        /*for (int c=0;c<eigenvectors.size();c++) 
		{
			bounding_box[r][c] = 0.0;

            for (int i=0;i<3;i++) 
			{
				bounding_box[r][c] += bounding_box_temp[r][i]*eigenvectors[i][c];
			}
        }*/
	}
	}
}

void PCA::display(void)
{
	Vector size = 10.0;

	//display our PCs
	//================
	glBegin(GL_LINES);
	glColor3f(1.0, 1.0, 0.0); 
	for(int j=0; j<eigenvectors.size(); j++)
	{
		glVertex3f(size.x*eigenvectors[j].x, 
				   size.y*eigenvectors[j].y, 
				   size.z*eigenvectors[j].z);

		glVertex3f(-size.x*eigenvectors[j].x, 
				   -size.y*eigenvectors[j].y, 
				   -size.z*eigenvectors[j].z);
	}
	glEnd();

	//display bounding box of our PCs
	//================================
	//FRONT
	glColor3f(1.0, 1.0, 1.0); 
	glBegin(GL_LINE_LOOP);
		glVertex3f(bounding_box[0].x, bounding_box[0].y, bounding_box[0].z);
		glVertex3f(bounding_box[1].x, bounding_box[1].y, bounding_box[1].z);
		glVertex3f(bounding_box[2].x, bounding_box[2].y, bounding_box[2].z);
		glVertex3f(bounding_box[3].x, bounding_box[3].y, bounding_box[3].z);
	glEnd();

	//LEFT
	glBegin(GL_LINE_LOOP);
		glVertex3f(bounding_box[3].x, bounding_box[3].y, bounding_box[3].z);
		glVertex3f(bounding_box[2].x, bounding_box[2].y, bounding_box[2].z);
		glVertex3f(bounding_box[5].x, bounding_box[5].y, bounding_box[5].z);
		glVertex3f(bounding_box[4].x, bounding_box[4].y, bounding_box[4].z);
	glEnd();

	//BACK
	glBegin(GL_LINE_LOOP);
		glVertex3f(bounding_box[4].x, bounding_box[4].y, bounding_box[4].z);
		glVertex3f(bounding_box[7].x, bounding_box[7].y, bounding_box[7].z);
		glVertex3f(bounding_box[6].x, bounding_box[6].y, bounding_box[6].z);
		glVertex3f(bounding_box[5].x, bounding_box[5].y, bounding_box[5].z);
	glEnd();

	//RIGHT
	glBegin(GL_LINE_LOOP);
		glVertex3f(bounding_box[0].x, bounding_box[0].y, bounding_box[0].z);
		glVertex3f(bounding_box[1].x, bounding_box[1].y, bounding_box[1].z);
		glVertex3f(bounding_box[6].x, bounding_box[6].y, bounding_box[6].z);
		glVertex3f(bounding_box[7].x, bounding_box[7].y, bounding_box[7].z);
	glEnd();

	//TOP
	glBegin(GL_LINE_LOOP);
		glVertex3f(bounding_box[1].x, bounding_box[1].y, bounding_box[1].z);
		glVertex3f(bounding_box[6].x, bounding_box[6].y, bounding_box[6].z);
		glVertex3f(bounding_box[5].x, bounding_box[5].y, bounding_box[5].z);
		glVertex3f(bounding_box[2].x, bounding_box[2].y, bounding_box[2].z);
	glEnd();

	//BOTTOM
	glBegin(GL_LINE_LOOP);
		glVertex3f(bounding_box[0].x, bounding_box[0].y, bounding_box[0].z);
		glVertex3f(bounding_box[7].x, bounding_box[7].y, bounding_box[7].z);
		glVertex3f(bounding_box[4].x, bounding_box[4].y, bounding_box[4].z);
		glVertex3f(bounding_box[3].x, bounding_box[3].y, bounding_box[3].z);
	glEnd();

}

//.............................................................................
// Subroutine to compute eigenvalues & eigenvectors of a real symmetric
// matrix, from IBM SSP manual (see p165).
// 
// 		       Ian Tickle April 1992
// 
// 		(modified by David Moss February 1993)
// 
// Eigenvalues & vectors of real symmetric matrix stored in triangular form.
// 
// Arguments:
// 
// mv = 0 to compute eigenvalues only.
// mv = 1 to compute eigenvalues & eigenvectors.
// 
// n - supplied dimension of n*n matrix.
// 
// a - supplied array of size n*(n+1)/2 containing n*n matrix in the order:
// 
// 			  0      1      2    ...
//     0    a[0]
//     1    a[1]   a[2]
//     2    a[3]   a[4]   a[5]   ...
//     ...
//     NOTE a is used as working space and is overwritten on return.
//     Eigenvalues are written into diagonal elements of a
//     i.e.  a[0]  a[2]  a[5]  for a 3*3 matrix.
// 
// r - Resultant matrix of eigenvectors stored columnwise in the same
//     order as eigenvalues.

void PCA::eigen(int mv, int n, long double* a, long double* r) const
{ 
    int i, il, ilq, ilr, im, imq, imr, ind, iq, j, l, ll, lm, lq, m, mm, mq;
  long double anorm, anrmx, cosx, cosx2, sincs, sinx, sinx2, thr, x, y;
  long double theshold = 1.0;

  if (mv)
  { for (i=1; i<n*n; i++) r[i]=0.;
    for (i=0; i<n*n; i+=n+1) r[i]=1.;
  }

/* Initial and final norms (anorm & anrmx). */
  anorm=0.;
  iq=0;
  for (i=0; i<n; i++) for (j=0; j<=i; j++)
  { if (j!=i) anorm+=a[iq]*a[iq];
    ++iq;
  }

  if (anorm>0.)
  { anorm=sqrt(2.*anorm);
    anrmx=theshold*anorm/n;

/* Compute threshold and initialise flag. */
    thr=anorm;
    do
    { thr/=n;
      do
      { ind=0;
	l=0;
	do
	{ lq=l*(l+1)/2;
	  ll=l+lq;
	  m=l+1;
	  ilq=n*l;
	  do

/* Compute sin & cos. */
	  { mq=m*(m+1)/2;
	    lm=l+mq;
	    if (fabs(a[lm])>=thr)
	    { ind=1;
	      mm=m+mq;
	      x=.5*(a[ll]-a[mm]);
	      y=-a[lm]/sqrt(a[lm]*a[lm]+x*x);
	      if (x<0.) y=-y;
	      sinx=y/sqrt(2.*(1.+(sqrt(1.-y*y))));
	      sinx2=sinx*sinx;
	      cosx=sqrt(1.-sinx2);
	      cosx2=cosx*cosx;
	      sincs=sinx*cosx;

/* Rotate l & m columns. */
	      imq=n*m;
	      for (i=0; i<n; i++)
	      { iq=i*(i+1)/2;
		if (i!=l && i!=m)
		{ if (i<m) im=i+mq;
		  else im=m+iq;
		  if (i<l) il=i+lq;
		  else il=l+iq;
		  x=a[il]*cosx-a[im]*sinx;
		  a[im]=a[il]*sinx+a[im]*cosx;
		  a[il]=x;
		}
		if (mv)
		{ ilr=ilq+i;
		  imr=imq+i;
		  x=r[ilr]*cosx-r[imr]*sinx;
		  r[imr]=r[ilr]*sinx+r[imr]*cosx;
		  r[ilr]=x;
		}
	      }

	      x=2.*a[lm]*sincs;
	      y=a[ll]*cosx2+a[mm]*sinx2-x;
	      x=a[ll]*sinx2+a[mm]*cosx2+x;
	      a[lm]=(a[ll]-a[mm])*sincs+a[lm]*(cosx2-sinx2);
	      a[ll]=y;
	      a[mm]=x;
	    }

/* Tests for completion.
   Test for m = last column. */
	  } while (++m!=n);

/* Test for l =penultimate column. */
	} while (++l!=n-1);
      } while (ind);

/* Compare threshold with final norm. */
    } while (thr>anrmx);
  }
}       // Matrix::eigen