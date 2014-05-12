/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include <qlayout.h>
#include <qlabel.h> 
#include <qslider.h>
//Added by qt3to4:
#include <QPixmap>
//#include <qmotifplusstyle.h>	//style

#include <math.h> 

#include "QtCropEditorDialogue.h"

QtCropEditorDialogue::QtCropEditorDialogue(QWidget * parent, const char * name, bool modal, Qt::WFlags f, QString app_path) 
:Q3TabDialog(parent, name, modal,f)
{
	appPath = app_path;
	setCaption("Crop Editor");
	//setIcon( QPixmap( "Icons/crop.png" ));
	build_layout();
}

QtCropEditorDialogue::~QtCropEditorDialogue(void)
{
}

void QtCropEditorDialogue::reset(void)
{
	croptab->reset();
}

void QtCropEditorDialogue::build_layout(void)
{
	croptab = new QtCropWidget(this, "filttab", appPath);
	croptab->create();
 
	//our tab pages
	addTab(croptab, "Crop");

	//some buttons
	setCancelButton("Close");
	setOkButton(0);

	connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(close_event()));
}

void QtCropEditorDialogue::rebuild_tabs(void)
{
}
void QtCropEditorDialogue::undo_event(void)
{
	emit undo_signal();
}
void QtCropEditorDialogue::close_event(void)
{
	reset();
	croptab->exit();
}
void QtCropEditorDialogue::reload_event(void)
{
	emit reload_signal();
}
void QtCropEditorDialogue::apply_event(void)
{
	if(croptab==currentPage())
	{
		//emit crop_signal();
	}
}
