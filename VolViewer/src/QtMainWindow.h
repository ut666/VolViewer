/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QTMAINWINDOWN_H
#define QTMAINWINDOWN_H

#include <qstatusbar.h>
#include <qcheckbox.h> 
#include <qdialog.h> 
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <qtoolbutton.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qcolordialog.h>
#include <qdesktopwidget.h>
#include <qfilesystemwatcher.h>
#include <qmainwindow.h>
#include <QCloseEvent>
#include <QResizeEvent>
#include <QLabel>
#include <QFileDialog>
#include <QTime>
#include <QTextStream>

#include <q3tabdialog.h>

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <string>

#include "MyDefines.h"
#include "SVNObject.h"

#include "QtGLObjWin.h"
#include "QtGLWidget.h"
#include "QtTransferFunctionEditorDialogue.h"
#include "QtVolumeEditorDialogue.h"
#include "QtClippingEditorDialogue.h"
#include "QtViewEditorDialogue.h"
#include "QtLightEditorDialogue.h"
#include "QtStereoEditorDialogue.h"
#include "QtMovieEditorDialogue.h"
#include "QtMeasuringEditorDialogue.h"
#include "QtRenderViewDialogue.h"
#include "QtDatasetViewDialogue.h"
#include "QtFilterEditorDialogue.h"
#include "QtCropEditorDialogue.h"
#include "QtCameraEditorDialogue.h"
#include "Tokenizer.h"
#include "Edge.h"
#include "VolumeScript.h"

#ifdef LIBTIFF_SUPPORT
	#include "QtTransferFuncDialogue.h"
#endif

#ifdef OMERO_SUPPORT
	#include "OMEROLoader.h"
	#include "QtOMEROLoader.h"
#endif

class QtGLObjWin;
class QtGLWidget;
class QtVolumeEditorDialogue;

class QtMainWindow: public QMainWindow
{
    Q_OBJECT
public:
	
	SVNObject svn;
	QString appPath;

	int main_width, main_height;
	int moveX, moveY;
	int maximised;
	int image_mode;
	int channel_mode;
	int consolle;
	int auto_centre;
	int view_orientation;
	int window_main_screen;
	
	VolumeScript volscript;
	QString watchfile;

	bool promptChannels;

	int timelapsehelper;
	QStringList timelapsedirs;

	QToolButton * intentb;
	QToolButton * redtb;
	QToolButton * greentb;
	QToolButton * bluetb;

	QString default_filename;
	QString last_working_dir;

#ifdef OMERO_SUPPORT
	QString serverurl;
	int serverport;
	QString username;
	OMEROLoader omeroLoader;
#endif

#ifdef LIBTIFF_SUPPORT
	QtTransferFuncDialogue* transferfunc16bit_dialogue;
#endif

    QtMainWindow(QString abbsapppath);
    ~QtMainWindow();
	QtGLObjectWindow* glwindow;

	//our tabbed widgets
	QtTransferFunctionEditorDialogue* transferfunction_editor_tabdlg;
    QtVolumeEditorDialogue* volume_editor_tabdlg;
	QtClippingEditorDialogue* clipping_editor_tabdlg;
	QtViewEditorDialogue* view_editor_tabdlg;
	QtLightEditorDialogue* light_editor_tabdlg;
	QtStereoEditorDialogue* stereo_editor_tabdlg;
	QtMovieEditorDialogue* movie_editor_tabdlg;
	QtMeasuringEditorDialogue* measure_editor_tabdlg;
	QtFilterEditorDialogue* filter_editor_tabdlg;
	QtCropEditorDialogue* crop_editor_tabdlg;
	QtRenderViewDialogue* renderview_editor_tabdlg;
	QtCameraEditorDialogue* camera_editor_tabdlg;

	QtDatasetViewDialogue* datasetview_editro_tabdlg;

	QFileSystemWatcher* qfilesystemwatcher;
	QFileSystemWatcher* watchfile_qfilesystemwatcher;

