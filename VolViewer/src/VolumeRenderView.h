/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef VOLRENVIEW_H
#define VOLRENVIEW_H

#include "Vector.h"
#include "ArcBall.h"
#include "Camera.h"
#include "PlaneWidgetInCube.h"
#include "PlaneWidget.h" 
#include "TfuncState.h"

#define PERSP_PROJECTION	0
#define ORTHO_PROJECTION	1

class VolumeRenderView
{

public:

	int projection_toggle;

	int mouse_active;
	Vector mouse_current;
	Vector mouse_centre;

	//our bounding box (8 vertices)
	Vector bb_v1, bb_v2, bb_v3, bb_v4, bb_v5, bb_v6, bb_v7, bb_v8;

	//clipping planes
	PlaneWidgetInCube cutting_planes;
	int plane_rendermode[6];

	//used for mouse click unproject in qtglwidget
	double mvmatrix[16];
	double projmatrix[16];
	int viewport[4];

	//camera interaction
	ArcBall arcball;

	//viewing parameters
	Camera camera;
	float zoom;

	Vector up;
	Vector eye;
	Vector at;

	//tx, ty in texture space
	Vector tex_translate;

	// window dimensions
	int winWidth, winHeight, winMaxDim;

	//transfer function
	TfuncState luminance;
	TfuncState alpha;
	TfuncState redluminance;
	TfuncState redalpha;
	TfuncState greenluminance;
	TfuncState greenalpha;
	TfuncState blueluminance;
	TfuncState bluealpha;

	bool redclip_toggle, blueclip_toggle, greenclip_toggle;

	VolumeRenderView();
	~VolumeRenderView();

	void rotateX(float angle);
	void rotateY(float angle);
	void rotateZ(float angle);

	void move_left(float amount);
	void move_up(float amount);
	void move_forward(float amount);

	void pitchUp(float angle);
	void headingLeft(float angle);

	void lookat(void);

	void mouse_move(QMouseEvent* e);
	void mouse_press(QMouseEvent* e);
	void mouse_release(void);
	void mouseRotate(double x_angle, double y_angle);
	void mouseLook( double heading_angle, double pitch_angle);

	void reset_view();
	void reset_zoom();
	void reset_translate();

	void load(QString fn);
	void load_version0(QString fn);
	void load_version1(QString fn);
	float check_file_version(QString fn);

	void save(QString fn);
	void save(ofstream& fout);

	const VolumeRenderView& operator= (const VolumeRenderView& right);
};

#endif //VOLRENVIEW_H