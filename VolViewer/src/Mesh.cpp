/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "Mesh.h"

bool _FACE_NORM, _VERT_NORM;
bool M_VERTS, M_NORMS, M_COLOURS, M_TEXCOORDS;

Mesh::Mesh()
{
	glextensions=NULL;
	vbo1_name = vbo2_name = vbo3_name = 0;
	nv = nf = 0;
	has_vbo=false;

	vbo_indices = NULL;
	vbo_verts = vbo_norms = NULL;
}

Mesh::Mesh(const Mesh& m)
{
	*this = m;
}

Mesh::~Mesh()
{
 //   glextensions->glDeleteBuffersARB(1, &vbo1_name);
 //   glextensions->glDeleteBuffersARB(1, &vbo2_name);
	
	reset();
}

void Mesh::reset(void)
{
	glextensions=NULL;
	vbo1_name = vbo2_name = vbo3_name = 0;
	nv = nf = 0;
	has_vbo=false;

	vert.clear();
	vertnorm.clear();
	vertcol.clear();
	texCoords.clear();
	faces.clear();
	localfaces.clear();
}
void Mesh::set_numverts(int n)
{
	nv = n;

	vert.clear();
	vertnorm.clear();
	vertcol.clear();
	texCoords.clear();

	if(nv==0) return;

	vert.resize(nv);
	vertnorm.resize(nv);
	vertcol.resize(nv);
	texCoords.resize(nv);

	if(vert.empty())
	{
		nv=0;
		printf("ERROR: ALLOCATING MEMORY, Mesh::set_numverts()\n"); 
	}
	if(vertnorm.empty())
	{
		nv=0;
		printf("ERROR: ALLOCATING MEMORY, Mesh::set_numverts()\n"); 
	}
	if(vertcol.empty())
	{
		nv=0;
		printf("ERROR: ALLOCATING MEMORY, Mesh::set_numverts()\n"); 
	}	
	if(texCoords.empty())
	{
		nv=0;
		printf("ERROR: ALLOCATING MEMORY, Mesh::set_numverts()\n"); 
	}
}

