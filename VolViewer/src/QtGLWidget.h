/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QTGLWIDGET_H
#define QTGLWIDGET_H

#define ISOSURFACE 0
#define	DEGTORAD(x)	( ((x) * PI) / 180.0 )
#define	RADTODEG(x)	( ((x) * 180.0) / PI )
#define PI 	3.141592653589793238462643383279f

#include <qapplication.h>
#include <qgl.h>
#include <qdatetime.h>
#include <qcolordialog.h>
#include <qevent.h>
#include <qsizepolicy.h>
#include <qtimer.h>
#include <QKeyEvent>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QGestureEvent>

#include <q3filedialog.h>
#include <q3progressdialog.h>
#include <q3dragobject.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <glut.h>

#include "MyDefines.h"
#include "Global.h"
#include "Vector.h"
#include "MarchingCubes.h"
#include "ColourFun.h"
#include "ArcBall.h"
#include "Filter.h"
#include "PCA.h"
#include "Camera.h"
#include "VolumeRender.h"
#include "VolumeObject.h"
#include "VolIO.h"
#include "BSpline.h"
#include "Edge.h"
#include "FilterDefinitions.h"
#include "Quaternion.h"
#include "Timer.h"
#include "TfuncState.h"
#include "MeasureDefinitions.h"

#ifdef OPENCL_SUPPORT
	#include "OpenCLWrapper.h"
	#include "OpenCLExecuter.h"
#endif


class QGestureEvent;
class QPanGesture;
class QPinchGesture;
class QSwipeGesture;

static int timer_interval = 1;			// timer interval (millisec)

enum {R, G, B, A};
enum {OBJ_ANGLE, CUTTING, OBJ_ZOOM, OBJ_TRANSLATE, OBJ_ARB2DPLANE}; 

class QtGLWidget : public QGLWidget
{
    Q_OBJECT

public:

	float angleX;

	bool showGUI;

	bool depth_peal;
	float headingangle;
	float pitchangle;

	int argc_mode;
	int render;

	Timer timer;

	unsigned int* depthimgdata;

	//global mouse position
	Vector global_mousepos;
	Vector mouspos;
	float dx, dy;
	float dw;
	int tempx,tempy;
	int stopped;
	
	float zoomstart;

	QFont serifFontBG;
	QFont serifFont;
	QFont serifFontLabel;

#ifdef OPENCL_SUPPORT
	//OpenCL interface
	OpenCLWrapper ocl_wrap;
	OpenCLExecuter ocl_gaussian;
	OpenCLExecuter ocl_dilate;
	OpenCLExecuter ocl_erode;
	OpenCLExecuter ocl_peronamalik;
	OpenCLExecuter ocl_boundingbox;
	OpenCLExecuter ocl_reduction;
	OpenCLExecuter ocl_median;
	OpenCLExecuter ocl_bilateral;
#endif

	//volume object and utilitisourceobjectes
	VolumeObject	volobject;
	VolumeRender	volrender;
	VolIO			volio;

	//our marching cubes class
	MCubes mcubes;

	QString movie_dir;
	QString currentworkingdir;
	
	QDialog* labeller_dialogue;
	int labellermax;
	QLabel* labellermax_lb;
	QSlider* labellermax_slider;
	int labellermin;
	QLabel* labellermin_lb;
	QSlider* labellermin_slider;

	//toggle plane
	int planes_toggle;
	int active;

	Q3ProgressDialog* pb;

	bool filter_smthdilate_cont;

	vector< VolumeRenderView > volumeviews;

	int maxflow_mode;
	vector < int > maxflowbg_indices;
	vector < int > maxflowfg_indices;

	float fpsval;
	//-----------------------------------------------
    QtGLWidget( QWidget* parent, const char* name, const QGLWidget* shareWidget=0 );
    QtGLWidget( const QGLFormat& format, QWidget* parent, const char* name, const QGLWidget* shareWidget=0 );
    ~QtGLWidget();

	void		set_app(QApplication* app);
	void		initvariables();
	
#ifdef OPENCL_SUPPORT
	//GPGPU/OPENCL STUFF
	void		intializeCL(void);
#endif

	//RENDERING STUFF	
	void		initializeGL();
	void		paintGL();
    void		resizeGL( int w, int h );
	void		renderTextBox(int x, int y, QString txt, QFont font, int offset);

