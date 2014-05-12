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

#include "QtMovieEditorDialogue.h"

QtMovieEditorDialogue::QtMovieEditorDialogue(QWidget * parent, const char * name, bool modal, Qt::WFlags f) 
:Q3TabDialog(parent, name, modal,f)
{
	setCaption("Movie Editor");
	//setIcon( QPixmap( "icons/Simple/movie.png" ));
	build_layout();
}

QtMovieEditorDialogue::~QtMovieEditorDialogue(void)
{
}

void QtMovieEditorDialogue::reset(void)
{
}

void QtMovieEditorDialogue::build_layout(void)
{
	//Create the tabs
	moviesettings_tab = new QtMovieSettingsWidget(this, "Movieettings_tab");
	moviesettings_tab->create();

	addTab(moviesettings_tab, "Movies");

	//only add a close button
	setOkButton(NULL);
	setCancelButton("Close");
}

void QtMovieEditorDialogue::rebuild_tabs(void)
{
	moviesettings_tab->rebuild();
}
void QtMovieEditorDialogue::reset_event(void)
{
}
void QtMovieEditorDialogue::apply_event(void)
{

}