	//some dialogue boxes
	QDialog* movie_dlg;
	QDialog* brick_dlg;
	QDialog* channel_dlg;
	QDialog* image_dlg;

	QToolButton * bgtb;
	
	QToolBar* buttonTools;
	QToolBar* fileTools;
	QToolBar* prefTools;
	QToolBar* settingsTools;
	QToolBar* aboutTool;

	QAction* scalebarAct;
	QAction* boundingboxAct;
	QAction* lightAct;
	QAction* axisAct;
	QAction* cursorAct;
	QAction* fpsAct;
	
    void about();
	void connect_signals_and_slots(void);
	void load_inifile(void);
	void load_commandlineparams(int argc, char **argv);

	void commandline_parse(QStringList commandList);

	void updateStatusBar(void);

protected:
    void closeEvent( QCloseEvent* );
	void resizeEvent ( QResizeEvent * );

public slots:

	void update_info_labels(void);
	void setWatchFile(QString fn);

	void setBG_event(int r, int g, int b);

	//Rendering events
	//-----------------
	//slice sampling lod of 3d texture
	void togglemoveLOD_event(void);

	//show/hide
	void toggleBB_event(void);
	void togglevoxels_event(void);
	void togglesurfaces_event(void);
	void togglestereo_event(void);
	void togglecursor_event();
	void togglefps_event(void);
	void toggleaxis_event(void);
	void togglelights_event(void);
	void toggle_scalebar_event(void);

	//toggle the blend function
	void blend_mode1_event();
	void blend_mode2_event();
	void blend_mode3_event();
	void blend_mode4_event();

	//rendering_mode
	void render0_event();
	void render1_event();
	void render2_event();
	void render3_event();

	//view, none, lines, quads, textured slices
	void slicemode0_event();
	void slicemode1_event();
	void slicemode2_event();
	void slicemode3_event();

	//view orientation
	void front_event(void);
	void back_event(void);
	void left_event(void);
	void right_event(void);
	void top_event(void);
	void bottom_event(void);
	void resetviewall_event(void);
	void resetviewzoom_event(void);
	void resetviewtrans_event(void);

	void ui_toggle_menubars(bool state);
	void ui_toggle_orthosections(bool state);

	//Load events
	//-----------------
	//Open PNG slice data & choose which channel we load into
	void openImage_event(void);
	void openImageDir(QString dn);
	void openImageChannelMode(int chanmode);
	void openImagechannelmsgbox_event();
	void openImageR_event(void);
	void openImageG_event(void);
	void openImageB_event(void);
	void openImageRGBA_event(void);
    void openImageI_event(void);

	//Open RAW data
	void openraw_event(void);
	void openRawFile(QString fn);

	//Open WLZ data
	void openwlz_event(void);

	//Open folder of volumes
	void openTimelapse_event(void);

#ifdef OMERO_SUPPORT
	void openOmeroImage_event(void);
	void openOmeroImage_Load(void);
	void openOmeroImage_LoadDataevent(void);
    void saveOmeroImage_event(void);
	void saveOmeroImage_SaveDataevent(void);
	void saveOmeroImage_Save(string name, string descp);
	void saveOmeroImage_Save(int dsid, string name, string descp);
#endif

	void runScript(QString s);
	void watchfileChanged(QString);

	void parse_inifile(QString fn);

	//Save events
	//-----------------
	//save slices
	void savetiff_event(void);
	void savepng_event(void);
	//save a raw volume
	void saveraw_event(void);
    //save an obj surface
	void saveobj_event(void);
	//save a PNG image, & choose (RENDERER,XY,XZ,YZ)
    void saveImage_event(void);
	void saveImagemsgbox_event();
	void saveImageREN_event();
	void saveImageXY_event();
	void saveImageXZ_event();
	void saveImageYZ_event();

	void save_renderScreenShot_event();

	void info_event();

