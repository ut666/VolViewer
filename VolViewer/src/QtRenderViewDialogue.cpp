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

#include "QtRenderViewDialogue.h"

QtRenderViewDialogue::QtRenderViewDialogue(QWidget * parent, const char * name, bool modal, Qt::WFlags f) 
:Q3TabDialog(parent, name, modal,f)
{
	renviewtab = 0;

	setCaption("Visual Bookmarks");
	//setIcon( QPixmap( "Icons/bookmarks.png" ));
	build_layout();
}

QtRenderViewDialogue::~QtRenderViewDialogue(void)
{
}

void QtRenderViewDialogue::reset(void)
{
	renviewtab->reset();
}

void QtRenderViewDialogue::build_layout(void)
{
	renviewtab = new QtRenderViewWidget(this, "renviewtab");
	renviewtab->create();
 
	//our tab pages
	addTab(renviewtab, "Visual Bookmarks");

	//some buttons
	setCancelButton("Close");
	setOkButton(0);

	connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(close_event()));

	resize(320, 600);
}

void QtRenderViewDialogue::rebuild_tabs(void)
{
}
void QtRenderViewDialogue::undo_event(void)
{
	//emit undo_signal();
}
void QtRenderViewDialogue::close_event(void)
{
	reset();
	renviewtab->exit();
}
void QtRenderViewDialogue::reload_event(void)
{
	//emit reload_signal();
}
void QtRenderViewDialogue::apply_event(void)
{
	if(renviewtab==currentPage())
	{
		//emit crop_signal();
	}
}
