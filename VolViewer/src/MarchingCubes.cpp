#include "MarchingCubes.h"
#include "MarchingCubesLookupTab.h"

MCubes::MCubes()
{
	tfunc_lookup = NULL;
	volobj = NULL;

	finished = 0;
	iDataSetSize = 0;
	fTargetValue = 0.0;
}

void MCubes::clear(void)
{
	tfunc_lookup = NULL;
	volobj = NULL;

	//mesh->reset();
	mesh=NULL;

	iDataSetSize = 0;
	fStepSize = 0;
	fTargetValue = 0.0;

	finished = 0;
}
void MCubes::progress(int i, int size)
{
	float prog = ( (float)(i+1)/(float)(size) )*100.0f;
	printf("\r\r Progress = %.2f", prog);
}
//fGetOffset finds the approximate point of intersection of the surface
// between two points with the values fValue1 and fValue2
GLfloat MCubes::fGetOffset(GLfloat fValue1, GLfloat fValue2, GLfloat fValueDesired)
{
	GLdouble fDelta = fValue2 - fValue1;

    if(fDelta == 0.0)
    {
            return 0.5;
    }

    return (fValueDesired - fValue1)/fDelta;
}

GLfloat MCubes::fSample(GLfloat x, GLfloat y, GLfloat z, int gaussflag)
{
	GLdouble fResult = 0.0;
	int xx,yy,zz;

	/*
	xx = x*((float)volobj->texwidth);
	yy = z*((float)volobj->texheight);
	zz = y*((float)volobj->texdepth);
	*/
	
	/*float _x = x/fStepSize;
	float _y = y/fStepSize;
	float _z = z/fStepSize;

	_x /= float(iDataSetSize);
	_y /= float(iDataSetSize);
	_z /= float(iDataSetSize);*/

	xx = x*((float)volobj->texwidth-1);
	yy = z*((float)volobj->texheight-1);
	zz = y*((float)volobj->texdepth-1);

	if(xx<0) xx =0;
	if(yy<0) yy =0;
	if(zz<0) zz =0;
	if(xx>volobj->texwidth-1) xx =volobj->texwidth-1;
	if(yy>volobj->texheight-1) yy =volobj->texheight-1;
	if(zz>volobj->texdepth-1) zz =volobj->texdepth-1;

	int index1d = ((((zz*volobj->texheight)+yy)*volobj->texwidth)+xx);	
	
	//no gassian blurring....
	//if(gaussflag==0)
	{
		//printf("hello\n");
		fResult = ((float)((r*tfunc_lookup[3*volobj->texture3d[3*index1d+0]+0])+(g*tfunc_lookup[3*volobj->texture3d[3*index1d+1]+1])+(b*tfunc_lookup[3*volobj->texture3d[3*index1d+2]+2])))/float(r+g+b);
		fResult = fResult;
		//printf("hello2\n");
	}
	/*else if (gaussflag==1)
	{
		if(gaussianvalues_map.find(index1d)!=gaussianvalues_map.end())
		{
			fResult = gaussianvalues_map[index1d];//gaussianvalues[index1d];
			helpB++;
		}
		else
		{
			double filtered_valueR=0.0;
			double filtered_valueG=0.0;
			double filtered_valueB=0.0;
			float kernX, kernY, kernZ;
			kernX=kernY=kernZ=0;
			int index=0;
			int x_iter=0;
			int y_iter=0;
			int z_iter=0;
			
			for(int i=-gaussian_kernel_radius; i<=gaussian_kernel_radius; i++)
			{
				for(int j=-gaussian_kernel_radius; j<=gaussian_kernel_radius; j++)
				{
					for(int k=-gaussian_kernel_radius; k<=gaussian_kernel_radius; k++)
					{
						kernX = xx+k;
						kernY = yy+j;
						kernZ = zz+i;
						
						if(kernX<0) kernX=0;
						if(kernX>(volobj->texwidth-1)) kernX=volobj->texwidth-1;

						if(kernY<0) kernY=0;
						if(kernY>(volobj->texheight-1)) kernY=volobj->texheight-1;

						if(kernZ<0) kernZ=0;
						if(kernZ>(volobj->texdepth-1)) kernZ=volobj->texdepth-1;

						index = 3*(((((kernZ)*volobj->texheight)+(kernY))*volobj->texwidth)+(kernX));
								
						filtered_valueR += ((float)r*(float)tfunc_lookup[(int)(3*volobj->texture3d[index+0])+0])*mask[x_iter][y_iter][z_iter];
						filtered_valueG += ((float)g*(float)tfunc_lookup[(int)(3*volobj->texture3d[index+1])+1])*mask[x_iter][y_iter][z_iter];
						filtered_valueB += ((float)b*(float)tfunc_lookup[(int)(3*volobj->texture3d[index+2])+2])*mask[x_iter][y_iter][z_iter];

						z_iter++;
					}
					
					y_iter++;
					z_iter=0;
				}

				x_iter++;
				y_iter=0;
			}
			fResult = (filtered_valueR+filtered_valueG+filtered_valueB)/(float)(r+g+b);
			gaussianvalues_map[index1d] = fResult;
		}
	}*/
	
	helpA++;
	return fResult;	
}

