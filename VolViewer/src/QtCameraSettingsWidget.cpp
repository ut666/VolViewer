/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "QtCameraSettingsWidget.h"
//Added by qt3to4:
#include <QLabel>


QtCameraSettingsWidget::QtCameraSettingsWidget(QWidget* parent, const char* name)
: QWidget( parent, name )
{
	camerafov = 45;
	camerastep = 5;
}
QtCameraSettingsWidget::~QtCameraSettingsWidget()
{
}
void QtCameraSettingsWidget::reset(void)
{
}
void QtCameraSettingsWidget::create(void)
{	
	//our camera mode buttons
	Q3ButtonGroup* camera_modegrp = new Q3ButtonGroup( 2, Qt::Horizontal, "Camera Mode:", this );
	camera_modegrp->setExclusive(true);
	connect(camera_modegrp, SIGNAL( clicked (int) ), this, SLOT( camerasettings_mode_slot(int) ) );

		camera_ortho_pb = new QPushButton("Orthogonal\nCamera", camera_modegrp);
		camera_ortho_pb->setToggleButton(TRUE);
		camera_ortho_pb->setOn(TRUE);
		camera_ortho_pb->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
		//connect( camera_ortho_pb, SIGNAL( clicked () ), this, SLOT( stereo_onoff_event() ) );

		camera_persp_pb = new QPushButton("Perspective\nCamera", camera_modegrp);
		camera_persp_pb->setToggleButton(TRUE);
		camera_persp_pb->setOn(FALSE);
		camera_persp_pb->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
		///connect( camera_persp_pb, SIGNAL( clicked () ), this, SLOT( stereo_onoff_event() ) );

	perpectivesettings_grp = new Q3ButtonGroup( 2, Qt::Vertical, "Perspective Camera:", this );
	perpectivesettings_grp->setDisabled(true);

			Q3ButtonGroup* cameraFOV_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", perpectivesettings_grp );
			camerafov_lb = new QLabel( "Field of View: 45", cameraFOV_grp );
			camerafov = 45;
			camerafov_slider = new QSlider ( 1, 180, 5, 45, Qt::Horizontal, cameraFOV_grp, "numbsliceslod_slider" );
			camerafov_slider->setTickmarks( QSlider::Below );
			camerafov_slider->setTracking(FALSE);
			camerafov_slider->setTickInterval(32);
			connect(camerafov_slider, SIGNAL(valueChanged(int)), this, SLOT(camerasettings_fov_slot(int)) );
			camerasettings_fov_slot(camerafov);


			Q3ButtonGroup* camerastep_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", perpectivesettings_grp );
			camerastep_lb = new QLabel( "Camera Step Size: 5", camerastep_grp );
			camerastep = 5;
			camerastep_slider = new QSlider ( 1, 100, 5, 5, Qt::Horizontal, camerastep_grp, "numbsliceslod_slider" );
			camerastep_slider->setTickmarks( QSlider::Below );
			camerastep_slider->setTracking(FALSE);
			camerastep_slider->setTickInterval(10);
			connect(camerastep_slider, SIGNAL(valueChanged(int)), this, SLOT(camerasettings_step_slot(int)) );
			camerasettings_step_slot(camerastep);


	Q3BoxLayout *vboxlayoutRIGHT = new Q3BoxLayout ( 0, Q3BoxLayout::TopToBottom, 0, 5, "vboxlayout");
	vboxlayoutRIGHT->addWidget(camera_modegrp, 0, Qt::AlignTop);
	vboxlayoutRIGHT->addWidget(perpectivesettings_grp, 0, Qt::AlignTop);
	
	Q3BoxLayout *topmost= new Q3BoxLayout ( this, Q3BoxLayout::LeftToRight, 10, 5, "topmost");
	topmost->addLayout(vboxlayoutRIGHT, 10);
}
void QtCameraSettingsWidget::apply(void)
{

}
void QtCameraSettingsWidget::reload(void)
{
}
void QtCameraSettingsWidget::rebuild(void)
{
}
void QtCameraSettingsWidget::camerasettings_setmode_slot(int id, int fov)
{
	printf("camerasettings_setmode_slot\n");
	if(id==0)
	{
		camera_ortho_pb->setOn(TRUE);
		camera_persp_pb->setOn(FALSE);
		perpectivesettings_grp->setDisabled(true);
		//emit camerasettings_mode_signal(0);
	}
	else
	{
		camera_persp_pb->setOn(TRUE);
		camera_ortho_pb->setOn(FALSE);
		perpectivesettings_grp->setDisabled(false);
		camerasettings_fov_slot(fov);
		//emit camerasettings_mode_signal(1);
	}
}
void QtCameraSettingsWidget::camerasettings_mode_slot(int id)
{
	if(id==0)
	{
		perpectivesettings_grp->setDisabled(true);
		emit camerasettings_mode_signal(0);
	}
	else
	{
		perpectivesettings_grp->setDisabled(false);
		emit camerasettings_mode_signal(1);
	}
}
void QtCameraSettingsWidget::camerasettings_fov_slot(int val)
{
	camerafov = val;
	camerafov_slider->setValue(camerafov);

	//update our slider label
	QString s;
	QString text;
	text = "Field of View: ";
	s.setNum(val);
	text += s;
	camerafov_lb->setText (text);

	emit camerasettings_fov_signal(camerafov);
}
void QtCameraSettingsWidget::camerasettings_step_slot(int val)
{
	camerastep = val;
	camerastep_slider->setValue(camerastep);

	//update our slider label
	QString s;
	QString text;
	text = "Camera Step Size: ";
	s.setNum(val);
	text += s;
	camerastep_lb->setText (text);

	emit camerasettings_step_signal(camerastep);

}


