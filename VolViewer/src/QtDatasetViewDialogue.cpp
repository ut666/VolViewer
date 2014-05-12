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

#include "QtDatasetViewDialogue.h"

QtDatasetViewDialogue::QtDatasetViewDialogue(QWidget * parent, const char * name, bool modal, Qt::WFlags f) 
:Q3TabDialog(parent, name, modal,f)
{
	setCaption("Dataset Viewer");
	//setIcon( QPixmap( "Icons/rec.png" ));
	build_layout();
}

QtDatasetViewDialogue::~QtDatasetViewDialogue(void)
{
}

void QtDatasetViewDialogue::reset(void)
{
	datasetviewtab->reset();
}

void QtDatasetViewDialogue::build_layout(void)
{
	datasetviewtab = new QtDatasetViewWidget(this, "filttab");
	datasetviewtab->create();
 
	//our tab pages
	addTab(datasetviewtab, "DatasetView");

	//some buttons
	setCancelButton("Close");
	setOkButton(0);

	connect(this, SIGNAL(cancelButtonPressed()), this, SLOT(close_event()));

	resize(512, 512);
}

void QtDatasetViewDialogue::rebuild_tabs(void)
{
}
void QtDatasetViewDialogue::undo_event(void)
{
	emit undo_signal();
}
void QtDatasetViewDialogue::close_event(void)
{
	reset();
	datasetviewtab->exit();
}
void QtDatasetViewDialogue::reload_event(void)
{
	emit reload_signal();
}
void QtDatasetViewDialogue::apply_event(void)
{
	if(datasetviewtab==currentPage())
	{
		//emit crop_signal();
	}
}
