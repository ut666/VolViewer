/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "QtViewSettingsWidget.h"
//Added by qt3to4:
#include <Q3Frame>
#include <QLabel>


QtViewSettingsWidget::QtViewSettingsWidget(QWidget* parent, const char* name)
: QWidget( parent, name )
{
}
QtViewSettingsWidget::~QtViewSettingsWidget()
{
}
void QtViewSettingsWidget::reset(void)
{
}
void QtViewSettingsWidget::create(void)
{
		//GENERAL VIEWING PREFERENCES
		Q3ButtonGroup *general_grp = new Q3ButtonGroup( 3, Qt::Vertical, "General Viewing Preferences:", this );
		general_grp->setLineWidth(2);
		general_grp->setFrameStyle( Q3Frame::Box | Q3Frame::Plain );
			   
			Q3Grid *colour_grid = new Q3Grid( 2, general_grp );
			colour_grid->setSpacing(5);

				Q3ButtonGroup *bb_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Bounding Box Colour:", colour_grid );

						bbcol_lb = new QLabel("", bb_grp);
						bbcol_lb->setFrameStyle(Q3Frame::Panel | Q3Frame::Raised);
						bbcol_lb->setFixedSize(32, 32);
						bbcol_lb->setBackgroundColor(bbcolour);

						QPushButton* bbcol_btn = new QPushButton("Change", bb_grp);
						connect( bbcol_btn, SIGNAL( clicked() ), this, SLOT(bb_colourdlg_event() ) );

				    Q3ButtonGroup *bg_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Background Colour", colour_grid );

						bgcol_lb = new QLabel("", bg_grp);
						bgcol_lb->setFrameStyle(Q3Frame::Panel | Q3Frame::Raised);
						bgcol_lb->setFixedSize(32, 32);
						bgcol_lb->setBackgroundColor(bgcolour);

						QPushButton* bgcol_btn = new QPushButton("Change", bg_grp);
						connect( bgcol_btn, SIGNAL( clicked() ), this, SLOT(bg_colourdlg_event() ) );

			Q3ButtonGroup *toggles_grp = new Q3ButtonGroup( 3, Qt::Horizontal, "Enable/Disable:", general_grp );

				QPushButton* volume_pb = new QPushButton("Volume", toggles_grp);
				volume_pb->setToggleButton(TRUE);
				volume_pb->setOn(TRUE);

				QPushButton* surface_pb = new QPushButton("Surface", toggles_grp);
				surface_pb->setToggleButton(TRUE);
				surface_pb->setOn(TRUE);

				QPushButton* cursor_pb = new QPushButton("Cursor", toggles_grp);
				cursor_pb->setToggleButton(TRUE);
				cursor_pb->setOn(TRUE);
 
				QPushButton* fps_pb = new QPushButton("FPS", toggles_grp);
				fps_pb->setToggleButton(TRUE);
				fps_pb->setOn(TRUE);

				QPushButton* axis_pb = new QPushButton("Axis", toggles_grp);
				axis_pb->setToggleButton(TRUE);
				axis_pb->setOn(TRUE);

				QPushButton* bb_pb = new QPushButton("Bounding Box", toggles_grp);
				bb_pb->setToggleButton(TRUE);
				bb_pb->setOn(TRUE);

	//TOPMOST LAYOUT
	Q3BoxLayout *topmost= new Q3BoxLayout ( this, Q3BoxLayout::TopToBottom, 5, 10, "topmost");
	topmost->addWidget(general_grp,1);
}
void QtViewSettingsWidget::apply(void)
{

}

void QtViewSettingsWidget::rebuild(void)
{
}
void QtViewSettingsWidget::bb_colourdlg_event(void)
{
	QColorDialog* coldialogue=0;
	bbcolour = coldialogue->getColor(bbcolour, this, "Choose a colour");
	bbcol_lb->setBackgroundColor(bbcolour);
}
void QtViewSettingsWidget::bg_colourdlg_event(void)
{
	QColorDialog* coldialogue=0;
	bgcolour = coldialogue->getColor(bgcolour, this, "Choose a colour");
	bgcol_lb->setBackgroundColor(bgcolour);
}
