/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "QtRenderSettingsWidget.h"
//Added by qt3to4:
#include <QPixmap>
#include <QLabel>


QtRenderSettingsWidget::QtRenderSettingsWidget(QWidget* parent, const char* name)
: QWidget( parent, name )
{
	numbslices = 0;
	numbsliceslod=0;

	fbosize = 0;
	min_fbosize = 0;
	
	postprocess = 100;
	silhouettes = 50;

	maxtexturesize = 0;

	r_toggle = 1.0;
	g_toggle = 1.0;
	b_toggle = 1.0;
	i_toggle = 1.0;

	renderquality = 10;
	renderquality_lod = 10;

}
QtRenderSettingsWidget::~QtRenderSettingsWidget()
{
}
void QtRenderSettingsWidget::reset(void)
{
}
void QtRenderSettingsWidget::create(void)
{

	//LOAD/SAVE
	Q3ButtonGroup *loadsave_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "File:", this );

		load = new QPushButton(QIcon(QPixmap("icons/fileopen.xpm")), "Open", loadsave_grp, "open");
		connect( load, SIGNAL( clicked () ), this, SLOT( load_event() ) );

		save = new QPushButton(QIcon(QPixmap("icons/filesave.xpm")), "Save", loadsave_grp, "save");
		connect( save, SIGNAL( clicked () ), this, SLOT( save_event() ) );

	//CHANNEL TOGGLE
	toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
	connect( toggle_grp, SIGNAL( clicked(int) ), this, SLOT( channel_button_event(int) ) );

		red = new QPushButton("Red", toggle_grp);
		red->setToggleButton(TRUE);
		red->setOn(TRUE);

		green = new QPushButton("Green", toggle_grp);
		green->setToggleButton(TRUE);
		green->setOn(TRUE);

		blue = new QPushButton("Blue", toggle_grp);
		blue->setToggleButton(TRUE);
		blue->setOn(TRUE);

/*		iso = new QPushButton("Iso-Surface", toggle_grp);
		iso->setToggleButton(TRUE);
		iso->setOn(TRUE);
*/		
		
		//renderquality size	
		Q3ButtonGroup* renderquality_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
		renderquality_lb = new QLabel( "Render Quality: 0", renderquality_grp );
		renderquality_slider = new QSlider ( 1, 10, 1, 0, Qt::Horizontal, renderquality_grp, "renderquality_slider" );
		renderquality_slider->setTickmarks( QSlider::Below );
		renderquality_slider->setTracking(TRUE);
		renderquality_slider->setTickInterval(1);
		connect(renderquality_slider, SIGNAL(valueChanged(int)), this, SLOT(renderquality_event(int)) );
//		renderquality_event(renderquality);

		//our renderquality_lod slider
		Q3ButtonGroup* renderquality_lod_maingrp = new Q3ButtonGroup( 1, Qt::Vertical, "", this );

			renderquality_lod_pb = new QPushButton("ON\nOFF", renderquality_lod_maingrp);
			renderquality_lod_pb->setToggleButton(TRUE);
			renderquality_lod_pb->setOn(TRUE);
			renderquality_lod_pb->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
			connect( renderquality_lod_pb, SIGNAL( clicked () ), this, SLOT( renderquality_lodtoggle_event() ) );

			//renderquality_lod size	
			renderquality_lod_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", renderquality_lod_maingrp );
			renderquality_lod_lb = new QLabel( "Render Quality (On Move): 0", renderquality_lod_grp );
			renderquality_lod_slider = new QSlider ( 1, 10, 1, 0, Qt::Horizontal, renderquality_lod_grp, "renderquality_lod_slider" );
			renderquality_lod_slider->setTickmarks( QSlider::Below );
			renderquality_lod_slider->setTracking(FALSE);
			renderquality_lod_slider->setTickInterval(1);
			connect(renderquality_lod_slider, SIGNAL(valueChanged(int)), this, SLOT(renderqualitylod_event(int)) );
//		renderqualitylod_event(renderquality_lod);

