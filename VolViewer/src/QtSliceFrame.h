/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QTSLICEFRAME_H
#define QTSLICEFRAME_H

#include <qpainter.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qcursor.h>
#include <qimage.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsView>	
#include <QGraphicsPixmapItem>

#include <iostream>
#include <math.h>
#include <map>
using namespace std;

#include "Vector.h"

typedef map<int, vector< Vector > > Labels;

class QtSliceFrame: public QGraphicsView 
{
	Q_OBJECT
		
public:

	float brushsize;

	Labels fg_mflowLabels;
	Labels bg_mflowLabels;

	float resizeX, resizeY;

	QGraphicsScene* scene;
	QPainter selection_painter;
	QGraphicsPixmapItem* pixmapitem;
	QGraphicsLineItem* lineitem1;
	QGraphicsLineItem* lineitem2;

	QPixmap* slice_pixmap;

	double zoomlevel;

	bool invert_on;
	bool cursor_on;

	//which slice is selected, (for 3d cursor)
	int selected;

	//do we have slice on or off?
	int slices;

	//mode, what type of slice are we
	int mode;

	//x,y,z coords of voxel space
	int x, y, z;
	int xres, yres, zres;
	int oldx, oldy, oldz;
	
	int mouse_x, mouse_y;

	//maxflow stuff
	int click_on;
	int dimension_mode; //2d vs 3d painting
	int draw_mode; //bg or fg painting

	QtSliceFrame(QWidget* parent=0, const char* name=0);
	~QtSliceFrame();

	void create(void);

	void draw(void);
	
	void computeXYZ(int xmouse, int ymouse);
	void centreonView(void);

	void initPixmap(QPixmap* pixmap);
	void updatePixmap(QPixmap* pixmap);

	void clearLabels(void);
	void updateLabels(void);

protected:
	void mouseMoveEvent   ( QMouseEvent * e );
	void mousePressEvent  ( QMouseEvent * e );
	void mouseReleaseEvent(QMouseEvent * e );
	void resizeEvent(QResizeEvent* e);

public slots:
	void sliceframe_drawmode_slot(int mode);
	void sliceframe_zoomin_slot(void);
	void sliceframe_zoomout_slot(void);
	void sliceframe_zoomreset_slot(void);
	void sliceframe_sectioncursor_slot(void);
	void sliceframe_sectioninvert_slot(void);
	void sliceframe_addlabel_slot(Vector slot);

private slots:
	void sliceframe_selected_slot(int);
	
signals:
	void sliceframe_selected_signal(int);
	void sliceframe_pixel_signal(int,int,int,int);
	//void sliceframe_addlabels_signal(int draw_mode, int mode, int x, int y, int z);
	void sliceframe_addlabel_signal(int, Vector);

};

#endif // QTHISTOGRAM_H
