/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "MeasureObject.h"
#include<glut.h>

MeasureObject::MeasureObject()
{
	growth_scale = 1.0;
}
MeasureObject::MeasureObject(vector< Vector > v, vector< Edge > e, vector< vector<int> > f)
{
	clear();

	for(int i=0; i<v.size(); i++)
	{
		vertices.push_back(v[i]);
	}

	for(int i=0; i<e.size(); i++)
	{
		edges.push_back(e[i]);
	}

	faces.resize(f.size());
	for(int i=0; i<f.size(); i++)
	{
		for(int j=0; j<f[i].size(); j++)
		{
			faces[i].push_back(f[i][j]);
		}
	}
}
MeasureObject::~MeasureObject()
{
}
void MeasureObject::clear(void)
{
	vertices.clear();
	edges.clear();

	face_normals.clear();
	face_colours.clear();

	for(int i=0; i<faces.size(); i++)
	{
		faces[i].clear();
	}
	faces.clear();

	face_kmin.clear();
	face_kmax.clear();
	face_vmin.clear();
	face_vmax.clear();

	karea.clear();
	anisotropy.clear();

	for(int i=0; i<vertex_labels.size(); i++)
	{
		vertex_labels[i].clear();
	}
	vertex_labels.clear();

	for(int i=0; i<edge_labels.size(); i++)
	{
		edge_labels[i].clear();
	}
	edge_labels.clear();

	for(int i=0; i<face_labels.size(); i++)
	{
		face_labels[i].clear();
	}
	face_labels.clear();

	vertex_list_names.clear();
	for(int i=0; i<vertex_list.size(); i++)
	{
		vertex_list[i].clear();
	}
	vertex_list.clear();

	edge_list_names.clear();
	for(int i=0; i<edge_list.size(); i++)
	{
		edge_list[i].clear();
	}
	edge_list.clear();

	face_list_names.clear();
	for(int i=0; i<face_list.size(); i++)
	{
		face_list[i].clear();
	}
	face_list.clear();
}
void MeasureObject::setgrowth(float edge_dt, vector<float> edge_kmx, float dt, vector<float> kmi, vector<float> kma, vector< Vector > vmi, vector< Vector > vma)
{
	//printf("setting growth\n");
	face_kmin.clear();
	face_kmax.clear();
	face_vmin.clear();
	face_vmax.clear();
	karea.clear();
	anisotropy.clear();

	edge_dT = edge_dt;

	edge_kmax.resize(edge_kmx.size());
	for(int i=0; i<edge_kmx.size(); i++) edge_kmax[i] = edge_kmx[i];

	face_dT =dt;

	face_kmin.resize(kmi.size());
	for(int i=0; i<kmi.size(); i++) face_kmin[i] = kmi[i];

	face_kmax.resize(kma.size());
	for(int i=0; i<kma.size(); i++)
	{
			face_kmax[i] = kma[i];
	}
	face_vmin.resize(vmi.size());
	for(int i=0; i<vmi.size(); i++) face_vmin[i] = vmi[i];

	face_vmax.resize(vma.size());
	for(int i=0; i<vma.size(); i++)
	{
			face_vmax[i] = vma[i];
	}
	compute_growth();
	evaluate_karea();
	evaluate_anisotropy();

	/*printf("face_kmin: %f\n", face_kmin.size());
	printf("face_kmax: %f\n", face_kmax.size());
	printf("face_vmin: %f\n", face_vmin.size());
	printf("face_vmax: %f\n", face_vmax.size());*/

	//	printf("avg_edgel: %f\n", avg_edgel);
//	printf("growth_scale: %f\n", growth_scale);
}
void MeasureObject::setlabels(vector< vector< string > > vlabels, vector< vector< string > > elabels, vector< vector< string > > flabels)
{
	vertex_labels.resize(vlabels.size());
	for(int i=0; i<vlabels.size(); i++)
		for(int j=0; j<vlabels[i].size(); j++)
			vertex_labels[i].push_back(vlabels[i][j]);

	edge_labels.resize(elabels.size());
	for(int i=0; i<elabels.size(); i++)
		for(int j=0; j<elabels[i].size(); j++)
			edge_labels[i].push_back(elabels[i][j]);

	face_labels.resize(flabels.size());
	for(int i=0; i<flabels.size(); i++)
		for(int j=0; j<flabels[i].size(); j++)
			face_labels[i].push_back(flabels[i][j]);

//	printf("vertex_labels size: %d\n", vertex_labels.size());
//	for(int i=0; i<vertex_labels.size(); i++)
//		printf("labels: %d\n", vertex_labels[i].size());
}
void MeasureObject::compute_growth(void)
{
	if(face_normals.empty())
	{
		for(int j=0; j<faces.size(); j++)
		{
			Vector norm;
			norm.get_normal(vertices[faces[j][2]], vertices[faces[j][1]], vertices[faces[j][0]]);
			norm.normalize();

			face_normals.push_back(norm);
		}
	}

	if(face_vmax.empty()) return;

	//ofstream fout;
	//fout.open("Growth.txt");
	
	Vector vface_vmin;
	face_vmin.clear();
	float max_face_kmax=0.0;
	for(int j=0; j<face_normals.size(); j++)
	{
		vface_vmin.cross(face_vmax[j], face_normals[j]);
		face_vmin.push_back(vface_vmin);

		Vector tempv;
		float tempf;
		if(face_kmax[j]<face_kmin[j])
		{
			tempv = face_vmin[j];
			tempf = face_kmin[j];

			face_vmin[j] = face_vmax[j];
			face_kmin[j] = face_kmax[j];
			face_vmax[j] = tempv;
			face_kmax[j] = tempf;
		}

		//fout<<face_kmin[j]<<","<<face_kmax[j]<<endl;

		if(face_kmax[j]>max_face_kmax) max_face_kmax = face_kmax[j];
	}
	if(max_face_kmax==0.0) max_face_kmax=1.0;

	//fout.close();

	float avg_edgel=0.0;
	for(int j=0; j<edges.size(); j++)
	{
		Vector vl = vertices[edges[j].v0]-vertices[edges[j].v1];

		avg_edgel += vl.length();
	}
	avg_edgel /= edges.size();
	if(avg_edgel==0.0) avg_edgel=1.0;

	growth_scale = (avg_edgel/max_face_kmax)*0.5f;
}
bool MeasureObject::find_edge(Edge e)
{
	for(int i=0; i<edges.size(); i++)
	{
		if(e.v0==edges[i].v0 && e.v1==edges[i].v1) return true;
		if(e.v1==edges[i].v0 && e.v0==edges[i].v1) return true;
	}

	return false;
}
void MeasureObject::build_edges(void)
{
	edges.clear();
	int v1, v2, v3;
	Edge e;
	for(int i=0; i<faces.size(); i++)
	{
		if(faces[i].size()==3)
		{
			v1 = faces[i][0];
			v2 = faces[i][1];
			v3 = faces[i][2];

			//e1 = v1 -> v2
			e.v0=v1; e.v1=v2;
			if(!find_edge(e)) edges.push_back(e);

			//e2 = v2 -> v3
			e.v0=v2; e.v1=v3;
			if(!find_edge(e)) edges.push_back(e);

			//e3 = v3 -> v1
			e.v0=v3; e.v1=v1;
			if(!find_edge(e)) edges.push_back(e);
		}
	}
}
void MeasureObject::evaluate_karea(void)
{
	if(face_kmax.empty()) return;

	karea.clear();

	maxkarea = -10000000000000000000.0;
	minkarea = 10000000000000000000.0;
	for(int i=0; i<face_kmax.size(); i++)
	{
		float kareaf = face_kmin[i] + face_kmax[i];

		if(kareaf>maxkarea) maxkarea = kareaf;
		if(kareaf<minkarea) minkarea = kareaf;

		karea.push_back(kareaf);
	}

	if(maxkarea==minkarea) maxkarea += 1.0;

	printf("maxkarea: %f\n", maxkarea);
	printf("minkarea: %f\n", minkarea);
}
void MeasureObject::evaluate_anisotropy(void)
{
	if(face_kmax.empty()) return;

	anisotropy.clear();

	maxaniso = -10000000000000000000.0;
	minaniso = 10000000000000000000.0;
	for(int i=0; i<face_kmax.size(); i++)
	{
		//printf("face_kmin: %f	face_kmax: %f\n", face_kmin[i], face_kmax[i]);

		//float aniso = (1.0-(face_kmin[i]/face_kmax[i]));
		float aniso = face_kmax[i]-face_kmin[i];

		if(aniso>maxaniso) maxaniso = aniso;
		if(aniso<minaniso) minaniso = aniso;

		anisotropy.push_back(aniso);
	}

	if(maxaniso==minaniso) maxaniso += 1.0;

	printf("maxaniso: %f\n", maxaniso);
	printf("minaniso: %f\n", minaniso);
}
void MeasureObject::display_selected_vert(int selected_vert)
{
	if(selected_vert>-1 && selected_vert<vertices.size())
	{
		glBegin(GL_POINTS);
		glVertex3fv(&vertices[selected_vert].x);	
		glEnd();
	}
}
void MeasureObject::display_selected_edge(int selected_edge)
{
	if(selected_edge>-1 && selected_edge<edges.size())
	{
		Vector v0 = vertices[edges[selected_edge].v0];
		Vector v1 = vertices[edges[selected_edge].v1];

		/*Vector edgevec = v0 - v1;
		float elength = edgevec.length();
		edgevec.normalize();
		elength *= 0.2;

		v0 = v0 - (elength*edgevec);
		v1 = v1 + (elength*edgevec);*/

		glBegin(GL_LINES);
			glVertex3fv(&v0.x);
			glVertex3fv(&v1.x);
		glEnd();

		/*
		glBegin(GL_LINES);
		glVertex3fv(&vertices[edges[selected_edge].v0].x);
		glVertex3fv(&vertices[edges[selected_edge].v1].x);
		glEnd();
		*/
	}
}
void MeasureObject::display_selected_face(int selected_face)
{
	Vector normal;
	if(selected_face>-1 && selected_face<faces.size())
	{
		glBegin(GL_POLYGON);
		for(int j=0; j<faces[selected_face].size(); j++)
		{
			glVertex3fv(&vertices[faces[selected_face][j]].x);
		}
		glEnd();
	}
}
void MeasureObject::display_verts(void)
{
	glDisable(GL_LIGHTING);
	glBegin(GL_POINTS);
	for(int i=0; i<vertices.size(); i++)
	{
		glVertex3fv(&vertices[i].x);
	}
	glEnd();
	glDisable(GL_LIGHTING);
}
void MeasureObject::display_edges(void)
{
	glDisable(GL_LIGHTING);
	//EDGES
	glBegin(GL_LINES);
	for(int i=0; i<edges.size(); i++)
	{
		glVertex3fv(&vertices[edges[i].v0].x);
		glVertex3fv(&vertices[edges[i].v1].x);
	}
	glEnd();
	glDisable(GL_LIGHTING);
}
void MeasureObject::display_arrowheads(float arrow_size)
{
	//float arrow_size = 2.0;

	glDisable(GL_LIGHTING);
	Vector d;
	Vector z = Vector(0,0,-1);
	Vector axis;
	float angle;

	GLUquadricObj* cylinder = gluNewQuadric();
	gluQuadricDrawStyle( cylinder, GLU_FILL );

	for(int i=0; i<edges.size(); i++)
	{
		d = vertices[edges[i].v1] - vertices[edges[i].v0];
		d.normalize();

		axis.cross(d, z);
		axis.normalize();

		angle = z.signed_angle(d, axis);
		
		//printf("angle: %f\n", angle*TWOPI);

		Vector edgevec = vertices[edges[i].v1] - vertices[edges[i].v0];
		float leng = edgevec.length();
		edgevec.normalize();

		Vector pos = vertices[edges[i].v1];
		//pos = pos - (edgevec*arrow_size*5.0f);

		glPushMatrix();
		glTranslatef(pos.x, pos.y, pos.z);
		glRotatef(angle*(180.0/PI), axis.x, axis.y, axis.z);
		
		gluCylinder(cylinder, 0.0, arrow_size, arrow_size*5.0, 10, 20);
		gluDisk(cylinder, 0.0, arrow_size, 30, 20);
		glPopMatrix();

	}

	gluDeleteQuadric(cylinder);
	glDisable(GL_LIGHTING);
}
void MeasureObject::display_faces(void)
{
	//FACES
	glEnable(GL_LIGHTING);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);
	for(int i=0; i<faces.size(); i++)
	{
	//	if(i==0) glColor3f(0.0, 0.25, 0.0);
	//	else if(i==1) glColor3f(0.25, 0.0, 0.0);
		if(faces[i].size()>2)
		{
			Vector normal;
			normal.get_normal(vertices[faces[i][0]], vertices[faces[i][1]], vertices[faces[i][2]]); 
			normal.normalize();

		/*	if(i>0 && i<100) glColor3f(0.5, 0.5, 0.0);
			else if(i>260 && i<344) glColor3f(0.5, 0.5, 0.0);
			else
				glColor3f(0.5, 0.5, 0.5);
*/
			//if(i==321) glColor3f(0,1,0);

			glNormal3fv(&normal.x);
			glBegin(GL_POLYGON);
			for(int j=0; j<faces[i].size(); j++)
			{
				//printf("vec: %f, %f, %f\n", vertices[faces[i][j]].x, vertices[faces[i][j]].y, vertices[faces[i][j]].z);
				glVertex3fv(&vertices[faces[i][j]].x);
			}
			glEnd();
		}
	}
	glDisable(GL_LIGHTING);
	glDisable(GL_POLYGON_OFFSET_FILL);
}
void MeasureObject::display_growth(void)
{
	glDisable(GL_LIGHTING);
	//GROWTH
	Vector f_centre;
	glLineWidth(2.0);
	for(int i=0; i<face_vmax.size(); i++)
	{
		//evaluate the face centroid
		f_centre = 0.0;
		for(int j=0; j<faces[i].size(); j++)
		{
			f_centre += vertices[faces[i][j]];
		}
		f_centre /= (float) faces[i].size();

		Matrix4x4 mat;
		Vector norm; 
		norm.cross(face_vmax[i], face_vmin[i]);
		norm.normalize();
		mat.find_rotationfromaxis(face_vmax[i], face_vmin[i], norm, Vector(1,0,0), Vector(0,1,0), Vector(0,0,1));

		glColor3f(0.75,0.75,0.75);
		//glEnable(GL_LIGHTING);
		//glEnable(GL_POLYGON_OFFSET_FILL);
		//glPolygonOffset(0.5, 0.5);
		//render_ellipsoid(f_centre, Vector(1.05f*growth_scale*(face_kmax[i]*0.5f), 1.05f*growth_scale*(face_kmin[i]*0.5f), 0.0), &mat, 32, GL_POLYGON);
		glDisable(GL_POLYGON_OFFSET_FILL);
		//glColor3f(0.5,0.5,0.5);
		render_ellipsoid(f_centre, Vector(growth_scale*(face_kmax[i]*0.5f), growth_scale*(face_kmin[i]*0.5f), 0.0), &mat, 32, GL_POLYGON);
		//glDisable(GL_LIGHTING);

		Vector gv1 = f_centre + growth_scale*((face_kmin[i]*0.5f)*face_vmin[i]);
		Vector gv2 = f_centre - growth_scale*((face_kmin[i]*0.5f)*face_vmin[i]);

		//printf("gv1: %f, %f, %f\n", gv1.x, gv1.y, gv1.z);
		//printf("gv2: %f, %f, %f\n", gv2.x, gv2.y, gv2.z);

		glLineWidth(4.0);
		glColor3f(0,1,0);
		glBegin(GL_LINES);
			glVertex3fv(&gv1.x);
			glVertex3fv(&gv2.x);
		glEnd();

		gv1 = f_centre + growth_scale*((face_kmax[i]*0.5f)*face_vmax[i]);
		gv2 = f_centre - growth_scale*((face_kmax[i]*0.5f)*face_vmax[i]);
		
		glColor3f(1,0,0);
		glBegin(GL_LINES);
			glVertex3fv(&gv1.x);
			glVertex3fv(&gv2.x);
		glEnd();
	}
	glDisable(GL_LIGHTING);
}
void MeasureObject::display_direction(void)
{
	glDisable(GL_LIGHTING);
	//DIRECTION - face_kmax
	Vector f_centre;
	Vector gv1;
	Vector gv2;
	glLineWidth(2.0);
	for(int i=0; i<faces.size(); i++)
	{
		//evaluate the face centroid
		f_centre = 0.0;
		for(int j=0; j<faces[i].size(); j++)
		{
			f_centre += vertices[faces[i][j]];
		}
		f_centre /= (float) faces[i].size();

		//display the face_kmax direction
		gv1 = f_centre + 0.5f*growth_scale*((0.5f)*face_vmax[i]); //face_kmax[i]*
		gv2 = f_centre - 0.5f*growth_scale*((0.5f)*face_vmax[i]); //face_kmax[i]*
		
		glColor3f(0,0,0);
		glBegin(GL_LINES);
			glVertex3fv(&gv1.x);
			glVertex3fv(&gv2.x);
		glEnd();
	}
	glDisable(GL_LIGHTING);
}
void MeasureObject::display_kminmax(void)
{
	if(face_vmax.empty()) return;
	if(face_vmin.empty()) return;

	//face_kmin/face_kmax
	glDisable(GL_LIGHTING);
	Vector f_centre;
	glLineWidth(2.0);
	for(int i=0; i<faces.size(); i++)
	{
		//evaluate the face centroid
		f_centre = 0.0;
		for(int j=0; j<faces[i].size(); j++)
		{
			f_centre += vertices[faces[i][j]];
		}
		f_centre /= (float) faces[i].size();

		Matrix4x4 mat;
		Vector norm; 
		norm.cross(face_vmax[i], face_vmin[i]);
		norm.normalize();
		mat.find_rotationfromaxis(face_vmax[i], face_vmin[i], norm, Vector(1,0,0), Vector(0,1,0), Vector(0,0,1));

		glColor3f(0.25,0.25,0.25);
		//glEnable(GL_LIGHTING);
		//glEnable(GL_POLYGON_OFFSET_FILL);
		//glPolygonOffset(0.5, 0.5);
		//render_ellipsoid(f_centre, Vector(1.05f*growth_scale*(face_kmax[i]*0.5f), 1.05f*growth_scale*(face_kmin[i]*0.5f), 0.0), &mat, 32, GL_POLYGON);
		glDisable(GL_POLYGON_OFFSET_FILL);
		//glColor3f(0.5,0.5,0.5);
		render_ellipsoid(f_centre, Vector(growth_scale*(face_kmax[i]*0.5f), growth_scale*(face_kmin[i]*0.5f), 0.0), &mat, 32, GL_POLYGON);
		//glDisable(GL_LIGHTING);

		Vector gv1 = f_centre + growth_scale*((face_kmin[i]*0.5f)*face_vmin[i]);
		Vector gv2 = f_centre - growth_scale*((face_kmin[i]*0.5f)*face_vmin[i]);

		//printf("gv1: %f, %f, %f\n", gv1.x, gv1.y, gv1.z);
		//printf("gv2: %f, %f, %f\n", gv2.x, gv2.y, gv2.z);

		glLineWidth(4.0);
		glColor3f(0,1,0);
		glBegin(GL_LINES);
			glVertex3fv(&gv1.x);
			glVertex3fv(&gv2.x);
		glEnd();

		gv1 = f_centre + growth_scale*((face_kmax[i]*0.5f)*face_vmax[i]);
		gv2 = f_centre - growth_scale*((face_kmax[i]*0.5f)*face_vmax[i]);
		
		glColor3f(1,0,0);
		glBegin(GL_LINES);
			glVertex3fv(&gv1.x);
			glVertex3fv(&gv2.x);
		glEnd();
	}

	glDisable(GL_LIGHTING);
	//printf("--------------------\n");
}
void MeasureObject::display_karea(void)
{
	if(karea.empty()) return;

	//FACES
	glDisable(GL_LIGHTING);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);
	for(int i=0; i<faces.size(); i++)
	{
		Vector c = colmap.compute_colour(minkarea, maxkarea, karea[i]) / 255.0f;
		glColor3fv(&c.x);

		if(faces.size()==face_normals.size()) glNormal3fv(&face_normals[i].x);
		glBegin(GL_POLYGON);
		for(int j=0; j<faces[i].size(); j++)
		{
			glVertex3fv(&vertices[faces[i][j]].x);
		}
		glEnd();
	}
	glDisable(GL_LIGHTING);
}
void MeasureObject::display_anisotropy(void)
{
	if(anisotropy.empty()) return;

	//FACES
	glDisable(GL_LIGHTING);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);
	for(int i=0; i<faces.size(); i++)
	{
		Vector c = colmap.compute_colour(minaniso, maxaniso, anisotropy[i])/255.0f;
		glColor3fv(&c.x);

		if(faces.size()==face_normals.size()) glNormal3fv(&face_normals[i].x);
		glBegin(GL_POLYGON);
		for(int j=0; j<faces[i].size(); j++)
		{
			glVertex3fv(&vertices[faces[i][j]].x);
		}
		glEnd();
	}
	glDisable(GL_LIGHTING);
}

