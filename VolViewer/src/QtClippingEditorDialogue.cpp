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

#include "QtClippingEditorDialogue.h"

QtClippingEditorDialogue::QtClippingEditorDialogue(QWidget * parent, const char * name, bool modal, Qt::WFlags f) 
:Q3TabDialog(parent, name, modal,f)
{
	setCaption("Clipping Editor");
	//setIcon( QPixmap( "Icons/Simple/cut.png" ));
	build_layout();
}

QtClippingEditorDialogue::~QtClippingEditorDialogue(void)
{
}

void QtClippingEditorDialogue::reset(void)
{
}

void QtClippingEditorDialogue::build_layout(void)
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

void QtClippingEditorDialogue::rebuild_tabs(void)
{
}
void QtClippingEditorDialogue::undo_event(void)
{
	emit undo_signal();
}
void QtClippingEditorDialogue::close_event(void)
{
	reset();
	cliptab->exit();

	//emit close_signal();
}
void QtClippingEditorDialogue::reload_event(void)
{
	emit reload_signal();
}
void QtClippingEditorDialogue::apply_event(void)
{
	if(cliptab==currentPage())
	{
		emit clip_signal();
	}
}
