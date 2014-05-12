/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "QtMovieSettingsWidget.h"
//Added by qt3to4:
#include <QPixmap>
#include <QLabel>
#include <Q3Frame>


QtMovieSettingsWidget::QtMovieSettingsWidget(QWidget* parent, const char* name)
: QWidget( parent, name )
{
	defaul_movie_dir = "";
}
QtMovieSettingsWidget::~QtMovieSettingsWidget()
{
}
void QtMovieSettingsWidget::reset(void)
{
}
void QtMovieSettingsWidget::create(void)
{
/*	//LOAD/SAVE
	Q3ButtonGroup *loadsave_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "File:", this );

		load = new QPushButton(QIcon(QPixmap("icons/fileopen.xpm")), "Open", loadsave_grp, "open");
		connect( load, SIGNAL( clicked () ), this, SLOT( load_event() ) );

		save = new QPushButton(QIcon(QPixmap("icons/filesave.xpm")), "Save", loadsave_grp, "save");
		connect( save, SIGNAL( clicked () ), this, SLOT( save_event() ) );
*/
	Q3ButtonGroup *options_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Options:", this );

		choosedir = new QPushButton(QIcon(QPixmap("icons/filesave.xpm")), "Choose Output Directory", options_grp, "open");
		connect(choosedir, SIGNAL( clicked () ), this, SLOT( choosedir_event() ) );

		defaul_movie_dir = "No Output Directory Selected!!!";
		choosedir_te = new QTextEdit( defaul_movie_dir, options_grp );
		choosedir_te->setReadOnly(true);
		choosedir_te->setFixedHeight(50);
		//choosedir_te->setMaximumHeight(200);
		//choosedir_te->setTextInteractionFlags(Qt::TextBrowserInteraction);
		//choosedir_te->setScaledContents(true);

		Q3ButtonGroup *suboptions_grp = new Q3ButtonGroup( 1, Qt::Vertical, "", options_grp);
			
			Q3ButtonGroup *slider_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", suboptions_grp );
				
				moviesampling=8;
				moviesampling_lb = new QLabel( "Frame Step Size: 1", slider_grp );
				moviesampling_slider = new QSlider (1, 16, 1, 0, Qt::Horizontal, slider_grp, "moviesampling_slider" );
				moviesampling_slider->setTickmarks(QSlider::Below);
				moviesampling_slider->setTracking(FALSE);
				moviesampling_slider->setTickInterval(2);
				connect(moviesampling_slider, SIGNAL(valueChanged(int)), this, SLOT(moviesampling_event(int)) );
				moviesampling_event(moviesampling);

			preview = new QPushButton("Preview Mode", suboptions_grp, "open");
			preview->setToggleButton(true);
			preview->setDown(false);

	Q3ButtonGroup *movie_layout = new Q3ButtonGroup( 1, Qt::Horizontal, "Movie Presets:", this );

		QPushButton *rotate360		= new QPushButton("Rotation",	movie_layout);
		QPushButton *rock			= new QPushButton("Rock",	movie_layout);
		QPushButton *orthosect		= new QPushButton("Orthogonal Sections",	movie_layout);
		QPushButton *chanfade		= new QPushButton("Channel Fade",	movie_layout);
		QPushButton *crosschanfade	= new QPushButton("Channel Cross-Fade",	movie_layout);
		QPushButton *clip			= new QPushButton("Clip",	movie_layout);
		QPushButton *renderview		= new QPushButton("Visual Bookmarks", movie_layout);

		connect(rotate360,			SIGNAL(clicked()), SLOT(movie_rotate_event()));
		connect(rock,				SIGNAL(clicked()), SLOT(movie_rock_rotate_event()));
		connect(orthosect,			SIGNAL(clicked()), SLOT(movie_orthosect_event()));
		connect(clip,				SIGNAL(clicked()), SLOT(movie_clip_event()));
		connect(chanfade,			SIGNAL(clicked()), SLOT(movie_chanfade_event()));
		connect(crosschanfade,		SIGNAL(clicked()), SLOT(movie_crosschanfade_event()));
		connect(renderview,		SIGNAL(clicked()), SLOT(movie_renderview_event()));

		chanfade->hide();
		crosschanfade->hide();
		clip->hide();

	//TOPMOST LAYOUT
	Q3BoxLayout *topmost= new Q3BoxLayout ( this, Q3BoxLayout::TopToBottom, 10, 5, "topmost");
	topmost->setAlignment(Qt::AlignTop);
//	topmost->addWidget(loadsave_grp, 0, Qt::AlignTop);
	topmost->addWidget(options_grp, 0, Qt::AlignTop);
	topmost->addWidget(movie_layout, 0, Qt::AlignTop);


	rotangle = 360.0;
	faderotangle = 1080.0;
	cliprotang = 360.0;
	rockangle = 90.0;

	channelOne_selected=0;
	channelTwo_selected=1;
	channelThree_selected=2;
}
void QtMovieSettingsWidget::apply(void)
{

}

