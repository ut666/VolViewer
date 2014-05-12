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

#include "QtCameraEditorDialogue.h"

QtCameraEditorDialogue::QtCameraEditorDialogue(QWidget * parent, const char * name, bool modal, Qt::WFlags f) 
:Q3TabDialog(parent, name, modal,f)
{
	setCaption("Camera Editor");
	//setIcon( QPixmap( "icons/config.png" ));
	
	build_layout();
}

QtCameraEditorDialogue::~QtCameraEditorDialogue(void)
{
}

void QtCameraEditorDialogue::reset(void)
{
}

void QtCameraEditorDialogue::build_layout(void)
{
	//Create the tabs

	camerasettings_tab = new QtCameraSettingsWidget(this, "camerasettings_tab");
	camerasettings_tab->create();

	addTab(camerasettings_tab, "Camera Settings");

	//only add a close button
	setOkButton(NULL);
	setCancelButton("Close");
}

void QtCameraEditorDialogue::rebuild_tabs(void)
{
//	rendersettings_tab->rebuild();
}
void QtCameraEditorDialogue::reset_event(void)
{
}
void QtCameraEditorDialogue::apply_event(void)
{

}
