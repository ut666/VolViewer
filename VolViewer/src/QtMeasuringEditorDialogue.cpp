/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include <qlayout.h>
#include <qlabel.h> 
#include <qslider.h>
#include <QPixmap>

#include <math.h> 

#include "QtMeasuringEditorDialogue.h"

QtMeasuringEditorDialogue::QtMeasuringEditorDialogue(QWidget * parent, const char * name, bool modal, Qt::WFlags f, QString appPath) 
:Q3TabDialog(parent, name, modal,f)
{
	apppath = appPath;
	QSizePolicy sizepolicy;
	sizepolicy.setHeightForWidth(TRUE);
	setSizePolicy(sizepolicy);
 
	setCaption("Measuring Editor");
	//setIcon( QPixmap( "Icons/ruler.png" ));

	build_layout();
}

QtMeasuringEditorDialogue::~QtMeasuringEditorDialogue(void)
{
}

void QtMeasuringEditorDialogue::reset(void)
{
	measuring_tab->reset();
}

void QtMeasuringEditorDialogue::build_layout(void)
{
//	printf("creating measuretab\n");

	//measuring tab
	measuring_tab = new QtMeasuringWidget(this, "measuretab", apppath);
	measuring_tab->create();

	//our tab pages
	addTab(measuring_tab, "Measuring");

	setCancelButton("Close");
	setOkButton(0);
}