void QtMovieSettingsWidget::rebuild(void)
{

}
void QtMovieSettingsWidget::load_event(void)
{

}
void QtMovieSettingsWidget::save_event(void)
{

}
void QtMovieSettingsWidget::moviesampling_event(int j)
{
	moviesampling_slider->setValue(j);
	moviesampling=j;

	//update our slider label
	QString s;
	QString text;
	text = "Frame Step Size: ";
	s.setNum(j);
	text += s;
	moviesampling_lb->setText (text);
}
void QtMovieSettingsWidget::choosedir_event(void)
{
  	defaul_movie_dir = QFileDialog::getExistingDirectory ( this, "Please pick a save directory.", defaul_movie_dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if(defaul_movie_dir.isEmpty()) defaul_movie_dir = "No Output Directory Selected!!!";
	choosedir_te->setText(defaul_movie_dir);
}
void QtMovieSettingsWidget::movie_rock_rotate_event(void)
{
	int preview_state = preview->isOn();
	if(defaul_movie_dir == "No Output Directory Selected!!!" && preview_state==0)
	{
		QMessageBox::warning(this, "Warning...",
		"Please specify an Output Directory or select Preview Mode \n",
		QMessageBox::Ok | QMessageBox::Default );		
		return;
	}

    rotationaxis_dlg = new QDialog(this, "Select Rotation Axis", TRUE);

	Q3BoxLayout* movie_layout = new Q3BoxLayout(rotationaxis_dlg , Q3BoxLayout::TopToBottom, 4, 4, "rotationmovie_layout");

	/*Q3ButtonGroup *axis_layout = new Q3ButtonGroup( 1, Qt::Vertical, "Select Axis of Rotation: ", this);

				xbutton		= new QPushButton("X",	axis_layout);
				xbutton->setToggleButton(true);
				xbutton->setOn(false);

				ybutton		= new QPushButton("Y",	axis_layout);
				ybutton->setToggleButton(true);
				ybutton->setOn(true);

				zbutton		= new QPushButton("Z",	axis_layout);
				zbutton->setToggleButton(true);
				zbutton->setOn(false);
	*/
	Q3ButtonGroup *angle_layout = new Q3ButtonGroup( 1, Qt::Vertical, "Specify Rock Angle: ", this);
		QString ang;
		ang.setNum(rockangle);
		angle_le = new QLineEdit(ang, angle_layout);
		angle_le->setInputMask("9000.00");

	QPushButton *okbutton		= new QPushButton("OK",	this);
	connect(okbutton, SIGNAL( clicked () ), this, SLOT( okbuttonrock_event() ) );

	QPushButton *cancelbutton		= new QPushButton("Cancel",	this);
	connect(cancelbutton, SIGNAL( clicked () ), this, SLOT( cancbuttonrot_event() ) );

	movie_layout->addWidget(angle_layout);
	//movie_layout->addWidget(axis_layout);
	movie_layout->addWidget(okbutton);
	movie_layout->addWidget(cancelbutton);

	rotationaxis_dlg->show();
}
void QtMovieSettingsWidget::movie_orthosect_event(void)
{
	int preview_state = preview->isOn();
	if(defaul_movie_dir == "No Output Directory Selected!!!" && preview_state==0)
	{
		QMessageBox::warning(this, "Warning...",
		"Please specify an Output Directory or select Preview Mode \n",
		QMessageBox::Ok | QMessageBox::Default );

		return;
	}
	emit movie_orthosectmovie_signal(this, moviesampling, preview_state, defaul_movie_dir);
}
void QtMovieSettingsWidget::movie_rotate_event(void)
{
	int preview_state = preview->isOn();
	if(defaul_movie_dir == "No Output Directory Selected!!!" && preview_state==0)
	{
		QMessageBox::warning(this, "Warning...",
		"Please specify an Output Directory or select Preview Mode \n",
		QMessageBox::Ok | QMessageBox::Default );		
		return;
	}

    rotationaxis_dlg = new QDialog(this, "Select Rotation Axis", TRUE);

	Q3BoxLayout* movie_layout = new Q3BoxLayout(rotationaxis_dlg , Q3BoxLayout::TopToBottom, 4, 4, "rotationmovie_layout");

	Q3ButtonGroup *axis_layout = new Q3ButtonGroup( 1, Qt::Vertical, "Select Axis of Rotation: ", this);

				xbutton		= new QPushButton("X",	axis_layout);
				xbutton->setToggleButton(true);
				xbutton->setOn(false);

				ybutton		= new QPushButton("Y",	axis_layout);
				ybutton->setToggleButton(true);
				ybutton->setOn(true);

				zbutton		= new QPushButton("Z",	axis_layout);
				zbutton->setToggleButton(true);
				zbutton->setOn(false);

	Q3ButtonGroup *angle_layout = new Q3ButtonGroup( 1, Qt::Vertical, "Specify Angle of Rotation: ", this);
		QString ang;
		ang.setNum(rotangle);
		angle_le = new QLineEdit(ang, angle_layout);
		angle_le->setInputMask("9000.00");

	QPushButton *okbutton		= new QPushButton("OK",	this);
	connect(okbutton, SIGNAL( clicked () ), this, SLOT( okbuttonrot_event() ) );

	QPushButton *cancelbutton		= new QPushButton("Cancel",	this);
	connect(cancelbutton, SIGNAL( clicked () ), this, SLOT( cancbuttonrot_event() ) );

	movie_layout->addWidget(angle_layout);
	movie_layout->addWidget(axis_layout);
	movie_layout->addWidget(okbutton);
	movie_layout->addWidget(cancelbutton);

	rotationaxis_dlg->show();
}
void QtMovieSettingsWidget::okbuttonrot_event(void)
{
	int preview_state = preview->isOn();
	if(defaul_movie_dir == "No Output Directory Selected!!!" && preview_state==0) return;

	int xaxis = xbutton->isOn();
	int yaxis = ybutton->isOn();
	int zaxis = zbutton->isOn();

	QString numb = angle_le->text();
	rotangle = numb.toFloat();

	rotationaxis_dlg->close();
	delete rotationaxis_dlg;

	emit movie_rotmovie_signal(this, moviesampling, preview_state, defaul_movie_dir, xaxis, yaxis, zaxis, rotangle);
}
void QtMovieSettingsWidget::cancbuttonrot_event(void)
{
	rotationaxis_dlg->close();
	delete rotationaxis_dlg;
}
void QtMovieSettingsWidget::movie_crosschanfade_event(void)
{
	int preview_state = preview->isOn();
	if(defaul_movie_dir == "No Output Directory Selected!!!" && preview_state==0)
	{
		QMessageBox::warning(this, "Warning...",
		"Please specify an Output Directory or select Preview Mode \n",
		QMessageBox::Ok | QMessageBox::Default );

		return;
	}

    rotationaxis_dlg = new QDialog(this, "Select Rotation Axis", TRUE);

	Q3BoxLayout* movie_layout = new Q3BoxLayout(rotationaxis_dlg , Q3BoxLayout::TopToBottom, 4, 4, "rotationmovie_layout");

	Q3ButtonGroup *axis_layout = new Q3ButtonGroup( 1, Qt::Vertical, "Select Axis of Rotation: ", this);

				xbutton		= new QPushButton("X",	axis_layout);
				xbutton->setToggleButton(true);
				xbutton->setOn(false);

				ybutton		= new QPushButton("Y",	axis_layout);
				ybutton->setToggleButton(true);
				ybutton->setOn(true);

				zbutton		= new QPushButton("Z",	axis_layout);
				zbutton->setToggleButton(true);
				zbutton->setOn(false);

	Q3ButtonGroup *angle_layout = new Q3ButtonGroup( 1, Qt::Vertical, "Specify Angle of Rotation: ", this);
		QString ang;
		ang.setNum(faderotangle);
		angle_le = new QLineEdit(ang, angle_layout);
		angle_le->setInputMask("9000.00");

	Q3ButtonGroup *channel_layout = new Q3ButtonGroup( 1, Qt::Vertical, "Specify Channel Cross-Fade Order: ", this);

			channelOne_combobox = new QComboBox(channel_layout, "channelOne_combobox");
			QStringList channelOne_items;
			channelOne_items += "Red";
			channelOne_items += "Green";
			channelOne_items += "Blue";
			channelOne_items += "None";
			channelOne_combobox->insertItems(0, channelOne_items);
			channelOne_combobox->setCurrentItem(0);
			connect(channelOne_combobox, SIGNAL(activated(int)), this, SLOT(channelOne_combobox_event(int)));
		
			channelTwo_combobox = new QComboBox(channel_layout, "channelTwo_combobox");
			QStringList channelTwo_items;
			channelTwo_items += "Red";
			channelTwo_items += "Green";
			channelTwo_items += "Blue";
			channelTwo_items += "None";
			channelTwo_combobox->insertItems(0, channelTwo_items);
			channelTwo_combobox->setCurrentItem(1);
			connect(channelTwo_combobox, SIGNAL(activated(int)), this, SLOT(channelTwo_combobox_event(int)));

			channelThree_combobox = new QComboBox(channel_layout, "channelThree_combobox");
			QStringList channelThree_items;
			channelThree_items += "Red";
			channelThree_items += "Green";
			channelThree_items += "Blue";
			channelThree_items += "None";
			channelThree_combobox->insertItems(0, channelThree_items);
			channelThree_combobox->setCurrentItem(2);
			connect(channelThree_combobox, SIGNAL(activated(int)), this, SLOT(channelThree_combobox_event(int)));
			
	QPushButton *okbutton		= new QPushButton("OK",	this);
	connect(okbutton, SIGNAL( clicked () ), this, SLOT( okbuttoncrossfade_event() ) );

	QPushButton *cancelbutton		= new QPushButton("Cancel",	this);
	connect(cancelbutton, SIGNAL( clicked () ), this, SLOT( cancbuttonrot_event() ) );

	movie_layout->addWidget(angle_layout);
	movie_layout->addWidget(axis_layout);
	movie_layout->addWidget(channel_layout);
	movie_layout->addWidget(okbutton);
	movie_layout->addWidget(cancelbutton);

	rotationaxis_dlg->show();
}
void QtMovieSettingsWidget::movie_renderview_event(void)
{
	int preview_state = preview->isOn();
	if(defaul_movie_dir == "No Output Directory Selected!!!" && preview_state==0)
	{
		QMessageBox::warning(this, "Warning...",
		"Please specify an Output Directory or select Preview Mode \n",
		QMessageBox::Ok | QMessageBox::Default );

		return;
	}

	emit movie_renderviewmovie_signal(this, moviesampling, preview_state, defaul_movie_dir);
}
void QtMovieSettingsWidget::okbuttonrenderview_event(void)
{

}

void QtMovieSettingsWidget::movie_chanfade_event(void)
{
	int preview_state = preview->isOn();
	if(defaul_movie_dir == "No Output Directory Selected!!!" && preview_state==0)
	{
		QMessageBox::warning(this, "Warning...",
		"Please specify an Output Directory or select Preview Mode \n",
		QMessageBox::Ok | QMessageBox::Default );

		return;
	}

    rotationaxis_dlg = new QDialog(this, "Select Rotation Axis", TRUE);

	Q3BoxLayout* movie_layout = new Q3BoxLayout(rotationaxis_dlg , Q3BoxLayout::TopToBottom, 4, 4, "rotationmovie_layout");

	Q3ButtonGroup *axis_layout = new Q3ButtonGroup( 1, Qt::Vertical, "Select Axis of Rotation: ", this);

				xbutton		= new QPushButton("X",	axis_layout);
				xbutton->setToggleButton(true);
				xbutton->setOn(false);

				ybutton		= new QPushButton("Y",	axis_layout);
				ybutton->setToggleButton(true);
				ybutton->setOn(true);

				zbutton		= new QPushButton("Z",	axis_layout);
				zbutton->setToggleButton(true);
				zbutton->setOn(false);

	Q3ButtonGroup *angle_layout = new Q3ButtonGroup( 1, Qt::Vertical, "Specify Angle of Rotation: ", this);
		QString ang;
		ang.setNum(faderotangle);
		angle_le = new QLineEdit(ang, angle_layout);
		angle_le->setInputMask("9000.00");

	Q3ButtonGroup *channel_layout = new Q3ButtonGroup( 1, Qt::Vertical, "Specify Channels to Fade: ", this);

			channelOne_combobox = new QComboBox(channel_layout, "channelOne_combobox");
			QStringList channelOne_items;
			channelOne_items += "Red";
			channelOne_items += "None";
			channelOne_combobox->insertItems(0, channelOne_items);
			channelOne_combobox->setCurrentItem(0);
			connect(channelOne_combobox, SIGNAL(activated(int)), this, SLOT(channelOne_combobox_event(int)));
		
			channelTwo_combobox = new QComboBox(channel_layout, "channelTwo_combobox");
			QStringList channelTwo_items;
			channelTwo_items += "Green";
			channelTwo_items += "None";
			channelTwo_combobox->insertItems(0, channelTwo_items);
			channelTwo_combobox->setCurrentItem(1);
			connect(channelTwo_combobox, SIGNAL(activated(int)), this, SLOT(channelTwo_combobox_event(int)));

			channelThree_combobox = new QComboBox(channel_layout, "channelThree_combobox");
			QStringList channelThree_items;
			channelThree_items += "Blue";
			channelThree_items += "None";
			channelThree_combobox->insertItems(0, channelThree_items);
			channelThree_combobox->setCurrentItem(1);
			connect(channelThree_combobox, SIGNAL(activated(int)), this, SLOT(channelThree_combobox_event(int)));
			
	QPushButton *okbutton		= new QPushButton("OK",	this);
	connect(okbutton, SIGNAL( clicked () ), this, SLOT( okbuttonfade_event() ) );

	QPushButton *cancelbutton		= new QPushButton("Cancel",	this);
	connect(cancelbutton, SIGNAL( clicked () ), this, SLOT( cancbuttonrot_event() ) );

	movie_layout->addWidget(angle_layout);
	movie_layout->addWidget(axis_layout);
	movie_layout->addWidget(channel_layout);
	movie_layout->addWidget(okbutton);
	movie_layout->addWidget(cancelbutton);

	rotationaxis_dlg->show();
}
void QtMovieSettingsWidget::okbuttonfade_event(void)
{
	int preview_state = preview->isOn();
	if(defaul_movie_dir == "No Output Directory Selected!!!" && preview_state==0)
	{
		return;
	}

	int xaxis = xbutton->isOn();
	int yaxis = ybutton->isOn();
	int zaxis = zbutton->isOn();

	QString numb = angle_le->text();
	faderotangle = numb.toFloat();

	rotationaxis_dlg->close();
	delete rotationaxis_dlg;

	emit movie_fademovie_signal(this, moviesampling, preview_state, defaul_movie_dir, xaxis, yaxis, zaxis, faderotangle, channelOne_selected, channelTwo_selected, channelThree_selected);
}

void QtMovieSettingsWidget::okbuttoncrossfade_event(void)
{
	printf("BOH!\n");

	int preview_state = preview->isOn();
	if(defaul_movie_dir == "No Output Directory Selected!!!" && preview_state==0)
	{
		return;
	}

	int xaxis = xbutton->isOn();
	int yaxis = ybutton->isOn();
	int zaxis = zbutton->isOn();

	QString numb = angle_le->text();
	faderotangle = numb.toFloat();

	rotationaxis_dlg->close();
	delete rotationaxis_dlg;

	emit movie_crossfademovie_signal(this, moviesampling, preview_state, defaul_movie_dir, xaxis, yaxis, zaxis, faderotangle, channelOne_selected, channelTwo_selected, channelThree_selected);
}
void QtMovieSettingsWidget::okbuttonrock_event(void)
{
	int preview_state = preview->isOn();
	if(defaul_movie_dir == "No Output Directory Selected!!!" && preview_state==0)
	{
		return;
	}

	QString numb = angle_le->text();
	rockangle = numb.toFloat();

	rotationaxis_dlg->close();
	delete rotationaxis_dlg;

	emit movie_rockmovie_signal(this, moviesampling, preview_state, defaul_movie_dir, rockangle);
}
void QtMovieSettingsWidget::channelOne_combobox_event(int id)
{
	channelOne_selected = id;
}
void QtMovieSettingsWidget::channelTwo_combobox_event(int id)
{
	channelTwo_selected = id;
}
void QtMovieSettingsWidget::channelThree_combobox_event(int id)
{
	channelThree_selected = id;
}
void QtMovieSettingsWidget::movie_clip_event(void)
{
	int preview_state = preview->isOn();
	if(defaul_movie_dir == "No Output Directory Selected!!!" && preview_state==0)
	{
		QMessageBox::warning(this, "Warning...",
		"Please specify an Output Directory or select Preview Mode \n",
		QMessageBox::Ok | QMessageBox::Default );
		return;
	}

    rotationaxis_dlg = new QDialog(this, "Select Rotation Axis", TRUE);

	Q3BoxLayout* movie_layout = new Q3BoxLayout(rotationaxis_dlg , Q3BoxLayout::TopToBottom, 4, 4, "rotationmovie_layout");

	Q3ButtonGroup *axis_layout = new Q3ButtonGroup( 1, Qt::Vertical, "Select Axis of Rotation: ", this);

				xbutton		= new QPushButton("X",	axis_layout);
				xbutton->setToggleButton(true);
				xbutton->setOn(false);

				ybutton		= new QPushButton("Y",	axis_layout);
				ybutton->setToggleButton(true);
				ybutton->setOn(true);

				zbutton		= new QPushButton("Z",	axis_layout);
				zbutton->setToggleButton(true);
				zbutton->setOn(false);

	Q3ButtonGroup *angle_layout = new Q3ButtonGroup( 1, Qt::Vertical, "Specify Angle of Rotation: ", this);
		QString ang;
		ang.setNum(cliprotang);
		angle_le = new QLineEdit(ang, angle_layout);
		angle_le->setInputMask("9000.00");

		clipafterrotationbutton = new QPushButton("Clip after Rotation",	angle_layout);
		clipafterrotationbutton->setToggleButton(true);
		clipafterrotationbutton->setOn(false);

	Q3ButtonGroup *clip_layout = new Q3ButtonGroup( 1, Qt::Vertical, "Specify a Clipping Axis: ", this);
	clip_layout->setExclusive(true);

				clipvbutton		= new QPushButton("View-Aligned",	clip_layout);
				clipvbutton->setToggleButton(true);
				clipvbutton->setOn(false);

				clipxbutton		= new QPushButton("X",	clip_layout);
				clipxbutton->setToggleButton(true);
				clipxbutton->setOn(false);

				clipybutton		= new QPushButton("Y",	clip_layout);
				clipybutton->setToggleButton(true);
				clipybutton->setOn(true);

				clipzbutton		= new QPushButton("Z",	clip_layout);
				clipzbutton->setToggleButton(true);
				clipzbutton->setOn(false);


	QPushButton *okbutton		= new QPushButton("OK",	this);
	connect(okbutton, SIGNAL( clicked () ), this, SLOT( okbuttonclip_event() ) );

	QPushButton *cancelbutton		= new QPushButton("Cancel",	this);
	connect(cancelbutton, SIGNAL( clicked () ), this, SLOT( cancbuttonrot_event() ) );

	movie_layout->addWidget(angle_layout);
	movie_layout->addWidget(axis_layout);
	movie_layout->addWidget(clip_layout);
	movie_layout->addWidget(okbutton);
	movie_layout->addWidget(cancelbutton);

	rotationaxis_dlg->show();
}
void QtMovieSettingsWidget::okbuttonclip_event(void)
{
	int preview_state = preview->isOn();
	if(defaul_movie_dir == "No Output Directory Selected!!!" && preview_state==0)
	{
		QMessageBox::warning(this, "Warning...",
		"Please specify an Output Directory or select Preview Mode \n",
		QMessageBox::Ok | QMessageBox::Default );		
		return;
	}

	int xaxis = xbutton->isOn();
	int yaxis = ybutton->isOn();
	int zaxis = zbutton->isOn();

	int clipmode;

	if(clipvbutton->isOn()) clipmode=0;
	if(clipxbutton->isOn()) clipmode=1;
	if(clipybutton->isOn()) clipmode=2;
	if(clipzbutton->isOn()) clipmode=3;

	int clipafterrot=0;

	if(clipafterrotationbutton->isOn()) clipafterrot = 1;

	QString numb = angle_le->text();
	cliprotang = numb.toFloat();

	rotationaxis_dlg->close();
	delete rotationaxis_dlg;

	emit movie_clipmovie_signal(this, moviesampling, preview_state, defaul_movie_dir, xaxis, yaxis, zaxis, cliprotang, clipmode, clipafterrot);
}
	