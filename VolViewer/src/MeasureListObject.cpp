/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "MeasureListObject.h"

MeasureListObject::MeasureListObject()
{
}
MeasureListObject::~MeasureListObject()
{
	original_data = "";
	time = 1;
	scale = Vector(1,1,1);
}
void MeasureListObject::clear(void)
{
	original_data = "";
	time = 1;
	scale = Vector(1,1,1);

	for(int i=0; i<measure_objects.size(); i++)
		measure_objects[i].clear();

	visible.clear();
	measure_objects.clear();
}
void MeasureListObject::add(MeasureObject obj)
{
	measure_objects.push_back(obj);
	visible.push_back(true);
}
int MeasureListObject::size(void)
{
	return measure_objects.size();
}
void MeasureListObject::erase(int i)
{
	//printf("MeasureListObject erase: %d of %d\n", i, measure_objects.size());
	measure_objects[i].clear();
	measure_objects.erase(measure_objects.begin()+i);
	visible.erase(visible.begin()+i);
	//printf("MeasureListObject size: %d\n", measure_objects.size());
}
void MeasureListObject::replace(int i, MeasureObject obj)
{
	measure_objects[i] = obj;
}
MeasureObject MeasureListObject::get(int i)
{
	return measure_objects[i];
}
void MeasureListObject::display_arrowheads(int current_object)
{
	for(int i=0; i<measure_objects.size(); i++)
	{
		if(visible[i]==true && current_object!=i)
		{
			measure_objects[i].display_arrowheads(1.5);
		}
	}
}