void MeasureObject::display(void)
{
	//selected edge = measure_magnetline_edge
	//selected face = current_measure_faceindex
	//selected vert = current_measure_vertindex

	//VERTICES
	glBegin(GL_POINTS);
	for(int i=0; i<vertices.size(); i++)
	{
		glVertex3fv(&vertices[i].x);
	}
	glEnd();

	//EDGES
	glBegin(GL_LINES);
	for(int i=0; i<edges.size(); i++)
	{
		glVertex3fv(&vertices[edges[i].v0].x);
		glVertex3fv(&vertices[edges[i].v1].x);
	}
	glEnd();
/*
	//FACES
	Vector normal;
	glColor4f(1.0,1.0,1.0,1);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0, 1.0);
	glEnable(GL_LIGHTING);
	for(int i=0; i<faces.size(); i++)
	{
	//	if(i==0) glColor3f(0.0, 0.25, 0.0);
	//	else if(i==1) glColor3f(0.25, 0.0, 0.0);

		glBegin(GL_POLYGON);
		//if(faces[i].size()>2)
		{
			normal.get_normal(vertices[faces[i][0]], vertices[faces[i][1]], vertices[faces[i][2]]); 
			normal.normalize();
			glNormal3fv(&normal.x);
		}

		for(int j=0; j<faces[i].size(); j++)
		{
			glVertex3fv(&vertices[faces[i][j]].x);
		}
		glEnd();
	}
	glDisable(GL_LIGHTING);
*/

	/*
	//GROWTH
	if(face_vmax.size()!=faces.size()) return;
	Vector f_centre;
	glLineWidth(2.0);
	for(int i=0; i<face_vmax.size(); i++)
	{
		//evaluate the face centroid
		f_centre = 0.0;
		for(int j=0; j<faces[i].size(); j++)
		{
			f_centre += vertices[faces[i][j]];
		}
		f_centre /= (float) faces[i].size();

		Matrix4x4 mat;
		Vector norm; 
		norm.cross(face_vmax[i], face_vmin[i]);
		norm.normalize();
		mat.find_rotationfromaxis(face_vmax[i], face_vmin[i], norm, Vector(1,0,0), Vector(0,1,0), Vector(0,0,1));

		glColor3f(0.75,0.75,0.75);
		//glEnable(GL_LIGHTING);
		//glEnable(GL_POLYGON_OFFSET_FILL);
		//glPolygonOffset(0.5, 0.5);
		//render_ellipsoid(f_centre, Vector(1.05f*growth_scale*(face_kmax[i]*0.5f), 1.05f*growth_scale*(face_kmin[i]*0.5f), 0.0), &mat, 32, GL_POLYGON);
		glDisable(GL_POLYGON_OFFSET_FILL);
		//glColor3f(0.5,0.5,0.5);
		render_ellipsoid(f_centre, Vector(growth_scale*(face_kmax[i]*0.5f), growth_scale*(face_kmin[i]*0.5f), 0.0), &mat, 32, GL_POLYGON);
		//glDisable(GL_LIGHTING);

		Vector gv1 = f_centre + growth_scale*((face_kmin[i]*0.5f)*face_vmin[i]);
		Vector gv2 = f_centre - growth_scale*((face_kmin[i]*0.5f)*face_vmin[i]);

		//printf("gv1: %f, %f, %f\n", gv1.x, gv1.y, gv1.z);
		//printf("gv2: %f, %f, %f\n", gv2.x, gv2.y, gv2.z);

		glLineWidth(4.0);
		glColor3f(0,1,0);
		glBegin(GL_LINES);
			glVertex3fv(&gv1.x);
			glVertex3fv(&gv2.x);
		glEnd();

		gv1 = f_centre + growth_scale*((face_kmax[i]*0.5f)*face_vmax[i]);
		gv2 = f_centre - growth_scale*((face_kmax[i]*0.5f)*face_vmax[i]);
		
		glColor3f(1,0,0);
		glBegin(GL_LINES);
			glVertex3fv(&gv1.x);
			glVertex3fv(&gv2.x);
		glEnd();
	}
	*/
	//printf("--------------------\n");
}
void MeasureObject::render_ellipsoid(Vector c, Vector r,  Matrix4x4* rotmat, int n, GLenum drawmode)
{
	int i,j;
	double theta1,theta2,theta3;
	Vector rr = Vector(1.0, 1.0, 1.0);
	Vector e,p, p1;

	for (j=0;j<n;j++) 
	{
		theta1 = j * TWOPI / n - (PI/2.0);
		theta2 = (j + 1) * TWOPI / n - (PI/2.0);

		glBegin(drawmode);
		//glBegin(GL_QUADS);
		//glBegin(GL_POINTS);
		for (i=0;i<=n;i++) 
		{
			theta3 = i * TWOPI / n;

			e.x = cos(theta2) * cos(theta3);
			e.y = sin(theta2);
			e.z = cos(theta2) * sin(theta3);

			//generate point on unit sphere
			p = r*e;
			
			//transform it to our ellipsoid
			p1 = *rotmat*p;

			//apply the centre translation
			p1 = c + p1;

			glNormal3f(e.x,e.y,e.z);
			glTexCoord2f(i/(double)n,2*(j+1)/(double)n);
			glVertex3fv(&p1.x);

			e.x = cos(theta1) * cos(theta3);
			e.y = sin(theta1);
			e.z = cos(theta1) * sin(theta3);

			//generate point on unit sphere
			p = r*e;

			//transform it to our ellipsoid
			p1 = *rotmat*p;

			//apply the centre translation
			p1 = c + p1;

			glNormal3f(e.x,e.y,e.z);
			glTexCoord2f(i/(double)n,2*j/(double)n);
			glVertex3fv(&p1.x);
			
		}
		glEnd();
	}
}
bool MeasureObject::validateMSR(void)
{
	if(edges.empty()) build_edges();

	//check all vertices belong to edges
	vector < bool > foundvert;
	foundvert.resize(vertices.size());
	for(int i=0; i<foundvert.size(); i++)
		foundvert[i] = false;

	for(int i=0; i<edges.size(); i++)
	{
		foundvert[edges[i].v0] = true;
		foundvert[edges[i].v1] = true;
	}

	for(int i=0; i<foundvert.size(); i++)
	{
		if(foundvert[i]==false)
		{
			printf("ERROR: Vertex %d does not belong to any edges\n", i);
			return false;
		}
	}

	//check all edges belong to faces
	vector < vector< int > > face_edges;

	int edge0, edge1;
	for(int k=0; k<edges.size(); k++)
	{
		edge0 = edges[k].v0;
		edge1 = edges[k].v1;
			
		bool hasedge0 = false;
		bool hasedge1 = false;
		vector< int > fedges;
		for(int l=0; l<faces.size(); l++)
		{
			hasedge0 = false;
			hasedge1 = false;

			for(int m=0; m<faces[l].size(); m++)
			{
				if(faces[l][m]==edge0)
				{
					hasedge0=true;
				}
				if(faces[l][m]==edge1)
				{
					hasedge1=true;
				}
			}

			if(hasedge0==true && hasedge1==true)
			{
				fedges.push_back(l);
			}
		}

		face_edges.push_back(fedges);
	}

	for(int k=0; k<face_edges.size(); k++)
	{
		if(face_edges[k].empty())
		{
			printf("ERROR: Edge %d does not belong to any faces\n", k);
			return false;
		}
	}

	return true;
}

