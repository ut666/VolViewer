/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "QtTransferFuncWidget.h"
//Added by qt3to4:
#include <Q3Frame>
#include <QPixmap>
#include <QLabel>

QtTransferFuncWidget::QtTransferFuncWidget(QWidget* parent, const char* name )
: QWidget( parent, name )
{
	setAcceptDrops(true);
	threshmin = 0;
	threshmax = 65535;
	
	histo16_logscale = 1;		
}

QtTransferFuncWidget::~QtTransferFuncWidget()
{
}
void QtTransferFuncWidget::dragndrop_slot(QString sFileName)
{	
	if (!sFileName.isEmpty()) 
	{
		if(sFileName.contains(".tf2", true) || sFileName.contains(".TF2", true)) 
		{
			tfunc_loadfile(sFileName);
		}
	}
}
void QtTransferFuncWidget::dragEnterEvent(QDragEnterEvent* event)
{
	event->accept(Q3TextDrag::canDecode(event));
}
void QtTransferFuncWidget::dropEvent(QDropEvent* event)
{
	Q3StrList URIs;

	if ( Q3UriDrag::decode(event, URIs) ) 
	{
		QString text = Q3UriDrag::uriToLocalFile(URIs.getFirst());
		dragndrop_slot(text);
	}
}
void QtTransferFuncWidget::create(void)
{    
    histogrampixmap_luminance = new QtHistoTfuncPixmap(65536);
    histogrampixmap_opacity = new QtHistoTfuncPixmap(65536);
 	
	connect(histogrampixmap_luminance, SIGNAL(tfunc_remapsignal(void)), this, SLOT(tfunc_remapslot(void)));
 	connect(histogrampixmap_opacity, SIGNAL(tfunc_remapsignal(void)), this, SLOT(tfunc_remapslot(void)));

	histogrampixmap_luminance->mode = 0;
	histogrampixmap_opacity->mode = 1;

	histogrampixmap_luminance->remaponfly = 0;
	histogrampixmap_opacity->remaponfly = 0;
	
    //=============================================================================
    //printf("opensave_grp\n");
    Q3ButtonGroup* opensave_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "File:", this );
    opensave_grp->setExclusive( FALSE );
    opensave_grp->setAlignment(Qt::AlignLeft);

        QPushButton* open = new QPushButton(QIcon(QPixmap("icons/fileopen.xpm")), "Open", opensave_grp, "open");
        connect( open, SIGNAL( clicked () ), this, SLOT( open_event() ) );

        QPushButton* save = new QPushButton(QIcon(QPixmap("icons/filesave.xpm")), "Save", opensave_grp, "save");
        connect( save, SIGNAL( clicked () ), this, SLOT( save_event() ) );

    //=============================================================================
	Q3ButtonGroup* toggle_grp = new Q3ButtonGroup( 4, Qt::Horizontal, "Channels:", this);
    toggle_grp->setExclusive( TRUE );
	toggle_grp->setAlignment(Qt::AlignHCenter);

		QPushButton* intensity_pb = new QPushButton("ALL", toggle_grp);
		intensity_pb->setToggleButton(TRUE);
		intensity_pb->setOn(TRUE);
		intensity_pb->setMaximumWidth(42);
		intensity_pb->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));

		QPushButton* red_pb = new QPushButton("R", toggle_grp);
		red_pb->setToggleButton(TRUE);
		red_pb->setOn(FALSE);
		red_pb->setPaletteForegroundColor(Qt::red); 
		red_pb->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
		red_pb->setMaximumWidth(42);

		QPushButton* green_pb = new QPushButton("G", toggle_grp);
		green_pb->setToggleButton(TRUE);
		green_pb->setOn(FALSE);
		green_pb->setPaletteForegroundColor(QColor(0,128,0)); 
		green_pb->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
		green_pb->setMaximumWidth(42);

		QPushButton* blue_pb = new QPushButton("B", toggle_grp);
		blue_pb->setToggleButton(TRUE);
		blue_pb->setOn(FALSE);
		blue_pb->setPaletteForegroundColor(Qt::blue); 
		blue_pb->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
		blue_pb->setMaximumWidth(42);
		
		red_pb->setEnabled(false);
		green_pb->setEnabled(false);
		blue_pb->setEnabled(false);
		
    Q3ButtonGroup* channelmode_grp = new Q3ButtonGroup( 2, Qt::Vertical, "Channel Mode:", this);
    channelmode_grp->setExclusive( TRUE );
	channelmode_grp->setAlignment(Qt::AlignLeft);
	connect( channelmode_grp, SIGNAL( clicked (int) ), this, SLOT( channelmode_button_event(int) ) );
		
		QPushButton* luminance_pb = new QPushButton("Intensity", channelmode_grp);
		luminance_pb->setToggleButton(TRUE);
		luminance_pb->setOn(FALSE);
		luminance_pb->setEnabled(false);

		QPushButton* opacity_pb = new QPushButton("Opacity", channelmode_grp);
		opacity_pb->setToggleButton(TRUE);
		opacity_pb->setOn(TRUE);
		
		Q3ButtonGroup *reset_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Reset:", this );

			QPushButton* resetcurrent_button = new QPushButton("Current", reset_grp, "restcurr");
			resetcurrent_button->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum, false));
			connect( resetcurrent_button, SIGNAL( clicked() ), this, SLOT(resetcurrent_button_action() ) );

			QPushButton* resetall_button = new QPushButton("All", reset_grp, "restall");
			resetall_button->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum, false));
			connect( resetall_button, SIGNAL( clicked() ), this, SLOT(resetall_button_action() ) );

		//button group to toggle on/off viewing on histrogram
		// ( HISTOGRAM, CUMMALATATIVE FREQUENCY, TRANSFER FUNCTION)
	   view_grp = new Q3ButtonGroup( 3, Qt::Horizontal, "View:", this );

				histcb = new QCheckBox("Histogram",	   view_grp );
				cumfcb = new QCheckBox("CumFrequency", view_grp );
				tfuncb = new QCheckBox("TransferFunc", view_grp );
				histcb->setChecked(TRUE);
				tfuncb->setChecked(TRUE);
				cumfcb->setChecked(FALSE);
				connect( histcb,	 SIGNAL( clicked() ), this, SLOT( histoview_action() ) );
				connect( cumfcb,	 SIGNAL( clicked() ), this, SLOT( cumfreview_action() ) );
				connect( tfuncb,	 SIGNAL( clicked() ), this, SLOT( transferfun_action() ) );

	   Q3ButtonGroup* update_grp = new Q3ButtonGroup( 2, Qt::Horizontal, "Histogram", this );

				remaponflycb = new QCheckBox("Auto-Update",	   update_grp);
				remaponflycb->setChecked(TRUE);
				connect( remaponflycb,	 SIGNAL( clicked() ), this, SLOT( remaponflycb_action() ) );

				logscalehistcb = new QCheckBox("Log Scale",	   update_grp);
				logscalehistcb->setChecked(TRUE);
				connect( logscalehistcb,	 SIGNAL( clicked() ), this, SLOT( logscalehistcb_action() ) );

		apply_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Transfer Function:", this );

			QPushButton* apply_button = new QPushButton("Apply", apply_grp, "apply");
			apply_button->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum, false));
			connect( apply_button, SIGNAL( clicked() ), this, SLOT(apply_button_action() ) );
		
		apply_grp->setEnabled(false);
		

    //=============================================================================

	//action group
	Q3ButtonGroup* action_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Actions:", this );
    action_grp->setExclusive( false );
	action_grp->setAlignment(Qt::AlignLeft);
	
		QPushButton* collapse_button = new QPushButton("Collapse to I", action_grp, "invert");
		collapse_button->setToggleButton(false);

		invert_button = new QPushButton("Invert Current", action_grp, "invert");
		invert_button->setToggleButton(true);
		//invert_button->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
		connect( invert_button, SIGNAL( pressed() ), this, SLOT(invert_button_action() ) );
	action_grp->hide();

    //histogram pixmap group

    /*Q3ButtonGroup* pixmap_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );

            pixmap16bit_lb = new QLabel(pixmap_grp);
            pixmap16bit_lb->setPixmap(*histogram16bit);

            pixmap8bit_lb = new QLabel(pixmap_grp);
            pixmap8bit_lb->setPixmap(*histogram8bit);*/

    //=============================================================================
    //threshold slider group
	//printf("threshold_grp");
    threshold_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
    threshold_grp->setExclusive( FALSE );

        threshmin_lb = new QLabel( "Min. Threshold:	0", threshold_grp );
        //our min threshold slider
        threshmin_slider = new QSlider ( 0, 65535, 1, 0, Qt::Horizontal, threshold_grp, "threshmin_slider" );
        threshmin_slider->setTickmarks( QSlider::Below );
        threshmin_slider->setTracking(FALSE);
		threshmin_slider->setPageStep(0);
		threshmin_slider->setSingleStep(0);	
        threshmin_slider->setTickInterval(65535.0/8.0);
		connect( threshmin_slider, SIGNAL(valueChanged(int)), this, SLOT(threshmin_action(int)) );
        //connect( threshmin_slider, SIGNAL(sliderReleased()), this, SLOT(released_slider_slot()) );

        threshmax_lb = new QLabel( "Max. Threshold:	65535", threshold_grp );
        //our max threshold slicer
        threshmax_slider = new QSlider ( 0, 65535, 1, 65535, Qt::Horizontal, threshold_grp, "threshmax_slider" );
        threshmax_slider->setTickmarks( QSlider::Below );
        threshmax_slider->setTracking(FALSE);
		threshmax_slider->setPageStep(0);
		threshmax_slider->setSingleStep(0);	   
		threshmax_slider->setTickInterval(65535.0/8.0);
        connect( threshmax_slider, SIGNAL(valueChanged(int)), this, SLOT(threshmax_action(int)) );
        //connect( threshmax_slider, SIGNAL(sliderReleased()), this, SLOT(released_slider_slot()) );

    //=============================================================================
	//brightness & contrast slider group
	Q3ButtonGroup* briconslider_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
	briconslider_grp->setAlignment(Qt::AlignLeft);
	//briconslider_grp->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding, false));
	briconslider_grp->setExclusive( FALSE );

			Q3ButtonGroup* bri_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", briconslider_grp );
			//briconslider_grp->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
			bri_grp->setExclusive( FALSE );
			//bri_grp->setAlignment(Qt::AlignJustify);
			//bri_grp->setFrameStyle(Q3Frame::NoFrame);

				QLabel* brightnesstext_lb = new QLabel( "Bright.", bri_grp );
				brightness = 100;
				//our min threshold slider
				brightness_slider = new QSlider ( 0, 200, 1, 100, Qt::Vertical, bri_grp, "threshmin_slider" );
				brightness_slider->setInvertedAppearance(true);
				brightness_slider->setInvertedControls(true);
				brightness_slider->setTickmarks( QSlider::TicksRight );
				brightness_slider->setTracking(FALSE);
				brightness_slider->setTickInterval(10);
				connect( brightness_slider, SIGNAL(valueChanged(int)), this, SLOT(brightness_action(int)) );
				//connect( brightness_slider, SIGNAL(sliderReleased()), this, SLOT(released_slider_slot()) );
				brightness_lb = new QLabel( "100%", bri_grp );

			Q3ButtonGroup* con_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", briconslider_grp );
			//briconslider_grp->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
			con_grp->setExclusive( FALSE );
			//con_grp->setAlignment(Qt::AlignJustify);
			//con_grp->setFrameStyle(Q3Frame::NoFrame);

				QLabel* contrasttext_lb = new QLabel( "Cont.", con_grp );
				contrast = 100;
				//our max threshold slicer
				contrast_slider = new QSlider ( 0, 200, 1, 100, Qt::Vertical, con_grp, "threshmax_slider" );
				contrast_slider->setInvertedAppearance(true);
				contrast_slider->setTickmarks( QSlider::TicksRight );
				contrast_slider->setInvertedControls(true);
				contrast_slider->setTracking(FALSE);
				contrast_slider->setTickInterval(10);
				connect( contrast_slider, SIGNAL(valueChanged(int)), this, SLOT(contrast_action(int)) );
				//connect( contrast_slider, SIGNAL(sliderReleased()), this, SLOT(released_slider_slot()) );
				contrast_lb = new QLabel( "100%", con_grp );
    //=============================================================================


	Q3BoxLayout* histtoplayout_centre = new Q3BoxLayout ( 0, QBoxLayout::LeftToRight, 0, 5, "histtoplayout_centre");
	histtoplayout_centre->addWidget(view_grp, 1);
	histtoplayout_centre->addWidget(update_grp, 0, Qt::AlignTop);
	//histtoplayout_centre->addWidget(mapping_grp, 0, Qt::AlignTop);
	
	Q3BoxLayout* histlayout_centre = new Q3BoxLayout ( 0, QBoxLayout::TopToBottom, 0, 5, "histlayout_centre");
	histlayout_centre->addLayout(histtoplayout_centre);
	//histlayout_centre->addWidget(hist, 10);
    histlayout_centre->addWidget(histogrampixmap_luminance, 10);
	histlayout_centre->addWidget(histogrampixmap_opacity, 10);
	histlayout_centre->addWidget(threshold_grp, 1);
	
	//briconslider_grp->hide();
	Q3BoxLayout* vboxlayout_centre = new Q3BoxLayout ( 0, QBoxLayout::LeftToRight, 0, 5, "vboxlayout_centre");
	vboxlayout_centre->addLayout(histlayout_centre);
	vboxlayout_centre->addWidget(briconslider_grp);
		
	Q3BoxLayout* vboxlayoutRIGHT = new Q3BoxLayout ( 0, QBoxLayout::TopToBottom, 0, 5, "vboxlayout");
	vboxlayoutRIGHT->addLayout(vboxlayout_centre);
	
	Q3BoxLayout* vboxlayoutLEFT = new Q3BoxLayout ( 0, QBoxLayout::TopToBottom, 0, 5, "vboxlayout");
	vboxlayoutLEFT->setAlignment(Qt::AlignTop);
	vboxlayoutLEFT->addWidget(opensave_grp, 0, Qt::AlignTop);
	vboxlayoutLEFT->addWidget(toggle_grp, 0, Qt::AlignTop);
	vboxlayoutLEFT->addWidget(channelmode_grp, 0, Qt::AlignTop);	
	vboxlayoutLEFT->addWidget(reset_grp, 0, Qt::AlignTop);
    vboxlayoutLEFT->addWidget(apply_grp, 0, Qt::AlignTop);
	
	Q3BoxLayout* topmost= new Q3BoxLayout ( this, QBoxLayout::LeftToRight, 5, 5, "topmost");
	topmost->addLayout(vboxlayoutLEFT);
	topmost->addLayout(vboxlayoutRIGHT);

	//printf("histlayout_centre");