//vGetNormal() finds the gradient of the scalar field at a point
//This gradient can be used as a very accurate vertx normal for lighting calculations
GLvoid MCubes::vGetNormal(Vector &rfNormal, GLfloat fX, GLfloat fY, GLfloat fZ, int gaussflag)
{		
        rfNormal.x = fSample(fX-fStepSize, fY, fZ, gaussflag) - fSample(fX+fStepSize, fY, fZ, gaussflag);
        rfNormal.y = fSample(fX, fY-fStepSize, fZ, gaussflag) - fSample(fX, fY+fStepSize, fZ, gaussflag);
        rfNormal.z = fSample(fX, fY, fZ-fStepSize, gaussflag) - fSample(fX, fY, fZ+fStepSize, gaussflag);
		rfNormal.normalize();
}

//vMarchCube1 performs the Marching Cubes algorithm on a single cube
GLvoid MCubes::vMarchCube(GLfloat x, GLfloat y, GLfloat z, GLfloat fScale, int gaussflag)
{
		extern GLint aiCubeEdgeFlags[256];
        extern GLint a2iTriangleConnectionTable[256][16];

        GLint iCorner, iVertex, iVertexTest, iEdge, iTriangle, iFlagIndex, iEdgeFlags;
        GLfloat fOffset;
        GLfloat afCubeValue[8];
		bool afCubeState[8];
        Vector asEdgeVertex[12];
        Vector asEdgeNorm[12];

		int xx,yy,zz;
		/*float _x = x/fStepSize;
		float _y = y/fStepSize;
		float _z = z/fStepSize;

		_x /= float(iDataSetSize);
		_y /= float(iDataSetSize);
		_z /= float(iDataSetSize);*/
		
		xx = x*((float)volobj->texwidth);
		yy = z*((float)volobj->texheight);
		zz = y*((float)volobj->texdepth);

		if(xx<0) xx =0;
		if(yy<0) yy =0;
		if(zz<0) zz =0;
		if(xx>volobj->texwidth-1) xx =volobj->texwidth-1;
		if(yy>volobj->texheight-1) yy =volobj->texheight-1;
		if(zz>volobj->texdepth-1) zz =volobj->texdepth-1;
		
        //Make a local copy of the values at the cube's corners
        for(iVertex = 0; iVertex < 8; iVertex++)
        {
            afCubeValue[iVertex] = fSample(x + a2fVertexOffset[iVertex][0]*fScale,
                                               y + a2fVertexOffset[iVertex][1]*fScale,
                                               z + a2fVertexOffset[iVertex][2]*fScale, gaussflag);   
		}

        //Find which vertices are inside of the surface and which are outside
        iFlagIndex = 0;
        for(iVertexTest = 0; iVertexTest < 8; iVertexTest++)
        {
			afCubeState[iVertexTest] = false;
			if(afCubeValue[iVertexTest] <= fTargetValue) //iFlagIndex |= 1<<iVertexTest;
			{
					iFlagIndex |= 1<<iVertexTest;
					afCubeState[iVertexTest] = true;
			}
        }
	
        //Find which edges are intersected by the surface
        iEdgeFlags = aiCubeEdgeFlags[iFlagIndex];

		/*
		int index1d = ((((zz*volobj->texheight)+yy)*volobj->texwidth)+xx);
		int helper=0;
		for(int itervert = 0; itervert < 8; itervert++)
		{
			if(afCubeState[itervert] == true) helper++;
		}
		
		//volobj->texture3d[3*index1d+0] = volobj->texture3d[3*index1d+1] = volobj->texture3d[3*index1d+2] =0;
		//printf("helper: %d\n",helper);
		if(helper==8)
		{
			volobj->texture3d[3*index1d+0] = 255;
		}
		*/
		/*else if(helper==8)
		{
			volobj->texture3d[3*index1d+1] = 255;
		}
		else
		{
			volobj->texture3d[3*index1d+0] = 255;
		}*/
		
		//If the cube is entirely inside or outside of the surface, then there will be no intersections
        if(iEdgeFlags == 0) 
        {
                return;
        }
		
		int face_index=0;
		Face f;
		Vector vert;
		set<Vector>::const_iterator iter_set;

        //Find the point of intersection of the surface with each edge
        for(iEdge = 0; iEdge < 12; iEdge++)
        {
                //if there is an intersection on this edge
                if(iEdgeFlags & (1<<iEdge))
                {			 
						fOffset = fGetOffset(afCubeValue[ a2iEdgeConnection[iEdge][0] ], 
                                                     afCubeValue[ a2iEdgeConnection[iEdge][1] ], fTargetValue);

						asEdgeVertex[iEdge].x = x + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][0]  +  fOffset * a2fEdgeDirection[iEdge][0]) * fScale;
                        asEdgeVertex[iEdge].y = y + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][1]  +  fOffset * a2fEdgeDirection[iEdge][1]) * fScale;
                        asEdgeVertex[iEdge].z = z + (a2fVertexOffset[ a2iEdgeConnection[iEdge][0] ][2]  +  fOffset * a2fEdgeDirection[iEdge][2]) * fScale;					
                        
						//asEdgeNorm[iEdge] = Vector(0,1,0);
						vGetNormal(asEdgeNorm[iEdge], asEdgeVertex[iEdge].x, asEdgeVertex[iEdge].y, asEdgeVertex[iEdge].z, gaussflag);
				}
        }

		//Save the resulting mesh->  There can be up to five triangles per cube
        for(iTriangle = 0; iTriangle < 5; iTriangle++)
        {
                if(a2iTriangleConnectionTable[iFlagIndex][3*iTriangle] < 0) break;
				
 				for(iCorner = 0; iCorner < 3; iCorner++)
                {
						iVertex = a2iTriangleConnectionTable[iFlagIndex][3*iTriangle+iCorner];																
						//asEdgeVertex[iVertex].round(5);
						
						vert = asEdgeVertex[iVertex];
						vert *= (float)volobj->maxres;
						vert -= (float)volobj->maxres/2.0;
						vert.w = mesh->vert.size();

						iter_set = verts_set.find(vert);
						
						//if we dont find the vertex
						if(iter_set==verts_set.end())
						{
							//add it to the mesh, and update
							//everything accordingly...
							mesh->vert.push_back(vert);
							mesh->vertnorm.push_back(asEdgeNorm[iVertex]);
							mesh->vertcol.push_back(Vector(0.5f, 0.5f, 0.7f, 1.0f));
							verts_set.insert(verts_set.end(), mesh->vert[mesh->vert.size()-1]);
							f.vertindex[iCorner] = mesh->vert.size()-1;
							f.vert[iCorner] = &mesh->vert[mesh->vert.size()-1];
						}
						else
						{
							//otherwise just update mesh with refence
							//to the vertex that already exists...
							f.vertindex[iCorner] = (*iter_set).w;
							f.vert[iCorner] = &mesh->vert[(*iter_set).w];
						}
				}

				mesh->localfaces.push_back(f);
				mesh->faces.push_back(&mesh->localfaces[mesh->localfaces.size()-1]);
        }
}
GLvoid MCubes::vMarchingCubes(bool* channels, int qual, int value, unsigned char* tfunc, float size, float sigma)
{
	if(volobj->texwidth==1 || volobj->texheight==1 || volobj->texdepth==1) return;

	//setup our lookup table function
	tfunc_lookup = tfunc;

	int gaussflag =0;
	gaussian_kernel_radius = (size-1)/2;	

	vector<bool> chans;
	chans.resize(3);

	//what channels are we working on?
	chans[0] = selected_channels[0] = channels[0];
	chans[1] = selected_channels[1] = channels[1];
	chans[2] = selected_channels[2] = channels[2];

	//apply our guassian mask...
/*	Filter filt;	
	filt.volobj = volobj;
	filt.apply_gaussian_filter_decomp(gaussian_kernel_radius, sigma, 1, chans);
	filt.volobj = NULL;
*/
	//lazy way to get the selected channel....
	r=g=b=0;
	if(selected_channels[0]==true) r=1;
	if(selected_channels[1]==true) g=1;
	if(selected_channels[2]==true) b=1;

	//work out our min_res, this will be our marching cubes max space
	/*int min_res = 999;
	if(min_res>=volobj->texwidth)  min_res = volobj->texwidth;
	if(min_res>=volobj->texheight) min_res = volobj->texheight;
	if(min_res>=volobj->texdepth)  min_res = volobj->texdepth;*/

	//setup our isovalues and quality
	iDataSetSize = (float)volobj->texwidth*((float)qual/100.0);
	fStepSize = 1.0/(float)(iDataSetSize);
	fTargetValue =  value;
	
	progressbar = new Q3ProgressDialog("Computing Isosurface...", "Cancel", iDataSetSize, 0, "progress", TRUE);		
	progressbar->setCaption("Please Wait");

//	gaussianvalues = new float[volobj->texwidth*volobj->texheight*volobj->texdepth];
//	for(int i=0; i<volobj->texwidth*volobj->texheight*volobj->texdepth; i++)
//		gaussianvalues[i] = -1;

	int setsize[3];
	int mapsize[3];
	
	helpA=0;
	helpB=0;

	int helper=0;
	int helper2=0;
	
	int vertsize=0;
	
	for(GLint iX = 0; iX < iDataSetSize; iX++)
	{		
		//progress(iX, iDataSetSize);
		progressbar->setProgress(iX);
		if (progressbar->wasCancelled())
			break;

		for(GLint iY = 0; iY < iDataSetSize; iY++)
		{
			for(GLint iZ = 0; iZ < iDataSetSize; iZ++)
			{
				vMarchCube(iX*fStepSize, iY*fStepSize, iZ*fStepSize, fStepSize, gaussflag);
			}
		}
	
		if(vertsize!=verts_set.size())
		{
			vertsize=verts_set.size();

			if(helper==2)
			{
				//printf("verts_set size1 %d\n", verts_set.size());

				set<Vector>::iterator current = verts_set.begin();
				advance(current, (int)setsize[0]);
				verts_set.erase(verts_set.begin(), current );

				//printf("verts_set size2 %d\n", verts_set.size());

				helper=0;
			}

			//for(int i=0; i<3; i++)
			//	printf("%f\n", setsize[i].x);
			
			setsize[helper] = verts_set.size();
			helper++;
		}
		//if(gaussianvalues_map.size())
		/*{
			if(helper2==3)
			{
				//printf("gaussianvalues_map size1 %d\n", gaussianvalues_map.size());

				map<int, float>::iterator current2 = gaussianvalues_map.begin();
				advance(current2, (int)mapsize[0]);
				gaussianvalues_map.erase(gaussianvalues_map.begin(), current2 );

				//printf("gaussianvalues_map size2 %d\n", gaussianvalues_map.size());

				helper2=0;
			}
			
			//printf("helper2 / mapsize: %d, %d\n",helper2,gaussianvalues_map.size());

			mapsize[helper2] = gaussianvalues_map.size();
			helper2++;
		}*/	
	}
	
	progressbar->setProgress(iDataSetSize);
	delete progressbar;
	progressbar = NULL;

	gaussianvalues_map.clear();
//	delete[] gaussianvalues;
//	gaussianvalues=NULL;

	verts_set.clear();
	mesh->nv = mesh->vert.size();
	mesh->nf = mesh->localfaces.size();
	printf("nv: %d, nf: %d\n", mesh->nv, mesh->nf);

	printf("b/a: %f\n", (float)helpB/(float)helpA);
}