	//VOLUME UTILITY FUNCTIONS (CPU based)
	void		crop(void);
	void		recentre(void);
	void		invcrop(void);
	void		realign(void);
    void		gaussian( int size, float sigma);
	void 		unsharp_slot(int size, float sigma, int bright, int cont);
	void		sobel(void);
	void		transfer_function( vector<Vector> cp, float size,  float border, int mode);

protected:

    //USER INTERACTION EVENTS
	void		keyPressEvent	(QKeyEvent * ); 
	void		keyReleaseEvent (QKeyEvent * );
	void		mouseMoveEvent  (QMouseEvent * e);
	void		mousePressEvent (QMouseEvent * e);
	void		mouseReleaseEvent(QMouseEvent * e);
	void		mouseDoubleClickEvent(QMouseEvent * e);

	void		captureDepthMap(void);

	//multi-touch events
	bool		event(QEvent* e);	
	bool		gestureEvent(QGestureEvent *event);
	void		panTriggered(QPanGesture*);
	void		pinchTriggered(QPinchGesture*);
	void		swipeTriggered(QSwipeGesture*);
	
	void		wheelEvent(QWheelEvent *event);
	void		dragEnterEvent(QDragEnterEvent* event);
	void		dropEvent(QDropEvent* event);
	void		processDepthClickCrop(int x, int y, int mode);
	void		processDepthClick(int x, int y, int mode);
	void		processClick(int x, int y, int mode);
	bool		processClickOnSelectedPlane(int x, int y);
	bool		processClickOnAnyPlane(int x, int y);
	void		processDepthClickTrans(int x, int y);
	void		process3DCursor(int x, int y);

	void		unprojectDepth(Vector v, float* vZ, Vector* scene);
	int			heightForWidth (int w);

public slots:

	//clipping plane editor slots
	void		cutplane_curvetrans_slot(bool);
	void		cutplane_curvecoords_slot(bool);
	void		cutplane_curveangle_slot(int, int);
	void		cutplane_select_slot(int selected);
	void		cutplane_invert_slot(int selected);
	void		cutplane_load_slot(QString fn);
	void		cutplane_save_slot(QString fn);
	void		cutplane_showall_slot(int selected);
	void		cutplane_resetall_slot(void);
	void		cutplane_resetcurrent_slot(void);
	void 		cutplane_recentre_slot(void);
	void		cutplane_crop_slot(void);
	void 		cutplane_realign_slot(void);
	void		cutplane_invcrop_slot(void);
	void		cutplane_redclip_slot(void);
	void		cutplane_greenclip_slot(void);
	void		cutplane_blueclip_slot(void);
	void		cutplane_isoclip_slot(void);
	void 		cutplane_applyall_drawmode_slot(void);
	void 		cutplane_interact_clicked_slot(int);
	void 		cutplane_unselect_slot(void);
	void 		cutplane_selected_slot(int);
	void 		cutplane_planeslider_slot(float dist);
	void 		cutplane_planerot1_slot(float ang);
	void 		cutplane_planerot2_slot(float ang);
	void 		cutplane_rendermode_slot(int);
	void 		cutplane_interact(float dy);
	void		cutplane_solid_slot(bool on);
	void		cutplane_flip_slot(bool on);

	//render settings widget
	void		rendersettings_framebuffer_lod_slot(int);
	void 		rendersettings_framebuffer_slot(int);
	void 		rendersettings_postprocess_slot(int);
	void 		rendersettings_silhouettes_slot(int);
	void 		rendersettings_numbslices_slot(int);
	void 		rendersettings_numbsliceslod_slot(int s);
	void 		rendersettings_numbsliceslodtoggle_slot(int);
	void 		rendersettings_framebuffertoggle_slot(int);
	void 		rendersettings_postprocesstoggle_slot(int);
	void 		rendersettings_maxtexturesize_slot(int);
	void 		rendersettings_renderquality_slot(float);
	void 		rendersettings_renderqualitylod_slot(float);
	void 		rendersettings_channelmode_slot(int r,int g,int b,int i);

#ifdef OPENCL_SUPPORT
	void updateCL_UI(void);
	void rendersettings_openclplat_slot(int);
#endif