void Mesh::set_numfaces(int n)
{	
	nf = n;

	faces.clear();
	localfaces.clear();

	if(nf==0) return;

	faces.resize(nf);
	localfaces.resize(nf);
	
	if(faces.empty())
	{
		nf=0;
		printf("ERROR: ALLOCATING MEMORY, Mesh::set_numfaces()\n"); 
	}
	if(localfaces.empty())
	{
		nf=0;
		printf("ERROR: ALLOCATING MEMORY, Mesh::set_numfaces()\n"); 
	}
} 
bool Mesh::load_OBJ_data(const char *Filename)
{
	GLfloat v0, v1, v2;
	GLint f0, f1, f2;
	
	v0 = v1 = v2 = 0;
	f0 = f1 = f2 = 0;

	GLint i[10];

	FILE* infile = fopen(Filename, "rt");

	if (infile==NULL)
	{
		cerr<<"ERROR: Opening File: "<<Filename<<endl;
		system("pause");
		exit(1);
	}

	cout<<"Opening: "<<Filename<<endl;

	vector<Vector> vertices;
	vector<Vector> normals;
	vector<Vector> texcoords;
	vector<Vector> colours;

	vector< vector<GLint> > indices;
	
	char buf[81];
	bool done = false;
	
	while (!done)
	{
	fgets(buf, 80, infile);
	if (feof(infile))
	{
	  done = true;
	}
	else
	{
	  switch (buf[0])
	  {	      
		case 'v':
	        {
			  if (sscanf(buf, "v %f %f %f", &v0, &v1, &v2) != 3)
	          {
	            break;
	          }
			  vertices.push_back(Vector(v0,v1,v2));
	        }
	        break;
	      case 'vn':
	        {
	          if (sscanf(buf, "vn %f %f %f", &v0, &v1, &v2) != 3)
	          {
	            break;
	          }
	          normals.push_back(Vector(v0,v1,v2)); 
	        }
	        break;
	      case 'vc':
	        {
	          if (sscanf(buf, "vn %f %f %f", &v0, &v1, &v2) != 3)
	          {
	            break;
	          }
	          colours.push_back(Vector(v0,v1,v2)); 
	        }
	      case 'vt':
	        {
	          if (sscanf(buf, "vt %f %f", &v0, &v1) != 2)
	          {
	            break;
			  }
	          texcoords.push_back(Vector(v0,v1)); 
	        }
	        break;
	      case 'f':
	        {
			  // We only support up to 3 vertices per face
			  int faceVerts;
	          if ((faceVerts = sscanf(buf, "f %i %i %i", i+0, i+1, i+2) != 3))	          				
	          if ((faceVerts = sscanf(buf, "f %i/%i/%i %i/%i/%i %i/%i/%i", i+0, i+3, i+3, i+1, i+3, i+3, i+2, i+3, i+3) != 9))	          				
		          if ((faceVerts = sscanf(buf, "f %i//%i %i//%i %i//%i", i+0, i+3, i+1, i+3, i+2, i+3) != 6))	          				
		          if ((faceVerts = sscanf(buf, "f %i/%i/ %i/%i/ %i/%i/", i+0, i+3, i+1, i+3, i+2, i+3) != 6))	          				
			          if ((faceVerts = sscanf(buf, "f %i// %i// %i//", i+0, i+1, i+2) != 3))	          				
						  break;

			  indices.push_back(vector<GLint>());
	          for (int x=0; x<3; x++)
				indices[indices.size()-1].push_back(i[x]-1);
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

	// Okay, got all the data.  Transfer it to the arrays for easy access.
	set_data(vertices, normals, colours, texcoords, indices);

	//init_vbo();
	return true;
}

bool Mesh::load_NET_data(const char *Filename)
{
	GLfloat v0, v1, v2;
	
	v0, v1, v2 = 0;

	GLint i[10];

	FILE* infile = fopen(Filename, "rt");
	
	if (infile==NULL)
	{
		cerr<<"ERROR Opening File: "<<Filename<<endl;
		system("pause");
		exit(1);
	}

	cout<<"Opening: "<<Filename<<endl;

	vector<Vector> vertices;
	vector<Vector> normals;
	vector<Vector> texcoords;
	vector<Vector> colours;

	vector< vector<GLint> > indices;
	
	char buf[81];
	bool done = false;
	
	while (!done)
	{
	fgets(buf, 80, infile);
	
	if (feof(infile))
	{
	  done = true;
	}
	else
	{
	  switch (buf[0])
	  {
	      case 'v':
	        {
			  if (sscanf(buf, "v %f %f %f", &v0, &v1, &v2) != 3)
	          {
	            return false;
	          }
			  vertices.push_back(Vector(v0,v1,v2));
	        }
	        break;
	      case 'n':
	        {
	          if (sscanf(buf, "n %f %f %f", &v0, &v1, &v2) != 3)
	          {
	            return false;
	          }
	          normals.push_back(Vector(v0,v1,v2)); 
	        }
	        break;
	      case 'c':
	        {
	          if (sscanf(buf, "c %f %f %f", &v0, &v1, &v2) != 3)
	          {
	            return false;
	          }
			  colours.push_back(Vector(v0,v1,v2)); 
	        }
	        break;
	      case 't':
	        {
	          if (sscanf(buf, "t %f %f", &v0, &v1) != 2)
	          {
	            return false;
			  }
	          texcoords.push_back(Vector(v0,v1)); 
	        }
	        break;
	      case 'f':
	        {
	          // We only support up to ten vertices per face so we don't really
	          // have to make this arbitrarily large.  But they have to be triangles, at least.
	          GLint faceVerts;
	          if ((faceVerts = sscanf(buf, "f %d %d %d %d %d %d %d %d %d %d", 
	          						  i+0, i+1, i+2, i+3, i+4, i+5, i+6, i+7, i+8, i+9)) < 3)
	          {
	            return false;
	          }
	          indices.push_back(vector<GLint>());
	          for (int x=0; x<3; x++)
	          {
	            indices[indices.size()-1].push_back(i[x]);
	          }
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
	
	// Okay, got all the data.  Transfer it to the arrays for easy access.
	set_data(vertices, normals, colours, texcoords, indices);
	return true;
}

void Mesh::set_data(const vector<Vector>& newVerts,
					  const vector<Vector>& newNorms,
					  const vector<Vector>& newColours,
					  const vector<Vector>& newTexcoords,
					  const vector<vector<int> >& newFaces)
{
  int x, y;

	printf("nv: %d nf: %d \n", newVerts.size(), newFaces.size());

  // Copy the vertices in.
  nv = newVerts.size();
  set_numverts(nv);
  
  if(newNorms.size())		M_NORMS = true;
  if(newNorms.size()==newVerts.size())	_VERT_NORM = true;
  if(newColours.size())		M_COLOURS = true;
  if(newTexcoords.size())	M_TEXCOORDS = true;
  
  for (x = 0; x<nv; x++)
  {
    vert[x].x = newVerts[x].x;
    vert[x].y = newVerts[x].y;
    vert[x].z = newVerts[x].z;
/*
	if(M_NORMS && _VERT_NORM)
	{
		vertnorm[x].x = newNorms[x].x;
		vertnorm[x].y = newNorms[x].y;
		vertnorm[x].z = newNorms[x].z;
	}
		else
		{
			vertnorm[x].x = 0.0;
			vertnorm[x].y = 0.0;
			vertnorm[x].z = 0.0;
		
		}
*/
	if(M_COLOURS)
	{
		//cout<<newColours[x].x<<" "<<newColours[x].y<<" "<<newColours[x].z<<endl;

		vertcol[x].x = newColours[x].x;
		vertcol[x].y = newColours[x].y;
		vertcol[x].z = newColours[x].z;
//		vertcol[x].w = newColours[x].w; 
	}
		else
		{
			vertcol[x].x = 1.0;
			vertcol[x].y = 1.0;
			vertcol[x].z = 1.0;
//			vertcol[x].w = 1.0;
		}
/*
	if(M_TEXCOORDS)
	{
	   texCoords[x].x = newTexcoords[x].x;
	   texCoords[x].y = newTexcoords[x].y;
	}
		else
		{	
		   texCoords[x].x = 0.0;
		   texCoords[x].y = 0.0;	
		}*/
   }

  // Copy the faces in.
  nf = newFaces.size();
  set_numfaces(nf);
  
  for (x = 0; x<nf; x++)
  {    
    
	//cout<<newFaces[x].size()<<endl;
	 
	for (y = 0; y<3; y++)
    {
	  localfaces[x].vert[y] = &vert[newFaces[x][y]]; 
   	  localfaces[x].vertindex[y] = newFaces[x][y];
    }
    
    faces[x] = &localfaces[x];
  }
}
bool Mesh::save_NET_data(const char *Filename)
{
	if(!nv) return false;
	if(!nf) return false;
 
	cout<<"	MESH: Saving "<<Filename<<"...";

	ofstream fout;										// Here we create an ofstream and we will call it "fout", like "cout".
														
	fout.open(Filename);								// We call a function from our ofstream object called open().

	int i;												// We just tell open() the file name we want to open or create.

	for (i = 0; i<nv; i++)
  	{
	    fout << "v " << vert[i].x<<" "<< vert[i].y <<" "<<vert[i].z<< endl ;
	    fout << "t " << texCoords[i].x<<" "<< texCoords[i].y<< endl ;
	    fout << "n " << vertnorm[i].x<<" "<< vertnorm[i].y <<" "<<vertnorm[i].z<< endl ;
	    fout << "c " << vertcol[i].x<<" "<< vertcol[i].y <<" "<<vertcol[i].z<< endl ;
  	}
  	
    for (i = 0; i<nf; i++)
  	{    
		fout << "f "<<localfaces[i].vertindex[0]<<" "<<localfaces[i].vertindex[1]<<" "<<localfaces[i].vertindex[2]<<endl;
  	}
															// If the file doesn't exist, it will create it for us once we start writing to it.																									
																												// close() closes the file for us. (very important)
	fout.close();										// We must always close the file after we are done with it.  It frees memory and make's it so we can access it again.

	cout<<" done..."<<endl;

	return true;
}
bool Mesh::save_MARKCANVAS_data(const char *Filename)
{
	if(!nv) return false;
	if(!nf) return false;

	cout<<nv<<endl;
	cout<<nf<<endl;

	cout<<"	MESH: Saving "<<Filename<<"...";

	ofstream fout;										// Here we create an ofstream and we will call it "fout", like "cout".
														
	fout.open(Filename);								// We call a function from our ofstream object called open().

	int i;												// We just tell open() the file name we want to open or create.
	
	fout<<"Nodes:  "<<nv<<endl;

	//Vertices...
	for (i = 0; i<nv; i++)
  	{
	    fout<<i+1<<" "<< vert[i].x<<" "<< vert[i].y<< endl;
  	}

	fout<<"Triangles:  "<<nf<<" "<<endl;

	//Faces...
    for (i = 0; i<nf; i++)
  	{    		
		fout<<localfaces[i].vertindex[0]+1<<" "
			<<localfaces[i].vertindex[1]+1<<" "
			<<localfaces[i].vertindex[2]+1<<endl;
  	}

	// If the file doesn't exist, it will create it for us once we start writing to it.																									
	// close() closes the file for us. (very important)
	// We must always close the file after we are done with it.  
	//It frees memory and make's it so we can access it again.
	fout.close();										

	cout<<" done..."<<endl;

	return true;
}
bool Mesh::save_OBJ_data(const char *Filename)
{
	printf("%d, %d\n", nv, nf);

	if(!nv) return false;
	if(!nf) return false;

	cout<<"	MESH: Saving "<<Filename<<"...";

	ofstream fout;										// Here we create an ofstream and we will call it "fout", like "cout".													
	fout.open(Filename);								// We call a function from our ofstream object called open().
	fout.precision(10); 

	//Vertices...
	int i;												
	for (i = 0; i<nv; i++)
  	{
	    fout << "v " <<vert[i].x<<" "<< vert[i].z <<" "<<vert[i].y<<endl;
  	}
	//Faces...
    for (i = 0; i<nf; i++)
  	{    		
		fout << "f "<<localfaces[i].vertindex[0]+1<<"/"<<localfaces[i].vertindex[0]+1<<"/ "
			        <<localfaces[i].vertindex[1]+1<<"/"<<localfaces[i].vertindex[1]+1<<"/ "
					<<localfaces[i].vertindex[2]+1<<"/"<<localfaces[i].vertindex[2]+1<<"/"<<endl;
  	}

	// If the file doesn't exist, it will create it for us once we start writing to it.																									
	// close() closes the file for us. (very important)
	// We must always close the file after we are done with it.  
	//It frees memory and make's it so we can access it again.
	fout.close();										

	cout<<" done..."<<endl;
	return true;
}

void Mesh::evaluate(int evaltype...)
{
	va_list ap;
	va_start(ap, evaltype);
	
	for(;;)
	{
		int p = va_arg(ap, int);
		if (p==0) break;
		
		if (p==MESH_CENTRE)
		{
			cout<<"MESH_CENTRE"<<endl;
			evaluate_centre();
		}
		if (p==MESH_TEXCOORDS)
		{
			cout<<"MESH_TEXCOORDS"<<endl;
			evaluate_texture_coords();
		}
		if (p==MESH_VERTEX_NORMALS) 
		{	
			_VERT_NORM = true;
			cout<<"MESH_VERTEX_NORMALS"<<endl;
			evaluate_normals();
		}
		if (p==MESH_FACE_NORMALS)
		{
			_FACE_NORM = true;
			cout<<"MESH_FACE_NORMALS"<<endl;
			evaluate_normals();
		}
	}
	
	va_end(ap);
}

// Utility functions to get the mesh in the right place.
void Mesh::evaluate_centre(void)
{
  if (nv == 0)
  {
    return;
  }

  float mins[3], maxs[3];

  mins[0] = maxs[0] = vert[0].x;
  mins[1] = maxs[1] = vert[0].y;
  mins[2] = maxs[2] = vert[0].z;

  int x;

  for (x=1; x<nv; x++)
  {

      mins[0] = min(mins[0], vert[x].x);
      mins[1] = min(mins[1], vert[x].y);
      mins[2] = min(mins[2], vert[x].z);
      maxs[0] = max(maxs[0], vert[x].x);
      maxs[1] = max(maxs[1], vert[x].y);
      maxs[2] = max(maxs[2], vert[x].z);
	
  }

  // Now subtract off the geometric center.
  for (x=0; x<nv; x++)
  {
	 vert[x].x -= (mins[0]+maxs[0])*0.5f;
     vert[x].y -= (mins[1]+maxs[1])*0.5f;
     vert[x].z -= (mins[2]+maxs[2])*0.5f;
  }
}
void Mesh::scalev(Vector scalefactor)
{
  if (nv == 0)
  {
    return;
  }

  int x;
  for (x=0; x<nv; x++)
  {
	  vert[x].x *= scalefactor.x;
	  vert[x].y *= scalefactor.y;
	  vert[x].z *= scalefactor.z;
  }
}

void Mesh::scale(float size)
{
  if (nv == 0)
  {
    return;
  }

  float maxs[3];

  maxs[0] = vert[0].x;
  maxs[1] = vert[0].y;
  maxs[2] = vert[0].z;

  int x;

  for (x=1; x<nv; x++)
  {
      maxs[0] = max(maxs[0], fabs(vert[x].x));
      maxs[1] = max(maxs[1], fabs(vert[x].y));
      maxs[2] = max(maxs[2], fabs(vert[x].z));
  }

  // Now scale to not more than that value.
  GLfloat maxDim = max(maxs[0], max(maxs[1], maxs[2]));
  GLfloat scaleFactor = -size / maxDim;

  // Now subtract off the geometric center.
  for (x=0; x<nv; x++)
  {
      vert[x].x *= scaleFactor;
      vert[x].y *= scaleFactor;
      vert[x].z *= scaleFactor;
  }
}

void Mesh::evaluate_normals()
{	
	_VERT_NORM = true;
	for (int x = 0; x<nf; x++)
	{
		//faces[x]->evaluate_face_normal();
		//printf("%d, %d, %d\n",localfaces[x].vertindex[0],localfaces[x].vertindex[1],localfaces[x].vertindex[2]);

		localfaces[x].evaluate_face_normal();
		//localfaces[x].facenormal.get_normal(vert[localfaces[x].vertindex[0]], vert[localfaces[x].vertindex[1]], vert[localfaces[x].vertindex[2]]);

		if(_VERT_NORM)
		{
			for (int y = 0; y<3; y++)
			{
				//if we want vertex normals, just add up face normals 
				//and let card normalise for us
				vertnorm[localfaces[x].vertindex[y]] += faces[x]->facenormal;
				vertnorm[localfaces[x].vertindex[y]].normalize();

			}
		}
	}
}

void Mesh::evaluate_texture_coords(void)
{
	float mins[3], maxs[3];
	
	mins[0] = maxs[0] = vert[0].x;
	mins[1] = maxs[1] = vert[0].y;
	mins[2] = maxs[2] = vert[0].z;
	
	int x;
	
	for (x=0; x<nv; x++)
	{
		mins[0] = min(mins[0], vert[x].x);
		mins[1] = min(mins[1], vert[x].y);
		mins[2] = min(mins[2], vert[x].z);
		
		maxs[0] = max(maxs[0], vert[x].x);
		maxs[1] = max(maxs[1], vert[x].y);
		maxs[2] = max(maxs[2], vert[x].z);
	}
	
	//--------------------
	// TEXTURE COORDS...
	//--------------------

	float uScale = 2;
	float uBias  = 0;
	float vScale = 2;
	float vBias  = 0;

	texCoords.clear();
	texCoords.resize(nv);
	
	for (x=0; x<nv; x++)
	{	
		texCoords[x].x = ((vert[x].x - mins[0]) / (maxs[0]-mins[0])) * uScale + uBias;
		texCoords[x].y = ((vert[x].y - mins[1]) / (maxs[1]-mins[1])) * vScale + vBias;
	}	
}

void Mesh::display_normals(void)
{
	int x, y;
		
	for (x = 0; x<nf; x++)
	{    
		glColor3f(1.0, 0.0, 0.0);
		for (y = 0; y<3; y++) display_vertex_normal(x,y);
	}
}

void Mesh::display_vertex_normal(int x, int y)
{
	vertnorm[localfaces[x].vertindex[y]].normalize();

	/*glPointSize(5.0);
	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_POINTS);
		glVertex3f(faces[x]->vert[y]->x, faces[x]->vert[y]->z, faces[x]->vert[y]->y);
	glEnd();*/

	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINES);
		glVertex3f(faces[x]->vert[y]->x, faces[x]->vert[y]->z, faces[x]->vert[y]->y);
		glVertex3f( (faces[x]->vert[y]->x+(1.0*vertnorm[localfaces[x].vertindex[y]].x) ),
					(faces[x]->vert[y]->z+(1.0*vertnorm[localfaces[x].vertindex[y]].z) ), 
					(faces[x]->vert[y]->y+(1.0*vertnorm[localfaces[x].vertindex[y]].y) ) );
	glEnd();
}

void Mesh::r_normal(GLenum surface, GLint texname)
{	
	if(nv==0) return;

	glBegin(surface);
	for (int x = 0; x<nf; x++)
	{    				
		glNormal3f(vertnorm[localfaces[x].vertindex[0]].x, vertnorm[localfaces[x].vertindex[0]].z, vertnorm[localfaces[x].vertindex[0]].y);
		glVertex3f(localfaces[x].vert[0]->x,localfaces[x].vert[0]->z,localfaces[x].vert[0]->y);
		
		glNormal3f(vertnorm[localfaces[x].vertindex[1]].x, vertnorm[localfaces[x].vertindex[1]].z, vertnorm[localfaces[x].vertindex[1]].y);		
		glVertex3f(localfaces[x].vert[1]->x,localfaces[x].vert[1]->z,localfaces[x].vert[1]->y);

		glNormal3f(vertnorm[localfaces[x].vertindex[2]].x, vertnorm[localfaces[x].vertindex[2]].z, vertnorm[localfaces[x].vertindex[2]].y);
		glVertex3f(localfaces[x].vert[2]->x,localfaces[x].vert[2]->z,localfaces[x].vert[2]->y);
	}
	glEnd();
}
void Mesh::r_vbo(GLenum surface)
{
	if(nv==0) return;
	if(nf==0) return;
	if(!has_vbo) return;

	//setup VBOs array pointers
	glEnableClientState(GL_NORMAL_ARRAY);
	glextensions->glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo1_name);
	glNormalPointer(GL_FLOAT, 0, (void*)(sizeof(GLfloat)*nv*3));
	glEnableClientState(GL_VERTEX_ARRAY);
	glextensions->glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo1_name);
	glVertexPointer(3, GL_FLOAT, 0, 0);
	glEnableClientState(GL_INDEX_ARRAY);
	glextensions->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vbo2_name);
	glIndexPointer(GL_UNSIGNED_INT, 0, 0);

	// start to render polygons
	glDrawElements(GL_TRIANGLES, nf*3, GL_UNSIGNED_INT, 0);

	// it is good idea to release VBOs with ID 0 after use.
	// Once bound with 0, all pointers in gl*Pointer() behave as real
	// pointer, so, normal vertex array operations are re-activated
	glDisableClientState(GL_NORMAL_ARRAY);
	glextensions->glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glextensions->glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	glEnableClientState(GL_INDEX_ARRAY);
	glextensions->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
}
void Mesh::init_vbo(void)
{	
	has_vbo = true;

	vbo_verts = new GLfloat[nv*3];
	vbo_norms = new GLfloat[nv*3];
	vbo_indices = new GLuint[nf*3];

	int index;
	for(int i=0; i<nv; i++)
	{
		index = 3*i;
		vbo_verts[index+0] = vert[i].x;
		vbo_verts[index+1] = vert[i].z;
		vbo_verts[index+2] = vert[i].y;

		vbo_norms[index+0] = vertnorm[i].x;
		vbo_norms[index+1] = vertnorm[i].z;
		vbo_norms[index+2] = vertnorm[i].y;
	}

	for(int x=0; x<nf; x++)
	{
		index = 3*x;
		vbo_indices[index+0] = GLuint(localfaces[x].vertindex[0]);
		vbo_indices[index+1] = GLuint(localfaces[x].vertindex[1]);
		vbo_indices[index+2] = GLuint(localfaces[x].vertindex[2]);
	}

	int bufferSize;

     // create VBO for verts & normals array
 	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
    glextensions->glGenBuffersARB(1, &vbo1_name);

    glextensions->glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo1_name);
    glextensions->glBufferDataARB(GL_ARRAY_BUFFER_ARB, sizeof(GLfloat)*nv*3*2, 0, GL_STATIC_DRAW_ARB);
    glextensions->glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, 0, sizeof(GLfloat)*nv*3, vbo_verts);
    glextensions->glBufferSubDataARB(GL_ARRAY_BUFFER_ARB, sizeof(GLfloat)*nv*3, sizeof(GLfloat)*nv*3, vbo_norms);
    
	glextensions->glGetBufferParameterivARB(GL_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &bufferSize);
    //cout << "Vertex and Normal Array in VBO: " << bufferSize << " bytes\n";

    // create VBO for index array
    glextensions->glGenBuffersARB(1, &vbo2_name);                        // create a vbo
    glextensions->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, vbo2_name);                    // activate vbo id to use
    glextensions->glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB, sizeof(GLuint)*nf*3, vbo_indices, GL_STATIC_DRAW_ARB); // upload data to video card
  
	glextensions->glGetBufferParameterivARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &bufferSize);
    //cout << "Index Array in VBO: " << bufferSize << " bytes\n";

	// it is good idea to release VBOs with ID 0 after use.
	// Once bound with 0, all pointers in gl*Pointer() behave as real
	// pointer, so, normal vertex array operations are re-activated
	glextensions->glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
	glextensions->glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);

	delete[] vbo_verts;
	vbo_verts = 0;

	delete[] vbo_norms;
	vbo_norms = 0;

	delete[] vbo_indices;
	vbo_indices = 0;
}