void MeasureObject::rescale(Vector rescale_factor)
{
	for(int i=0; i<vertices.size(); i++)
	{
		vertices[i] = vertices[i] * rescale_factor;
	}
}
double MeasureObject::bezierBlend(int k, double mu, int n)
{
	int nn,kn,nkn;
	double blend=1;

	nn = n;
	kn = k;
	nkn = n - k;

   while (nn >= 1) {
      blend *= nn;
      nn--;
      if (kn > 1) {
         blend /= (double)kn;
         kn--;
      }
      if (nkn > 1) {
         blend /= (double)nkn;
         nkn--;
      }
   }
   if (k > 0)
      blend *= pow(mu,(double)k);
   if (n-k > 0)
      blend *= pow(1-mu,(double)(n-k));

   return(blend);
}
void MeasureObject::display_beizer_patch(void)
{
	int numb_controlpnts = 3;

	if(vertices.size()!=numb_controlpnts*numb_controlpnts) return;

	int numb_i = numb_controlpnts-1;
	int numb_j = numb_controlpnts-1;

	vector< vector < Vector > > verts;
	verts.resize(numb_controlpnts);
	for(int i=0; i<verts.size(); i++) verts[i].resize(numb_controlpnts);

	for(int i=0; i<numb_controlpnts; i++)
		for(int j=0; j<numb_controlpnts; j++)
				verts[i][j] = vertices[i*numb_controlpnts+j];

	int sampling = 10;

	int res_i = 10*numb_i;
	int res_j = 10*numb_j;

	for(int i=0; i<bpatch.size(); i++) bpatch[i].clear();
	bpatch.clear();

	bpatch.resize(res_i);
	for(int i=0; i<bpatch.size(); i++) bpatch[i].resize(res_j);

	int i,j,ki,kj;
	float mui,muj,bi,bj;

   for (i=0;i<res_i;i++) {
	  mui = i / (double)(res_i-1);
	  for (j=0;j<res_j;j++) {
		 muj = j / (double)(res_j-1);
		 bpatch[i][j].x = 0;
		 bpatch[i][j].y = 0;
		 bpatch[i][j].z = 0;
		 for (ki=0;ki<=numb_i;ki++) {
			bi = bezierBlend(ki,mui,numb_i);
			for (kj=0;kj<=numb_j;kj++) {
			   bj = bezierBlend(kj,muj,numb_j);
			   bpatch[i][j].x += (verts[ki][kj].x * bi * bj);
			   bpatch[i][j].y += (verts[ki][kj].y * bi * bj);
			   bpatch[i][j].z += (verts[ki][kj].z * bi * bj);
			}
		 }
	  }
   }

   glBegin(GL_QUADS);
   for (i=0;i<res_i-1;i++) 
   {
      for (j=0;j<res_j-1;j++) 
	  {
            glVertex3fv(&bpatch[i][j].x);
            glVertex3fv(&bpatch[i][j+1].x);
            glVertex3fv(&bpatch[i+1][j+1].x);
            glVertex3fv(&bpatch[i+1][j].x);
      }
   }
   glEnd();
}
void MeasureObject::display_beizer_patch_texture(Vector res)
{
	int numb_controlpnts = 3;

	if(vertices.size()!=numb_controlpnts*numb_controlpnts) return;

	int numb_i = numb_controlpnts-1;
	int numb_j = numb_controlpnts-1;

	vector< vector < Vector > > verts;
	verts.resize(numb_controlpnts);
	for(int i=0; i<verts.size(); i++) verts[i].resize(numb_controlpnts);

	for(int i=0; i<numb_controlpnts; i++)
		for(int j=0; j<numb_controlpnts; j++)
				verts[i][j] = vertices[i*numb_controlpnts+j];

	int sampling = 10;

	int res_i = 10*numb_i;
	int res_j = 10*numb_j;

	for(int i=0; i<bpatch.size(); i++) bpatch[i].clear();
	bpatch.clear();

	bpatch.resize(res_i);
	for(int i=0; i<bpatch.size(); i++) bpatch[i].resize(res_j);

	int i,j,ki,kj;
	float mui,muj,bi,bj;

   for (i=0;i<res_i;i++) {
	  mui = i / (double)(res_i-1);
	  for (j=0;j<res_j;j++) {
		 muj = j / (double)(res_j-1);
		 bpatch[i][j].x = 0;
		 bpatch[i][j].y = 0;
		 bpatch[i][j].z = 0;
		 for (ki=0;ki<=numb_i;ki++) {
			bi = bezierBlend(ki,mui,numb_i);
			for (kj=0;kj<=numb_j;kj++) {
			   bj = bezierBlend(kj,muj,numb_j);
			   bpatch[i][j].x += (verts[ki][kj].x * bi * bj);
			   bpatch[i][j].y += (verts[ki][kj].y * bi * bj);
			   bpatch[i][j].z += (verts[ki][kj].z * bi * bj);
			}
		 }
	  }
   }

   Vector tex;

   glColor3f(1,1,1);
   glBegin(GL_QUADS);
   for (i=0;i<res_i-1;i++) 
   {
      for (j=0;j<res_j-1;j++) 
	  {
			tex = bpatch[i][j]/res;
			glTexCoord3fv(&tex.x);
            glVertex3fv(&bpatch[i][j].x);

			tex = bpatch[i][j+1]/res;
			glTexCoord3fv(&tex.x);
			glVertex3fv(&bpatch[i][j+1].x);
            			
			tex = bpatch[i+1][j+1]/res;
			glTexCoord3fv(&tex.x);
			glVertex3fv(&bpatch[i+1][j+1].x);
            			
			tex = bpatch[i+1][j]/res;
			glTexCoord3fv(&tex.x);
			glVertex3fv(&bpatch[i+1][j].x);
      }
   }
   glEnd();
}
void MeasureObject::addFace(vector < int > f)
{
	faces.push_back(f);

	//LABELS
	vector < string >  lbl;
	face_labels.push_back(lbl);

	//ADD EDGES
	//faces[j]

	vector < Edge > currEdges;

	Edge e;
	for(int i=0; i<f.size()-1; i++)
	{
		e.v0 = f[i];
		e.v1 = f[i+1];

		bool found = false;
		for(int j=0; j<edges.size(); j++)
		{
			if(edges[j].v0 == e.v0 && edges[j].v1 == e.v1)
			{
				found = true;
				break;
			}
			else if(edges[j].v0 == e.v1 && edges[j].v1 == e.v0)
			{
				found = true;
				break;
			}
		}

		if(found==false) addEdge(e);
	}

	e.v0 = f[f.size()-1];
	e.v1 = f[0];

	bool found = false;
	for(int j=0; j<edges.size(); j++)
	{
		if(edges[j].v0 == e.v0 && edges[j].v1 == e.v1)
		{
			found = true;
			break;
		}
		else if(edges[j].v0 == e.v1 && edges[j].v1 == e.v0)
		{
			found = true;
			break;
		}
	}

	if(found==false) addEdge(e);
}
void MeasureObject::addEdge(Edge e)
{
	edges.push_back(e);

	vector < string >  lbl;
	edge_labels.push_back(lbl);
}
void MeasureObject::addPoint(Vector v)
{
	vertices.push_back(v);

	vector < string >  lbl;
	vertex_labels.push_back(lbl);
}
const MeasureObject& MeasureObject::operator= (const MeasureObject& right)
{
	clear();

	name = right.name;
	drawmode = right.drawmode;

	for(int i=0; i<right.vertices.size(); i++)
		vertices.push_back(right.vertices[i]);

	for(int i=0; i<right.edges.size(); i++)
		edges.push_back(right.edges[i]);

	for(int i=0; i<right.face_normals.size(); i++)
		face_normals.push_back(right.face_normals[i]);

	for(int i=0; i<right.face_colours.size(); i++)
		face_colours.push_back(right.face_colours[i]);

	faces.resize(right.faces.size());
	for(int i=0; i<right.faces.size(); i++)
	{
		for(int j=0; j<right.faces[i].size(); j++)
			faces[i].push_back(right.faces[i][j]);
	}
	
	edge_dT = right.edge_dT;

	edge_kmax.resize(right.edge_kmax.size());
	for(int i=0; i<right.edge_kmax.size(); i++)
		edge_kmax[i] = right.edge_kmax[i];
	
	face_dT = right.face_dT;

	face_kmin.resize(right.face_kmin.size());
	for(int i=0; i<right.face_kmin.size(); i++)
		face_kmin[i] = right.face_kmin[i];

	face_vmin.resize(right.face_vmin.size());
	for(int i=0; i<right.face_vmin.size(); i++)
		face_vmin[i] = right.face_vmin[i];

	face_kmax.resize(right.face_kmax.size());
	for(int i=0; i<right.face_kmax.size(); i++)
		face_kmax[i] = right.face_kmax[i];

	face_vmax.resize(right.face_vmax.size());
	for(int i=0; i<right.face_vmax.size(); i++)
		face_vmax[i] = right.face_vmax[i];

	vertex_labels.resize(right.vertex_labels.size());
	for(int i=0; i<right.vertex_labels.size(); i++)
		for(int j=0; j<right.vertex_labels[i].size(); j++)
			vertex_labels[i].push_back(right.vertex_labels[i][j]);

	edge_labels.resize(right.edge_labels.size());
	for(int i=0; i<right.edge_labels.size(); i++)
		for(int j=0; j<right.edge_labels[i].size(); j++)
			edge_labels[i].push_back(right.edge_labels[i][j]);

	face_labels.resize(right.face_labels.size());
	for(int i=0; i<right.face_labels.size(); i++)
		for(int j=0; j<right.face_labels[i].size(); j++)
			face_labels[i].push_back(right.face_labels[i][j]);

/*
	vertex_list_names.resize(right.vertex_list_names.size());
	for(int i=0; i<right.vertex_list_names.size(); i++)
		vertex_list_names[i] = right.vertex_list_names[i];

	vertex_list.resize(right.vertex_list.size());
	for(int i=0; i<right.vertex_list.size(); i++)
		for(int j=0; j<right.vertex_list[i].size(); j++)
			vertex_list[i].push_back(right.vertex_list[i][j]);


	edge_list_names.resize(right.edge_list_names.size());
	for(int i=0; i<right.edge_list_names.size(); i++)
		edge_list_names[i] = right.edge_list_names[i];

	edge_list.resize(right.edge_list.size());
	for(int i=0; i<right.edge_list.size(); i++)
		for(int j=0; j<right.edge_list[i].size(); j++)
			edge_list[i].push_back(right.edge_list[i][j]);

	face_list_names.resize(right.face_list_names.size());
	for(int i=0; i<right.face_list_names.size(); i++)
		face_list_names[i] = right.face_list_names[i];

	face_list.resize(right.face_list.size());
	for(int i=0; i<right.face_list.size(); i++)
		for(int j=0; j<right.face_list[i].size(); j++)
			face_list[i].push_back(right.face_list[i][j]);
*/
	return *this;
}
