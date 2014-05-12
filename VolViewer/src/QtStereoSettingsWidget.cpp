/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "QtStereoSettingsWidget.h"
//Added by qt3to4:
#include <QLabel>


QtStereoSettingsWidget::QtStereoSettingsWidget(QWidget* parent, const char* name)
: QWidget( parent, name )
{

}
QtStereoSettingsWidget::~QtStereoSettingsWidget()
{
}
void QtStereoSettingsWidget::reset(void)
{
}
void QtStereoSettingsWidget::create(void)
{
	//LOAD/SAVE
/*	QButtonGroup *loadsave_grp = new QButtonGroup( 1, QGroupBox::Horizontal, "File:", this );

		load = new QPushButton(QIconSet(QPixmap("icons/fileopen.xpm")), "Open", loadsave_grp, "open");
		connect( load, SIGNAL( clicked () ), this, SLOT( load_event() ) );

		save = new QPushButton(QIconSet(QPixmap("icons/filesave.xpm")), "Save", loadsave_grp, "save");
		connect( save, SIGNAL( clicked () ), this, SLOT( save_event() ) );
*/
		//our stereo mode & on/off
		Q3ButtonGroup* stereo_modegrp = new Q3ButtonGroup( 1, Qt::Vertical, "", this );

			stereo_onoff_pb = new QPushButton("ON\nOFF", stereo_modegrp);
			stereo_onoff_pb->setToggleButton(TRUE);
			stereo_onoff_pb->setOn(TRUE);
			stereo_onoff_pb->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
			connect( stereo_onoff_pb, SIGNAL( clicked () ), this, SLOT( stereo_onoff_event() ) );

			//stereo_rendermode	
			stereo_rendermodegrp = new Q3ButtonGroup( 1, Qt::Horizontal, "Render Mode:", stereo_modegrp );
			stereo_rendermodegrp->setExclusive(TRUE);
			connect(stereo_rendermodegrp, SIGNAL(clicked(int)), this, SLOT(stereo_rendermode_event(int)) );

				stereo_quadbuffer_pb = new QPushButton("RIGHT/LEFT Quad Buffer", stereo_rendermodegrp);
				stereo_quadbuffer_pb->setToggleButton(TRUE);
				stereo_quadbuffer_pb->setOn(TRUE);

				stereo_RBanaglyth_pb = new QPushButton("RED/BLUE Anaglyth", stereo_rendermodegrp);
				stereo_RBanaglyth_pb->setToggleButton(TRUE);
				stereo_RBanaglyth_pb->setOn(FALSE);

				stereo_RGanaglyth_pb = new QPushButton("RED/GREEN Anaglyth", stereo_rendermodegrp);
				stereo_RGanaglyth_pb->setToggleButton(TRUE);
				stereo_RGanaglyth_pb->setOn(FALSE);
				
				stereo_RCanaglyth_pb = new QPushButton("RED/CYAN Anaglyth", stereo_rendermodegrp);
				stereo_RCanaglyth_pb->setToggleButton(TRUE);
				stereo_RCanaglyth_pb->setOn(FALSE);

		//stereo_seperation size	
		stereo_seperation_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
		stereo_seperation_lb = new QLabel( "Stereo Seperation: 0", stereo_seperation_grp );
		stereo_seperation_slider = new QSlider ( 0, 100, 1, 0, Qt::Horizontal, stereo_seperation_grp, "renderquality_slider" );
		stereo_seperation_slider->setTickmarks( QSlider::Below );
		stereo_seperation_slider->setTracking(FALSE);
		stereo_seperation_slider->setTickInterval(10);
		connect(stereo_seperation_slider, SIGNAL(valueChanged(int)), this, SLOT(stereo_seperation_event(int)) );

		//zscreen plane
		zscreen_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
		zscreen_lb = new QLabel( "Z-Screen Position: 10.0", zscreen_grp );
		zscreen_slider = new QSlider ( 1, 100, 1, 10, Qt::Horizontal, zscreen_grp, "zscreen_slider" );
		zscreen_slider->setTickmarks( QSlider::Below );
		zscreen_slider->setTracking(FALSE);
		zscreen_slider->setTickInterval(10);
		connect(zscreen_slider, SIGNAL(valueChanged(int)), this, SLOT(zscreen_event(int)) );
		zscreen_grp->setHidden(true);

	Q3BoxLayout *vboxlayoutRIGHT = new Q3BoxLayout ( 0, Q3BoxLayout::TopToBottom, 0, 5, "vboxlayout");
	vboxlayoutRIGHT->addWidget(stereo_modegrp, 0, Qt::AlignTop);
	vboxlayoutRIGHT->addWidget(stereo_seperation_grp, 0, Qt::AlignTop);
	vboxlayoutRIGHT->addWidget(zscreen_grp, 0, Qt::AlignTop);
	
//	QBoxLayout *vboxlayoutLEFT = new QBoxLayout ( 0, QBoxLayout::TopToBottom, 0, 5, "vboxlayout");
//	vboxlayoutLEFT->setAlignment(Qt::AlignTop);
//	vboxlayoutLEFT->addWidget(loadsave_grp, 0, Qt::AlignTop);

	Q3BoxLayout *topmost= new Q3BoxLayout ( this, Q3BoxLayout::LeftToRight, 10, 5, "topmost");
//	topmost->addLayout(vboxlayoutLEFT,1);
	topmost->addLayout(vboxlayoutRIGHT, 10);
}
void QtStereoSettingsWidget::apply(void)
{

}
void QtStereoSettingsWidget::reload(void)
{
}
void QtStereoSettingsWidget::rebuild(void)
{
}
void QtStereoSettingsWidget::load_event(void)
{

}
void QtStereoSettingsWidget::save_event(void)
{

}
void QtStereoSettingsWidget::set_fromini(int toggle, int mode, float value)
{
	if(toggle) stereo_onoff_pb->setOn(TRUE);
	else stereo_onoff_pb->setOn(FALSE);

	stereo_onoff_event();

	if(mode==0) stereo_quadbuffer_pb->setOn(TRUE);
	else if(mode==1) stereo_RBanaglyth_pb->setOn(TRUE);
	else if(mode==2) stereo_RGanaglyth_pb->setOn(TRUE);
	else if(mode==3) stereo_RCanaglyth_pb->setOn(TRUE);

	int weight = (float)value*100.0f;
	stereo_seperation_event(weight);
}

