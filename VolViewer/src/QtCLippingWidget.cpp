/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "QtCLippingWidget.h"
//Added by qt3to4:
#include <QPixmap>


QtCLippingWidget::QtCLippingWidget(QWidget* parent, const char* name)
: QWidget( parent, name )
{
	currentworkingdir="";
	selectedplane=-1;
	setAcceptDrops(true);

	for(int i=0; i<6; i++) rotmode[i] = 0;
}
QtCLippingWidget::~QtCLippingWidget()
{
}

void QtCLippingWidget::rebuild(void)
{
}
void QtCLippingWidget::reset(void)
{
}
void QtCLippingWidget::dragndrop_slot(QString sFileName)
{	
	if (!sFileName.isEmpty()) 
	{
		if(sFileName.contains(".slc", true) || sFileName.contains(".SLC", true)) 
		{
			emit cutplane_load_signal(sFileName);
		}
	}
}
void QtCLippingWidget::dragEnterEvent(QDragEnterEvent* event)
{
	event->accept(Q3TextDrag::canDecode(event));
}
void QtCLippingWidget::dropEvent(QDropEvent* event)
{
	Q3StrList URIs;

	if ( Q3UriDrag::decode(event, URIs) ) 
	{
		QString text = Q3UriDrag::uriToLocalFile(URIs.getFirst());
		dragndrop_slot(text);
	}
}
void QtCLippingWidget::create(void)
{
	Q3ButtonGroup *loadsave_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "File:", this );

		load_clip = new QPushButton(QIcon(QPixmap("Icons/fileopen.xpm")), "Open", loadsave_grp, "open");
		connect( load_clip, SIGNAL( clicked () ), this, SLOT( cutplane_load_event() ) );

		save_clip = new QPushButton(QIcon(QPixmap("Icons/filesave.xpm")), "Save", loadsave_grp, "save");
		connect( save_clip, SIGNAL( clicked () ), this, SLOT( cutplane_save_event() ) );

	Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );

			red = new QPushButton("R", toggle_grp);
			red->setToggleButton(TRUE);
			red->setOn(TRUE);
			red->setPaletteForegroundColor(Qt::red); 
			connect( red, SIGNAL( pressed() ), this, SLOT( red_button_event() ) );

			green = new QPushButton("G", toggle_grp);
			green->setToggleButton(TRUE);
			green->setOn(TRUE);
			green->setPaletteForegroundColor(QColor(0,128,0)); 
			connect( green, SIGNAL( pressed() ), this, SLOT( green_button_event() ) );

			blue = new QPushButton("B", toggle_grp);
			blue->setToggleButton(TRUE);
			blue->setOn(TRUE);
			blue->setPaletteForegroundColor(Qt::blue); 
			connect( blue, SIGNAL( pressed() ), this, SLOT( blue_button_event() ) );
	
		/*QPushButton* iso = new QPushButton("Iso-Surface", toggle_grp);
		iso->setToggleButton(TRUE);
		iso->setOn(TRUE);
		connect( iso, SIGNAL( pressed() ), this, SLOT( iso_button_event() ) );*/

	Q3ButtonGroup *reset_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Reset:", this );

		resetcur = new QPushButton("Current", reset_grp);
		connect( resetcur, SIGNAL( pressed() ), this, SLOT(resetcurr_button_event() ) );

		reset1 = new QPushButton("All", reset_grp);
		connect( reset1, SIGNAL( pressed() ), this, SLOT( resetall_button_event() ) );

	Q3ButtonGroup *options_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Actions:", this );

		planeselectcombo = new QComboBox(false, options_grp, "");
		
		QStringList planeselectcombo_items;
		planeselectcombo_items += "Bottom";
		planeselectcombo_items += "Top";
		planeselectcombo_items += "Back";
		planeselectcombo_items += "Front";
		planeselectcombo_items += "Left";
		planeselectcombo_items += "Right";
		planeselectcombo_items += "None";		

		planeselectcombo->insertItems(0, planeselectcombo_items);
		planeselectcombo->setCurrentItem(6);	

		connect(planeselectcombo, SIGNAL(activated(int)), this, SLOT(planespinbox_event(int)));

		pickplane = new QPushButton("Select", options_grp);
		pickplane->setToggleButton(TRUE);
		connect( pickplane, SIGNAL( pressed() ), this, SLOT(pickplane_button_event() ) );

		unpickplane = new QPushButton("Un-Select", options_grp);
		unpickplane->setToggleButton(FALSE);
		connect( unpickplane, SIGNAL( pressed() ), this, SLOT(unpickplane_button_event() ) );
	
	QStringList planecombo_items;
	planecombo_items += "Off";
	planecombo_items += "Wireframe";
	planecombo_items += "Solid Texture";
	planecombo_items += "Masked Texture";
	//planecombo_items += "Curved Texture";

	selectedclip_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Selected Clipping Plane:", this );

		Q3ButtonGroup* selectedclipusb_grp = new Q3ButtonGroup( 2, Qt::Horizontal, "View Mode:", selectedclip_grp );
		
			planecombo = new QComboBox (false, selectedclipusb_grp, "");
			planecombo->insertItems(0, planecombo_items);
			planecombo->setCurrentItem(0);
			connect(planecombo, SIGNAL(activated(int)), this, SLOT(planecombo_event(int)));

			QPushButton* applyall = new QPushButton("Apply to All", selectedclipusb_grp, "applyall");
			connect( applyall, SIGNAL( clicked() ), this, SLOT( applyall_event() ) );

		Q3ButtonGroup *interact_grp = new Q3ButtonGroup( 3, Qt::Horizontal, "Interaction Mode:", selectedclip_grp );
		interact_grp->setExclusive(true);
		connect(interact_grp, SIGNAL(clicked(int)), this, SLOT(interact_clicked_event(int)));

			plane_trans = new QPushButton("Translation", interact_grp);
			plane_trans->setToggleButton(true);
			plane_trans->setOn(true);

			plane_rotX = new QPushButton("Rotation AXIS 1", interact_grp);
			plane_rotX->setToggleButton(true);
			plane_rotX->setOn(false);

			plane_rotY = new QPushButton("Rotation AXIS 2", interact_grp);
			plane_rotY->setToggleButton(true);
			plane_rotY->setOn(false);
			
	selectedclip_grp->setEnabled(false);

	Q3ButtonGroup *curveplane_grp = new Q3ButtonGroup( 2, Qt::Vertical, "Curved Plane:", this );

		//our curveplane_xang slider
		Q3ButtonGroup* xang_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", curveplane_grp );
		curveplane_xang_lb = new QLabel( "X-angle: 0", xang_grp );
		curveplane_xang_slider = new QSlider ( -1024, 1024, 10, 0, Qt::Horizontal, xang_grp, "curveplane_xang_slider" );
		curveplane_xang_slider->setTickmarks( QSlider::Below );
		curveplane_xang_slider->setTracking(FALSE);
		curveplane_xang_slider->setTickInterval(10);
		connect(curveplane_xang_slider, SIGNAL(valueChanged(int)), this, SLOT(curveplane_xang_event(int)) );
		curveplane_xang = 0;
		//l_altitude_event(l_altitude);

		//our curveplane_yang slider
		Q3ButtonGroup* yang_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", curveplane_grp );
		curveplane_yang_lb = new QLabel( "Y-angle: 0", yang_grp );
		curveplane_yang_slider = new QSlider ( -1024, 1024, 10, 0, Qt::Horizontal, yang_grp, "curveplane_yang_slider" );
		curveplane_yang_slider->setTickmarks( QSlider::Below );
		curveplane_yang_slider->setTracking(FALSE);
		curveplane_yang_slider->setTickInterval(10);
		connect(curveplane_yang_slider, SIGNAL(valueChanged(int)), this, SLOT(curveplane_yang_event(int)) );
		curveplane_yang = 0;
		///l_azimuth_event(l_azimuth);

		curveplanetexcoords_btn = new QPushButton("Toggle TexCoords", curveplane_grp);
		curveplanetexcoords_btn->setToggleButton(true);
		connect(curveplanetexcoords_btn, SIGNAL( pressed() ), this, SLOT(curveplanetexcoords_button_event() ) );
		
		curveplanetranslate_btn = new QPushButton("Translate", curveplane_grp);
		curveplanetranslate_btn->setToggleButton(true);
		connect(curveplanetranslate_btn, SIGNAL( pressed() ), this, SLOT(curveplanetranslate_button_event() ) );
		curveplane_grp->hide();

