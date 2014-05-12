/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef MEASURELISTOBJECT_H
#define MEASURELISTOBJECT_H

#include <qgl.h>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

#include "Tokenizer.h"

#include "MeasureDefinitions.h"
#include "MeasureObject.h"
#include "Vector.h"
#include "Edge.h"

class MeasureListObject
{
public:

	//represents an MSR object/file
	float time;
	Vector scale;
	QString original_data;
	vector< MeasureObject > measure_objects;

	//gui helper toggle flags
	vector<bool> visible;

	MeasureListObject();
	~MeasureListObject();

	void			clear(void);
	int				size(void);
	void			erase(int i);
	void			replace(int i, MeasureObject obj);
	void			add(MeasureObject obj);
	MeasureObject	get(int i);
	
	void			display(int current_object);
	void			display_arrowheads(int current_object);

	string			tokenread_line(Tokenizer* t);

	void			save_file(QString fn, QString originalfilename, QString algorithm, Vector res);

	float			check_file_version(QString fn);
	void			open_fileversion0(QString fn);	//MSR_VERSION=0
	void			open_fileversion1(QString fn);	//MSR_VERSION=0.1
	void			open_fileversion2(QString fn);	//MSR_VERSION=1.2
	void			open_file(QString fn);

	void			evaluate_aabb(Vector* aabb);

	void setData(vector<QString> temp_namestrings, vector< vector<Vector> > temp_measuring_vertices, vector< vector<Edge> > temp_measuring_edges, vector< vector< vector<int> > > temp_measuring_faces, vector<int> temp_measuring_drawmodes, vector<float> temp_measuring_edge_dt, vector< vector<float> > temp_measuring_edge_kmax, vector<float> temp_measuring_dt, vector< vector<float> > temp_measuring_kmax, vector< vector<float> > temp_measuring_kmin, vector< vector< Vector > > temp_measuring_vmax, vector< vector< Vector > > temp_measuring_vmin, vector< vector< Vector > > temp_measuring_norm);
	
	void setData2(vector<QString> temp_namestrings,
				  vector< vector<Vector> > temp_measuring_vertices, vector< vector<Edge> > temp_measuring_edges, vector< vector< vector<int> > > temp_measuring_faces, 
				  vector<int> temp_measuring_drawmodes,
				  vector<float> temp_measuring_edge_dt, vector< vector<float> > temp_measuring_edge_kmax,
				  vector<float> temp_measuring_dt, vector< vector<float> > temp_measuring_kmax, vector< vector<float> > temp_measuring_kmin, vector< vector< Vector > > temp_measuring_vmax, vector< vector< Vector > > temp_measuring_vmin, 
				  vector< vector< Vector > > temp_measuring_norm, 
				  vector< vector< vector< string > > > temp_vertlabel, vector< vector< vector< string > > > temp_edgelabel, vector< vector< vector< string > > > temp_facelabel);
	
	const MeasureListObject& operator= (const MeasureListObject& right);
};
	
#endif // MEASURELISTOBJECT_H
