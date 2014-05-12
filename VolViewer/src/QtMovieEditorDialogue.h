/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QMovieEDITOR_H
#define QMovieEDITOR_H

#include <q3tabdialog.h> 
#include <qpainter.h>
#include <qlabel.h>

#include "QtMovieSettingsWidget.h"

class QtMovieEditorDialogue: public Q3TabDialog
{
	 Q_OBJECT

public:	

	QtMovieEditorDialogue(QWidget * parent=0, const char * name=0, bool modal=FALSE, Qt::WFlags f=0);
	~QtMovieEditorDialogue();
	
	QtMovieSettingsWidget* moviesettings_tab;

	void reset(void);
	void build_layout(void);
	void rebuild_tabs(void);

private slots:
	void reset_event(void);
	void apply_event(void);

};

#endif // QMovieEDITOR_H