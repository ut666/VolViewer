/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef MEASUREOBJECT_H
#define MEASUREOBJECT_H

#include <qgl.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>
using namespace std;

#include <glut.h>

#include "Vector.h"
#include "Edge.h"
#include "ColourMap.h"
#include "Matrix4x4.h"

class MeasureObject
{
public:

	//our object name
	string name;
	
	//the geometry data
	int drawmode;
	vector< Vector > vertices;
	vector< Edge > edges;
	vector< vector<int> > faces;

	//the labels
	vector< vector< string > > vertex_labels;
	vector< vector< string > > edge_labels;
	vector< vector< string > > face_labels;

	//growth directives
	float edge_dT;
	vector< float > edge_kmax;
	float face_dT;
	vector< Vector > face_normals;
	vector< Vector > face_colours;
	vector< float > face_kmin;
	vector< float > face_kmax;
	vector< Vector > face_vmin;
	vector< Vector > face_vmax;

	//the lists (TODO)
	vector< string > vertex_list_names;
	vector< vector< int > > vertex_list;
	vector< string > edge_list_names;
	vector< vector< int > > edge_list;
	vector< string > face_list_names;
	vector< vector< int > > face_list;

	//internal helpers for growth
	ColourMap colmap;
	float growth_scale;
	float maxkarea;
	float minkarea;
	vector< float > karea;
	float maxaniso;
	float minaniso;
	vector< float > anisotropy;

	//experimental beizer patch support
	vector< vector< Vector > > bpatch;

	MeasureObject();
	MeasureObject(vector< Vector > v, vector< Edge > e, vector< vector<int> > f);
	~MeasureObject();

	void addPoint(Vector v);
	void addEdge(Edge e);
	void addFace(vector < int > f);

	void evaluate_karea(void);
	void evaluate_anisotropy(void);
	
	void build_edges(void);
	bool find_edge(Edge e);

	void setgrowth(float edge_dt, vector<float> edge_kmax, float dt, vector<float> kmi, vector<float> kma, vector< Vector > vmi, vector< Vector > vma);
	void compute_growth(void);
	void setlabels(vector< vector< string > > vlabels, vector< vector< string > > elabels, vector< vector< string > > flabels);

	void rescale(Vector rescale_factor);

	void clear(void);
	void display(void);

	void display_verts(void);
	void display_edges(void);
	void display_faces(void);
	void display_growth(void);
	void display_arrowheads(float arrow_size);

	void display_kminmax(void);
	void display_karea(void);
	void display_direction(void);
	void display_anisotropy(void);

	void display_selected_vert(int selected_vert);
	void display_selected_edge(int selected_edge);
	void display_selected_face(int selected_face);
	
	double bezierBlend(int k, double mu, int n);
	void display_beizer_patch(void);
	void display_beizer_patch_texture(Vector res);

	void render_ellipsoid(Vector c, Vector r,  Matrix4x4* rotmat, int n, GLenum drawmode);

	bool validateMSR(void);

	const MeasureObject& operator= (const MeasureObject& right);
};

#endif // MEASUREOBJECT_H
