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

#include "QtFilterEditorDialogue.h"

QtFilterEditorDialogue::QtFilterEditorDialogue(QWidget * parent, const char * name, bool modal, Qt::WFlags f) 
:Q3TabDialog(parent, name, modal,f)
{
	setCaption("Filter Editor");
	//setIcon( QPixmap( "Icons/filter.png" ));
	build_layout();
}

QtFilterEditorDialogue::~QtFilterEditorDialogue(void)
{
}

void QtFilterEditorDialogue::reset(void)
{
}

void QtFilterEditorDialogue::build_layout(void)
{
	filtertab = new QtFilterWidget(this, "filttab");
	filtertab->create();
 
	//our tab pages
	addTab(filtertab, "Filters");

	//some buttons
	setCancelButton("Close");
	setOkButton(0);

	connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(close_event()));
}

void QtFilterEditorDialogue::rebuild_tabs(void)
{
}
void QtFilterEditorDialogue::undo_event(void)
{
	emit undo_signal();
}
void QtFilterEditorDialogue::close_event(void)
{
	reset();
	filtertab->exit();
}
void QtFilterEditorDialogue::reload_event(void)
{
	emit reload_signal();
}
void QtFilterEditorDialogue::apply_event(void)
{
	if(filtertab==currentPage())
	{
		emit filter_signal();
	}
}