//vMarchingCubes iterates over the entire dataset, calling vMarchCube on each cube
GLvoid MCubes::vMarchingCubes(bool* channels, int qual, int value, unsigned char* tfunc)
{
	if(volobj->texwidth==1 || volobj->texheight==1 || volobj->texdepth==1) return;

	//setup our lookup table function
	tfunc_lookup = tfunc;

	//what channels are we working on?
	selected_channels[0] = channels[0];
	selected_channels[1] = channels[1];
	selected_channels[2] = channels[2];

	//lazy way to get the selected channel....
	r=g=b=0;
	if(selected_channels[0]==true) r=1;
	if(selected_channels[1]==true) g=1;
	if(selected_channels[2]==true) b=1;

	//work out our min_res, this will be our marching cubes max space
	/*int min_res = 999;
	if(min_res>=volobj->texwidth)  min_res = volobj->texwidth;
	if(min_res>=volobj->texheight) min_res = volobj->texheight;
	if(min_res>=volobj->texdepth)  min_res = volobj->texdepth;*/

	//setup our isovalues and quality
	iDataSetSize = (float)volobj->texwidth*((float)qual/100.0);
	fStepSize = 1.0/(float)(iDataSetSize);
	fTargetValue =  value;
	
	progressbar = new Q3ProgressDialog("Computing Isosurface...", "Cancel", iDataSetSize, 0, "progress", TRUE);		
	progressbar->setCaption("Please Wait");

	int gaussflag = 0;
	int setsize[3];
	int helper=0;
	int vertsize=0;
	
	for(GLint iX = 0; iX < iDataSetSize; iX++)
	{		
		//progress(iX, iDataSetSize);
		progressbar->setProgress(iX);
		if (progressbar->wasCancelled())
			break;

		for(GLint iY = 0; iY < iDataSetSize; iY++)
		{
			for(GLint iZ = 0; iZ < iDataSetSize; iZ++)
			{
				vMarchCube(iX*fStepSize, iY*fStepSize, iZ*fStepSize, fStepSize, gaussflag);
			}
		}
		if(vertsize!=verts_set.size())
		{
			vertsize=verts_set.size();

			if(helper==2)
			{
				//printf("verts_set size1 %d\n", verts_set.size());

				set<Vector>::iterator current = verts_set.begin();
				advance(current, (int)setsize[0]);
				verts_set.erase(verts_set.begin(), current );

				//printf("verts_set size2 %d\n", verts_set.size());

				helper=0;
			}

			//for(int i=0; i<3; i++)
			//	printf("%f\n", setsize[i].x);
			
			setsize[helper] = verts_set.size();
			helper++;
		}
	}
	
	progressbar->setProgress(iDataSetSize);
	delete progressbar;
	progressbar = NULL;

	verts_set.clear();
	mesh->nv = mesh->vert.size();
	mesh->nf = mesh->localfaces.size();

	tfunc_lookup = NULL;
	
//	printf("nv: %d, nf: %d\n", mesh->nv, mesh->nf);
}