#ifdef OPENCL_SUPPORT
		//max texture size	
		Q3ButtonGroup* openclplat_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
		QLabel* openclplat_lb = new QLabel( "OpenCL Platform: ", openclplat_grp );

		openclplat_combobox = new QComboBox(openclplat_grp, "openclplat_combobox");
		QStringList openclplat_items;
		openclplat_items += "NONE";

		openclplat_combobox->insertItems(0, openclplat_items);
		openclplat_combobox->setCurrentItem(0);
		connect(openclplat_combobox, SIGNAL(activated(int)), this, SLOT(openclplat_event(int)));
#endif
		//max texture size	
		Q3ButtonGroup* maxtexturesize_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
		maxtexturesize_lb = new QLabel( "Max Texture Size: ", maxtexturesize_grp );
		/*maxtexturesize_slider = new QSlider ( 1, 1024, 32, 256, QSlider::Horizontal, maxtexturesize_grp, "maxtexturesize_slider" );
		maxtexturesize_slider->setTickmarks( QSlider::Below );
		maxtexturesize_slider->setTracking(FALSE);
		maxtexturesize_slider->setTickInterval(32);
		connect(maxtexturesize_slider, SIGNAL(valueChanged(int)), this, SLOT(maxtexturesize_event(int)) );
		maxtexturesize_event(maxtexturesize);*/

		maxtexturesize_combobox = new QComboBox(maxtexturesize_grp, "maxtexturesize_combobox");
		QStringList maxtexturesize_items;
		maxtexturesize_items += "8";
		maxtexturesize_items += "16";
		maxtexturesize_items += "32";
		maxtexturesize_items += "64";
		maxtexturesize_items += "128";
		maxtexturesize_items += "256";
		maxtexturesize_items += "512";
		maxtexturesize_items += "1024";
		maxtexturesize_items += "2048";

		maxtexturesize_combobox->insertItems(0, maxtexturesize_items);
		maxtexturesize_combobox->setCurrentItem(5);
		connect(maxtexturesize_combobox, SIGNAL(activated(int)), this, SLOT(maxtexturesize_event(int)));

		//our #slices slider
		Q3ButtonGroup* numbslices_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
		numbslices_lb = new QLabel( "Number of Slices: 0", numbslices_grp );
		numbslices_slider = new QSlider ( 1, 1024, 32, 256, Qt::Horizontal, numbslices_grp, "numbslices_slider" );
		numbslices_slider->setTickmarks( QSlider::Below );
		numbslices_slider->setTracking(FALSE);
		numbslices_slider->setTickInterval(32);
		connect(numbslices_slider, SIGNAL(valueChanged(int)), this, SLOT(numbslices_event(int)) );
		numbslices_event(numbslices);

		//our fbosize slider
		Q3ButtonGroup* fbosize_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
		fbosize_lb = new QLabel( "Frame-Buffer: 0", fbosize_grp );
		fbosize_slider = new QSlider ( 1, 1024, 32, 512, Qt::Horizontal, fbosize_grp, "fbosize_slider" );
		fbosize_slider->setTickmarks( QSlider::Below );
		fbosize_slider->setTracking(FALSE);
		fbosize_slider->setTickInterval(32);
		connect(fbosize_slider, SIGNAL(valueChanged(int)), this, SLOT(fbosize_event(int)) );
		fbosize_event(fbosize);
		
		//our postprocess slider
		Q3ButtonGroup* postprocess_maingrp = new Q3ButtonGroup( 1, Qt::Vertical, "", this );

			postprocess_pb = new QPushButton("ON\nOFF", postprocess_maingrp);
			postprocess_pb->setToggleButton(TRUE);
			postprocess_pb->setOn(TRUE);
			postprocess_pb->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
			connect( postprocess_pb, SIGNAL( clicked () ), this, SLOT( postprocesstoggle_event() ) );

			postprocess_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", postprocess_maingrp );
			postprocess_lb = new QLabel( "Post-Processing: 0", postprocess_grp );
			postprocess_slider = new QSlider ( 0, 200, 5, 50, Qt::Horizontal, postprocess_grp, "postprocess_slider" );
			postprocess_slider->setTickmarks( QSlider::Below );
			postprocess_slider->setTracking(FALSE);
			postprocess_slider->setTickInterval(10);
			connect(postprocess_slider, SIGNAL(valueChanged(int)), this, SLOT(postprocess_event(int)) );
			postprocess_event(postprocess);

	/*	QButtonGroup* silhouettes_grp = new QButtonGroup( 1, QGroupBox::Horizontal, "", this );
		silhouettes_lb = new QLabel( "Silhouettes: 0", silhouettes_grp );
		silhouettes_slider = new QSlider ( 0, 200, 1, 50, QSlider::Horizontal, silhouettes_grp, "silhouettes_slider" );
		silhouettes_slider->setTickmarks( QSlider::Below );
		silhouettes_slider->setTracking(FALSE);
		silhouettes_slider->setTickInterval(10);
		connect(silhouettes_slider, SIGNAL(valueChanged(int)), this, SLOT(silhouettes_event(int)) );
		silhouettes_event(silhouettes);
*/

		//our LOD #slices slider
		Q3ButtonGroup* numbsliceslod_maingrp = new Q3ButtonGroup( 1, Qt::Vertical, "", this );

			numbsliceslod_pb = new QPushButton("Sub-Slice \n ON/OFF", numbsliceslod_maingrp);
			numbsliceslod_pb->setToggleButton(TRUE);
			numbsliceslod_pb->setOn(TRUE);
			numbsliceslod_pb->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
			connect( numbsliceslod_pb, SIGNAL( clicked () ), this, SLOT( numbsliceslodtoggle_event() ) );
	
			Q3ButtonGroup* numbsliceslod_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", numbsliceslod_maingrp );
			numbsliceslod_lb = new QLabel( "LOD Number of Slices: 0", numbsliceslod_grp );
			numbsliceslod_slider = new QSlider ( 1, 1024, 32, 256, Qt::Horizontal, numbsliceslod_grp, "numbsliceslod_slider" );
			numbsliceslod_slider->setTickmarks( QSlider::Below );
			numbsliceslod_slider->setTracking(FALSE);
			numbsliceslod_slider->setTickInterval(32);
			connect(numbsliceslod_slider, SIGNAL(valueChanged(int)), this, SLOT(numbsliceslod_event(int)) );
			numbsliceslod_event(numbsliceslod);
	
		//our min_fbosize slider
		Q3ButtonGroup* min_fbosize_maingrp = new Q3ButtonGroup( 1, Qt::Vertical, "", this );

			min_fbosize_pb = new QPushButton("Sub-Sample \n ON/OFF", min_fbosize_maingrp);
			min_fbosize_pb->setToggleButton(TRUE);
			min_fbosize_pb->setOn(TRUE);
			min_fbosize_pb->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
			connect( min_fbosize_pb, SIGNAL( clicked () ), this, SLOT( framebuffertoggle_event() ) );

			Q3ButtonGroup* min_fbosize_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", min_fbosize_maingrp );
			min_fbosize_lb = new QLabel( "LOD Frame-Buffer: 0", min_fbosize_grp );
			min_fbosize_slider = new QSlider ( 1, 1024, 32, 256, Qt::Horizontal, min_fbosize_grp, "min_fbosize_slider" );
			min_fbosize_slider->setLineStep(32);
			min_fbosize_slider->setTickmarks( QSlider::Below );
			min_fbosize_slider->setTracking(FALSE);
			min_fbosize_slider->setTickInterval(32);
			connect(min_fbosize_slider, SIGNAL(valueChanged(int)), this, SLOT(min_fbosize_event(int)) );
			min_fbosize_event(min_fbosize);

			// ( int minValue, int maxValue, int LineStep, int PageStep, int value, Orientation, QWidget * parent, const char * name=0 ) 
			//min_fbosize_slider2 = new QScrollBar ( 1, 1024, 32, 32, 256, QSlider::Horizontal, min_fbosize_grp, "min_fbosize_slider" );



	Q3BoxLayout *vboxlayoutRIGHT = new Q3BoxLayout ( 0, Q3BoxLayout::TopToBottom, 0, 5, "vboxlayout");
	vboxlayoutRIGHT->addWidget(toggle_grp, 0, Qt::AlignTop);

	vboxlayoutRIGHT->addWidget(maxtexturesize_grp, 0, Qt::AlignTop);