const Mesh& Mesh::operator= (const Mesh& right)
{
	glextensions=right.glextensions;
	vbo1_name = right.vbo1_name;
	vbo2_name = right.vbo2_name;
	vbo3_name = right.vbo3_name;
	nv = right.nv;
	nf = right.nf;
	
	has_vbo = right.has_vbo;

	set_numverts(right.nv);

	for(int i=0; i<nv; i++)
	{
		vert[i] = right.vert[i];
		vertcol[i] = right.vertcol[i];
		vertnorm[i] = right.vertnorm[i];
	}

	set_numfaces(right.nf);

	for(int j=0; j<nf; j++)
	{
		localfaces[j].facenormal = right.localfaces[j].facenormal;

		localfaces[j].vertindex[0] = right.localfaces[j].vertindex[0];
		localfaces[j].vertindex[1] = right.localfaces[j].vertindex[1];
		localfaces[j].vertindex[2] = right.localfaces[j].vertindex[2];
		
		localfaces[j].vert[0] = &vert[localfaces[j].vertindex[0]];
		localfaces[j].vert[1] = &vert[localfaces[j].vertindex[1]];
		localfaces[j].vert[2] = &vert[localfaces[j].vertindex[2]];
		
		faces[j] = &localfaces[j];
	}

	return *this;
}