	//isosurface editor
	void		isosurface_new_slot(QString name, int quality, int value, int smooth, Vector col, bool* channels, int clpstate);
	void		isosurface_get_slot(QString name, int* quality, int* value, int* smooth, Vector* col, int* nv, int* nf, int* clpstate);
	void		isosurface_del_slot(QString name);
	void		isosurface_update_slot(QString oldname, QString name, int quality, int value, int smooth, Vector col, bool* channels, int clpst);
	void		isosurface_load_slot(QString fn);
	void		isosurface_save_slot(void);
	void		isosurface_crop_slot(int selected);

	//crop widget
	void		crop_start_slot(void);
	void		crop_end_slot(vector<bool> channels);
	void		crop_invsel_slot(int);
	void		crop_reset_slot(void);

	//camera widget
	void		camerasettings_step_slot(int step);
	void		camerasettings_fov_slot(int fov);
	void		camerasettings_mode_slot(int id);

	//measure widget
	void		measure_start_slot(void);
	void 		measure_stop_slot(void);
	void 		measure_pause_slot(void);
	void 		measure_edit_slot(void);

	void 		measure_update_item_slot(int index, MeasureObject obj, int draw_mode);
	void 		measure_add_item_slot(MeasureObject obj, int draw_mode);

	void 		measure_update_current_drawmode_slot(int drawmode);
	void		measure_update_current_editmode_slot(int editmode);
	void		measure_update_current_viewmode_slot(int viewmode);

	void		measure_update_current_slot(MeasureObject obj);
	void 		measure_update_current_slot(int index);

	void 		measure_delete_item_slot(int index);
	void 		measure_eval_triangles_slot(int index);
	void 		measure_evalcurrent_triangles_slot(void);
	void 		measure_setcutplane_slot(int, float, float, float);
	void		measure_clearcurrent_slot(void);
	void		measure_clearall_slot(void);
	void		measure_cancel_slot(void);
	void		measure_rebuild_current_edges(void);
	void		measure_update_visible_slot(int, bool);

	void		measure_modify_start_slot(void);
	void		measure_modify_stop_slot(void);
	void		measure_modify_zoomcurrent(float zoom);
	void		measure_modify_rotatecurrent(int mode, float angle);
	void		measure_labels_slot(int, int, int, int, int, int, int);

	void		measure_rmv_currentvert_slot(MeasureObject, int);
	void		measure_rmv_currentface_slot(int);
	void		measure_rmv_currentedge_slot(int);
	void		measure_rmv_currentlabel_slot(int index, bool delgeom);

	void 		measure_selected_vertindex_slot(int index);
	void		measure_selected_faceindex_slot(int index);
	void		measure_selected_edgeindex_slot(int index);

	void		measure_facealign_orient_slot(bool on);
	void		measure_section_toggle_slot(bool on);
	void		measure_section_thickn_slot(int thickn);
	void		measure_section_trans_slot(int trans);
	void		measure_facealign_updateview_slot(bool state);

	void		meausre_setuprendering_slot(void);

	//lighting
	void		light_altitude_slot(float);
	void 		light_azimuth_slot(float);
	void 		light_colour_slot(Vector, Vector, Vector);
	void 		light_volshader_slot(int);
	void 		light_gooch_lighting_slot(Vector, Vector, int, int, int, int);
	void		light_depthcue_slot(Vector, int);
	void		light_depthpeal_slot(int, int);

	//stereo
	void 		stereo_onoff_slot(int);
	void 		stereo_rendermode_slot(int);
	void 		stereo_seperation_slot(float);
	void		stereo_zscreen_slot(int zscreen);

	//filter
	void		filter_slot(int filtertype, vector<bool> channels, vector<int> intparams, vector<double> doubparams);
	void		filter_smthdilate_slot(int srcchan, int dstchan, int seed_window, int minsize, int gaussian_width, float gaussian_strength, int gaussian_window, int show_sections, int show_volume);

#ifdef OPENCL_SUPPORT
	void		filter_gpu_smthdilate_slot(int srcchan, int dstchan, int seed_window, int minsize, int gaussian_width, float gaussian_strength, int gaussian_window, int show_sections, int show_volume);
#endif
	void		filter_smthdilateabort_slot(void);
	void		filter_maxflow_interact_slot(int mode);

	//movie editor slots
	void		movie_orthosectmovie_slot(QWidget* parent, int sampling, int preview, QString dn);
	void		movie_rockmovie_slot(QWidget* parent, int sampling, int preview, QString dn, float ang);
	void		movie_rotmovie_slot(QWidget* parent, int sampling, int preview, QString dn, int x, int y, int z, float ang);
	void		movie_clipmovie_slot(QWidget* parent, int sampling, int preview, QString dn, int x, int y, int z, float ang, int clipmode, int clipafterrot);
	void		movie_renderviewmovie_slot(QWidget* parent, int sampling, int preview, QString dn);