#ifdef OPENCL_SUPPORT
	vboxlayoutRIGHT->addWidget(openclplat_grp, 0, Qt::AlignTop);
#endif

	vboxlayoutRIGHT->addWidget(renderquality_grp, 0, Qt::AlignTop);
	vboxlayoutRIGHT->addWidget(renderquality_lod_maingrp, 0, Qt::AlignTop);

	vboxlayoutRIGHT->addWidget(numbslices_grp, 0, Qt::AlignTop);
	vboxlayoutRIGHT->addWidget(fbosize_grp, 0, Qt::AlignTop);

	vboxlayoutRIGHT->addWidget(numbsliceslod_maingrp, 0, Qt::AlignTop);
	vboxlayoutRIGHT->addWidget(min_fbosize_maingrp, 0, Qt::AlignTop);

//	vboxlayoutRIGHT->addWidget(silhouettes_grp, 0, Qt::AlignTop);
	
	vboxlayoutRIGHT->addWidget(postprocess_maingrp, 0, Qt::AlignTop);
	
	Q3BoxLayout *vboxlayoutLEFT = new Q3BoxLayout ( 0, Q3BoxLayout::TopToBottom, 0, 5, "vboxlayout");
	vboxlayoutLEFT->setAlignment(Qt::AlignTop);
	vboxlayoutLEFT->addWidget(loadsave_grp, 0, Qt::AlignTop);

	Q3BoxLayout *topmost= new Q3BoxLayout ( this, Q3BoxLayout::LeftToRight, 10, 5, "topmost");
	topmost->addLayout(vboxlayoutLEFT,1);
	topmost->addLayout(vboxlayoutRIGHT, 10);

	loadsave_grp->hide();
	toggle_grp->hide();
	numbslices_grp->hide();
	fbosize_grp->hide();
	min_fbosize_maingrp->hide();
	numbsliceslod_maingrp->hide();
}
void QtRenderSettingsWidget::apply(void)
{

}
void QtRenderSettingsWidget::reload(void)
{
//	maxtexturesize_event(maxtexturesize);
//	renderquality_event(renderquality);
//	renderqualitylod_event(renderquality_lod);
}

