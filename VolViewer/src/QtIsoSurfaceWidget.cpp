/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "QtIsoSurfaceWidget.h"
//Added by qt3to4:
#include <Q3Frame>
#include <QPixmap>
#include <QLabel>


QtIsoSurfaceWidget::QtIsoSurfaceWidget(QWidget* parent, const char* name)
: QWidget( parent, name )
{
	isoqual=0;
	isoval=0;
	isosmooth=0;
	lb_index=0;
	nv = nf = 0;
	selected=-1;
	currentworkingdir="";
	channels[0]=true;
	channels[1]=true;
	channels[2]=true;
	setAcceptDrops(true);
}
QtIsoSurfaceWidget::~QtIsoSurfaceWidget()
{
}
void QtIsoSurfaceWidget::dragndrop_slot(QString sFileName)
{	
	if (!sFileName.isEmpty()) 
	{
		if(sFileName.contains(".obj", true) || sFileName.contains(".OBJ", true)) 
		{
			emit isosurface_load_signal(sFileName);
		}
	}
}
void QtIsoSurfaceWidget::dragEnterEvent(QDragEnterEvent* event)
{
	event->accept(Q3TextDrag::canDecode(event));
}
void QtIsoSurfaceWidget::dropEvent(QDropEvent* event)
{
	Q3StrList URIs;

	if ( Q3UriDrag::decode(event, URIs) ) 
	{
		QString text = Q3UriDrag::uriToLocalFile(URIs.getFirst());
		dragndrop_slot(text);
	}
}
void QtIsoSurfaceWidget::rebuild(void)
{
	isoqual_event(isoqual);
	isoval_event(isoval);
	isosmooth_event(isosmooth);
}
void QtIsoSurfaceWidget::reset(void)
{
/*	isoqual=100;
    isoqual_lb->setText( "IsoSurface Quality:	100%");
	isoqual_slider->setValue(isoqual);

	isoval=0;
    isoval_lb->setText( "IsoSurface Value:	0");
	isoval_slider->setValue(isoval);*/
}
void QtIsoSurfaceWidget::create(void)
{
	Q3ButtonGroup* opensave_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "File:", this );
    opensave_grp->setExclusive( FALSE );
	opensave_grp->setAlignment(Qt::AlignLeft);
	//opensave_grp->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));

		QPushButton* open = new QPushButton(QIcon(QPixmap("Icons/fileopen.xpm")), "Open", opensave_grp, "open");
		connect( open, SIGNAL( clicked () ), this, SLOT( open_event() ) );
		//open->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
		
		QPushButton* save = new QPushButton(QIcon(QPixmap("Icons/filesave.xpm")), "Save", opensave_grp, "save");
		connect( save, SIGNAL( clicked () ), this, SLOT( save_event() ) );
		//save->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));

	Q3ButtonGroup* addrmv_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Actions:", this );
    addrmv_grp->setExclusive( FALSE );

		QPushButton* add = new QPushButton("New", addrmv_grp, "new");
		add->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
		connect( add, SIGNAL( clicked() ), this, SLOT( add_event() ) );
		
		QPushButton* del = new QPushButton("Delete", addrmv_grp, "del");
		del->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
		connect( del, SIGNAL( clicked() ), this, SLOT( del_event() ) );
		
	Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );

		red = new QPushButton("R", toggle_grp);
		red->setToggleButton(TRUE);
		red->setOn(TRUE);
		red->setPaletteForegroundColor(Qt::red); 
		connect( red, SIGNAL( clicked() ), this, SLOT( redchannel_button_event() ) );

		green = new QPushButton("G", toggle_grp);
		green->setToggleButton(TRUE);
		green->setOn(TRUE);
		green->setPaletteForegroundColor(QColor(0,128,0)); 
		connect( green, SIGNAL( clicked() ), this, SLOT( greenchannel_button_event() ) );

		blue = new QPushButton("B", toggle_grp);
		blue->setToggleButton(TRUE);
		blue->setOn(TRUE);
		blue->setPaletteForegroundColor(Qt::blue); 
		connect( blue, SIGNAL( clicked() ), this, SLOT( bluechannel_button_event() ) );

	Q3ButtonGroup* list_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
    list_grp->setExclusive( FALSE );

		listbox = new Q3ListBox(list_grp, "ListBox", 0);
		listbox->setColumnMode(1);
		connect(listbox, SIGNAL(doubleClicked(Q3ListBoxItem*)), this, SLOT(listbox_event(Q3ListBoxItem*)));

	Q3BoxLayout * left_blayout = new Q3BoxLayout ( 0, Q3BoxLayout::TopToBottom, 0, 5, "left_blayout");
	left_blayout->setAlignment(Qt::AlignTop);
    left_blayout->addWidget( opensave_grp, 0, Qt::AlignTop);
    left_blayout->addWidget( addrmv_grp, 0, Qt::AlignTop);

	Q3BoxLayout * rigth_blayout = new Q3BoxLayout ( 0, Q3BoxLayout::TopToBottom, 0, 5, "rigth_blayout");
    rigth_blayout->addWidget( toggle_grp, 0);
    rigth_blayout->addWidget( list_grp, 0);
	
	//our transfertab top level layout
	Q3BoxLayout * isotab_blayout = new Q3BoxLayout ( this, Q3BoxLayout::LeftToRight, 10, 5, "isotab_blayout");
    isotab_blayout->addLayout( left_blayout, 1);
    isotab_blayout->addLayout( rigth_blayout, 10);
}
void QtIsoSurfaceWidget::open_event(void)
{
	QFileDialog* fd = new QFileDialog( this );
	QString fn = fd->getOpenFileName(currentworkingdir, "*.obj", this );
//	delete fd;

	if(fn.isEmpty()) return;

	emit isosurface_load_signal(fn);
}
void QtIsoSurfaceWidget::save_event(void)
{
	emit isosurface_save_signal();
}
void QtIsoSurfaceWidget::redchannel_button_event(void)
{
	if(red->isOn())
	{
		channels[0] = true;
	}
	else
	{
		channels[0] = false;
	}
}
void QtIsoSurfaceWidget::greenchannel_button_event(void)
{

	if(green->isOn())
	{
		channels[1] = true;
	}
	else
	{
		channels[1] = false;
	}

}
void QtIsoSurfaceWidget::bluechannel_button_event(void)
{
	if(blue->isOn())
	{
		channels[2] = true;
	}
	else
	{
		channels[2] = false;
	}
}
void QtIsoSurfaceWidget::msg_event(void)
{
	//===========================================================
	//open up a message box with all the editable details
	//===========================================================
	msgbox = new QDialog(this);
	
	Q3ButtonGroup *msgbox_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", msgbox);

	Q3ButtonGroup *name_grp = new Q3ButtonGroup( 2, Qt::Horizontal, "", msgbox_grp);

		Q3ButtonGroup *namesub_grp = new Q3ButtonGroup( 2, Qt::Vertical, "Name:", name_grp);
		namesub_grp->setFrameStyle(QFrame::NoFrame);
			//QLabel* name_lb = new QLabel("Name:	", namesub_grp);
			name_le = new QLineEdit(mname, namesub_grp);
			name_le->setFixedHeight(24);

		Q3ButtonGroup *colsub_grp = new Q3ButtonGroup( 2, Qt::Vertical, "Colour:", name_grp);
		colsub_grp->setFrameStyle(QFrame::NoFrame);

			QPixmap pixmap = QPixmap(name_le->height()*0.5, name_le->height()*0.5);
			pixmap.fill(colour);
			col_btn = new QPushButton("", colsub_grp);
			col_btn->setFixedSize(name_le->height(), name_le->height());
			col_btn->setPixmap(pixmap);
			connect( col_btn, SIGNAL( clicked() ), this, SLOT(msgbox_colourdlg_event() ) );

		//QPalette pal = QApplication::palette(col_btn);
		//QColor bg = QColor("lightgray");
		//pal.setColor(QPalette::Base, bg);

	Q3ButtonGroup *info_grp = new Q3ButtonGroup( 4, Qt::Horizontal, "", msgbox_grp);
		QString numbs;
		QLabel* numverts_lb = new QLabel("nv:	", info_grp);
		numverts_lb->setFixedWidth(20);
		QLineEdit* numverts_le = new QLineEdit(numbs.setNum(nv,10), info_grp);
		numverts_le->setFrame(false);
		numverts_le->setBackgroundRole(QPalette::Window);
		numverts_le->setReadOnly(true);
		numverts_le->setFixedWidth(80);
		QLabel* numfaces_lb = new QLabel(" nf:	", info_grp);
		numfaces_lb->setFixedWidth(20);
		QLineEdit* numfaces_le = new QLineEdit(numbs.setNum(nf,10), info_grp);
		numfaces_le->setFrame(false);
		numfaces_le->setBackgroundRole(QPalette::Window);
		numfaces_le->setReadOnly(true);
		numfaces_le->setFixedWidth(80);

	Q3ButtonGroup* threshold_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", msgbox_grp );
    threshold_grp->setExclusive( FALSE );

		//our isosurface quality slider
		isoqual_lb = new QLabel( "Sampling Quality:	100%", threshold_grp );
		isoqual_slider = new QSlider ( 0, 100, 12.5, 100, Qt::Horizontal, threshold_grp, "isoqual_slider" );
		isoqual_slider->setLineStep(25);
		isoqual_slider->setTickInterval(25);
		isoqual_slider->setTickmarks( QSlider::Below );
		isoqual_slider->setTracking(TRUE);
		connect( isoqual_slider, SIGNAL(valueChanged(int)), this, SLOT(isoqual_event(int)) );
		isoqual_event(isoqual);

	/*	isosmooth_lb = new QLabel( "Gaussian Smoothing:	100%", threshold_grp );
		isosmooth_slider = new QSlider ( 0, 100, 25, 100, Qt::Horizontal, threshold_grp, "isosmooth_slider" );
		isosmooth_slider->setLineStep(25);
		isosmooth_slider->setTickInterval(25);
		isosmooth_slider->setTickmarks( QSlider::Below );
		isosmooth_slider->setTracking(TRUE);
		connect( isosmooth_slider, SIGNAL(valueChanged(int)), this, SLOT(isosmooth_event(int)) );
		isosmooth_event(isosmooth);*/

		//our isosurface value slider
		isoval_lb = new QLabel( "IsoSurface Value:	0", threshold_grp );
		isoval_slider = new QSlider ( 0, 255, 1, 0, Qt::Horizontal, threshold_grp, "isoval_slider" );
		isoval_slider->setTickmarks( QSlider::Below );
		isoval_slider->setTracking(TRUE);
		isoval_slider->setTickInterval(8);
		connect( isoval_slider, SIGNAL(valueChanged(int)), this, SLOT(isoval_event(int)) );
		isoval_event(isoval);
	
		Q3ButtonGroup *clip_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Clipping: ", msgbox_grp);
		clipstate = new QPushButton("ON/OFF", clip_grp);
		clipstate->setToggleButton(TRUE);
		
		if(clip_state==1) clipstate->setOn(TRUE);
		else clipstate->setOn(FALSE);


	/*Q3ButtonGroup *crop_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", msgbox_grp);
		cropstate = new QPushButton("Crop Data", crop_grp);
		connect( cropstate, SIGNAL( clicked() ), this, SLOT(crop_button_event() ) );
	*/
	Q3ButtonGroup *button_grp = new Q3ButtonGroup( 1, Qt::Vertical, "", msgbox_grp);
	QPushButton* ok_btn = new QPushButton("OK", button_grp);
	connect( ok_btn, SIGNAL( clicked() ), this, SLOT(msgbox_ok() ) );
	QPushButton* cancelbtn = new QPushButton("CANCEL", button_grp);
	connect( cancelbtn, SIGNAL( clicked() ), this, SLOT(msgbox_cancel() ) );

	Q3BoxLayout* topmost_blayout = new Q3BoxLayout ( msgbox, Q3BoxLayout::TopToBottom, 0, 0, "topmost_blayout");
    topmost_blayout->addWidget( msgbox_grp, 1);

	msgbox->exec();
}
bool QtIsoSurfaceWidget::check_unique_name(QString text)
{
	//find out if any item has this name already
	//we only accept unique names
	for(int i=0; i<lb_index; i++)
	{
		if(text==listbox->text(i) && i!=selected)
		{
			//regen a unique name
		/*	QString s;
			mname = "Iso-Surface-";
			s.setNum(lb_index);
			mname += s;
			name_le->setText(mname);*/

			return false;
		}
	}

	return true;
}
void QtIsoSurfaceWidget::listbox_event(Q3ListBoxItem * item)
{
	//find out which item we have selected (double clicked)
	for(int i=0; i<lb_index; i++)
	{
		if(listbox->isSelected(i))
		{
			selected = i;
		}
	}
	
	//get the items name
	mname = listbox->text(selected); 

	//get item values for currently selected item (use name)
	Vector col;
	QString oldname = mname;
	emit isosurface_get_signal(mname, &isoqual, &isoval, &isosmooth, &col, &nv, &nf, &clip_state);
	colour = QColor(col.x*255, col.y*255, col.z*255);

	//printf("%d , %d \n", nv, nf);
	//printf("clip_state: %d\n",clip_state);

	//get a gui to allow for changing of item values
	msg_event();

	//===========================================================
	// pressed OK on message box, so update accordingly
	//===========================================================
	if(msgbox_state==1)
	{
		//get new name, if any
		mname = name_le->text();

		//change QColor to Vector
		col = Vector(colour.red()/255.0, colour.green()/255.0, colour.blue()/255.0);

		if(isoqual==0) return;

		if(clipstate->isOn()) clip_state = 1;
		else clip_state = 0;

//		printf("clip_state: %d\n",clip_state);

		//apply changes to item
		emit isosurface_update_signal(oldname, mname, isoqual, isoval, isosmooth, col, channels, clip_state);
		
		//update list item
		QPixmap* icon = new QPixmap(10, 10);
		icon->fill(colour);
		listbox->changeItem(*icon, mname, selected);
	}
	else
	{

	}

	//cleanup, delete widgets
//	delete msgbox;
}
void QtIsoSurfaceWidget::addnew_event(QString name, Vector col)
{
	//printf("addnew_event()\n");
	nv = nf = 0;

	//update our list size variable
	lb_index++;

	mname = name;

	//change QColor to Vector
	colour = QColor(col.x*255, col.y*255, col.z*255);
	
	QPixmap* icon = new QPixmap(10, 10);
	icon->fill(colour);

	listbox->insertItem(*icon, name, lb_index);
	listbox->setSelected(lb_index, true);

	clip_state = 1;
}
void QtIsoSurfaceWidget::add_event(void)
{
	//default add item values
	msgbox_state=-1;
	selected=-1;
	QString s;
	mname = "Iso-Surface-";
	s.setNum(lb_index);
	mname += s;
	colour = Qt::white;
  	isoval=0;
	isoqual=25;
	isosmooth=25;
	nv = nf = 0;
	clip_state=1;

	//get a gui to allow for changing of item values
	msg_event();

	//===========================================================
	// pressed OK on message box, so update accordingly
	//===========================================================
	if(msgbox_state==1)
	{
		//get new name, if any
		mname = name_le->text();

		//change QColor to Vector
		Vector col = Vector(colour.red()/255.0, colour.green()/255.0, colour.blue()/255.0);

		if(isoqual==0) return;

		if(clipstate->isOn()) clip_state = 1;
		else clip_state = 0;

		//printf("%d, %d, %d\n", (int)channels[0], (int)channels[1], (int)channels[2]);

		//emit our changes to QtGLWidget
		emit isosurface_new_signal(mname, isoqual, isoval, isosmooth, col, channels, clip_state);

		//===========================================================
		//let our listbox know what we did
		//===========================================================
		QPixmap* icon = new QPixmap(10, 10);
		icon->fill(colour);

		mname = name_le->text();
		listbox->insertItem(*icon, mname, lb_index);
		listbox->setSelected(lb_index, true);

		//update our list size variable
		lb_index++;
	}
	else
	{

	}

	//cleanup, delete widgets
//	delete msgbox;
}
void QtIsoSurfaceWidget::del_all(void)
{
	nv = nf = 0;
	for(int i=0; i<lb_index; i++)
	{
		mname = listbox->text(i); 
		listbox->removeItem(i);
		emit isosurface_del_signal(mname);
	}
}
void QtIsoSurfaceWidget::del_event(void)
{
	if(lb_index<1) return;

	int i;

	switch( QMessageBox::warning(this, "Deleting...",
		"Are you sure you want to delete the selected item? \n",
		QMessageBox::Yes,
		QMessageBox::No | QMessageBox::Default )) 
	{
	case QMessageBox::Yes: 

		for(i=0; i<lb_index; i++)
		{
			//===========================================================
			//find out which item we have selected
			//===========================================================
			if(listbox->isSelected(i))
			{
				mname = listbox->text(i); 
				listbox->removeItem(i);
				emit isosurface_del_signal(mname);

				lb_index--;
				return;
			}
		}
		
		break;
	case QMessageBox::No: // Abort or Cancel
		// abort
		break;
	}
}