	//sliceframe slots
	void		sliceframe_pixel_slot(int x, int y, int z, int mode);
	void		sliceframe_updatepixel(void);

	//renderview slots
	void		renderview_saveview_slot(void);
	void		renderview_updateview_slot(int sel);
	void		renderview_setview_slot(VolumeRenderView view);
	void		renderview_renderview_slot(VolumeRenderView view);
	void		renderview_interpolate_slot(vector< VolumeRenderView > volumeviews);

	//datasetview slots
	void		datasetview_loadFolder_slot(QString dn);
	void		datasetview_updatethumbnail_slot(int i);

	//general slots
	void		redraw_event(void);
		
	//tfunc editor slots
	void		tfunc_rebuild_histogram_slot(void);
	void		tfunc_histoupdate_slot(int toggle);	

#ifdef LIBTIFF_SUPPORT
	//16 bit tfuns
	void		tfunc_remap16bit_slot(int min_l, int max_l, const vector<Vector>& tfunc_curvepts_luminance, int min_o, int max_o, const vector<Vector>& tfunc_curvepts_opacity);
#endif

	//hist
	void		hist_tfunc_slot(int mode, TfuncState tfunc);
	
	//==================================================================================================================================
	
	void		setcurrentworkingdir_slot(QString dn);
	void		brightcontrast_slot(int b, int c);
	void		thresholdmin_slot(int);
	void		thresholdmax_slot(int);
    void		threshold_slot( int min, int max);
	
signals:

	void		mw_openImageDir_sinal(QString dn);
	void		mw_initImageDir_sinal(QString dn);

	void		update_channels_signal(void);
	void		updatestatus_cursor_signal(void);

	void		measure_updatecurrent_signal(MeasureObject obj);

	void		measure_point_signal(Vector v);
	void		measure_editpoint_signal(Vector v);
	void		measure_editpoint_select_signal(int sel);
	void		measure_savecutplane_signal(int, float, float, float);
	void		measure_close_signal(void);
	void		measure_clearcurrent_signal(void);
	void		measure_editgrppoint_signal(int i, Vector v);
	void		measure_addvertface_signal(int v);
	void		measure_rmvvertface_signal(int v);
	void		measure_endvertface_signal(void);
	void		measure_edge_select_signal(int sel);
	void		measure_face_select_signal(int sel);

#ifdef OPENCL_SUPPORT
	void		rendersettings_setopenclplat_signal(int, vector<string>);
#endif

	void		renderview_updateview_signal(int, QPixmap*, VolumeRenderView);
	void		renderview_addview_signal(QPixmap*, VolumeRenderView);
	void		renderview_addsavedview_signal(QPixmap*);
	void		renderview_getallviews_signal(void);

	void		datasetview_setThumbnail_signal(int i, QPixmap thumbnail);
	void		datasetview_saveCurrentView_signal(VolumeRenderView view);

	void		arc_mode_signal(void);
	void		frames_redraw_signal(void);
	void		single_channel_signal(void);
 
	void		dragndrop_signal(QString filename);
	void		openImageDir_signal(QString dirname);

	void		cutplane_enableplanepanel_signal();
	void		cutplane_selected_signal(int index,int drawmode);
	void		cutplane_selectedupdate_signal(int index, int drawmode);
	void		cutplane_planeslider_signal(int val);
	void		cutplane_unselect_signal(void);
	
	void		tfunc_histogram_signal(double*);
	void		tfunc_updateAll_signal(TfuncState lum, TfuncState alp, TfuncState redlum, TfuncState redalp, TfuncState greenlum, TfuncState greenalp, TfuncState bluelum, TfuncState bluealpha);

#ifdef LIBTIFF_SUPPORT
	void		tfunc_16bit_histogram_signal(double* histo16,double* histo8);
#endif

	void		isosurface_addnew_signal(QString name, Vector col);
	void 		save_sectionimage_signal(QString fn, int axis, int index);
	void		timelapseEvent(void);
	void		sliceframe_drawmode_signal(int mode);

	void		camerasettings_setmode_signal(int id, int fov);

	void		camerasettings_toggleUI_signal(bool);
};

#endif // QTGLWIDGET_H