void QtRenderSettingsWidget::rebuild(void)
{
	maxtexturesize_event(maxtexturesize);
	postprocess_event(postprocess);

	renderquality_event(renderquality);
	renderqualitylod_event(renderquality_lod);

	/*fbosize_event(fbosize);
	min_fbosize_event(min_fbosize);

	numbslices_event(numbslices);
	numbsliceslod_event(numbsliceslod);*/
}
void QtRenderSettingsWidget::load_event(void)
{

}
void QtRenderSettingsWidget::save_event(void)
{

}

void QtRenderSettingsWidget::numbslices_event(int j)
{
	int temp = j;

	temp = temp%32;
	temp = j-temp;
	j = temp;

	numbslices_slider->setValue(j);
	numbslices=j;

	//update our slider label
	QString s;
	QString text;
	text = "Number of Slices: ";
	s.setNum(j);
	text += s;
	numbslices_lb->setText (text);

	emit rendersettings_numbslices_signal(j);
}
void QtRenderSettingsWidget::renderquality_lodtoggle_event(void)
{
	if(renderquality_lod_pb->isOn()) 
	{
		emit rendersettings_numbsliceslodtoggle_signal(1);
		emit rendersettings_framebuffertoggle_signal(1);
		renderquality_lod_grp->setEnabled(true);
	}
	else 
	{
		emit rendersettings_numbsliceslodtoggle_signal(0);
		emit rendersettings_framebuffertoggle_signal(0);
		renderquality_lod_grp->setEnabled(false);
	}
}
void QtRenderSettingsWidget::renderqualitylod_event(int j)
{
	renderquality_lod_slider->setValue(j);
	renderquality_lod=j;

	//update our slider label
	QString s;
	QString text;
	text = "Render Quality (On Move): ";
	s.setNum(j);
	text += s;
	renderquality_lod_lb->setText (text);

	float weight = (float)j/10.0f;
	weight -= 0.5;
	weight = 1.0+(2.0*weight);

	emit rendersettings_renderqualitylod_signal(weight);
}