/*
    Q3BoxLayout *histlayout_centre = new Q3BoxLayout ( 0, QBoxLayout::TopToBottom, 0, 5, "tfunc_histlayout_centre");
    histlayout_centre->addWidget(view_grp, 1);
    histlayout_centre->addWidget(histogrampixmap_luminance, 10);
	histlayout_centre->addWidget(histogrampixmap_opacity, 10);
    histlayout_centre->addWidget(threshold_grp, 1);
	
    Q3BoxLayout *vboxlayoutRIGHT = new Q3BoxLayout ( 0, QBoxLayout::LeftToRight, 0, 5, "tfunc_vboxlayout1");
    vboxlayoutRIGHT->addLayout(histlayout_centre);
	vboxlayoutRIGHT->addWidget(briconslider_grp);
	
    Q3BoxLayout *vboxlayoutLEFT = new Q3BoxLayout ( 0, QBoxLayout::TopToBottom, 0, 5, "tfunc_vboxlayout2");
    vboxlayoutLEFT->setAlignment(Qt::AlignTop);
    vboxlayoutLEFT->addWidget(opensave_grp, 0, Qt::AlignTop);
	vboxlayoutLEFT->addWidget(toggle_grp, 0, Qt::AlignTop);
    vboxlayoutLEFT->addWidget(channelmode_grp, 0, Qt::AlignTop);
    vboxlayoutLEFT->addWidget(update_grp, 0, Qt::AlignTop);
	vboxlayoutLEFT->addWidget(action_grp, 0, Qt::AlignTop);
    vboxlayoutLEFT->addWidget(reset_grp, 0, Qt::AlignTop);
    vboxlayoutLEFT->addWidget(apply_grp, 0, Qt::AlignTop);
	
    Q3BoxLayout *topmost= new Q3BoxLayout ( this, QBoxLayout::LeftToRight, 0, 5, "tfunc_topmost");
    topmost->addLayout(vboxlayoutLEFT, 1);
    topmost->addLayout(vboxlayoutRIGHT, 10);
	*/
	channelmode_button_event(1);

	histogrampixmap_luminance->remaponfly = 1;
	histogrampixmap_opacity->remaponfly = 1;
}
void QtTransferFuncWidget::open_event(void)
{
	QFileDialog* fd = new QFileDialog(0);

	QString fn = fd->getOpenFileName(currentworkingdir, "*.tf2", this );
	tfunc_loadfile(fn);
	delete fd;
}
void QtTransferFuncWidget::tfunc_loadfile(QString fn)
{
	if(fn.isEmpty()) return;

	//temp storage for param parsing
	string label;
	int i;
	int size;
	Vector v;

	//clear our BSs
	histogrampixmap_luminance->funceditor.bs.clear();
	histogrampixmap_opacity->funceditor.bs.clear();
	
	//input file for reading in parameter ranges
	ifstream input_file; 
	input_file.open(fn.latin1());

	//tokenize our input file
	Tokenizer token(input_file);

	//until oef is reached step through each token
	while(!input_file.eof()){
		if(token.ttype()==TT_WORD)
		{
			//get the first string lable
			label = token.sval();
			
			if(label=="LUMINANCE_THRESHOLD_LEVELS")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) histogrampixmap_luminance->minthr = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) histogrampixmap_luminance->maxthr = token.nval();
				token.nextToken();
			}			
			else if(label=="LUMINANCE_TF_CONTROL_POINTS")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) size = token.nval();
				
				for(i=0; i<size; i++)
				{
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.x = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.y = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.z = token.nval();

					histogrampixmap_luminance->funceditor.bs.control_points.push_back(v);
				}
			}				
			if(label=="OPACITY_THRESHOLD_LEVELS")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) histogrampixmap_opacity->minthr = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) histogrampixmap_opacity->maxthr = token.nval();
				token.nextToken();
			}			
			else if(label=="OPACITY_TF_CONTROL_POINTS")
			{		
				token.nextToken();
				if(token.ttype()==TT_NUMBER) size = token.nval();
				
				for(i=0; i<size; i++)
				{
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.x = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.y = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.z = token.nval();

					histogrampixmap_opacity->funceditor.bs.control_points.push_back(v);
				}
			}				
			//else if we dont know what it is, go to next label
			else
			{
				//f<<"WARNING: Unrecognized Label..."<<endl;
				token.nextToken();
			}
		}
		else token.nextToken();
	}

	input_file.close();

	//compute our BS curve from our loaded control points
	histogrampixmap_opacity->funceditor.bs.compute();
	histogrampixmap_luminance->funceditor.bs.compute();

	histogrampixmap_opacity->update();
	histogrampixmap_luminance->update();
	update();

	remaponflycb->setChecked(TRUE);
	
	if(channel_mode==0)
	{
		histogrampixmap_luminance->remaponfly = -1;
		
		threshmin_action(histogrampixmap_luminance->minthr);
		threshmin_slider->setValue(histogrampixmap_luminance->minthr);
		threshmax_action(histogrampixmap_luminance->maxthr);
		threshmax_slider->setValue(histogrampixmap_luminance->maxthr);
		histogrampixmap_luminance->remaponfly = 1;
		update();
		
		histogrampixmap_luminance->tfunc_remap16bit();
	}
	else
	{
		histogrampixmap_opacity->remaponfly = -1;
		threshmin_action(histogrampixmap_opacity->minthr);
		threshmin_slider->setValue(histogrampixmap_opacity->minthr);
		threshmax_action(histogrampixmap_opacity->maxthr);
		threshmax_slider->setValue(histogrampixmap_opacity->maxthr);
		histogrampixmap_opacity->remaponfly = 1;
		update();

		histogrampixmap_opacity->tfunc_remap16bit();	
	}
}

