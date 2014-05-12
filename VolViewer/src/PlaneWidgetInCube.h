/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef PLANEWIDGETINCUBE_H
#define PLANEWIDGETINCUBE_H

#include <algorithm>
#include <vector>
#include <set>
#include <qgl.h>
using namespace std;

#include "glext.h"

#include "Matrix4x4.h"
#include "Vector.h"
#include "PlaneWidget.h"
#include "Face.h"
#include "Tokenizer.h"
#include "Quaternion.h"
#include "Edge.h"

class PlaneWidgetInCube
{
	public:

	Vector bbvmin, bbvmax;

	Vector curve_translate;
	float curve_angleX, curve_angleY;

	int rotmode;

	Matrix4x4 arbmat;
	int invert_cut;

	vector< vector<Vector> > view_algined_slices;

	vector< vector<Vector> > plane_clippoints;
	vector<Vector> points;
	vector<Vector> cutpoints;
	vector<Vector> slice;

	vector<int> viewsorted;
	
	Vector clipvolcentre;
	vector < Vector > clipvolcentre_points;

	int renderall;
	int render_selected;
	int moviemode;

	//cube size
	float size;
	float Xsize,Ysize,Zsize;
	//cube coords
	Vector v1, v2, v3, v4, v5, v6, v7, v8;

	Vector p1, p2, p3, p4;

	//int selected;
	int selected[6];

	vector<PlaneWidget> planes;
	
	vector< vector<Vector> > planeplanepoints;

	vector< vector<Vector> > planeplanepoints2;

	//centre of plane intersection in cube
	Vector centre;

	PlaneWidgetInCube();
	~PlaneWidgetInCube();

	void clear(void);
	void reset_selected(void);
	void reset_all(void);
	void init(float cube_size);
	void init(float cube_sizeX, float cube_sizeY, float cube_sizeZ);
	void init(Vector c, Vector size);
	void init2(float cube_size);
	void invert(void);
	void add();
	void remove();
	void translate_by_vec(Vector t);
	void display(int index, int surface);
	void display(float linesize,float  pointsize, float bg_oversize, Vector col, Vector eye);
	void display_texmapped(void);
	void display2();
	void select(int i);
	void setup(void);
	void disable(void);
	void enable(void);
	void disable(int id);
	void enable(int id);
	void eval_centre(void);
	void eval_viewaligned_slices(int numb_slices, float rot_mat[16], Vector eye, Vector at);
	void eval_planeintersections(void);
	void eval_planeplaneintersections(void);
	void viewsort(Vector eye);
	bool intersectLineLine(Vector p1,Vector p2,Vector p3,Vector p4,Vector *pa,Vector *pb, double *mua, double *mub);
	
	void plane_translate(float dist, int selected_);
	void plane_rotate1(Vector cen, float angle, int selected_);
	void plane_rotate2(Vector cen, float angle, int selected_);

	void interpolate(PlaneWidgetInCube src, PlaneWidgetInCube dst, float i);

	void save(QString filename);
	void load(QString filename);

	void mouseMove(Vector mousepos);
	void mousePress(Vector mousepos, int type);
	void mouseRelease();

	const PlaneWidgetInCube& operator= (const PlaneWidgetInCube& right);
};

#endif //PLANEWIDGETINCUBE