void QtRenderSettingsWidget::renderquality_event(int j)
{
	renderquality_slider->setValue(j);
	renderquality=j;

	//update our slider label
	QString s;
	QString text;
	text = "Render Quality: ";
	s.setNum(j);
	text += s;
	renderquality_lb->setText (text);

	float weight = (float)j/10.0f;
	weight -= 0.5;
	weight = 1.0+(2.0*weight);

	emit rendersettings_renderquality_signal(weight);
}
#ifdef OPENCL_SUPPORT
void QtRenderSettingsWidget::openclplat_event(int j)
{
	emit rendersettings_openclplat_signal(j);
}
void QtRenderSettingsWidget::rendersettings_setopenclplat_slot(int index, vector<string> plats)
{
	printf("rendersettings_setopenclplat_slot\n");
	openclplat_combobox->clear();
	QStringList openclplat_items;

	for(int i=0; i<plats.size(); i++)
		openclplat_items += plats[i].data();

	openclplat_combobox->insertItems(0, openclplat_items);

	if(index<openclplat_items.size()) openclplat_combobox->setCurrentItem(index);
	else openclplat_combobox->setCurrentItem(0);
}
#endif
void QtRenderSettingsWidget::maxtexturesize_event(int j)
{
	if(j==0)
	{
		maxtexturesize = 8;
		emit rendersettings_maxtexturesize_signal(8);
	}
	else if(j==1)
	{
		maxtexturesize = 16;
		emit rendersettings_maxtexturesize_signal(16);
	}
	else if(j==2)
	{
		maxtexturesize = 32;
		emit rendersettings_maxtexturesize_signal(32);
	}
	else if(j==3)
	{
		maxtexturesize = 64;
		emit rendersettings_maxtexturesize_signal(64);
	}
	else if(j==4)
	{
		maxtexturesize = 128;
		emit rendersettings_maxtexturesize_signal(128);
	}
	else if(j==5)
	{
		maxtexturesize = 256;
		emit rendersettings_maxtexturesize_signal(256);
	}
	else if(j==6)
	{
		maxtexturesize = 512;
		emit rendersettings_maxtexturesize_signal(512);
	}
	else if(j==7)
	{
		maxtexturesize = 1024;
		emit rendersettings_maxtexturesize_signal(1024);
	}
	else if(j==8)
	{
		maxtexturesize = 2048;
		emit rendersettings_maxtexturesize_signal(2048);
	}
	else
	{
		maxtexturesize=j;
		if(maxtexturesize==8)
		{
			maxtexturesize_combobox->setCurrentItem(0);
			emit rendersettings_maxtexturesize_signal(8);
		}
		else if(maxtexturesize==16)
		{
			maxtexturesize_combobox->setCurrentItem(1);
			emit rendersettings_maxtexturesize_signal(16);
		}
		else if(maxtexturesize==32)
		{
			maxtexturesize_combobox->setCurrentItem(2);
			emit rendersettings_maxtexturesize_signal(32);
		}
		else if(maxtexturesize==64)
		{
			maxtexturesize_combobox->setCurrentItem(3);
			emit rendersettings_maxtexturesize_signal(64);
		}
		else if(maxtexturesize==128)
		{
			maxtexturesize_combobox->setCurrentItem(4);
			emit rendersettings_maxtexturesize_signal(128);
		}
		else if(maxtexturesize==256)
		{
			maxtexturesize_combobox->setCurrentItem(5);
			emit rendersettings_maxtexturesize_signal(256);
		}
		else if(maxtexturesize==512)
		{
			maxtexturesize_combobox->setCurrentItem(6);
			emit rendersettings_maxtexturesize_signal(512);
		}
		else if(maxtexturesize==1024)
		{
			maxtexturesize_combobox->setCurrentItem(7);
			emit rendersettings_maxtexturesize_signal(1024);
		}
		else if(maxtexturesize==2048)
		{
			maxtexturesize_combobox->setCurrentItem(8);
			emit rendersettings_maxtexturesize_signal(2048);
		}
	}

//	printf("j: %d\n", j);
//	printf("maxtexturesize: %d\n", maxtexturesize);

/*	int temp = j;

	temp = temp%32;
	temp = j-temp;
	j = temp;

	maxtexturesize_slider->setValue(j);
	maxtexturesize=j;

	//update our slider label
	QString s;
	QString text;
	text = "Max Texture Size: ";
	s.setNum(j);
	text += s;
	maxtexturesize_lb->setText(text);*/

	
}
void QtRenderSettingsWidget::numbsliceslod_event(int j)
{
	int temp = j;

	temp = temp%32;
	temp = j-temp;
	j = temp;

	numbsliceslod_slider->setValue(j);
	numbsliceslod=j;

	//update our slider label
	QString s;
	QString text;
	text = "LOD Number of Slices: ";
	s.setNum(j);
	text += s;
	numbsliceslod_lb->setText (text);

	emit rendersettings_numbsliceslod_signal(j);
}
void QtRenderSettingsWidget::postprocess_event(int j)
{
	int temp = j;

	temp = temp%5;
	temp = j-temp;
	j = temp;

	postprocess_slider->setValue(j);
	postprocess=j;

	//update our slider label
	QString s;
	QString text;
	text = "Post-Process: ";
	s.setNum(j);
	text += s;
	postprocess_lb->setText (text);
	
	emit rendersettings_postprocess_signal(j);
}
void QtRenderSettingsWidget::silhouettes_event(int j)
{
	silhouettes_slider->setValue(j);
	silhouettes=j;

	//update our slider label
	QString s;
	QString text;
	text = "Silhouettes: ";
	s.setNum(j);
	text += s;
	silhouettes_lb->setText (text);
	
	emit rendersettings_silhouettes_signal(j);
}
void QtRenderSettingsWidget::fbosize_event(int j)
{
	int temp = j;

	temp = temp%32;
	temp = j-temp;
	j = temp;

	fbosize_slider->setValue(j);
	fbosize=j;

	//update our slider label
	QString s;
	QString text;
	text = "Frame-Buffer: ";
	s.setNum(j);
	text += s;
	fbosize_lb->setText (text);
	
	emit rendersettings_framebuffer_signal(j);
}
void QtRenderSettingsWidget::min_fbosize_event(int j)
{
	int temp = j;

	temp = temp%32;
	temp = j-temp;
	j = temp;

	min_fbosize_slider->setValue(j);
	min_fbosize=j;

	//update our slider label
	QString s;
	QString text;
	text = "LOD Frame-Buffer: ";
	s.setNum(j);
	text += s;
	min_fbosize_lb->setText (text);
	
	emit rendersettings_framebuffer_lod_signal(j);
}