void QtIsoSurfaceWidget::msgbox_ok(void)
{
	//check that user has give an unique name for item
	if(check_unique_name(name_le->text()))
	{
		msgbox_state=1;
		msgbox->close();
	}
	else
	{
		//if user hasnt let them know about it
		QMessageBox::warning(this, "Warning...",
				"Please enter a UNIQUE name for the item! \n",
				QMessageBox::Ok, QMessageBox::Default );
	}
}
void QtIsoSurfaceWidget::msgbox_cancel(void)
{
	msgbox_state=0;
	msgbox->close();
}
void QtIsoSurfaceWidget::msgbox_colourdlg_event(void)
{
	QColorDialog* coldialogue=0;
	colour = coldialogue->getColor(colour, this, "Choose a colour");
	
	QPixmap pixmap = QPixmap(name_le->height()*0.5, name_le->height()*0.5);
	pixmap.fill(colour);
	col_btn->setPixmap(pixmap);
}

//isosurface tab, slider events
void QtIsoSurfaceWidget::isoqual_event(int j)
{
	float val = (float)j/12.5;
	j = int(val+0.5)*12.5;

	isoqual_slider->setValue(j);
	isoqual=j;

	//update our slider label
	QString s;
	QString text;
	text = "IsoSurface Quality:	";
	s.setNum(j);
	text += s;
	text += "%";
	isoqual_lb->setText (text);
}
void QtIsoSurfaceWidget::isosmooth_event(int j)
{
	float val = (float)j/25.0;
	j = int(val+0.5)*25.0;

	isosmooth_slider->setValue(j);
	isosmooth=j;

	//update our slider label
	QString s;
	QString text;
	text = "Gaussian Smoothing:	";
	s.setNum(j);
	text += s;
	text += "%";
	isosmooth_lb->setText (text);
}

void QtIsoSurfaceWidget::isoval_event(int j)
{
	isoval_slider->setValue(j);
	isoval=j;

	//update our slider label
	QString s;
	QString text;
	text = "IsoSurface Value:	";
	s.setNum(j);
	text += s;
	isoval_lb->setText (text);
}

void QtIsoSurfaceWidget::crop_button_event(void)
{
	emit isosurface_crop_signal(selected);
}
void QtIsoSurfaceWidget::isosurface_setcurrentworkingdir_slot(QString dn)
{
	currentworkingdir = dn;
}