	//Editor Widgets
	//------------------------
	void transfer_editor_event();
	void volume_editor_event();
	void transform_editor_event();
	void viewpref_event();
	void lightpref_event();
	void moviepref_event();
	void measurepref_event();
    void tfunction_event();
	void filterpref_event();
	void croppref_event();
	void renderviewpref_event();
	void cameraviewpref_event();
	void about_event();

	//Layout Manager Widget
	//------------------------
	void custom_layout_event();

	void minibutton_I_event(void);
	void minibutton_red_event(void);
	void minibutton_green_event(void);
	void minibutton_blue_event(void);
	void minibutton_mip_event(void);
	void minibutton_atten_event(void);
	void minibutton_bb_event(void);
	void minibutton_lights_event(void);
	void minibutton_axis_event(void);
	void minibutton_cursor_event(void);
	void minibutton_surfaces_event(void);
	void minibutton_bg_event(void);
	void minibutton_fps_event(void);
	void minibutton_info_event(void);
	void minibutton_stereo_event(void);
	void minibutton_fullscreen_event(void);
	void minibutton_hq_event(void);
	void minibutton_mq_event(void);
	void minibutton_lq_event(void);

	void minibutton_filter_event(void);
	void minibutton_label_event(void);
	void minibutton_vertfinder_event(void);
	
	void minibutton_savelayout_event(void);
	void minibutton_restorelayout_event(void);

	void fileChanged(QString fn);
	
signals:
	void redraw_event(void);
	void frames_redraw_signal(void);
	void measure_setscale2_signal(float x, float y, float z);
	void measure_setspacing_signal(float x, float y, float z);
	void measure_setrescale_signal(float x, float y, float z);		
	void measure_setoriginalfn_signal(QString fn);
	void currentworkingdir_signal(QString dn);

#ifdef LIBTIFF_SUPPORT
	void tfunc_is16bit_signal(bool value);
	void tfunc_16bit_histogram_signal(double* histo16, double* histo8);
#endif

public slots:
	void mw_initImageDir_slot(QString dn);
	void mw_openImageDir_slot(QString dn);

	void uploadTextureSlot(void);

	void update_channels_slot(void);
	void updatestatus_cursor_slot(void);

	void sliceframe_pixel_slot(int x,int y,int z, int mode);

	void datasetview_saveSettings_slot(QString dn, QString subdn);
	void datasetview_loadSettings_slot(QString dn, QString subdn);
	void datasetview_edit_MSR_slot(QString src, QString dst, int index);
	void datasetview_align_slot(DatasetView dsv);
	void datasetview_computestacks_slot(QString dn, DatasetView dsv);

	void arc_mode_slot(void);
	void dragndrop_slot(QString filename);
	void single_channel_slot(void);
	void measure_setscale_slot(float x, float y, float z);

	void timelapseEvent(void);

	void measureCursorOn_slot(void);
	void measureCursorOff_slot(void);

	void camerasettings_toggleUI_slot(bool);

private:
    QString filename;

	QMenu *openMenu;
	QMenu *saveMenu;

	QToolButton * cameraviewpref;

	QToolButton * clipping;
	QToolButton * measurepref;
	QToolButton * croppref;
	QMenu *viewMenu;

	int blend_id;
	int max_blend_id;
	int min_blend_id;
	int atten_blend_id;
	int render0_id;
	int render1_id;
	int render2_id;
	int shader0_id;
	int shader1_id;
	int shader2_id;
	int shader3_id;
	int shader4_id;
	int shader5_id;
	int slicemode0_id;
	int slicemode1_id;
	int slicemode2_id;
	int slicemode3_id;
	int jpgR_id;
	int jpgG_id;
	int jpgB_id;
	int jpgRGB_id;
	int movelod_id;
	int bb_id;
	int vol_id;
	int lights_id;
	int stereo_id;
	int surf_id;
	int arb2d_id;
	int cursor_id;
	int fps_id;	
	int axis_id;
};


#endif
