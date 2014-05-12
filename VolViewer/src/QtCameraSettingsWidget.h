/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QtCameraSettingsWidget_H
#define QtCameraSettingsWidget_H

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

class QtCameraSettingsWidget: public QWidget
{
	Q_OBJECT
		
public:

	Q3ButtonGroup* perpectivesettings_grp;
	QPushButton* camera_ortho_pb;
	QPushButton* camera_persp_pb;

	//perpective FOV
	QLabel* camerafov_lb;
	QSlider* camerafov_slider;
	int camerafov;

	QLabel* camerastep_lb;
	QSlider* camerastep_slider;
	int camerastep;

	QtCameraSettingsWidget(QWidget* parent, const char* name);
	~QtCameraSettingsWidget();

	void create(void);
	void reset(void);
	void rebuild(void);
	void apply(void);
	void reload(void);

public slots:
	void camerasettings_setmode_slot(int id, int fov);

private slots:
	void camerasettings_step_slot(int val);
	void camerasettings_fov_slot(int val);
	void camerasettings_mode_slot(int id);
signals:
	void camerasettings_mode_signal(int id);
	void camerasettings_fov_signal(int fov);
	void camerasettings_step_signal(int step);

};

#endif // QtStereoSettingsWidget_H