void QtTransferFuncWidget::save_event(void)
{
	printf("QtHistoTfuncPixmap save()\n");
	int i;
	
	QFileDialog* fd = new QFileDialog(this);
	QString fn = fd->getSaveFileName(currentworkingdir, "*.tf2", this );

	if(fn.isEmpty()) return;

	//check the user entered the extension, otherwise add it to the filename
	if(fn[int(fn.length()-4)]!='.') fn += ".tfn";

	if (fn.isEmpty()) return;

	ofstream fout;												
	fout.open(fn.latin1());

	fout<<"// Transfer Function File"<<endl<<endl;
	fout<<"//=========================================================="<<endl;
	fout<<"// Threshold levels:"<<endl;
	fout<<"// min, max"<<endl;
	fout<<"// int, int"<<endl;
	fout<<"//=========================================================="<<endl;
	fout<<"LUMINANCE_THRESHOLD_LEVELS"<<endl;
	fout<<histogrampixmap_luminance->minthr<<"	"<<histogrampixmap_luminance->maxthr<<endl;
	fout<<"// Trasnfer Functions"<<endl;
	fout<<"// size, cp.x, cp.y, cp.z ....."<<endl;
	fout<<"// int, float, float, float ...."<<endl;
	fout<<"//=========================================================="<<endl;

	fout<<"LUMINANCE_TF_CONTROL_POINTS"<<endl;
	fout<<histogrampixmap_luminance->funceditor.bs.control_points.size()<<endl;
	for(i=0; i<histogrampixmap_luminance->funceditor.bs.control_points.size(); i++)
	{
		fout<<histogrampixmap_luminance->funceditor.bs.control_points[i].x<<"	"<<histogrampixmap_luminance->funceditor.bs.control_points[i].y<<"	"<<histogrampixmap_luminance->funceditor.bs.control_points[i].z<<endl;
	}
	fout<<endl;
	
	fout<<"// Transfer Function File"<<endl<<endl;
	fout<<"//=========================================================="<<endl;
	fout<<"// Threshold levels:"<<endl;
	fout<<"// min, max"<<endl;
	fout<<"// int, int"<<endl;
	fout<<"//=========================================================="<<endl;
	fout<<"OPACTIY_THRESHOLD_LEVELS"<<endl;
	fout<<histogrampixmap_opacity->minthr<<"	"<<histogrampixmap_opacity->maxthr<<endl;
	fout<<"// Trasnfer Functions"<<endl;
	fout<<"// size, cp.x, cp.y, cp.z ....."<<endl;
	fout<<"// int, float, float, float ...."<<endl;
	fout<<"//=========================================================="<<endl;

	fout<<"OPACITY_TF_CONTROL_POINTS"<<endl;
	fout<<histogrampixmap_opacity->funceditor.bs.control_points.size()<<endl;
	for(i=0; i<histogrampixmap_opacity->funceditor.bs.control_points.size(); i++)
	{
		fout<<histogrampixmap_opacity->funceditor.bs.control_points[i].x<<"	"<<histogrampixmap_opacity->funceditor.bs.control_points[i].y<<"	"<<histogrampixmap_opacity->funceditor.bs.control_points[i].z<<endl;
	}
	fout<<endl;

	fout.close();

	delete fd;
//	histogrampixmap_luminance->save();
}
void QtTransferFuncWidget::rebuild(void)
{
}
void QtTransferFuncWidget::reset(void)
{
	resetall_button_action();
}
void QtTransferFuncWidget::resetcurrent_button_action(void)
{
	if(channel_mode==0)
	{
		int temp = histogrampixmap_luminance->remaponfly;
		histogrampixmap_luminance->remaponfly = -1;

		histogrampixmap_luminance->funceditor.reset();	
		histogrampixmap_luminance->minthr = threshmin;
		histogrampixmap_luminance->maxthr = threshmax;
		
		threshmin_action(0);
		threshmin_slider->setValue(0);
		threshmax_action(65535);
		threshmax_slider->setValue(65535);
		
		histogrampixmap_luminance->contrast = 100;
		histogrampixmap_luminance->brightness = 100;
		contrast_slider->setValue(100);
		brightness_slider->setValue(100);	
		emit tfunc_brightcontrast_signal(1, histogrampixmap_luminance->brightness, histogrampixmap_luminance->contrast);

		histogrampixmap_luminance->remaponfly = temp;
		histogrampixmap_luminance->tfunc_remap16bit();
	}
	else
	{
		int temp = histogrampixmap_opacity->remaponfly;

		histogrampixmap_opacity->remaponfly = -1;

		histogrampixmap_opacity->funceditor.reset();	
		histogrampixmap_opacity->minthr = threshmin;
		histogrampixmap_opacity->maxthr = threshmax;
		
		threshmin_action(0);
		threshmin_slider->setValue(0);
		threshmax_action(65535);
		threshmax_slider->setValue(65535);
		
		histogrampixmap_opacity->contrast = 100;
		histogrampixmap_opacity->brightness = 100;
		contrast_slider->setValue(100);
		brightness_slider->setValue(100);	
		emit tfunc_brightcontrast_signal(2, histogrampixmap_opacity->brightness, histogrampixmap_opacity->contrast);

		histogrampixmap_opacity->remaponfly = temp;
		histogrampixmap_opacity->tfunc_remap16bit();
	}
}
void QtTransferFuncWidget::resetall_button_action(void)
{
	int temp = histogrampixmap_luminance->remaponfly;
	histogrampixmap_luminance->remaponfly = -1;
	histogrampixmap_opacity->remaponfly = -1;

	histogrampixmap_luminance->funceditor.reset();	
	histogrampixmap_luminance->minthr = 0;
	histogrampixmap_luminance->maxthr = 65535;

	histogrampixmap_opacity->funceditor.reset();	
	histogrampixmap_opacity->minthr = 0;
	histogrampixmap_opacity->maxthr = 65535;
	
	threshmin_action(0);
	threshmin_slider->setValue(0);
	threshmax_action(65535);
	threshmax_slider->setValue(65535);

	histogrampixmap_luminance->contrast = 100;
	histogrampixmap_luminance->brightness = 100;
	contrast_slider->setValue(100);
	brightness_slider->setValue(100);	
	emit tfunc_brightcontrast_signal(1, histogrampixmap_luminance->brightness, histogrampixmap_luminance->contrast);

	histogrampixmap_opacity->contrast = 100;
	histogrampixmap_opacity->brightness = 100;
	contrast_slider->setValue(100);
	brightness_slider->setValue(100);	
	emit tfunc_brightcontrast_signal(2, histogrampixmap_opacity->brightness, histogrampixmap_opacity->contrast);
	
	histogrampixmap_opacity->remaponfly = temp;
	histogrampixmap_luminance->remaponfly = temp;

	histogrampixmap_luminance->tfunc_remap16bit();
}
void QtTransferFuncWidget::apply_button_action(void)
{
	emit tfunc_remap16bit_signal(histogrampixmap_luminance->minthr, histogrampixmap_luminance->maxthr, histogrampixmap_luminance->funceditor.bs.curve_points,histogrampixmap_opacity->minthr, histogrampixmap_opacity->maxthr, histogrampixmap_opacity->funceditor.bs.curve_points);
}
void QtTransferFuncWidget::transferfun_action(void)
{
 //       rebuild();
	histogrampixmap_luminance->tfunc_enable = !histogrampixmap_luminance->tfunc_enable;
	histogrampixmap_opacity->tfunc_enable = !histogrampixmap_opacity->tfunc_enable;
	
	histogrampixmap_luminance->update();
	histogrampixmap_opacity->update();
	
	update();
}
void QtTransferFuncWidget::histoview_action(void)
{
//	rebuild();
	histogrampixmap_luminance->hist_enable = !histogrampixmap_luminance->hist_enable;
	histogrampixmap_opacity->hist_enable = !histogrampixmap_opacity->hist_enable;

	histogrampixmap_luminance->update();
	histogrampixmap_opacity->update();

	update();
}
void QtTransferFuncWidget::cumfreview_action(void)
{
//	rebuild();
	histogrampixmap_luminance->cumfreq_enable = !histogrampixmap_luminance->cumfreq_enable;
	histogrampixmap_opacity->cumfreq_enable = !histogrampixmap_opacity->cumfreq_enable;

	histogrampixmap_luminance->update();
	histogrampixmap_opacity->update();

	update();
}

