/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QTVOLEDITOR_H
#define QTVOLEDITOR_H

#include <q3tabdialog.h> 
#include <qpainter.h>
#include <qlabel.h>

//our tab widgets
#include "QtIsoSurfaceWidget.h"
//#include "QtFilterWidget.h"
class QtVolumeEditorDialogue: public Q3TabDialog
{
	 Q_OBJECT

public:		

	//our tab widgets
	QtIsoSurfaceWidget*	isosurface_tab;
	//QtFilterWidget*	    filter_tab;

	QtVolumeEditorDialogue(QWidget * parent=0, const char * name=0, bool modal=FALSE, Qt::WFlags f=0);
	~QtVolumeEditorDialogue();
	
	void reset(void);
	void build_layout(void);
	void rebuild_tabs(void);

private slots:
	void apply_event(void);
	void reload_event(void);
	void undo_event(void);
	void close_event(void);

signals:
	void reload_signal(void);
	void undo_signal(void);
	void close_signal(void);
};

#endif // QTVOLEDITOR_H