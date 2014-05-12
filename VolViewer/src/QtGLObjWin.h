/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */

#ifndef QTGLOBJWIN_H
#define QTGLOBJWIN_H

#include <qwidget.h>
#include <qstring.h>
#include <qimage.h> 
#include <qslider.h>
#include <q3canvas.h>
#include <qcursor.h>
#include <QPixmap>
#include <QLabel>
#include <QDockWidget>
#include <QBoxLayout>

#include <q3progressdialog.h>
#include <q3filedialog.h> 

#include <vector>
using namespace std;

#include "Matrix4x4.h"
#include "Vector.h"
#include "QtSliceFrame.h"

class QtGLWidget;

class QtGLObjectWindow : public QWidget
{
    Q_OBJECT
public:
  		
	//our GL widget
	QString apppath;

	QtGLWidget* glvoxel;
	QFrame* glvoxelframe;

	QDockWidget* topbdock;
	QDockWidget* cursordock;
	QDockWidget* xydock;
	QDockWidget* yzdock;
	QDockWidget* xzdock;
	QDockWidget* infodock;
	
	//sliders + labels to control slices
	QLabel*  slider_yz_lb;
	QSlider* slider_yz;
	QLabel*  slider_xz_lb;
	QSlider* slider_xz;
	QLabel*  slider_xy_lb;
	QSlider* slider_xy;

	//cursor info, just above our slices
	QLabel* cursor_label;

	//file info
	QLabel* info_label;

//	QDialog* info_dlg;
//	QLabel* filename_lb;
//	QLabel* resolution_lb;
//	QLabel* datatype_lb;

	//arrays to keep our slice images
	unsigned char* slice_arrayXY;
	unsigned char* slice_arrayXZ;
	unsigned char* slice_arrayYZ;

	//some QLabels to put our slice image in
    QtSliceFrame* xylb;
    QtSliceFrame* xzlb;
    QtSliceFrame* yzlb;

	//QPixmaps for each slice
	//so we dont create one each time we update
	//a slice
	QPixmap xy_pixmap;
	QPixmap xz_pixmap;
	QPixmap yz_pixmap;

	//allows toggle of the section in the window, ie maximize the rendering
	int sections_toggle;
	
	//general progress bar
	Q3ProgressDialog*		progressbar;
  
	QtGLObjectWindow( QWidget* parent = 0, const char* name = 0, QString apppath="");
    ~QtGLObjectWindow();

	void create(void);
	void destroy(void);
	
public slots:
	void makeXYPixmapEvent(int z); 
	void makeXZPixmapEvent(int y);
	void makeYZPixmapEvent(int x);
	void saveimageEvent(QString fn, int mode); 
	void frames_redraw_slot(void);
	void save_sectionimage_slot(QString fn, int axis, int index);
	void filter_maxflow_buildlabels_slot(void);
	void filter_maxflow_clear_slot(void);
	void filter_maxflow_open_slot(void);
	void filter_maxflow_save_slot(void);
	void filter_maxflow_brush_slot(float);
	void filter_maxflow_dimmode_slot(int);

	void sliceframe_addlabel_slot(int, Vector);

signals:

	void histogram_signal(int* histo);

private:
	QCursor cursor;
};


#endif // QTGLOBJWIN_H