void QtTransferFuncWidget::remaponflycb_action(void)
{
 	histogrampixmap_opacity->remaponfly = !histogrampixmap_opacity->remaponfly;
 	histogrampixmap_luminance->remaponfly = !histogrampixmap_luminance->remaponfly;
	
	if(histogrampixmap_luminance->remaponfly==1 || histogrampixmap_opacity->remaponfly==1) apply_grp->setEnabled(false);
	else apply_grp->setEnabled(true);
}
void QtTransferFuncWidget::logscalehistcb_action(void)
{
 	histogrampixmap_luminance->logscale = !histogrampixmap_luminance->logscale;
 	histogrampixmap_opacity->logscale = !histogrampixmap_opacity->logscale;
	update();
}

void QtTransferFuncWidget::tfunc_remapslot(void)
{
	if(channel_mode==0)
	{	
		if(histogrampixmap_luminance->remaponfly==1) 	emit tfunc_remap16bit_signal(histogrampixmap_luminance->minthr, histogrampixmap_luminance->maxthr, histogrampixmap_luminance->funceditor.bs.curve_points,histogrampixmap_opacity->minthr, histogrampixmap_opacity->maxthr, histogrampixmap_opacity->funceditor.bs.curve_points);


	}
	else
	{	
		if(histogrampixmap_opacity->remaponfly==1) 	emit tfunc_remap16bit_signal(histogrampixmap_luminance->minthr, histogrampixmap_luminance->maxthr, histogrampixmap_luminance->funceditor.bs.curve_points,histogrampixmap_opacity->minthr, histogrampixmap_opacity->maxthr, histogrampixmap_opacity->funceditor.bs.curve_points);

	}
}
void QtTransferFuncWidget::threshmin_action(int j)
{	
//	printf("QtTransferFuncWidget::threshmin_action\n");

	//check slider bounds
	if(j>threshmax)
	{
		j = threshmax;
		threshmin_slider->setValue(j);
	}
	threshmin = j;
	
	//update our slider label
	QString s;
	QString text;
	text = "Min. Threshold:	";
	s.setNum(j);
	text += s;
	threshmin_lb->setText (text);

	if(channel_mode==0)
	{
		histogrampixmap_luminance->minthr = threshmin;
		histogrampixmap_luminance->maxthr = threshmax;
		
		if(histogrampixmap_luminance->remaponfly==1) 	emit tfunc_remap16bit_signal(histogrampixmap_luminance->minthr, histogrampixmap_luminance->maxthr, histogrampixmap_luminance->funceditor.bs.curve_points,histogrampixmap_opacity->minthr, histogrampixmap_opacity->maxthr, histogrampixmap_opacity->funceditor.bs.curve_points);


	}
	else
	{
		histogrampixmap_opacity->minthr = threshmin;
		histogrampixmap_opacity->maxthr = threshmax;
		
		if(histogrampixmap_opacity->remaponfly==1) 	emit tfunc_remap16bit_signal(histogrampixmap_luminance->minthr, histogrampixmap_luminance->maxthr, histogrampixmap_luminance->funceditor.bs.curve_points,histogrampixmap_opacity->minthr, histogrampixmap_opacity->maxthr, histogrampixmap_opacity->funceditor.bs.curve_points);

	}

}
void QtTransferFuncWidget::threshmax_action(int j)
{
//	printf("QtTransferFuncWidget::threshmax_action\n");

	//check slider bounds
	if(j<threshmin)
	{
		j = threshmin;
		threshmax_slider->setValue(j);
	}
	threshmax = j;

	//update our slider label
	QString s;
	QString text;
	text = "Max Threshold:	";
	s.setNum(j);
	text += s;
	threshmax_lb->setText (text);
	
	if(channel_mode==0)
	{
		histogrampixmap_luminance->minthr = threshmin;
		histogrampixmap_luminance->maxthr = threshmax;
		histogrampixmap_luminance->tfunc_remap16bit();
	}
	else
	{
		histogrampixmap_opacity->minthr = threshmin;
		histogrampixmap_opacity->maxthr = threshmax;
		histogrampixmap_opacity->tfunc_remap16bit();	
	}
}
void QtTransferFuncWidget::tfunc_16bit_histogram_slot(double* histo16, double* histo8)
{
	printf("QtTransferFuncWidget::tfunc_16bit_histogram_slot\n");

	histogrampixmap_luminance->sethistogramdata(histo16);
	histogrampixmap_opacity->sethistogramdata(histo16);
	update();
}

