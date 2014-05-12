/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QTCAMEDITOR_H
#define QTCAMEDITOR_H

#include <q3tabdialog.h> 
#include <qpainter.h>
#include <qlabel.h>

#include "QtCameraSettingsWidget.h"

class QtCameraEditorDialogue: public Q3TabDialog
{
	 Q_OBJECT

public:	

	QtCameraSettingsWidget* camerasettings_tab;

	QtCameraEditorDialogue(QWidget * parent=0, const char * name=0, bool modal=FALSE, Qt::WFlags f=0);
	~QtCameraEditorDialogue();
	
	void reset(void);
	void build_layout(void);
	void rebuild_tabs(void);

private slots:
	void reset_event(void);
	void apply_event(void);

signals:
	void reset_signal(void);
};

#endif // QTCAMEDITOR_H