/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "MeshListObject.h"

MeshListObject::MeshListObject()
{
	glextensions=NULL;
}
MeshListObject::~MeshListObject()
{
	glextensions=NULL;

	for(int i=0; i<meshes.size(); i++)
		meshes[i].glextensions = NULL;
	clear_all();
}
void MeshListObject::clear_all(void)
{
	names.clear();
	colours.clear();
	meshes.clear();
	qualities.clear();
	values.clear();
	smoothsize.clear();
	smoothsigma.clear();
	clipstate.clear();
}
Mesh* MeshListObject::new_mesh(void)
{
	meshes.push_back(Mesh());
	return &meshes[meshes.size()-1];
}


void MeshListObject::add(QString name, Vector col,int qual, int val, float smoothsiz, float smoothsigm, int clpst)
{
	names.push_back(name);
	colours.push_back(col);
	qualities.push_back(qual);
	values.push_back(val);
	smoothsize.push_back(smoothsiz);
	smoothsigma.push_back(smoothsigm);
	clipstate.push_back(clpst);

	meshes[meshes.size()-1].glextensions = glextensions;
	meshes[meshes.size()-1].init_vbo();
}
void MeshListObject::del(QString name)
{
	int iterpos=-1;
	for(int i=0; i<names.size(); i++)
	{
		if(names[i]==name)
		{
			meshes[i].reset();
			iterpos=i;
		}

		if(iterpos==i) break;
	}

	if(iterpos==-1) return;

	// erase the elements
	names.erase(names.begin()+iterpos);
	colours.erase(colours.begin()+iterpos);
	meshes.erase(meshes.begin()+iterpos);
	qualities.erase(qualities.begin()+iterpos);
	values.erase(values.begin()+iterpos);
	smoothsize.erase(smoothsize.begin()+iterpos);
	smoothsigma.erase(smoothsigma.begin()+iterpos);
	clipstate.erase(clipstate.begin()+iterpos);
}
void MeshListObject::get(QString name, int* quality, int* value, float* smoothsiz, float* smoothsigm, Vector* col, int* nv, int* nf, int* cipstate)
{
	for(int i=0; i<names.size(); i++)
	{
		printf("%s, %s \n", names[i].latin1(), name.latin1());

		if(names[i]==name)
		{
			*smoothsiz=smoothsize[i];
			*smoothsigm=smoothsigma[i];
			*value = values[i];
			*col = colours[i];
			*quality = qualities[i];
			*nv = meshes[i].nv;
			*nf = meshes[i].nf;
			*cipstate = clipstate[i];

			return;
		}
	}
}
int MeshListObject::set(QString oldname, QString name, int quality, int value, float smoothsiz, float smoothsigm, Vector col, int clpst)
{
	for(int i=0; i<names.size(); i++)
	{
		if(names[i]==oldname)
		{
			if(values[i]!=value || qualities[i]!=quality || smoothsize[i]!=smoothsiz || smoothsigma[i]!=smoothsigm)
			{
				//printf("update!!\n");
				smoothsize[i]=smoothsiz;
				smoothsigma[i]=smoothsigm;
				colours[i] = col;
				qualities[i] = quality;
				values[i] = value;
				names[i] = name;
				clipstate[i] = clpst;

				return i;			
			}
			else
			{
				//printf("dont update!!\n");
				smoothsize[i]=smoothsiz;
				smoothsigma[i]=smoothsigm;
				colours[i] = col;
				qualities[i] = quality;
				values[i] = value;
				names[i] = name;
				clipstate[i] = clpst;

				return -1;			
			}
		}
		else
		{
			cout<<"uh oh"<<endl;
		}
	}	

	return -1;			
}
void MeshListObject::copy(QString name)
{
}
void MeshListObject::save_obj(QString filename, int j)
{
	int i;
	QString fn, numb;
	fn = filename;

	ofstream fout;										// Here we create an ofstream and we will call it "fout", like "cout".													
	fout.open(fn.latin1());								// We call a function from our ofstream object called open().
	
	//cout<<fn.latin1()<<endl;

	if (!fout.is_open())
	{
		cout<<"ERROR!!!"<<endl;
		return;
	}

	fout.precision(10); 

	 //end name with a special character so that we can have complex names, easier for tokeniser
	fout<<"Name "<<"\""<<names[j].latin1()<<"\""<<endl;
	fout<<"Colours "<<colours[j].x<<" "<<colours[j].y<<" "<<colours[j].z<<endl;
	fout<<"ISOQuality "<<qualities[j]<<endl;
	fout<<"ISOValue "<<values[j]<<endl;
	fout<<"SMOOTHSigma "<<smoothsigma[j]<<endl;
	fout<<"SMOOTHSize "<<smoothsize[j]<<endl;
	fout<<"NumbV "<<meshes[j].nv<<endl;
	fout<<"NumbF "<<meshes[j].nf<<endl;

	progressbar = new Q3ProgressDialog("Saving OBJ Files...", "Cancel", meshes[j].nv+meshes[j].nv+meshes[j].nf, 0, "progress", TRUE);		
	progressbar->setCaption("Please Wait");

	//Vertex Info...
	for (i = 0; i<meshes[j].nv; i++)
	{
		progressbar->setProgress(i);
		if (progressbar->wasCancelled())
		break;

		fout << "v " <<meshes[j].vert[i].x<<" "<< meshes[j].vert[i].y <<" "<<meshes[j].vert[i].z<<endl;
	}
	for(i=0; i<meshes[j].nv; i++) fout<<"vc "<<colours[j].x<<" "<<colours[j].y<<" "<<colours[j].z<<endl;
	for (i = 0; i<meshes[j].nv; i++)
	{
		progressbar->setProgress(meshes[j].nv+i);
		if (progressbar->wasCancelled())
		break;

		fout << "vn " <<meshes[j].vertnorm[i].x<<" "<<meshes[j].vertnorm[i].y <<" "<<meshes[j].vertnorm[i].z<<endl;
	}
	
	//Faces Info...
	for (i = 0; i<meshes[j].nf; i++)
	{    		
		progressbar->setProgress(meshes[j].nv+meshes[j].nv+i);
		if (progressbar->wasCancelled())
		break;

		fout << "f "<<meshes[j].localfaces[i].vertindex[0]+1<<" "
					<<meshes[j].localfaces[i].vertindex[1]+1<<" "
					<<meshes[j].localfaces[i].vertindex[2]+1<<endl;
	}

	// If the file doesn't exist, it will create it for us once we start writing to it.																									
	// close() closes the file for us. (very important)
	// We must always close the file after we are done with it.  
	//It frees memory and make's it so we can access it again.
	fout.close();

	progressbar->setProgress(meshes[j].nv+meshes[j].nv+meshes[j].nf);
	delete progressbar;
}
void MeshListObject::save(QString dirname)
{
	int i, j;
	QString fn, numb;
	
	//remove the filename of first file to get only dirname
/*	int pos = filename.findRev(QString("/").latin1(), -1, false);
	QString dirname = filename.replace(pos+1, filename.length()-pos+1, "");

	if(filename.isEmpty())
	{
		cout<<"ERROR!!!"<<endl;
		return;
	}
*/
	for(j=0; j<meshes.size(); j++)
	{	
		fn = dirname.latin1();
		fn += names[j].latin1();
		fn += ".obj";

		ofstream fout;										// Here we create an ofstream and we will call it "fout", like "cout".													
		fout.open(fn.latin1());								// We call a function from our ofstream object called open().
		
		cout<<fn.latin1()<<endl;

		if (!fout.is_open())
		{
			cout<<"ERROR!!!"<<endl;
			return;
		}

		fout.precision(10); 

		 //end name with a special character so that we can have complex names, easier for tokeniser
		fout<<"Name "<<"\""<<names[j].latin1()<<"\""<<endl;
		fout<<"Colours "<<colours[j].x<<" "<<colours[j].y<<" "<<colours[j].z<<endl;
		fout<<"ISOQuality "<<qualities[j]<<endl;
		fout<<"ISOValue "<<values[j]<<endl;
		fout<<"SMOOTHSigma "<<smoothsigma[j]<<endl;
		fout<<"SMOOTHSize "<<smoothsize[j]<<endl;
		fout<<"NumbV "<<meshes[j].nv<<endl;
		fout<<"NumbF "<<meshes[j].nf<<endl;

		progressbar = new Q3ProgressDialog("Saving OBJ Files...", "Cancel", meshes[j].nv+meshes[j].nv+meshes[j].nf, 0, "progress", TRUE);		
		progressbar->setCaption("Please Wait");

		//Vertex Info...
		for (i = 0; i<meshes[j].nv; i++)
		{
			progressbar->setProgress(i);
			if (progressbar->wasCancelled())
			break;

			fout << "v " <<meshes[j].vert[i].x<<" "<< meshes[j].vert[i].y <<" "<<meshes[j].vert[i].z<<endl;
		}
		for(i=0; i<meshes[j].nv; i++) fout<<"vc "<<colours[j].x<<" "<<colours[j].y<<" "<<colours[j].z<<endl;
		for (i = 0; i<meshes[j].nv; i++)
		{
			progressbar->setProgress(meshes[j].nv+i);
			if (progressbar->wasCancelled())
			break;

			fout << "vn " <<meshes[j].vertnorm[i].x<<" "<<meshes[j].vertnorm[i].y <<" "<<meshes[j].vertnorm[i].z<<endl;
		}
		
		//Faces Info...
		for (i = 0; i<meshes[j].nf; i++)
		{    		
			progressbar->setProgress(meshes[j].nv+meshes[j].nv+i);
			if (progressbar->wasCancelled())
			break;

			fout << "f "<<meshes[j].localfaces[i].vertindex[0]+1<<" "
						<<meshes[j].localfaces[i].vertindex[1]+1<<" "
						<<meshes[j].localfaces[i].vertindex[2]+1<<endl;
		}

		// If the file doesn't exist, it will create it for us once we start writing to it.																									
		// close() closes the file for us. (very important)
		// We must always close the file after we are done with it.  
		//It frees memory and make's it so we can access it again.
		fout.close();
	
		progressbar->setProgress(meshes[j].nv+meshes[j].nv+meshes[j].nf);
		delete progressbar;
	}
}
void MeshListObject::load(QString fn)
{
	//temp storage for parsing...
	Mesh m;
	string name;
	Vector v;
	float numb;
	int nv, nvn, nf, f0, f1, f2;
	nv=nvn=nf=0;
	string label;
	
	//input file for reading in parameter ranges
	ifstream input_file; 
	input_file.open(fn.latin1());

	if (!input_file.is_open())
	{
		cout<<"ERROR!!!"<<endl;
		return;
	}

	int numbverts=0;
	int numbfaces=0;
	
	//tokenize our input file
	Tokenizer token(input_file);

	//until oef is reached step through each token
	while(!input_file.eof())
	{
		if(token.ttype()==TT_WORD)
		{
			//get the first string lable
			label = token.sval();
			
			if(label=="Name")
			{	
				token.nextToken();
				if(token.ttype()==TT_STRING) name = token.sval();

				names.push_back(QString(name.data()));
			}
			else if(label=="Colours")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) v.x = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) v.y = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) v.z = token.nval();

				colours.push_back(v);
			}
			else if(label=="ISOQuality")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) numb = token.nval();

				qualities.push_back(numb);
			}
			else if(label=="ISOValue")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) numb = token.nval();

				values.push_back(numb);
			}
			else if(label=="SMOOTHSigma")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) numb = token.nval();

				smoothsigma.push_back(numb);
			}
			else if(label=="SMOOTHSize")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) numb = token.nval();

				smoothsize.push_back(numb);
			}
			else if(label=="NumbV")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) numb = token.nval();

				m.set_numverts(numb);
				numbverts=1;
			}
			else if(label=="NumbF")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) numb = token.nval();

				m.set_numfaces(numb);
				numbfaces=1;
			}
			else if(label=="v")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) v.x = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) v.y = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) v.z = token.nval();

				if(numbverts) m.vert[nv] = v;
				else m.vert.push_back(v);

				nv++;
			}
			else if(label=="vn")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) v.x = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) v.y = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) v.z = token.nval();

				if(numbverts) m.vertnorm[nvn] = v;
				else m.vertnorm.push_back(v);
				
				nvn++;
			}
			else if(label=="f")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) f0 = (int)token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) f1 = (int)token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) f2 = (int)token.nval();

				if(numbfaces)
				{
					m.localfaces[nf].vert[0] = &m.vert[f0-1];
					m.localfaces[nf].vert[1] = &m.vert[f1-1];
					m.localfaces[nf].vert[2] = &m.vert[f2-1];
					m.localfaces[nf].vertindex[0] = f0-1;
					m.localfaces[nf].vertindex[1] = f1-1;
					m.localfaces[nf].vertindex[2] = f2-1;
					m.faces[nf] = &m.localfaces[nf];
				}
				else
				{
					Face f;

					f.vert[0] = &m.vert[f0-1];
					f.vert[1] = &m.vert[f1-1];
					f.vert[2] = &m.vert[f2-1];
					
					f.vertindex[0] = f0-1;
					f.vertindex[1] = f1-1;
					f.vertindex[2] = f2-1;
					
					f.facenormal.get_normal(m.vert[f0-1],m.vert[f1-1],m.vert[f2-1]);

					m.localfaces.push_back(f);
					m.faces.push_back(&m.localfaces[m.localfaces.size()-1]);
				}

				nf++;
			}
			//else if we dont know what it is, go to next label
			else
			{
				//cout<<"WARNING: Unrecognized Label..."<<endl;
				token.nextToken();
			}
		}
		else token.nextToken();
	}

	input_file.close();

	//m.reset();
	//m.load_OBJ_data(fn.latin1());

	if(numbfaces==0)
	{
		printf("Settting up MeshListObject from external OBJ\n");
		
		//todo: make unique name... and meshojbsize in name..
		names.push_back(fn.latin1());
		colours.push_back(Vector(1,1,1));
		qualities.push_back(100);
		values.push_back(50);
		smoothsize.push_back(50);
		smoothsigma.push_back(50);
		clipstate.push_back(1);

		m.load_OBJ_data(fn.latin1());
		
		nv = m.nv;
		nf = m.nf;
		numbverts = m.nv;
		numbfaces = m.nf;

	//	printf("nv: %d, %d\n",nv,m.vert.size());
	//	printf("nf: %d, %d\n",nf,m.localfaces.size());
	//  m.vertcol.resize(nv);
	//  m.vertnorm.resize(nv);

		//m.scale(1.0);
		//m.scale(128.0);
		//MESH_CENTRE, 
		m.evaluate(0, MESH_VERTEX_NORMALS, MESH_CENTRE);
	}
	else
	{
		clipstate.push_back(1);
		m.evaluate(0, MESH_VERTEX_NORMALS);
	}
	
	/*for(int i=0; i<m.nv; i++)
	{
		m.vert[i] -= Vector(512,32,512);
	}*/
	
	meshes.push_back(m);
	meshes[meshes.size()-1].glextensions = glextensions;
	meshes[meshes.size()-1].init_vbo();

	m.reset();
}