/*	Q3ButtonGroup *clipoperators_grp = new Q3ButtonGroup( 3, Qt::Horizontal, "Clipping Operators:", this );

		crop = new QPushButton("Crop Data", clipoperators_grp);
		connect( crop, SIGNAL( pressed() ), this, SLOT(crop_button_event() ) );

		invcrop = new QPushButton("Inverse-Crop Data", clipoperators_grp);
		connect( invcrop, SIGNAL( pressed() ), this, SLOT(invcrop_button_event() ) );

		recentre = new QPushButton("Recentre Data", clipoperators_grp);
		connect( recentre, SIGNAL( pressed() ), this, SLOT(recentre_button_event() ) );

		realign = new QPushButton("Realign Data", clipoperators_grp);
		connect( realign, SIGNAL( pressed() ), this, SLOT(realign_button_event() ) );	
		realign->hide();
*/	
	Q3BoxLayout *vboxlayoutRIGHT = new Q3BoxLayout ( 0, Q3BoxLayout::TopToBottom, 0, 5, "vboxlayout");
	vboxlayoutRIGHT->addWidget(toggle_grp, 0, Qt::AlignTop);
	vboxlayoutRIGHT->addWidget(selectedclip_grp, 0, Qt::AlignTop);
	vboxlayoutRIGHT->addWidget(curveplane_grp, 0, Qt::AlignTop);	

	Q3BoxLayout *vboxlayoutLEFT = new Q3BoxLayout ( 0, Q3BoxLayout::TopToBottom, 0, 5, "vboxlayout");
	vboxlayoutLEFT->setAlignment(Qt::AlignTop);
	vboxlayoutLEFT->addWidget(loadsave_grp, 0, Qt::AlignTop);
	vboxlayoutLEFT->addWidget(options_grp, 0, Qt::AlignTop);
	vboxlayoutLEFT->addWidget(reset_grp, 0, Qt::AlignTop);

	Q3BoxLayout *topmost= new Q3BoxLayout ( this, Q3BoxLayout::LeftToRight, 10, 5, "topmost");
	topmost->addLayout(vboxlayoutLEFT,1);
	topmost->addLayout(vboxlayoutRIGHT, 10);
}
void QtCLippingWidget::curveplanetranslate_button_event(void)
{
	if(curveplanetranslate_btn->isOn()) emit cutplane_curvetrans_signal(false);
	else emit cutplane_curvetrans_signal(true);
}
void QtCLippingWidget::curveplanetexcoords_button_event(void)
{
	if(curveplanetexcoords_btn->isOn()) emit cutplane_curvecoords_signal(false);
	else emit cutplane_curvecoords_signal(true);
}
void QtCLippingWidget::curveplane_xang_event(int j)
{
	int temp = j;

	curveplane_xang_slider->setValue(j);
	curveplane_xang=j;

	//update our slider label
	QString s;
	QString text;
	text = "X-angle: ";
	s.setNum(j);
	text += s;
	curveplane_xang_lb->setText (text);

	emit cutplane_curveangle_signal(curveplane_xang, curveplane_yang);
}
void QtCLippingWidget::curveplane_yang_event(int j)
{
	int temp = j;

	curveplane_yang_slider->setValue(j);
	curveplane_yang=j;

	//update our slider label
	QString s;
	QString text;
	text = "Y-angle: ";
	s.setNum(j);
	text += s;
	curveplane_yang_lb->setText (text);

	emit cutplane_curveangle_signal(curveplane_xang, curveplane_yang);
}
void QtCLippingWidget::cutplane_unselect_slot(void)
{
	selectedplane = 0;
	planeselectcombo->setCurrentItem(selectedplane);
	selectedclip_grp->setEnabled(false);
	unpickplane_button_event();
}
void QtCLippingWidget::cutplane_selectedupdate_slot(int index, int drawmode)
{
	selectedplane = index;

	planecombo_event(drawmode);
	planecombo->setCurrentItem(drawmode);
}
void QtCLippingWidget::applyall_event(void)
{
	emit cutplane_applyall_drawmode_signal();
}

