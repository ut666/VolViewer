/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QVIEWEDITOR_H
#define QVIEWEDITOR_H

#include <q3tabdialog.h> 
#include <qpainter.h>
#include <qlabel.h>

//#include "QtViewSettingsWidget.h"
#include "QtRenderSettingsWidget.h"
//#include "QtSlicesSettingsWidget.h"
//#include "QtCameraSettingsWidget.h"

class QtViewEditorDialogue: public Q3TabDialog
{
	 Q_OBJECT

public:	

	QtViewEditorDialogue(QWidget * parent=0, const char * name=0, bool modal=FALSE, Qt::WFlags f=0);
	~QtViewEditorDialogue();
	
	/*QtViewSettingsWidget* viewsettings_tab;*/
	QtRenderSettingsWidget* rendersettings_tab;
	/*QtSlicesSettingsWidget* slicessettings_tab;
	QtCameraSettingsWidget* camerasettings_tab;*/

	void reset(void);
	void build_layout(void);
	void rebuild_tabs(void);

private slots:
	void reset_event(void);
	void apply_event(void);

signals:
	void reset_signal(void);
};

#endif // QMEASUREEDITOR_H