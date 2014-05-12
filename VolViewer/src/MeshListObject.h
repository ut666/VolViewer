/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef MESHLISTOBJECT_H
#define MESHLISTOBJECT_H

#include <qstring.h>
#include <q3progressdialog.h>

#include <vector>

#include "Vector.h"
#include "Mesh.h"
#include "Tokenizer.h"

class MeshListObject
{
public:
	Q3ProgressDialog*		progressbar;
	GL_Ext*					glextensions;

	vector< QString >	names;
	vector<Vector> colours;
	vector< Mesh >	meshes;
	vector< int >	qualities;
	vector< int >	values;
	vector< float >	smoothsize;
	vector< float >	smoothsigma;
	vector< int > clipstate;

	MeshListObject();
	~MeshListObject();

	void clear_all(void);

	Mesh* new_mesh(void);

	void add(QString name, Vector col,int qual, int val, float smoothsize, float smoothsigma, int clipstate);
	void del(QString name);
	void copy(QString name);
	
	void get(QString name, int* quality, int* value, float* smoothsize, float* smoothsigma, Vector* col, int* nv, int* nf, int* clipstat);
	int set(QString oldname, QString name, int quality, int value, float smoothsize, float smoothsigma, Vector col, int clpst);

	void save_obj(QString filename, int j);
	void save(QString filename);
	void load(QString filename);
};

#endif // MESHLISTOBJECT_H