void QtTransferFuncWidget::channelmode_button_event(int mode)
{	
	threshmin = 0;
	threshmax = 65535;
	channel_mode = mode;

	int temp = histogrampixmap_luminance->remaponfly;
	histogrampixmap_luminance->remaponfly = -1;
	histogrampixmap_opacity->remaponfly = -1;

	if(mode==0)
	{
		histogrampixmap_luminance->show();
		histogrampixmap_opacity->hide();
		
		threshmin_action(histogrampixmap_luminance->minthr);
		threshmin_slider->setValue(histogrampixmap_luminance->minthr);

		threshmax_action(histogrampixmap_luminance->maxthr);
		threshmax_slider->setValue(histogrampixmap_luminance->maxthr);

		contrast_slider->setValue(200-histogrampixmap_luminance->contrast);
		brightness_slider->setValue(200-histogrampixmap_luminance->brightness);	
		
		update();
	}
	else
	{
		histogrampixmap_luminance->hide();
		histogrampixmap_opacity->show();
	
		threshmin_action(histogrampixmap_opacity->minthr);
		threshmin_slider->setValue(histogrampixmap_opacity->minthr);

		threshmax_action(histogrampixmap_opacity->maxthr);
		threshmax_slider->setValue(histogrampixmap_opacity->maxthr);

		contrast_slider->setValue(200-histogrampixmap_opacity->contrast);
		brightness_slider->setValue(200-histogrampixmap_opacity->brightness);	

		update();
	}

	histogrampixmap_luminance->remaponfly = temp;
	histogrampixmap_opacity->remaponfly = temp;	
}
void QtTransferFuncWidget::brightness_action(int i)
{
	//update our slider label
	brightness = 200-i;
	QString text;
	text.setNum(brightness);
	text += "%";
	brightness_lb->setText (text);
	
	if(channel_mode==0)
	{
		histogrampixmap_luminance->brightness = brightness;
		emit tfunc_brightcontrast_signal(1, histogrampixmap_luminance->brightness, histogrampixmap_luminance->contrast);
	}
	else
	{	
		histogrampixmap_opacity->brightness = brightness;
		emit tfunc_brightcontrast_signal(2, histogrampixmap_opacity->brightness, histogrampixmap_opacity->contrast);
	}
}
void QtTransferFuncWidget::contrast_action(int i)
{
	//update our slider label
	contrast = 200-i;
	QString text;
	text.setNum(contrast);
	text += "%";
	contrast_lb->setText (text);
	
	if(channel_mode==0)
	{
		histogrampixmap_luminance->contrast = contrast;
		emit tfunc_brightcontrast_signal(1, histogrampixmap_luminance->brightness, histogrampixmap_luminance->contrast);
	}
	else
	{	
		histogrampixmap_opacity->contrast = contrast;
		emit tfunc_brightcontrast_signal(2, histogrampixmap_opacity->brightness, histogrampixmap_opacity->contrast);
	}
}
void QtTransferFuncWidget::tfunc16_setcurrentworkingdir_slot(QString dn)
{
	currentworkingdir = dn;
}
void QtTransferFuncWidget::invert_button_action(void)
{
	if(invert_button->isOn())
	{
		if(channel_mode==0)	emit tfunc_invert_signal(1, 3);
		else	emit tfunc_invert_signal(2, 3);
	}
	else
	{
		if(channel_mode==0)	emit tfunc_invert_signal(1, 3);
		else	emit tfunc_invert_signal(2, 3);
	}
}