void QtStereoSettingsWidget::stereo_onoff_event(void)
{
	if(stereo_onoff_pb->isOn())
	{
		//printf("ON\n");
		emit stereo_onoff_signal(1);

		stereo_rendermodegrp->setEnabled(TRUE);
		stereo_seperation_grp->setEnabled(TRUE);
		zscreen_grp->setEnabled(TRUE);
	}
	else
	{
		//printf("OFF\n");
		emit stereo_onoff_signal(0);

		stereo_rendermodegrp->setEnabled(FALSE);
		stereo_seperation_grp->setEnabled(FALSE);
		zscreen_grp->setEnabled(FALSE);
	}
}
void QtStereoSettingsWidget::stereo_rendermode_event(int id)
{
//	printf("%d\n", id);

	emit stereo_rendermode_signal(id);
/*	if(id==0)
	{
		//Anaglyth mode
		emit stereo_rendermode_signal(0);
	}
	else
	{
		//Quad buffer mode
		emit stereo_rendermode_signal(1);
	}*/
}
void QtStereoSettingsWidget::stereo_seperation_event(int value)
{
//	printf("%d\n", value);

	float weight = (float)value/100.0f;

	stereo_seperation_slider->setValue(value);

	//update our slider label
	QString s;
	QString text;
	text = "Stereo Seperation: ";
	s.setNum(weight);
	text += s;
	stereo_seperation_lb->setText (text);

	emit stereo_seperation_signal(weight);
}
void QtStereoSettingsWidget::zscreen_event(int value)
{
	zscreen_slider->setValue(value);

	//update our slider label
	QString s;
	QString text;
	text = "Z-Screen Position: ";
	s.setNum(value);
	text += s;
	zscreen_lb->setText (text);

	emit stereo_zscreen_signal(value);
}



