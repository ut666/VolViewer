/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QTMEASEDITOR_H
#define QTMEASEDITOR_H

#include <q3tabdialog.h> 
#include <qpainter.h>
#include <qlabel.h>

//our tab widgets
#include "QtMeasuringWidget.h"

class QtMeasuringEditorDialogue: public Q3TabDialog
{
	 Q_OBJECT

public:		

	//our tab widgets
	QString apppath;
	QtMeasuringWidget*	measuring_tab;

	QtMeasuringEditorDialogue(QWidget * parent=0, const char * name=0, bool modal=FALSE, Qt::WFlags f=0, QString appPath="");
	~QtMeasuringEditorDialogue();
	
	void reset(void);
	void build_layout(void);
	void rebuild_tabs(void);

signals:
	void reload_signal(void);
	void undo_signal(void);
	void close_signal(void);
};

#endif // QTMEASEDITOR_H