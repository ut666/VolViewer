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

#include "QtViewEditorDialogue.h"

QtViewEditorDialogue::QtViewEditorDialogue(QWidget * parent, const char * name, bool modal, Qt::WFlags f) 
:Q3TabDialog(parent, name, modal,f)
{
	setCaption("Settings Editor");
	//setIcon( QPixmap( "icons/Simple/spanner.png" ));
	
	build_layout();
}

QtViewEditorDialogue::~QtViewEditorDialogue(void)
{
}

void QtViewEditorDialogue::reset(void)
{
}

void QtViewEditorDialogue::build_layout(void)
{
	//Create the tabs
/*	viewsettings_tab = new QtViewSettingsWidget(this, "viewsettings_tab");
	viewsettings_tab->create();
*/
	rendersettings_tab = new QtRenderSettingsWidget(this, "rendersettings_tab");
	rendersettings_tab->create();

/*
	slicessettings_tab = new QtSlicesSettingsWidget(this, "slicessettings_tab");
	slicessettings_tab->create();
*/
//	lightettings_tab = new QtLightSettingsWidget(this, "lightettings_tab");
//	lightettings_tab->create();

/*	camerasettings_tab = new QtCameraSettingsWidget(this, "camerasettings_tab");
	camerasettings_tab->create();
*/	
	//add them to the widget
//	addTab(viewsettings_tab, "Viewing");
//	addTab(slicessettings_tab, "Slices");
//	addTab(camerasettings_tab, "Camera");
//	addTab(lightettings_tab, "Lighting");
	addTab(rendersettings_tab, "Settings");

	//only add a close button
	setOkButton(NULL);
	setCancelButton("Close");
}

void QtViewEditorDialogue::rebuild_tabs(void)
{
//	printf("BOH\n");
	rendersettings_tab->rebuild();
}
void QtViewEditorDialogue::reset_event(void)
{
}
void QtViewEditorDialogue::apply_event(void)
{

}
