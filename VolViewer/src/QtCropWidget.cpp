/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "QtCropWidget.h"
//Added by qt3to4:
#include <QPixmap>


QtCropWidget::QtCropWidget(QWidget* parent, const char* name, QString app_path)
: QWidget( parent, name )
{
	appPath = app_path;
	currentworkingdir="";
	setAcceptDrops(true);

	channels.resize(3);
	channels[0] = channels[1] = channels[2] = true;
}
QtCropWidget::~QtCropWidget()
{
}

void QtCropWidget::rebuild(void)
{
}
void QtCropWidget::reset(void)
{
	lloopmode_button->setOn(false);
	invcrop->setOn(false);

	emit crop_reset_signal();
}
void QtCropWidget::dragndrop_slot(QString sFileName)
{	
}
void QtCropWidget::dragEnterEvent(QDragEnterEvent* event)
{
	event->accept(Q3TextDrag::canDecode(event));
}
void QtCropWidget::dropEvent(QDropEvent* event)
{
	Q3StrList URIs;

	if ( Q3UriDrag::decode(event, URIs) ) 
	{
		QString text = Q3UriDrag::uriToLocalFile(URIs.getFirst());
		dragndrop_slot(text);
	}
}
void QtCropWidget::create(void)
{
	Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
	connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
	toggle_grp->setExclusive(false);

		red = new QPushButton("R", toggle_grp);
		red->setToggleButton(TRUE);
		red->setOn(true);
		red->setPaletteForegroundColor(Qt::red); 
		channels[0] = true;
		//connect( red, SIGNAL( pressed() ), this, SLOT( red_button_event() ) );

		green = new QPushButton("G", toggle_grp);
		green->setToggleButton(true);
		green->setOn(true);
		green->setPaletteForegroundColor(QColor(0,128,0)); 
		channels[1] = true;
		//connect( green, SIGNAL( pressed() ), this, SLOT( green_button_event() ) );

		blue = new QPushButton("B", toggle_grp);
		blue->setToggleButton(true);
		blue->setOn(true);
		blue->setPaletteForegroundColor(Qt::blue); 
		channels[2] = true;
		//connect( blue, SIGNAL( pressed() ), this, SLOT( blue_button_event() ) );

	Q3ButtonGroup *act_grp = new Q3ButtonGroup( 2, Qt::Horizontal, "", this );
	act_grp->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
	act_grp->setLineWidth(0);
	act_grp->setMidLineWidth(0);
	act_grp->setMargin(0);
	act_grp->setInsideMargin(5);
	act_grp->setInsideSpacing(5);

		Q3ButtonGroup *crop_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Start/Stop Crop Shape:", act_grp );
		
			QPixmap lloopmodeIcon = QPixmap( appPath+"/Icons/crop0.png" );
			lloopmode_button		= new QPushButton(lloopmodeIcon, "",	crop_grp);
			lloopmode_button->setToolTip("Line Loop");
			lloopmode_button->setToggleButton(true);
			lloopmode_button->setOn(false);
			lloopmode_button->setFixedSize(66, 66);
			lloopmode_button->setIconSize(QSize(64,64));
			connect(lloopmode_button, SIGNAL( clicked () ), this, SLOT( cropmode_event() ) );

		Q3ButtonGroup *clipoperators_grp = new Q3ButtonGroup( 2, Qt::Vertical, "Actions:", act_grp );
			invcrop = new QPushButton("Invert Crop", clipoperators_grp);
			invcrop->setToggleButton(true);
			invcrop->setOn(false);
			emit crop_invsel_signal(0);
			connect( invcrop, SIGNAL( pressed() ), this, SLOT(invcrop_button_event() ) );
			recentre = new QPushButton("Recentre Data", clipoperators_grp);
			connect( recentre, SIGNAL( pressed() ), this, SLOT(recentre_button_event() ) );

	Q3BoxLayout *vboxlayoutRIGHT = new Q3BoxLayout ( 0, Q3BoxLayout::TopToBottom, 0, 5, "vboxlayout");
	vboxlayoutRIGHT->addWidget(toggle_grp, 0, Qt::AlignTop);
	vboxlayoutRIGHT->addWidget(act_grp, 0, Qt::AlignTop);

	Q3BoxLayout *topmost= new Q3BoxLayout ( this, Q3BoxLayout::LeftToRight, 10, 5, "topmost");
	topmost->addLayout(vboxlayoutRIGHT, 10);
}
void QtCropWidget::invcrop_button_event(void)
{
	if(invcrop->isOn())
	{
		QPixmap lloopmodeIcon = QPixmap( appPath+"/Icons/crop0.png" );
		lloopmode_button->setIcon(lloopmodeIcon);
		emit crop_invsel_signal(1);
	}
	else
	{
		QPixmap lloopmodeIcon = QPixmap( appPath+"/Icons/crop1.png" );
		lloopmode_button->setIcon(lloopmodeIcon);
		emit crop_invsel_signal(0);
	}
}
void QtCropWidget::recentre_button_event(void)
{
	emit crop_recentre_signal();
}
void QtCropWidget::cropmode_event(void)
{
	if(lloopmode_button->isOn())
	{
		//printf("on\n");
		emit crop_start_signal();
	}
	else
	{
		//printf("off\n");
		emit crop_end_signal(channels);
	}
}
void QtCropWidget::exit(void)
{
}
void QtCropWidget::reset_button_event(void)
{
}
void QtCropWidget::channel_button_event(int selected)
{
	if(red->isOn())
	{
		channels[0] = true;
	}
	else
	{
		channels[0] = false;
	}

	if(green->isOn())
	{
		channels[1] = true;
	}
	else
	{
		channels[1] = false;
	}

	if(blue->isOn())
	{
		channels[2] = true;
	}
	else
	{
		channels[2] = false;
	}

}
void QtCropWidget::load_event(void)
{
}
void QtCropWidget::save_event(void)
{
}