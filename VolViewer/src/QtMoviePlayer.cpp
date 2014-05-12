/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include <QtGui>

#include "QtMoviePlayer.h"

 QtMoviePlayer::QtMoviePlayer(QWidget *parent, vector< QPixmap >* frames)
     : QWidget(parent)
{
	movieframes = frames;

	if(movieframes->empty()) return;

	timeLine = new QTimeLine(1000, this);

	movieLabel = new QLabel(tr("No movie loaded"));
	movieLabel->setAlignment(Qt::AlignCenter);
	movieLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	movieLabel->setBackgroundRole(QPalette::Dark);
	movieLabel->setAutoFillBackground(true);
	movieLabel->setScaledContents(true);
	movieLabel->resize(256,256);

	createControls();
	createButtons();

	mainLayout = new QVBoxLayout;
	mainLayout->addWidget(movieLabel);
	mainLayout->addLayout(controlsLayout);
	mainLayout->addLayout(buttonsLayout);
	setLayout(mainLayout);


	//-----------------------------------------------------------------------------------------

	connect(playButton, SIGNAL(clicked()), timeLine, SLOT(start()));
	connect(stopButton, SIGNAL(clicked()), timeLine, SLOT(stop()));
	connect(pauseButton, SIGNAL(clicked(bool)), timeLine, SLOT(setPaused(bool)));
	connect(saveButton, SIGNAL(clicked(void)), this, SLOT(saveButton_event(void)));
    connect(speedSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setFrameRate(int)));
	connect(frameSlider, SIGNAL(valueChanged(int)), this, SLOT(goToFrame(int)));
	connect(timeLine, SIGNAL(frameChanged(int)), frameSlider, SLOT(setValue(int)));
	connect(timeLine, SIGNAL(stateChanged(QTimeLine::State)), this, SLOT(updateButtons()));


	frameSlider->setMinimum(0);
	frameSlider->setMaximum(movieframes->size()-1);

	// Construct a 1-second timeline with a frame range of 0 - 100
	timeLine->setFrameRange(0, movieframes->size());
	timeLine->setLoopCount(0);

	setWindowTitle(tr("Movie Player"));
	resize(400, 400);

	current_index = 0;

	// get label dimensions
	int w = movieLabel->width();
	int h = movieLabel->height();

	// set a scaled pixmap to a w x h window keeping its aspect ratio 
	movieLabel->setScaledContents(false);

	QPalette	palette = movieLabel->palette();
	palette.setColor(movieLabel->backgroundRole(),QColor(Qt::black));
	movieLabel->setPalette(palette);
	movieLabel->setAutoFillBackground(true);

	movieLabel->setPixmap(movieframes->at(current_index).scaled(w,h,Qt::KeepAspectRatio));

	/*QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	sizePolicy.setHeightForWidth(true);
	setSizePolicy( sizePolicy); */
}
int QtMoviePlayer::heightForWidth ( int w )
{
	return w;
}
void QtMoviePlayer::updateButtons()
{
	playButton->setEnabled(timeLine->state() == QTimeLine::NotRunning);
	pauseButton->setEnabled(timeLine->state() != QTimeLine::NotRunning);
	pauseButton->setChecked(timeLine->state() == QTimeLine::Paused);
	stopButton->setEnabled(timeLine->state() != QTimeLine::NotRunning);
}


 void QtMoviePlayer::createControls()
 {
     frameLabel = new QLabel(tr("Current frame:"));

     frameSlider = new QSlider(Qt::Horizontal);
     frameSlider->setTickPosition(QSlider::TicksBelow);
     frameSlider->setTickInterval(1);

     speedLabel = new QLabel(tr("Speed:"));

     speedSpinBox = new QSpinBox;
     speedSpinBox->setRange(1, 9999);
     speedSpinBox->setValue(100);
     speedSpinBox->setSuffix(tr("%"));

     controlsLayout = new QGridLayout;
     controlsLayout->addWidget(frameLabel, 1, 0);
     controlsLayout->addWidget(frameSlider, 1, 1, 1, 2);
     controlsLayout->addWidget(speedLabel, 2, 0);
     controlsLayout->addWidget(speedSpinBox, 2, 1);
 }

 void QtMoviePlayer::createButtons()
 {
     QSize iconSize(36, 36);

     playButton = new QToolButton;
     playButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
     playButton->setIconSize(iconSize);
     playButton->setToolTip(tr("Play"));

     pauseButton = new QToolButton;
     pauseButton->setCheckable(true);
     pauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
     pauseButton->setIconSize(iconSize);
     pauseButton->setToolTip(tr("Pause"));

     stopButton = new QToolButton;
     stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));
     stopButton->setIconSize(iconSize);
     stopButton->setToolTip(tr("Stop"));

     saveButton = new QToolButton;
	 saveButton->setIcon(style()->standardIcon(QStyle::SP_DriveFDIcon));
     saveButton->setIconSize(iconSize);
     saveButton->setToolTip(tr("Save"));

	 buttonsLayout = new QHBoxLayout;
     buttonsLayout->addStretch();
	 buttonsLayout->addWidget(saveButton);
	 buttonsLayout->addSpacing(36);
     buttonsLayout->addStretch();
     buttonsLayout->addWidget(playButton);
     buttonsLayout->addWidget(pauseButton);
     buttonsLayout->addWidget(stopButton);
     buttonsLayout->addStretch();
 }
void QtMoviePlayer::resizeEvent ( QResizeEvent * event )
{
	// get label dimensions
	int w = movieLabel->width();
	int h = movieLabel->height();

	// set a scaled pixmap to a w x h window keeping its aspect ratio 
	movieLabel->setPixmap(movieframes->at(current_index).scaled(w,h,Qt::KeepAspectRatio));
}

void QtMoviePlayer::goToFrame(int i)
{
	current_index = i;
	//movieLabel->setPixmap(movieframes->at(i));

	// get label dimensions
	int w = movieLabel->width();
	int h = movieLabel->height();

	// set a scaled pixmap to a w x h window keeping its aspect ratio 
	movieLabel->setPixmap(movieframes->at(i).scaled(w,h,Qt::KeepAspectRatio));
}
void QtMoviePlayer::setFrameRate(int i)
{
	float time = 1000.0*((float)100/(float)i);
	timeLine->setDuration(time);
	timeLine->setCurrentTime(0);
 }
void QtMoviePlayer::saveButton_event(void)
{
	QFileDialog* fd = new QFileDialog( this );
	fd->setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);

	//Load our folder structure.
	QString dn = fd->getExistingDirectory(this, "Please select a Directory", 0);

	if(dn.isEmpty())
	{
		return;
	}

	for(int i=0; i<movieframes->size(); i++)
	{
		QString numb;
		numb.setNum(i);
		movieframes->at(i).save(dn+"/Frame_"+numb+".png", "PNG");
	}
}
