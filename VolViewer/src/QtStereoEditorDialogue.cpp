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

#include "QtStereoEditorDialogue.h"

QtStereoEditorDialogue::QtStereoEditorDialogue(QWidget * parent, const char * name, bool modal, Qt::WFlags f) 
:Q3TabDialog(parent, name, modal,f)
{
	setCaption("Stereo Editor");
	//setIcon( QPixmap( "icons/config.png" ));
	
	build_layout();
}

QtStereoEditorDialogue::~QtStereoEditorDialogue(void)
{
}

void QtStereoEditorDialogue::reset(void)
{
}

void QtStereoEditorDialogue::build_layout(void)
{
	//Create the tabs

	stereosettings_tab = new QtStereoSettingsWidget(this, "stereosettings_tab");
	stereosettings_tab->create();

	addTab(stereosettings_tab, "Stereo Rendering");

	//only add a close button
	setOkButton(NULL);
	setCancelButton("Close");
}

void QtStereoEditorDialogue::rebuild_tabs(void)
{
//	rendersettings_tab->rebuild();
}
void QtStereoEditorDialogue::reset_event(void)
{
}
void QtStereoEditorDialogue::apply_event(void)
{

}