void QtRenderSettingsWidget::channel_button_event(int id)
{
	int channel_selected = id;

	if(channel_selected==0)
	{
		if(red->isOn()) r_toggle=1.0;
		else r_toggle=0.0;

	//	toggle_grp->setExclusive(TRUE);
	//	red->setOn(TRUE);
	//	green->setOn(FALSE);
	//	blue->setOn(FALSE);

	}
	else if(channel_selected==1)
	{
		if(green->isOn()) g_toggle=1.0;
		else g_toggle=0.0;

	//	toggle_grp->setExclusive(TRUE);
	//	red->setOn(TRUE);
	//	green->setOn(FALSE);
	//	blue->setOn(FALSE);
	}
	else if(channel_selected==2)
	{
		if(blue->isOn()) b_toggle=1.0;
		else b_toggle=0.0;
		toggle_grp->setExclusive(FALSE);
	}
	else if(channel_selected==3)
	{
		if(iso->isOn()) i_toggle=1.0;
		else i_toggle=0.0;
		toggle_grp->setExclusive(FALSE);
	}

	emit rendersettings_channelmode_signal(r_toggle,g_toggle,b_toggle,i_toggle);
}

void QtRenderSettingsWidget::numbsliceslodtoggle_event(void)
{
	if(numbsliceslod_pb->isOn()) emit rendersettings_numbsliceslodtoggle_signal(1);
	else emit rendersettings_numbsliceslodtoggle_signal(0);
}
void QtRenderSettingsWidget::framebuffertoggle_event(void)
{
	if(min_fbosize_pb->isOn()) emit rendersettings_framebuffertoggle_signal(1);
	else emit rendersettings_framebuffertoggle_signal(0);

}
void QtRenderSettingsWidget::postprocesstoggle_event(void)
{
	if(postprocess_pb->isOn())
	{
		emit rendersettings_postprocesstoggle_signal(1);
		postprocess_grp->setEnabled(true);
	}
	else 
	{
		emit rendersettings_postprocesstoggle_signal(0);
		postprocess_grp->setEnabled(false);
	}

}

