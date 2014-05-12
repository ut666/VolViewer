/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QLIGHTEDITOR_H
#define QLIGHTEDITOR_H

#include <q3tabdialog.h> 
#include <qpainter.h>
#include <qlabel.h>

#include "QtLightSettingsWidget.h"

class QtLightEditorDialogue: public Q3TabDialog
{
	 Q_OBJECT

public:	

	QtLightEditorDialogue(QWidget * parent=0, const char * name=0, bool modal=FALSE, Qt::WFlags f=0);
	~QtLightEditorDialogue();
	
	QtLightSettingsWidget* lightettings_tab;

	void reset(void);
	void build_layout(void);
	void rebuild_tabs(void);

private slots:
	void reset_event(void);
	void apply_event(void);

signals:
	void reset_signal(void);
};

#endif // QLIGHTEDITOR_H