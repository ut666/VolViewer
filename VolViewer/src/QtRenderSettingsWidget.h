/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QtRenderSettingsWidget_H
#define QtRenderSettingsWidget_H

#include <qpainter.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qslider.h>
#include <qstring.h>
#include <q3buttongroup.h>
#include <qcheckbox.h>
#include <qimage.h>
#include <qpushbutton.h> 
#include <qcolordialog.h>
#include <qcombobox.h>
#include <q3grid.h>
#include <qscrollbar.h>
#include <q3boxlayout.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

using namespace std;

#include "MyDefines.h"
#include "Vector.h"
#include "ColourFun.h"

class QtRenderSettingsWidget: public QWidget
{
	Q_OBJECT
		
public:

	QtRenderSettingsWidget(QWidget* parent, const char* name);
	~QtRenderSettingsWidget();

	QPushButton* load;
	QPushButton* save;

#ifdef OPENCL_SUPPORT
	//opencl platform
	QComboBox* openclplat_combobox;
#endif

	//max texture size
	QComboBox* maxtexturesize_combobox;

	//channel toggle widgets
	int r_toggle, g_toggle, b_toggle, i_toggle;
	Q3ButtonGroup *toggle_grp;
	QPushButton* red;
	QPushButton* green;
	QPushButton* blue;
	QPushButton* iso;

	//renderquality
	QLabel* renderquality_lb;
	QSlider* renderquality_slider;
	int renderquality;

	//renderquality_lod
	Q3ButtonGroup* renderquality_lod_grp;
	QPushButton* renderquality_lod_pb;
	QLabel* renderquality_lod_lb;
	QSlider* renderquality_lod_slider;
	int renderquality_lod;

	//maxtexturesize
	QLabel* maxtexturesize_lb;
	QSlider* maxtexturesize_slider;
	int maxtexturesize;

	//#slices lod
	QPushButton* numbsliceslod_pb;
	QLabel* numbsliceslod_lb;
	QSlider* numbsliceslod_slider;
	int numbsliceslod;

	//#slices
	QLabel* numbslices_lb;
	QSlider* numbslices_slider;
	int numbslices;

	//amount of post processing
	Q3ButtonGroup* postprocess_grp;
	QPushButton* postprocess_pb;
	QLabel* postprocess_lb;
	QSlider* postprocess_slider;
	int postprocess;

	//silhouettes
	QLabel* silhouettes_lb;
	QSlider* silhouettes_slider;
	int silhouettes;

	//size of fbo
	QLabel* fbosize_lb;
	QSlider* fbosize_slider;
	int fbosize;

	//size of fbo on move
	QPushButton* min_fbosize_pb;
	QLabel* min_fbosize_lb;
	QSlider* min_fbosize_slider;
	int min_fbosize;

	void create(void);
	void reset(void);
	void rebuild(void);
	void apply(void);
	void reload(void);

public slots:

#ifdef OPENCL_SUPPORT
	void rendersettings_setopenclplat_slot(int index, vector<string> plats);
#endif

private slots:
	
	void load_event(void);
	void save_event(void);

	void channel_button_event(int id);
	void maxtexturesize_event(int j);
	void numbsliceslod_event(int j);
	void numbslices_event(int j);
	void silhouettes_event(int j);
	void postprocess_event(int j);
	void fbosize_event(int j);
	void min_fbosize_event(int j);
	void numbsliceslodtoggle_event(void);
	void framebuffertoggle_event(void);
	void postprocesstoggle_event(void);
	void renderquality_lodtoggle_event(void);
	void renderquality_event(int j);
	void renderqualitylod_event(int j);

#ifdef OPENCL_SUPPORT
	void openclplat_event(int);
#endif

signals:

#ifdef OPENCL_SUPPORT
	void rendersettings_openclplat_signal(int);
#endif

	void rendersettings_framebuffer_signal(int);
	void rendersettings_framebuffer_lod_signal(int);
	void rendersettings_maxtexturesize_signal(int);
	void rendersettings_numbsliceslodtoggle_signal(int);
	void rendersettings_numbslices_signal(int);
	void rendersettings_numbsliceslod_signal(int);
	void rendersettings_postprocess_signal(int);
	void rendersettings_framebuffertoggle_signal(int);
	void rendersettings_postprocesstoggle_signal(int);
	void rendersettings_silhouettes_signal(int);
	void rendersettings_channelmode_signal(int,int,int,int);
	void rendersettings_renderqualitylod_signal(float);
	void rendersettings_renderquality_signal(float);
};

#endif // QtRenderSettingsWidget_H