void MeasureListObject::evaluate_aabb(Vector* aabb)
{

	//					FRONT
	//			   2o----------o1(MAX,MAX,MAX)
	//				-		   -
	//				-		   -
	//				-		   -
	//				-		   -
	//				3o----------o0
	//					BACK
	// 		       5o----------o6
	//	 			-		   -
	//				-		   -
	//				-		   -
	//				-		   -
	//(MIN,MIN,MIN)4o----------o7
	
	float mins[3], maxs[3];
	float t0 = 0.0f;
	mins[0] = 9999999999999999999.9;
	maxs[0] = -9999999999999999999.9;
	mins[1] = 9999999999999999999.9;
	maxs[1] = -9999999999999999999.9;
	mins[2] = 9999999999999999999.9;
	maxs[2] = -9999999999999999999.9;
	
	for(int i=0; i<measure_objects.size(); i++)
	{
		for (int x=0; x<measure_objects[i].vertices.size(); x++)
		{
			mins[0] = min(mins[0], measure_objects[i].vertices[x].x);
			mins[1] = min(mins[1], measure_objects[i].vertices[x].y);
			mins[2] = min(mins[2], measure_objects[i].vertices[x].z);
		
			maxs[0] = max(maxs[0], measure_objects[i].vertices[x].x);
			maxs[1] = max(maxs[1], measure_objects[i].vertices[x].y);
			maxs[2] = max(maxs[2], measure_objects[i].vertices[x].z);
		}
	}
	//FRONT
	aabb[0].x = maxs[0] + t0;
	aabb[0].y = mins[1] - t0;
	aabb[0].z = maxs[2] + t0;
	aabb[1].x = maxs[0] + t0;
	aabb[1].y = maxs[1] + t0;
	aabb[1].z = maxs[2] + t0;
	aabb[2].x = mins[0] - t0;
	aabb[2].y = maxs[1] + t0;
	aabb[2].z = maxs[2] + t0;
	aabb[3].x = mins[0] - t0;
	aabb[3].y = mins[1] - t0;
	aabb[3].z = maxs[2] + t0;

	//BACK
	aabb[4].x = mins[0] - t0;
	aabb[4].y = mins[1] - t0;
	aabb[4].z = mins[2] - t0;
	aabb[5].x = mins[0] - t0;
	aabb[5].y = maxs[1] + t0;
	aabb[5].z = mins[2] - t0;
	aabb[6].x = maxs[0] + t0;
	aabb[6].y = maxs[1] + t0;
	aabb[6].z = mins[2] - t0;
	aabb[7].x = maxs[0] + t0;
	aabb[7].y = mins[1] - t0;
	aabb[7].z = mins[2] - t0;
}
void MeasureListObject::display(int current_object)
{
	for(int i=0; i<measure_objects.size(); i++)
	{
		if(visible[i]==true && current_object!=i)
		{
			glPushMatrix();

			measure_objects[i].display_verts();
			measure_objects[i].display_edges();
			measure_objects[i].display_faces();
			measure_objects[i].display_growth();
			glPopMatrix();

		}
	}

}
void MeasureListObject::save_file(QString fn, QString originalfilename, QString algorithm, Vector res)
{	
	printf("SAVNG FILE\n");

	if(!fn.isEmpty())
	{
		ofstream fout;
		fout.open(fn.latin1());

		fout<<"MSR_VERSION = 1.2"<<endl;
		if(!originalfilename.isEmpty())	fout<<"ORIGINALDATA = PATH '"<<originalfilename.latin1()<<"'"<<endl;
		else fout<<"ORIGINALDATA = PATH '"<<original_data.latin1()<<"'"<<endl;
		fout<<"SCALE = "<<scale.x<<" "<<scale.y<<" "<<scale.z<<endl;
		fout<<"TIME = "<<time<<endl;
	//	fout<<"SCALE = "<<unitX/voxX<<" "<<unitY/voxY<<" "<<unitZ/voxZ<<endl;
	//	fout<<"TIME = "<<time_value<<endl;
		fout<<"ALGORITHM = '"<<algorithm.latin1()<<"'"<<endl;
		fout<<"IMAGESIZE = "<<res.x<<" "<<res.y<<" "<<res.z<<endl;
		fout<<"OBJECTCOUNT = "<<measure_objects.size()<<endl;
		for(int i=0; i<measure_objects.size(); i++)
		{
			fout<<"OBJECT = '"<<measure_objects[i].name<<"'"<<endl;

			QString objtypestring="";
			int objtype = measure_objects[i].drawmode;
			if(objtype==MEASURE_DRAWMODE_POINTS) objtypestring="'POINTS'";
			else if(objtype==MEASURE_DRAWMODE_LINES) objtypestring="'LINES'";
			else if(objtype==MEASURE_DRAWMODE_MAGNETLINE) objtypestring="'MESH'";
			
			fout<<"RENDERTYPE = "<<objtypestring.latin1()<<endl;
			fout<<"VERTCOUNT = "<<measure_objects[i].vertices.size()<<endl;
			
			for(int j=0; j<measure_objects[i].vertices.size(); j++)
			{
				fout<<"VERT = "<<measure_objects[i].vertices[j].x<<" "<<measure_objects[i].vertices[j].y<<" "<<measure_objects[i].vertices[j].z<<endl;
			}
			for(int j=0; j<measure_objects[i].vertex_labels.size(); j++)
			{
				fout<<"VERTLABEL = "<<measure_objects[i].vertex_labels[j].size();
				for(int k=0; k<measure_objects[i].vertex_labels[j].size(); k++)
					fout<<" '"<<measure_objects[i].vertex_labels[j][k]<<"'";
				fout<<endl;
			}

			if(!measure_objects[i].edges.empty())
			{
				fout<<"EDGECOUNT = "<<measure_objects[i].edges.size()<<endl;
				for(int j=0; j<measure_objects[i].edges.size(); j++)
				{
					fout<<"EDGE = "<<measure_objects[i].edges[j].v0<<" "<<measure_objects[i].edges[j].v1<<endl;
				}
				for(int j=0; j<measure_objects[i].edge_labels.size(); j++)
				{
					fout<<"EDGELABEL = "<<measure_objects[i].edge_labels[j].size();
					for(int k=0; k<measure_objects[i].edge_labels[j].size(); k++)
						fout<<" '"<<measure_objects[i].edge_labels[j][k]<<"'";
					fout<<endl;
				}

				if(!measure_objects[i].edge_kmax.empty())
				{
					fout<<"EDGEGROWTHDT = "<<measure_objects[i].edge_dT<<endl;
					for(int j=0; j<measure_objects[i].edge_kmax.size(); j++)
						fout<<"EDGEGROWTH = "<<measure_objects[i].edge_kmax[j]<<endl;
				}
			}
			if(!measure_objects[i].faces.empty())
			{
				fout<<"FACECOUNT = "<<measure_objects[i].faces.size()<<endl;
				for(int j=0; j<measure_objects[i].faces.size(); j++)
				{
					fout<<"FACE = ";
					for(int k=0; k<measure_objects[i].faces[j].size(); k++)
						fout<<measure_objects[i].faces[j][k]<<" ";
					fout<<endl;
				}
				for(int j=0; j<measure_objects[i].face_labels.size(); j++)
				{
					fout<<"FACELABEL = "<<measure_objects[i].face_labels[j].size();
					for(int k=0; k<measure_objects[i].face_labels[j].size(); k++)
						fout<<" '"<<measure_objects[i].face_labels[j][k]<<"'";
					fout<<endl;
				}

				if(!measure_objects[i].face_kmax.empty())
				{
					fout<<"FACEGROWTHDT = "<<measure_objects[i].face_dT<<endl;
					for(int j=0; j<measure_objects[i].face_kmax.size(); j++)
						fout<<"FACEGROWTH = "<<measure_objects[i].face_kmax[j]<<" "<<measure_objects[i].face_kmin[j]<<" "<<measure_objects[i].face_vmax[j].x<<" "<<measure_objects[i].face_vmax[j].y<<" "<<measure_objects[i].face_vmax[j].z<<endl;
				}
			}
		}

		fout.close();
	}
}
string MeasureListObject::tokenread_line(Tokenizer* t)
{
	t->nextToken();
	string l = "";
	while(t->ttype() != TT_EOL)
	{
		if(t->ttype()==TT_STRING) l += t->sval();
		else if(t->ttype()==TT_WORD) l += t->sval();
		else if(t->ttype()==TT_NUMBER)
		{
			std::stringstream out;
			out << t->nval();
			l += out.str();
		}
		t->nextToken();

		l += " ";
	}

	return l;
}
float MeasureListObject::check_file_version(QString fn)
{
	float versionnumb=-1;
	printf("Checking MSR version: %s\n", fn.latin1());

	if(!fn.isEmpty())
	{
		string label;
		ifstream input_file; 
		input_file.open(fn.latin1());
		//input_file.setf(0,ios::floatfield);
		//input_file.precision(10);

		if(!input_file.is_open()) return -3;

		Tokenizer token(input_file);
		token.eolIsSignificant(true);

		while(!input_file.eof())
		{
			if(token.ttype()==TT_WORD)
			{
				//get the first string lable
				label = token.sval();
				
				if(label=="MSR_VERSION")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						versionnumb = token.nval();
						input_file.close();
						return versionnumb;
					}
				}
				else
				{
					token.nextToken();
				}
			}
			else token.nextToken();
		}

		input_file.close();
	}
	else return -3;

	return versionnumb;
}
void MeasureListObject::open_fileversion0(QString fn)
{
	printf("MSR loading: %s\n", fn.latin1());
	if(!fn.isEmpty())
	{
		ifstream input_file; 
		input_file.open(fn.latin1());

		if(!input_file.is_open()) return;

		Tokenizer token(input_file);
		token.eolIsSignificant(true);

		string label;
		Vector v;
		Edge e;
		
		vector<QString> temp_namestrings;
		vector< vector<Vector> > temp_measuring_vertices;
		vector< vector<Edge> > temp_measuring_edges;
		vector<int> temp_measuring_drawmodes;
		vector< vector< vector<int> > > temp_measuring_faces;

		vector< float > temp_measuring_edge_dt;
		vector< vector< float > > temp_measuring_edge_kmax;

		vector< float > temp_measuring_dt;
		vector< vector<float> > temp_measuring_kmax;
		vector< vector<float> > temp_measuring_kmin;
		vector< vector<Vector> > temp_measuring_vmax;
		vector< vector<Vector> > temp_measuring_vmin;
		vector< vector<Vector> > temp_measuring_norm;

		int mindex=-1;

		//until oef is reached step through each token
		while(!input_file.eof())
		{
			if(token.ttype()==TT_WORD)
			{
				//get the first string lable
				label = token.sval();
				
				if(label=="OBJECTCOUNT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						int size = token.nval();
						temp_measuring_vertices.resize(size);
						temp_measuring_edges.resize(size);
						temp_measuring_faces.resize(size);
					}
				}
				else if(label=="OBJECTNAME")
				{
					//if(current_measurement.size()!=0) measuring_vertices.push_back(current_measurement);
					mindex++;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_STRING)
					{
						label = token.sval();
						QString name = label.data();
						temp_namestrings.push_back(name);
					}
				}
				else if(label=="OBJECTVERTCOUNT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						int size = token.nval();
					}
				}
				else if(label=="OBJECTTYPE")
				{		
					//skip the equal sign
					token.nextToken(); 
					token.nextToken();
					if(token.ttype()==TT_STRING)
					{
						label = token.sval();;
					
						QString objtype = label.data();

						if(label == "POINTS") temp_measuring_drawmodes.push_back(MEASURE_DRAWMODE_POINTS);
						else if(label == "LINES") temp_measuring_drawmodes.push_back(MEASURE_DRAWMODE_LINES);
						else if(label == "MAGNETIC_LINES") temp_measuring_drawmodes.push_back(MEASURE_DRAWMODE_MAGNETLINE);
						else if(label == "FACES") temp_measuring_drawmodes.push_back(MEASURE_DRAWMODE_MAGNETLINE);
					}
				}
				/*else if(label=="PLANE")
				{
					//skip the equal sign
					token.nextToken(); 
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.x = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.y = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.z = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.w = token.nval();

					//printf("v: %f, %f, %f %f\n", v.x, v.y, v.z,v.w);				
					measuring_planes.push_back(v);
				}*/
				else if(label=="VERT")
				{		
					//skip the equal sign
					token.nextToken(); 
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.x = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.y = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.z = token.nval();
					
					v.w = 0.0;
					temp_measuring_vertices[mindex].push_back(v);
				}
				else if(label=="EDGE")
				{		
					//skip the equal sign
					token.nextToken(); 
					token.nextToken();
					if(token.ttype()==TT_NUMBER) e.v0 = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) e.v1 = token.nval();
				
					temp_measuring_edges[mindex].push_back(e);
				}				//else we dont know what it is, go to next label
				else if(label=="FACE")
				{		
					//skip the equal sign
					token.nextToken(); 

					vector<int> face;
					int faceindex;
					bool endofline=false;

					while(endofline==false)
					{
						token.nextToken();

						if(token.ttype()==TT_NUMBER)
						{
							faceindex = token.nval();
							face.push_back(faceindex);
							//printf("faceindex: %d\n", faceindex);
						}
						else if(token.ttype()==TT_EOL || token.ttype()==TT_EOF)
						{
							temp_measuring_faces[mindex].push_back(face);
							endofline = true;
							//printf("end of line\n");
						}
					}				
				}				//else we dont know what it is, go to next label
				else
				{
					token.nextToken();
				}
			}
			else token.nextToken();
		}

		input_file.close();
	
		//Set the data in our MeasureObjects
		setData(temp_namestrings, temp_measuring_vertices, temp_measuring_edges, temp_measuring_faces, temp_measuring_drawmodes, 
				temp_measuring_edge_dt, temp_measuring_edge_kmax,
				temp_measuring_dt, temp_measuring_kmax, temp_measuring_kmin, temp_measuring_vmax, temp_measuring_vmin, 
				temp_measuring_norm);
	}
}
void MeasureListObject::open_fileversion1(QString fn)
{
	printf("MSR loading: %s\n", fn.latin1());
	if(!fn.isEmpty())
	{
		ifstream input_file; 
		input_file.open(fn.latin1());

		if(!input_file.is_open()) return;

		Tokenizer token(input_file);
		token.eolIsSignificant(true);

		string label;
		Vector v;
		Edge e;
		
		vector<QString> temp_namestrings;
		vector< vector<Vector> > temp_measuring_vertices;
		vector< vector<Edge> > temp_measuring_edges;
		vector<int> temp_measuring_drawmodes;
		vector< vector< vector<int> > > temp_measuring_faces;

		vector< float > temp_measuring_edge_dt;
		vector< vector< float > > temp_measuring_edge_kmax;

		vector< float > temp_measuring_dt;
		vector< vector< float> > temp_measuring_kmax;
		vector< vector< float> > temp_measuring_kmin;
		vector< vector< Vector > > temp_measuring_vmax;
		vector< vector< Vector > > temp_measuring_vmin;
		vector< vector< Vector > > temp_measuring_norm;
		
		int mindex=-1;
		Vector scale;

		//until oef is reached step through each token
		while(!input_file.eof())
		{
			if(token.ttype()==TT_WORD)
			{
				//get the first string lable
				label = token.sval();
				
				if(label=="OBJECTCOUNT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						int size = token.nval();
						temp_measuring_vertices.resize(size);
						temp_measuring_edges.resize(size);
						temp_measuring_faces.resize(size);
						
						temp_measuring_norm.resize(size);
						temp_measuring_kmin.resize(size);
						temp_measuring_kmax.resize(size);
						temp_measuring_vmax.resize(size);
						temp_measuring_vmin.resize(size);
					}
				}
				/*else if(label=="SCALE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) scale.x = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) scale.y = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) scale.z = token.nval();
				}*/
				else if(label=="OBJECTNAME")
				{
					//if(current_measurement.size()!=0) measuring_vertices.push_back(current_measurement);
					mindex++;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_STRING)
					{
						label = token.sval();
						QString name = label.data();
						temp_namestrings.push_back(name);
					}
				}
				else if(label=="VERTCOUNT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						int size = token.nval();
					}
				}
				else if(label=="OBJECTTYPE")
				{		
					//skip the equal sign
					token.nextToken(); 
					token.nextToken();
					if(token.ttype()==TT_STRING)
					{
						label = token.sval();;
					
						QString objtype = label.data();

						if(label == "POINTS") temp_measuring_drawmodes.push_back(MEASURE_DRAWMODE_POINTS);
						else if(label == "LINES") temp_measuring_drawmodes.push_back(MEASURE_DRAWMODE_LINES);
						else if(label == "MAGNETIC_LINES") temp_measuring_drawmodes.push_back(MEASURE_DRAWMODE_MAGNETLINE);
						else if(label == "FACES") temp_measuring_drawmodes.push_back(MEASURE_DRAWMODE_MAGNETLINE);
					}
				}
				/*else if(label=="PLANE")
				{
					//skip the equal sign
					token.nextToken(); 
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.x = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.y = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.z = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.w = token.nval();

					//printf("v: %f, %f, %f %f\n", v.x, v.y, v.z,v.w);				
					measuring_planes.push_back(v);
				}*/
				else if(label=="VERT")
				{		
					//skip the equal sign
					token.nextToken(); 
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.x = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.y = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.z = token.nval();
					
					v.w = 0.0;
					temp_measuring_vertices[mindex].push_back(v);
				}
				else if(label=="EDGE")
				{		
					//skip the equal sign
					token.nextToken(); 
					token.nextToken();
					if(token.ttype()==TT_NUMBER) e.v0 = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) e.v1 = token.nval();
				
					temp_measuring_edges[mindex].push_back(e);
				}				//else we dont know what it is, go to next label
				else if(label=="FACE")
				{		
					//skip the equal sign
					token.nextToken(); 

					vector<int> face;
					int faceindex;
					bool endofline=false;

					while(endofline==false)
					{
						token.nextToken();

						if(token.ttype()==TT_NUMBER)
						{
							faceindex = token.nval();
							face.push_back(faceindex);
							//printf("faceindex: %d\n", faceindex);
						}
						else if(token.ttype()==TT_EOL || token.ttype()==TT_EOF)
						{
							temp_measuring_faces[mindex].push_back(face);
							endofline = true;
							//printf("end of line\n");
						}
					}				
				}				
				else if(label=="FACENORMAL")
				{
					Vector norm;

					//skip the equal sign
					token.nextToken(); 
					token.nextToken();
					if(token.ttype()==TT_NUMBER) norm.x = token.nval();
					//printf("token.nval(): %f\n",token.nval());
					token.nextToken();
					if(token.ttype()==TT_NUMBER) norm.y = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) norm.z = token.nval();

					if(fabs(norm.length()-1.0)>1e-6)
					{
						printf("NORMAL (%f, %f, %f)\n", norm.x, norm.y, norm.z);
						printf("NORMAL length = %f\n", norm.length());
						printf("ERROR: NORMAL is not a unit vector\n");
						return;
					}

					temp_measuring_norm[mindex].push_back(norm);
					//printf("norm: %f, %f, %f\n", norm.x, norm.y, norm.z);
				}
				else if (label=="FACEGROWTHDT")
				{
					float dT;

					//skip the equal sign
					token.nextToken(); 
					token.nextToken();

					if(token.ttype()==TT_NUMBER) dT = token.nval();

					temp_measuring_dt[mindex] = dT;
				}
				else if(label=="FACEGROWTH")
				{		
					float kmax, kmin;
					Vector vmax;

					//skip the equal sign
					token.nextToken(); 
					token.nextToken();

					if(token.ttype()==TT_NUMBER) kmax = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) kmin = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) vmax.x = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) vmax.y = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) vmax.z = token.nval();

					temp_measuring_kmax[mindex].push_back(kmax);
					temp_measuring_kmin[mindex].push_back(kmin);

					/*if(vmax.length()!=1.0)
					{
						printf("ERROR: VMAX is not a unit vector\n");
						return;
					}*/

					temp_measuring_vmax[mindex].push_back(vmax);

					//printf("kmax: %f, kmin: %f\n", kmax, kmin);
					//printf("vmax: %f, %f, %f\n", vmax.x, vmax.y, vmax.z);
				}	
				else //else we dont know what it is, go to next label
				{
					token.nextToken();
				}
			}
			else token.nextToken();
		}

		input_file.close();
	
		//Set the data in our MeasureObjects
		setData(temp_namestrings, temp_measuring_vertices, temp_measuring_edges, temp_measuring_faces, temp_measuring_drawmodes, 
				temp_measuring_edge_dt, temp_measuring_edge_kmax,
				temp_measuring_dt, temp_measuring_kmax, temp_measuring_kmin, temp_measuring_vmax, temp_measuring_vmin, 
				temp_measuring_norm);

	}
}
void MeasureListObject::open_fileversion2(QString fn)
{
	printf("MSR loading: %s\n", fn.latin1());
	if(!fn.isEmpty())
	{
		ifstream input_file; 
		input_file.open(fn.latin1());

		if(!input_file.is_open()) return;

		Tokenizer token(input_file);
		token.eolIsSignificant(true);

		string label;
		Vector v;
		Edge e;

		//MSR HEADER
		QString temp_originaldatastring;
		Vector temp_scale;
		int temp_object_count;
		QString temp_datestring;
		QString temp_timestring;

		//MSR BODY
		vector<QString> temp_namestrings;

		vector< int > temp_vertsize;
		vector< vector<Vector> > temp_measuring_vertices;

		vector< int > temp_edgesize;
		vector< vector<Edge> > temp_measuring_edges;

		vector< int > temp_facesize;
		vector< vector< vector<int> > > temp_measuring_faces;

		vector<int> temp_measuring_drawmodes;

		vector< float > temp_measuring_edge_dt;
		vector< vector< float > > temp_measuring_edge_kmax;

		vector< float > temp_measuring_dt;
		vector< vector< float > > temp_measuring_kmax;
		vector< vector< float > > temp_measuring_kmin;
		vector< vector< Vector > > temp_measuring_vmax;
		vector< vector< Vector > > temp_measuring_vmin;
		vector< vector< Vector > > temp_measuring_norm;
		
		vector< vector< vector< string > > > temp_vertlabel;
		vector< vector< vector< string > > > temp_edgelabel;
		vector< vector< vector< string > > > temp_facelabel;

		int mindex=-1;

		//until oef is reached step through each token
		while(!input_file.eof())
		{
			if(token.ttype()==TT_WORD)
			{
				//get the first string lable
				label = token.sval();
				
				if(label=="ORIGINALDATA")
				{
					token.nextToken();
					token.nextToken();
					temp_originaldatastring = tokenread_line(&token).data();

					//printf("original data: %s\n", temp_originaldatastring.toAscii().constData());
				}
				else if(label=="SCALE")
				{
					//skip the equal sign
					token.nextToken(); 
					token.nextToken();
					if(token.ttype()==TT_NUMBER) temp_scale.x = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) temp_scale.y = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) temp_scale.z = token.nval();

					//printf("scale: %f, %f, %f\n", temp_scale.x, temp_scale.y, temp_scale.z);
				}
				else if(label=="OBJECTCOUNT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						temp_object_count = token.nval();
						temp_measuring_vertices.resize(temp_object_count);
						temp_measuring_edges.resize(temp_object_count);
						temp_measuring_faces.resize(temp_object_count);
						temp_vertlabel.resize(temp_object_count);
						temp_edgelabel.resize(temp_object_count);
						temp_facelabel.resize(temp_object_count);
						
						temp_measuring_edge_dt.resize(temp_object_count);
						temp_measuring_edge_kmax.resize(temp_object_count);

						temp_measuring_norm.resize(temp_object_count);
						temp_measuring_kmin.resize(temp_object_count);
						temp_measuring_kmax.resize(temp_object_count);
						temp_measuring_vmax.resize(temp_object_count);
						temp_measuring_vmin.resize(temp_object_count);
						temp_measuring_dt.resize(temp_object_count);
					//	printf("object count: %d\n", temp_object_count);
					}
				}
				else if(label=="OBJECT")
				{
					//if(current_measurement.size()!=0) measuring_vertices.push_back(current_measurement);
					mindex++;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_STRING)
					{
						label = token.sval();
						QString name = label.data();
						temp_namestrings.push_back(name);
						//printf("object: %s\n", name.toAscii().constData());
					}
				}
				else if(label=="DATE")
				{
					token.nextToken();
					//token.nextToken();
					temp_datestring = tokenread_line(&token).data();

					//printf("date: %s\n", temp_datestring.toAscii().constData());
				}
				else if(label=="TIME")
				{
					token.nextToken();
					//token.nextToken();
					temp_timestring = tokenread_line(&token).data();

					//printf("time: %s\n", temp_timestring.toAscii().constData());
				}
				else if(label=="VERTCOUNT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						int size = token.nval();
						temp_vertsize.push_back(size);
					}

					//printf("VERTCOUNT: %d\n", temp_vertsize[mindex]);
				}
				else if(label=="VERT")
				{		
					//skip the equal sign
					token.nextToken(); 
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.x = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.y = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.z = token.nval();
					
					v.w = 0.0;
					temp_measuring_vertices[mindex].push_back(v);

					//printf("VERT size: %d\n", temp_measuring_vertices[mindex].size());
					//printf("VERT %f %f %f\n", v.x, v.y, v.z);
				}
				else if(label=="VERTLABEL")
				{		
					int numblbls;

					//skip the equal sign
					token.nextToken(); 
					token.nextToken();
					if(token.ttype()==TT_NUMBER) numblbls = token.nval();
					
					//printf("VERTLABEL: %d\n", numblbls);

					if(numblbls!=0)
					{
						bool endofline=false;

						vector < string > temp_labelstrings;

						token.nextToken();
						while(!endofline)
						{
							if(token.ttype()==TT_STRING)
							{
								label = token.sval();
								QString name = label.data();
								//printf(" %s\n", name.toAscii().constData());
								temp_labelstrings.push_back(name.toAscii().constData());
							
								endofline = false;
								token.nextToken();
							}
							else if(token.ttype()==TT_EOL || token.ttype()==TT_EOF)
							{
								temp_vertlabel[mindex].push_back(temp_labelstrings);
								endofline = true;
								//printf("end of line\n");
							}
						}	
					}
					else
					{
						vector < string > temp_labelstrings;
						temp_vertlabel[mindex].push_back(temp_labelstrings);
					}
				}
				else if(label=="EDGECOUNT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						int size = token.nval();
						temp_edgesize.push_back(size);
					}

					//printf("EDGECOUNT: %d\n", temp_edgesize[mindex]);
				}
				else if(label=="EDGE")
				{		
					//skip the equal sign
					token.nextToken(); 
					token.nextToken();
					if(token.ttype()==TT_NUMBER) e.v0 = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) e.v1 = token.nval();
				
					temp_measuring_edges[mindex].push_back(e);

					//printf("EDGE %d %d\n", e.v0, e.v1);
				}	
				else if(label=="EDGELABEL")
				{		
					int numblbls;

					//skip the equal sign
					token.nextToken(); 
					token.nextToken();
					if(token.ttype()==TT_NUMBER) numblbls = token.nval();
					
					//printf("EDGELABEL: %d\n", numblbls);

					if(numblbls!=0)
					{
						bool endofline=false;

						vector < string > temp_labelstrings;

						token.nextToken();
						while(!endofline)
						{
							if(token.ttype()==TT_STRING)
							{
								label = token.sval();
								QString name = label.data();
								//printf(" %s\n", name.toAscii().constData());
								temp_labelstrings.push_back(name.toAscii().constData());
							
								endofline = false;
								token.nextToken();
							}
							else if(token.ttype()==TT_EOL || token.ttype()==TT_EOF)
							{
								temp_edgelabel[mindex].push_back(temp_labelstrings);
								endofline = true;
								//printf("end of line\n");
							}
						}	
					}
					else
					{
						vector < string > temp_labelstrings;
						temp_edgelabel[mindex].push_back(temp_labelstrings);
					}
				}
				else if (label=="EDGEGROWTHDT")
				{
					float dT;

					//skip the equal sign
					token.nextToken(); 
					token.nextToken();

					if(token.ttype()==TT_NUMBER) dT = token.nval();

					temp_measuring_edge_dt[mindex] = dT;
				}
				else if(label=="EDGEGROWTH")
				{		
					float kmax;

					//skip the equal sign
					token.nextToken(); 
					token.nextToken();

					if(token.ttype()==TT_NUMBER) kmax = token.nval();

					temp_measuring_edge_kmax[mindex].push_back(kmax);
				}
				else if(label=="FACECOUNT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						int size = token.nval();
						temp_facesize.push_back(size);
					}

					//printf("FACECOUNT: %d\n", temp_facesize[mindex]);
				}
				else if(label=="FACE")
				{	
					//printf("FACE\n");

					//skip the equal sign
					token.nextToken(); 

					vector<int> face;
					int faceindex;
					bool endofline=false;

					token.nextToken();
					while(!endofline)
					{
						if(token.ttype()==TT_NUMBER)
						{
							faceindex = token.nval();
							face.push_back(faceindex);
							endofline = false;
							token.nextToken();
							//printf("faceindex: %d\n", faceindex);
						}
						else if(token.ttype()==TT_EOL || token.ttype()==TT_EOF)
						{
							temp_measuring_faces[mindex].push_back(face);
							endofline = true;
							//printf("end of line\n");
						}
					}	

					//printf("FACE ");
					//for(int i=0; i<face.size(); i++) printf(" %d ", face[i]);
					//printf("\n");
				}		
				else if(label=="FACENORMAL")
				{
					Vector norm;

					//skip the equal sign
					token.nextToken(); 
					token.nextToken();
					if(token.ttype()==TT_NUMBER) norm.x = token.nval();
					//printf("token.nval(): %f\n",token.nval());
					token.nextToken();
					if(token.ttype()==TT_NUMBER) norm.y = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) norm.z = token.nval();

					if(fabs(norm.length()-1.0)>1e-6)
					{
						//printf("NORMAL (%f, %f, %f)\n", norm.x, norm.y, norm.z);
						//printf("NORMAL length = %f\n", norm.length());
						//printf("ERROR: NORMAL is not a unit vector\n");
						return;
					}

					temp_measuring_norm[mindex].push_back(norm);
					//printf("norm: %f, %f, %f\n", norm.x, norm.y, norm.z);
				}				
				else if(label=="FACELABEL")
				{		
					int numblbls;

					//skip the equal sign
					token.nextToken(); 
					token.nextToken();
					if(token.ttype()==TT_NUMBER) numblbls = token.nval();
					
					//printf("FACELABEL: %d\n", numblbls);

					if(numblbls!=0)
					{
						bool endofline=false;

						vector < string > temp_labelstrings;

						token.nextToken();
						while(!endofline)
						{
							if(token.ttype()==TT_STRING)
							{
								label = token.sval();
								QString name = label.data();
								//printf(" %s\n", name.toAscii().constData());
								temp_labelstrings.push_back(name.toAscii().constData());
							
								endofline = false;
								token.nextToken();
							}
							else if(token.ttype()==TT_EOL || token.ttype()==TT_EOF)
							{
								temp_facelabel[mindex].push_back(temp_labelstrings);
								endofline = true;
								//printf("end of line\n");
							}
						}	
					}
					else
					{
						vector < string > temp_labelstrings;
						temp_facelabel[mindex].push_back(temp_labelstrings);
					}
				}
				else if (label=="FACEGROWTHDT")
				{
					float dT;

					//skip the equal sign
					token.nextToken(); 
					token.nextToken();

					if(token.ttype()==TT_NUMBER) dT = token.nval();

					temp_measuring_dt[mindex] = dT;
				}
				else if(label=="FACEGROWTH")
				{		
					float kmax, kmin;
					Vector vmax;

					//skip the equal sign
					token.nextToken(); 
					token.nextToken();

					if(token.ttype()==TT_NUMBER) kmax = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) kmin = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) vmax.x = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) vmax.y = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) vmax.z = token.nval();

					temp_measuring_kmax[mindex].push_back(kmax);
					temp_measuring_kmin[mindex].push_back(kmin);

					if(kmax<kmin)
					{
						printf("ERROR: KMIN is bigger than KMAX!!!\n");
					}
					/*if(vmax.length()!=1.0)
					{
						printf("ERROR: VMAX is not a unit vector\n");
						return;
					}*/
					temp_measuring_vmax[mindex].push_back(vmax);

					//printf("kmax: %f, kmin: %f\n", kmax, kmin);
					//printf("vmax: %f, %f, %f\n", vmax.x, vmax.y, vmax.z);
				}	
				else //else we dont know what it is, go to next label
				{
					token.nextToken();
				}
			}
			else
			{
				token.nextToken();
			}
		}
		input_file.close();
	
		//Rescale all other objects to same scale
		Vector rescale = scale/temp_scale;
		printf("rescale: %f, %f, %f\n", rescale.x, rescale.y, rescale.z);
		for(int i=0; i<measure_objects.size(); i++)
			measure_objects[i].rescale(rescale);

		original_data = temp_originaldatastring;
		scale = temp_scale;

		//Set the data in our MeasureObjects
		setData2(temp_namestrings, 
				 temp_measuring_vertices, temp_measuring_edges, temp_measuring_faces, 
				 temp_measuring_drawmodes, 
				 temp_measuring_edge_dt, temp_measuring_edge_kmax,
				 temp_measuring_dt, temp_measuring_kmax, temp_measuring_kmin, temp_measuring_vmax, temp_measuring_vmin, 
				 temp_measuring_norm, 
				 temp_vertlabel, temp_edgelabel, temp_facelabel);
	}
}
void MeasureListObject::open_file(QString fn)
{
	float versumb = check_file_version(fn);
	versumb += 0;

	if(versumb==-1.0f)
	{
		printf("MSR_VERSION=0\n");
		open_fileversion0(fn);
	}
	else if(versumb==0.1f)
	{
		printf("MSR_VERSION=0.1\n");
		open_fileversion1(fn);
	}
	else if(versumb==1.2f)
	{
		printf("MSR_VERSION=1.2\n");
		open_fileversion2(fn);
	}
	else
	{
		printf("WARNING: UNKNOWN MSR_VERSION!\n");
	}
}
void MeasureListObject::setData(vector<QString> temp_namestrings, vector< vector<Vector> > temp_measuring_vertices, vector< vector<Edge> > temp_measuring_edges, vector< vector< vector<int> > > temp_measuring_faces, vector<int> temp_measuring_drawmodes, vector<float> temp_measuring_edge_dt, vector< vector<float> > temp_measuring_edge_kmax, vector<float> temp_measuring_dt, vector< vector<float> > temp_measuring_kmax, vector< vector<float> > temp_measuring_kmin, vector< vector< Vector > > temp_measuring_vmax, vector< vector< Vector > > temp_measuring_vmin, vector< vector< Vector > > temp_measuring_norm)
{
	//First we check data validity.
	//TODO
	
	//Now assign the data
	for(int i=0; i<temp_measuring_vertices.size(); i++)
	{	
		MeasureObject obj = MeasureObject(temp_measuring_vertices[i], temp_measuring_edges[i], temp_measuring_faces[i]);
		obj.name = temp_namestrings[i].toLatin1().data();
		if(!temp_measuring_kmin.empty()) obj.setgrowth(temp_measuring_edge_dt[i], temp_measuring_edge_kmax[i], temp_measuring_dt[i], temp_measuring_kmin[i], temp_measuring_kmax[i], temp_measuring_vmin[i], temp_measuring_vmax[i]);
		if(temp_measuring_drawmodes.size()>i) obj.drawmode = temp_measuring_drawmodes[i];
		else obj.drawmode = MEASURE_DRAWMODE_MAGNETLINE;

		add(obj);
	}	
}
void MeasureListObject::setData2(vector<QString> temp_namestrings, vector< vector<Vector> > temp_measuring_vertices, vector< vector<Edge> > temp_measuring_edges, vector< vector< vector<int> > > temp_measuring_faces, vector<int> temp_measuring_drawmodes, vector<float> temp_measuring_edge_dt, vector< vector<float> > temp_measuring_edge_kmax, vector<float> temp_measuring_dt, vector< vector<float> > temp_measuring_kmax, vector< vector<float> > temp_measuring_kmin, vector< vector< Vector > > temp_measuring_vmax, vector< vector< Vector > > temp_measuring_vmin, vector< vector< Vector > > temp_measuring_norm, vector< vector< vector< string > > > temp_vertlabel, vector< vector< vector< string > > > temp_edgelabel, vector< vector< vector< string > > > temp_facelabel)
{
	//First we check data validity.
	//TODO

	//Now assign the data
	for(int i=0; i<temp_measuring_vertices.size(); i++)
	{		
		MeasureObject obj = MeasureObject(temp_measuring_vertices[i], temp_measuring_edges[i], temp_measuring_faces[i]);
		obj.name = temp_namestrings[i].toLatin1().data();

		if(!temp_vertlabel.empty() || !temp_edgelabel.empty() || !temp_facelabel.empty()) obj.setlabels(temp_vertlabel[i], temp_edgelabel[i], temp_facelabel[i]);
		if(!temp_measuring_kmin.empty()) obj.setgrowth(temp_measuring_edge_dt[i], temp_measuring_edge_kmax[i], temp_measuring_dt[i], temp_measuring_kmin[i], temp_measuring_kmax[i], temp_measuring_vmin[i], temp_measuring_vmax[i]);
			
		if(temp_measuring_drawmodes.size()>i) obj.drawmode = temp_measuring_drawmodes[i];
		else obj.drawmode = MEASURE_DRAWMODE_MAGNETLINE;

		add(obj);
	}
}
const MeasureListObject& MeasureListObject::operator= (const MeasureListObject& right)
{
	clear();

	for(int i=0; i<right.measure_objects.size(); i++)
		measure_objects.push_back(right.measure_objects[i]);

	for(int i=0; i<right.visible.size(); i++)
		visible.push_back(right.visible[i]);

	return *this;
}
