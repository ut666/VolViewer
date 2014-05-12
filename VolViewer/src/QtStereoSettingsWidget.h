/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QtStereoSettingsWidget_H
#define QtStereoSettingsWidget_H

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

#include "Vector.h"
#include "ColourFun.h"

class QtStereoSettingsWidget: public QWidget
{
	Q_OBJECT
		
public:

	QtStereoSettingsWidget(QWidget* parent, const char* name);
	~QtStereoSettingsWidget();

	QPushButton* load;
	QPushButton* save;

	//inter ocular distance
	Q3ButtonGroup* stereo_seperation_grp;
	QLabel* stereo_seperation_lb;
	QSlider* stereo_seperation_slider;
	int stereo_seperation;

	//z-screen position
	Q3ButtonGroup* zscreen_grp;
	QLabel* zscreen_lb;
	QSlider* zscreen_slider;
	int zscreen;

	//renderquality_lod
	QPushButton* stereo_onoff_pb;

	Q3ButtonGroup* stereo_rendermodegrp;
	QPushButton* stereo_RBanaglyth_pb;
	QPushButton* stereo_RGanaglyth_pb;
	QPushButton* stereo_RCanaglyth_pb;
	QPushButton* stereo_quadbuffer_pb;

	void create(void);
	void reset(void);
	void rebuild(void);
	void apply(void);
	void reload(void);

	void set_fromini(int toggle, int mode, float value);

private slots:
	
	void load_event(void);
	void save_event(void);

	void stereo_onoff_event(void);
	void stereo_rendermode_event(int);
	void stereo_seperation_event(int);
	void zscreen_event(int);

signals:

	void stereo_onoff_signal(int);
	void stereo_rendermode_signal(int);
	void stereo_seperation_signal(float);
	void stereo_zscreen_signal(float);
};

#endif // QtStereoSettingsWidget_H