void QtCLippingWidget::planespinbox_event(int index)
{
	printf("planespinbox_event: selected: %d\n", index);

	selectedplane = index;
	planeselectcombo->setCurrentItem(index);
		
	if(selectedplane==6)
	{
		selectedplane=-1;
		selectedclip_grp->setEnabled(false);
	}
	else
	{
		selectedclip_grp->setEnabled(true);

		pickplane->setOn(false);

		if(rotmode[index]==0) plane_trans->setOn(true);
		else if(rotmode[index]==1) plane_rotX->setOn(true);
		else if(rotmode[index]==2) plane_rotY->setOn(true);

		emit cutplane_interact_clicked_signal(rotmode[selectedplane]);
	}

	emit cutplane_selected_signal(selectedplane);
}
void QtCLippingWidget::cutplane_enableplanepanel_slot(void)
{
	printf("cutplane_enableplanepanel_slot\n");
	selectedclip_grp->setEnabled(true);
	//pickplane->setOn(false);
	
	selectedclip_grp->setEnabled(true);
	pickplane->setOn(false);
	emit cutplane_showall_signal(0);
}

void QtCLippingWidget::planecombo_event(int drawmode)
{
	emit cutplane_rendermode_signal(drawmode);
}

void QtCLippingWidget::cutplane_selected_slot(int index, int pdrawmode)
{
	selectedplane = index;
	planespinbox_event(selectedplane);

	if(index!=-1)
	{
		selectedclip_grp->setEnabled(true);

		planecombo_event(pdrawmode);
		planecombo->setCurrentItem(pdrawmode);

		pickplane_button_event();
		pickplane->toggle();
	}
	else
	{
		selectedclip_grp->setEnabled(false);
		pickplane_button_event();
		pickplane->toggle();
	}
}
void QtCLippingWidget::exit(void)
{
}

