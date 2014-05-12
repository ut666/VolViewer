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

#include "QtVolumeEditorDialogue.h"

QtVolumeEditorDialogue::QtVolumeEditorDialogue(QWidget * parent, const char * name, bool modal, Qt::WFlags f) 
:Q3TabDialog(parent, name, modal,f)
{
	QSizePolicy sizepolicy;
	sizepolicy.setHeightForWidth(TRUE);
	setSizePolicy(sizepolicy);
 
	setCaption("Iso-Surface Editor");
	//setIcon( QPixmap( "icons/Simple/surface.png" ));

	build_layout();
}

QtVolumeEditorDialogue::~QtVolumeEditorDialogue(void)
{
}

void QtVolumeEditorDialogue::reset(void)
{
	isosurface_tab->reset();
//	filter_tab->reset();
}

void QtVolumeEditorDialogue::build_layout(void)
{
	//iso value tab, needs a pointer to our histogram data
	isosurface_tab = new QtIsoSurfaceWidget(this, "isovaluetab");
	isosurface_tab->create();

//	filter_tab = new QtFilterWidget(this, "filter_tab");
//	filter_tab->create();

	//our tab pages
	addTab(isosurface_tab, "IsoSurface");
//	addTab(filter_tab, "3D Volume Filters");

	//some buttons
//	setApplyButton ("Apply");
//	setDefaultButton("Undo");
//	setHelpButton("Reload");
	setCancelButton("Close");
	setOkButton(0);

	connect(this, SIGNAL(applyButtonPressed()), this, SLOT(apply_event()));
	connect(this, SIGNAL(defaultButtonPressed()), this, SLOT(undo_event()));
	connect(this, SIGNAL(helpButtonPressed()), this, SLOT(reload_event()));
	connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(close_event()));

	resize(400, 400);
}

void QtVolumeEditorDialogue::rebuild_tabs(void)
{	
//	isosurface_tab->rebuild();
//	threshold_tab->rebuild();
//	floodfill_tab->rebuild();
//	filter_tab->rebuild();
}
void QtVolumeEditorDialogue::reload_event(void)
{
	emit reload_signal();
}
void QtVolumeEditorDialogue::undo_event(void)
{
	emit undo_signal();
}
void QtVolumeEditorDialogue::close_event(void)
{
	reset();
//	emit close_signal();

}
void QtVolumeEditorDialogue::apply_event(void)
{
	/*if(isosurface_tab==currentPage())
	{
		isosurface_tab->emit_isofurface();
	}*/
/*	else if(filter_tab==currentPage())
	{
		filter_tab->emit_filter();
	}*/
}
