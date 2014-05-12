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

#include "QtTransformEditorDialogue.h"

QtTransformEditorDialogue::QtTransformEditorDialogue(QWidget * parent, const char * name, bool modal, Qt::WFlags f) 
:Q3TabDialog(parent, name, modal,f)
{
	setCaption("Clipping Editor");
	setIcon( QPixmap( "icons/Simple/cut.png" ));
	build_layout();
}

QtTransformEditorDialogue::~QtTransformEditorDialogue(void)
{
}

void QtTransformEditorDialogue::reset(void)
{
}

void QtTransformEditorDialogue::build_layout(void)
{
	cliptab = new QtCLippingWidget(this, "cliptab");
	cliptab->create();
 
	//our tab pages
	addTab(cliptab, "Clipping");

	//some buttons
//	setApplyButton ("Apply");
//	setDefaultButton("Undo");
//	setHelpButton("Reload");
	setCancelButton("Close");
	setOkButton(0);

//	connect(this, SIGNAL(applyButtonPressed()), this, SLOT(apply_event()));
//	connect(this, SIGNAL(defaultButtonPressed()), this, SLOT(undo_event()));
//	connect(this, SIGNAL(helpButtonPressed()), this, SLOT(reload_event()));
	connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(close_event()));
}

void QtTransformEditorDialogue::rebuild_tabs(void)
{
}
void QtTransformEditorDialogue::undo_event(void)
{
	emit undo_signal();
}
void QtTransformEditorDialogue::close_event(void)
{
	reset();
	cliptab->exit();

	//emit close_signal();
}
void QtTransformEditorDialogue::reload_event(void)
{
	emit reload_signal();
}
void QtTransformEditorDialogue::apply_event(void)
{
	if(cliptab==currentPage())
	{
		emit clip_signal();
	}
}