void QtCLippingWidget::invert_button_event(void)
{
	if(invert_clip->isOn()) emit cutplane_invert_signal(0);
	else emit cutplane_invert_signal(1);
}
void QtCLippingWidget::unpickplane_button_event(void)
{
	selectedclip_grp->setEnabled(false);
	planespinbox_event(6);
	pickplane->setOn(false);
	emit cutplane_unselect_signal();
	emit cutplane_showall_signal(0);
}
void QtCLippingWidget::pickplane_button_event(void)
{
	if(!pickplane->isOn()) emit cutplane_showall_signal(1);
	else emit cutplane_showall_signal(0);
}
void QtCLippingWidget::resetall_button_event(void)
{
	pickplane->setOn(false);
	plane_trans->setOn(true);
	for(int i=0; i<6; i++) rotmode[i] = 0;
	emit cutplane_interact_clicked_signal(0);
	planespinbox_event(6);
	emit cutplane_resetall_signal();
}
void QtCLippingWidget::resetcurr_button_event(void)
{
	emit cutplane_resetcurrent_signal();
	pickplane->setOn(false);
	planespinbox_event(6);
}
void QtCLippingWidget::cutplane_load_event(void)
{
	QString fn = QFileDialog::getOpenFileName(currentworkingdir, "*.slc", this );
	
	if ( !fn.isEmpty() ) 
	{	
		emit cutplane_load_signal(fn);
	}
	else
	{

	}
}
void QtCLippingWidget::cutplane_setcurrentworkingdir_slot(QString dn)
{
	currentworkingdir = dn;
}
void QtCLippingWidget::cutplane_save_event(void)
{	
	QString fn = QFileDialog::getSaveFileName(currentworkingdir, "*.slc", this );
  	  
	if ( !fn.isEmpty() ) 
	{	
		if(fn[int(fn.length()-4)]!='.') fn += ".slc";

		emit cutplane_save_signal(fn);
	}
	else
	{

	}
}

void QtCLippingWidget::red_button_event(void)
{
	emit cutplane_redclip_signal();
}
void QtCLippingWidget::green_button_event(void)
{
	emit cutplane_greenclip_signal();
}
void QtCLippingWidget::blue_button_event(void)
{
	emit cutplane_blueclip_signal();
}
void QtCLippingWidget::iso_button_event(void)
{
	emit cutplane_isoclip_signal();
}
void QtCLippingWidget::recentre_button_event(void)
{
	emit cutplane_recentre_signal();
}
void QtCLippingWidget::crop_button_event(void)
{
	emit cutplane_crop_signal();
}
void QtCLippingWidget::realign_button_event(void)
{
	emit cutplane_realign_signal();
}
void QtCLippingWidget::invcrop_button_event(void)
{
	emit cutplane_invcrop_signal();
}
void QtCLippingWidget::interact_clicked_event(int sel)
{
	rotmode[selectedplane] = sel;

	//printf("sel: %d\n", sel);
	emit cutplane_interact_clicked_signal(sel);
}
void QtCLippingWidget::measure_setcutplane_slot(int selected, float translate, float rotang1, float rotang2)
{
	printf("QtCLippingWidget:: selected: %d\n", selected);
	cutplane_selected_slot(selected, 0);
	pickplane->setOn(false);
	emit cutplane_showall_signal(0);
}
void QtCLippingWidget::measure_disableunselect_slot()
{
	unpickplane->setDisabled(true);
	planeselectcombo->setDisabled(true);
	reset1->setDisabled(true);
	resetcur->setDisabled(true);
	pickplane->setDisabled(true);
//	crop->setDisabled(true);
//	invcrop->setDisabled(true);
//	recentre->setDisabled(true);
}
void QtCLippingWidget::measure_enableunselect_slot()
{
	unpickplane->setDisabled(false);
	planeselectcombo->setDisabled(false);
	reset1->setDisabled(false);
	resetcur->setDisabled(false);
	pickplane->setDisabled(false);
//	crop->setDisabled(false);
//	invcrop->setDisabled(false);
//	recentre->setDisabled(false);
}
