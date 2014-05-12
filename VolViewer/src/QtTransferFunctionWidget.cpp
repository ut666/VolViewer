/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "QtTransferFunctionWidget.h"
//Added by qt3to4:
#include <Q3Frame>
#include <QPixmap>
#include <QLabel>

QtTransferFunctionWidget::QtTransferFunctionWidget(QWidget* parent, const char* name )
: QWidget( parent, name )
{
/*	QSizePolicy sizepolicy;
	sizepolicy.setHeightForWidth(TRUE);
	setSizePolicy(sizepolicy);
	*/
	transfer_mode = 1;
	channel_selected=0;
	channelmode_selected=0;

	threshmin = 0;
	threshmax = 255;
	brightness = 100;
	contrast = 100;
	updating_labels=0;
	setAcceptDrops(true);
}

QtTransferFunctionWidget::~QtTransferFunctionWidget()
{
	hist=NULL;
}
void QtTransferFunctionWidget::dragndrop_slot(QString sFileName)
{	
	if (!sFileName.isEmpty()) 
	{
		if(sFileName.contains(".tfn", true) || sFileName.contains(".TFN", true)) 
		{
			setCursor(QCursor(Qt::waitCursor));
			hist->loadfile(sFileName);
			setCursor(QCursor(Qt::waitCursor));

			emit hist->reload_transfer_function_parameters();
			hist->emit_tfunc_reloadall();

			histcb->setChecked(TRUE);
			tfuncb->setChecked(TRUE);
			cumfcb->setChecked(FALSE);	
			channel_button_event(channel_selected);	
			setCursor(QCursor(Qt::arrowCursor));
		}
	}
}
void QtTransferFunctionWidget::dragEnterEvent(QDragEnterEvent* event)
{
	event->accept(Q3TextDrag::canDecode(event));
}
void QtTransferFunctionWidget::dropEvent(QDropEvent* event)
{
	Q3StrList URIs;

	if ( Q3UriDrag::decode(event, URIs) ) 
	{
		QString text = Q3UriDrag::uriToLocalFile(URIs.getFirst());
		dragndrop_slot(text);
	}
}
void QtTransferFunctionWidget::create(void)
{    
	hist = new QtHistogram(this, "histogram", 256);

	opensave_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "File:", this );
    opensave_grp->setExclusive( FALSE );
	opensave_grp->setAlignment(Qt::AlignLeft);

		QPushButton* open = new QPushButton(QIcon(QPixmap("icons/fileopen.xpm")), "Open", opensave_grp, "open");
		//open->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
		connect( open, SIGNAL( clicked () ), this, SLOT( open_event() ) );
		QPushButton* save = new QPushButton(QIcon(QPixmap("icons/filesave.xpm")), "Save", opensave_grp, "save");
		//save->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
		connect( save, SIGNAL( clicked () ), this, SLOT( save_event() ) );

	toggle_grp = new Q3ButtonGroup( 4, Qt::Horizontal, "Channels:", this);
    toggle_grp->setExclusive( TRUE );
	toggle_grp->setAlignment(Qt::AlignHCenter);
	connect( toggle_grp, SIGNAL( clicked (int) ), this, SLOT( channel_button_event(int) ) );

		/*Q3ButtonGroup* itoggle_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", toggle_grp);
		itoggle_grp->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
		itoggle_grp->setLineWidth(0);
		itoggle_grp->setMidLineWidth(0);
		itoggle_grp->setMargin(0);
		itoggle_grp->setInsideMargin(0);
		itoggle_grp->setInsideSpacing(0);*/

			intensity_pb = new QPushButton("ALL", toggle_grp);
			intensity_pb->setToggleButton(TRUE);
			intensity_pb->setOn(TRUE);
			intensity_pb->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
			intensity_pb->setMaximumWidth(42);

		/*Q3ButtonGroup* rgbtoggle_grp = new Q3ButtonGroup( 3, Qt::Horizontal, "", toggle_grp);
		rgbtoggle_grp->setFrameStyle(QFrame::NoFrame|QFrame::Plain);
		rgbtoggle_grp->setLineWidth(0);
		rgbtoggle_grp->setMidLineWidth(0);
		rgbtoggle_grp->setMargin(0);
		rgbtoggle_grp->setInsideMargin(0);
		rgbtoggle_grp->setInsideSpacing(0);*/

			red_pb = new QPushButton("R", toggle_grp);
			red_pb->setToggleButton(TRUE);
			red_pb->setOn(FALSE);
			red_pb->setPaletteForegroundColor(Qt::red); 
			red_pb->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
			red_pb->setMaximumWidth(42);

			green_pb = new QPushButton("G", toggle_grp);
			green_pb->setToggleButton(TRUE);
			green_pb->setOn(FALSE);
			green_pb->setPaletteForegroundColor(QColor(0,128,0)); 
			green_pb->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
			green_pb->setMaximumWidth(42);

			blue_pb = new QPushButton("B", toggle_grp);
			blue_pb->setToggleButton(TRUE);
			blue_pb->setOn(FALSE);
			blue_pb->setPaletteForegroundColor(Qt::blue); 
			blue_pb->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
			blue_pb->setMaximumWidth(42);

	channelmode_grp = new Q3ButtonGroup( 2, Qt::Vertical, "Channel Mode:", this);
    channelmode_grp->setExclusive( TRUE );
	channelmode_grp->setAlignment(Qt::AlignLeft);
	connect( channelmode_grp, SIGNAL( clicked (int) ), this, SLOT( channelmode_button_event(int) ) );
		
		luminance_pb = new QPushButton("Intensity", channelmode_grp);
		luminance_pb->setToggleButton(TRUE);
		luminance_pb->setOn(FALSE);

		opacity_pb = new QPushButton("Opacity", channelmode_grp);
		opacity_pb->setToggleButton(TRUE);
		opacity_pb->setOn(TRUE);

	reset_grp = new Q3ButtonGroup( 2, Qt::Vertical, "Reset:", this );

		QPushButton* resetcurr_button = new QPushButton("Current", reset_grp, "restcurr");
		resetcurr_button->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum, false));
		connect( resetcurr_button, SIGNAL( clicked() ), this, SLOT(resetcurr_button_action() ) );

		QPushButton* resetall_button = new QPushButton("All", reset_grp, "restcurr");
		resetall_button->setSizePolicy(QSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum, false));
		connect( resetall_button, SIGNAL( clicked() ), this, SLOT(resetall_button_action() ) );

		//button group to toggle on/off viewing on histrogram
		// ( HISTOGRAM, CUMMALATATIVE FREQUENCY, TRANSFER FUNCTION)
		view_grp = new Q3ButtonGroup( 3, Qt::Horizontal, "View:", this);
			
			histcb = new QCheckBox("Histogram",	   view_grp );
			cumfcb = new QCheckBox("CumFrequency", view_grp );
			tfuncb = new QCheckBox("TransferFunc", view_grp );
			histcb->setChecked(TRUE);
			tfuncb->setChecked(TRUE);
			cumfcb->setChecked(FALSE);
			connect( histcb,	 SIGNAL( clicked() ), this, SLOT( histoview_action() ) );
			connect( cumfcb,	 SIGNAL( clicked() ), this, SLOT( cumfreview_action() ) );
			connect( tfuncb,	 SIGNAL( clicked() ), this, SLOT( transferfun_action() ) );
		view_grp->hide();

		update_grp = new Q3ButtonGroup( 2, Qt::Horizontal, "Histogram:", this);
			
			updatehistcb = new QCheckBox("Update",	   update_grp);
			updatehistcb->setChecked(FALSE);
			connect( updatehistcb,	 SIGNAL( clicked() ), this, SLOT( updatehisto_action() ) );
			updatehisto_action();

			logscalehistcb = new QCheckBox("Log Scale",	   update_grp);
			logscalehistcb->setChecked(TRUE);
			connect( logscalehistcb,	 SIGNAL( clicked() ), this, SLOT( logscalehistcb_action() ) );
			logscalehistcb_action();

		mapping_grp = new Q3ButtonGroup( 2, Qt::Horizontal, "Mapping:", this);
		mapping_grp->setExclusive( true );

			linearmapcb = new QCheckBox("Linear",	   mapping_grp);
			linearmapcb->setChecked(FALSE);
			connect( linearmapcb,	 SIGNAL( clicked() ), this, SLOT( linearmapcb_action() ) );
			linearmapcb_action();

			bsplinecb = new QCheckBox("B-Spline",	   mapping_grp);
			bsplinecb->setChecked(TRUE);
			connect( bsplinecb,	 SIGNAL( clicked() ), this, SLOT( bsplinecb_action() ) );
			bsplinecb_action();


	//threshold slider group
	threshold_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
    threshold_grp->setExclusive( FALSE );

		threshmin_lb = new QLabel( "Min. Threshold:	0", threshold_grp );
		//our min threshold slider
		threshmin_slider = new QSlider ( 0, 255, 1, 0, Qt::Horizontal, threshold_grp, "threshmin_slider" );
		threshmin_slider->setTickmarks( QSlider::Below );
		threshmin_slider->setTracking(FALSE);
		threshmin_slider->setTickInterval(8);
		connect( threshmin_slider, SIGNAL(valueChanged(int)), this, SLOT(threshmin_action(int)) );
//		threshmin = 0;
//		threshmin_slider->setValue(0);

		threshmax_lb = new QLabel( "Max. Threshold:	255", threshold_grp );
		//our max threshold slicer
		threshmax_slider = new QSlider ( 0, 255, 1, 255, Qt::Horizontal, threshold_grp, "threshmax_slider" );
		threshmax_slider->setTickmarks( QSlider::Below );
		threshmax_slider->setTracking(FALSE);
		threshmax_slider->setTickInterval(8);
		connect( threshmax_slider, SIGNAL(valueChanged(int)), this, SLOT(threshmax_action(int)) );
//		threshmax = 255;
//		threshmax_slider->setValue(0);

	//brightness & contrast slider group
	briconslider_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
	briconslider_grp->setAlignment(Qt::AlignLeft);
	briconslider_grp->setExclusive( FALSE );

			Q3ButtonGroup* bri_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", briconslider_grp );
			bri_grp->setExclusive( FALSE );

				QLabel* brightnesstext_lb = new QLabel( "Brightness", bri_grp );
				brightness = 100;
				//our min threshold slider
				brightness_slider = new QSlider ( 0, 200, 1, 100, Qt::Vertical, bri_grp, "brightness_slider" );
				brightness_slider->setInvertedAppearance(true);
				brightness_slider->setInvertedControls(true);
				brightness_slider->setTickmarks( QSlider::TicksRight );
				brightness_slider->setTracking(FALSE);
				brightness_slider->setTickInterval(10);
				connect( brightness_slider, SIGNAL(valueChanged(int)), this, SLOT(brightness_action(int)) );
				brightness_lb = new QLabel( "100%", bri_grp );
			//bri_grp->hide();
	
			Q3ButtonGroup* con_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", briconslider_grp );
			con_grp->setExclusive( FALSE );

				QLabel* contrasttext_lb = new QLabel( "Contrast", con_grp );
				contrast = 100;
				//our max threshold slicer
				contrast_slider = new QSlider ( 0, 200, 1, 100, Qt::Vertical, con_grp, "contrast_slider" );
				contrast_slider->setInvertedAppearance(true);
				contrast_slider->setTickmarks( QSlider::TicksRight );
				contrast_slider->setInvertedControls(true);
				contrast_slider->setTracking(FALSE);
				contrast_slider->setTickInterval(10);
				connect( contrast_slider, SIGNAL(valueChanged(int)), this, SLOT(contrast_action(int)) );
				contrast_lb = new QLabel( "100%", con_grp );
			//con_grp->hide();

			/*
			Q3ButtonGroup *curvepoints_grid = new Q3ButtonGroup( 1, Qt::Vertical, "Control Points:", this );
			curvepoints_grid->setExclusive(TRUE);

				QPushButton* movepoint_button = new QPushButton("<->", curvepoints_grid, "restcurr");
				movepoint_button->resize(16,16);
				movepoint_button->setToggleButton(TRUE);
				movepoint_button->setOn(TRUE);
				connect( movepoint_button, SIGNAL( clicked() ), this, SLOT(resetcurr_button_action() ) );

				QPushButton* addpoint_button = new QPushButton("+", curvepoints_grid, "restcurr");
				addpoint_button->resize(16,16);
				addpoint_button->setToggleButton(TRUE);
				addpoint_button->setOn(FALSE);
				connect( addpoint_button, SIGNAL( clicked() ), this, SLOT(resetcurr_button_action() ) );

				QPushButton* deletepoint_button = new QPushButton("-", curvepoints_grid, "restcurr");
				deletepoint_button->resize(16,16);
				deletepoint_button->setToggleButton(TRUE);
				deletepoint_button->setOn(FALSE);
				connect( deletepoint_button, SIGNAL( clicked() ), this, SLOT(resetcurr_button_action() ) );
*/
	Q3BoxLayout* histtoplayout_centre = new Q3BoxLayout ( 0, QBoxLayout::LeftToRight, 0, 5, "histtoplayout_centre");
	histtoplayout_centre->addWidget(view_grp, 1);
	histtoplayout_centre->addWidget(update_grp, 0, Qt::AlignTop);
	histtoplayout_centre->addWidget(mapping_grp, 0, Qt::AlignTop);
	
	histlayout_centre = new Q3BoxLayout ( 0, QBoxLayout::TopToBottom, 0, 5, "histlayout_centre");
	histlayout_centre->addLayout(histtoplayout_centre);
	histlayout_centre->addWidget(hist, 10);
	histlayout_centre->addWidget(threshold_grp, 1);
	
	//briconslider_grp->hide();
	vboxlayout_centre = new Q3BoxLayout ( 0, QBoxLayout::LeftToRight, 0, 5, "vboxlayout_centre");
	vboxlayout_centre->addLayout(histlayout_centre);
	vboxlayout_centre->addWidget(briconslider_grp);
		
	vboxlayoutRIGHT = new Q3BoxLayout ( 0, QBoxLayout::TopToBottom, 0, 5, "vboxlayout");
	vboxlayoutRIGHT->addLayout(vboxlayout_centre);
	
	vboxlayoutLEFT = new Q3BoxLayout ( 0, QBoxLayout::TopToBottom, 0, 5, "vboxlayout");
	vboxlayoutLEFT->setAlignment(Qt::AlignTop);
	vboxlayoutLEFT->addWidget(opensave_grp, 0, Qt::AlignTop);
	vboxlayoutLEFT->addWidget(toggle_grp, 0, Qt::AlignTop);
	vboxlayoutLEFT->addWidget(channelmode_grp, 0, Qt::AlignTop);	
	vboxlayoutLEFT->addWidget(reset_grp, 0, Qt::AlignTop);
	
	topmost= new Q3BoxLayout ( this, QBoxLayout::LeftToRight, 5, 5, "topmost");
	topmost->addLayout(vboxlayoutLEFT);
	topmost->addLayout(vboxlayoutRIGHT);

	//default as I/RGB and opacity
	channel_button_event(0);
	channelmode_button_event(1);
	
	//our histogram widget
	//hist->draw_transfer();
}
void QtTransferFunctionWidget::linearmapcb_action(void)
{
	if(linearmapcb->checkState()==Qt::Checked)
	{
		hist->luminance.functioneditor.bs.linear=true;
		hist->alpha.functioneditor.bs.linear=true;
		hist->redluminance.functioneditor.bs.linear=true;
		hist->redalpha.functioneditor.bs.linear=true;
		hist->greenluminance.functioneditor.bs.linear=true;
		hist->greenalpha.functioneditor.bs.linear=true;
		hist->blueluminance.functioneditor.bs.linear=true;
		hist->bluealpha.functioneditor.bs.linear=true;
	}
	else
	{
	}

	hist->luminance.functioneditor.bs.compute();
	hist->alpha.functioneditor.bs.compute();
	hist->redluminance.functioneditor.bs.compute();
	hist->redalpha.functioneditor.bs.compute();
	hist->greenluminance.functioneditor.bs.compute();
	hist->greenalpha.functioneditor.bs.compute();
	hist->blueluminance.functioneditor.bs.compute();
	hist->bluealpha.functioneditor.bs.compute();

	hist->update();
	hist->emit_tfunc_current();
}
void QtTransferFunctionWidget::bsplinecb_action(void)
{
	if(bsplinecb->checkState()==Qt::Checked)
	{
		hist->luminance.functioneditor.bs.linear=false;
		hist->alpha.functioneditor.bs.linear=false;
		hist->redluminance.functioneditor.bs.linear=false;
		hist->redalpha.functioneditor.bs.linear=false;
		hist->greenluminance.functioneditor.bs.linear=false;
		hist->greenalpha.functioneditor.bs.linear=false;
		hist->blueluminance.functioneditor.bs.linear=false;
		hist->bluealpha.functioneditor.bs.linear=false;
	}
	else
	{
	}

	hist->luminance.functioneditor.bs.compute();
	hist->alpha.functioneditor.bs.compute();
	hist->redluminance.functioneditor.bs.compute();
	hist->redalpha.functioneditor.bs.compute();
	hist->greenluminance.functioneditor.bs.compute();
	hist->greenalpha.functioneditor.bs.compute();
	hist->blueluminance.functioneditor.bs.compute();
	hist->bluealpha.functioneditor.bs.compute();

	hist->update();
	hist->emit_tfunc_current();
}

void QtTransferFunctionWidget::resetcurr_button_action(void)
{
	setCursor(QCursor(Qt::waitCursor));
 
	printf("resetcurr_button_action\n");

	if(transfer_mode==1)
	{
		hist->reset_l();
	}
	if(transfer_mode==2)
	{
		hist->reset_la();
	}
	if(transfer_mode==3)
	{
		hist->reset_r();
	}
	if(transfer_mode==4)
	{
		hist->reset_g();
	}
	if(transfer_mode==5)
	{
		hist->reset_b();
	}
	if(transfer_mode==6)
	{
		hist->reset_ra();
	}
	if(transfer_mode==7)
	{
		hist->reset_ga();
	}
	if(transfer_mode==8)
	{
		hist->reset_ba();
	}

	hist->emit_tfunc_current();

	histcb->setChecked(TRUE);
	tfuncb->setChecked(TRUE);
	cumfcb->setChecked(FALSE);
	linearmapcb->setChecked(FALSE);
	bsplinecb->setChecked(TRUE);
	channel_button_event(channel_selected);	
	setCursor(QCursor(Qt::arrowCursor));
}
void QtTransferFunctionWidget::resetall_button_action(void)
{
	setCursor(QCursor(Qt::waitCursor));

	hist->reset();

	hist->emit_tfunc_all();

	histcb->setChecked(TRUE);
	tfuncb->setChecked(TRUE);
	cumfcb->setChecked(FALSE);
	linearmapcb->setChecked(FALSE);
	bsplinecb->setChecked(TRUE);
	channel_button_event(channel_selected);	
	setCursor(QCursor(Qt::arrowCursor));
}
void QtTransferFunctionWidget::open_event(void)
{
	setCursor(QCursor(Qt::waitCursor));
	hist->load();
	setCursor(QCursor(Qt::waitCursor));

	emit hist->reload_transfer_function_parameters();
	hist->emit_tfunc_reloadall();

//	hist->draw_transfer();
//	hist->emit_redraw();

	histcb->setChecked(TRUE);
	tfuncb->setChecked(TRUE);
	cumfcb->setChecked(FALSE);	
	linearmapcb->setChecked(FALSE);
	bsplinecb->setChecked(TRUE);
	channel_button_event(channel_selected);	
	setCursor(QCursor(Qt::arrowCursor));
}
void QtTransferFunctionWidget::save_event(void)
{
	hist->save();

	setCursor(QCursor(Qt::arrowCursor));
}
void QtTransferFunctionWidget::rebuild(void)
{
	//printf("BOH\n");
	//update our histogram
	hist->draw_transfer();
	//hist->update();
}
void QtTransferFunctionWidget::reset(void)
{
	histcb->setChecked(TRUE);
	tfuncb->setChecked(TRUE);
	cumfcb->setChecked(FALSE);

	channel_button_event(0);

	hist->reset();
	hist->set_transfer_mode(transfer_mode);
	
	rebuild();
}
void QtTransferFunctionWidget::channelmode_button_event(int id)
{
	channelmode_selected=id;
	updating_labels=1;

	if(channel_selected==0)
	{			
		if(channelmode_selected==0)
		{
			transfer_mode=1;
			update_slider_labels(hist->luminance.threshold_min, hist->luminance.threshold_max, hist->luminance.brightness, hist->luminance.contrast);
		}
		else if(channelmode_selected==1)
		{
			transfer_mode=2;
			update_slider_labels(hist->alpha.threshold_min, hist->alpha.threshold_max, hist->alpha.brightness, hist->alpha.contrast);
		}
	}
	else if(channel_selected==1)
	{	
		if(channelmode_selected==0)
		{
			transfer_mode=3;
			update_slider_labels(hist->redluminance.threshold_min, hist->redluminance.threshold_max, hist->redluminance.brightness, hist->redluminance.contrast);
		}
		else if(channelmode_selected==1)
		{
			transfer_mode=6;
			update_slider_labels(hist->redalpha.threshold_min, hist->redalpha.threshold_max, hist->redalpha.brightness, hist->redalpha.contrast);
		}	
	}
	else if(channel_selected==2)
	{	
		if(channelmode_selected==0)
		{
			transfer_mode=4;
			update_slider_labels(hist->greenluminance.threshold_min, hist->greenluminance.threshold_max, hist->greenluminance.brightness, hist->greenluminance.contrast);
		}
		else if(channelmode_selected==1)
		{
			transfer_mode=7;
			update_slider_labels(hist->greenalpha.threshold_min, hist->greenalpha.threshold_max, hist->greenalpha.brightness, hist->greenalpha.contrast);
		}	
	}
	else if(channel_selected==3)
	{
		if(channelmode_selected==0)
		{
			transfer_mode=5;
			update_slider_labels(hist->blueluminance.threshold_min, hist->blueluminance.threshold_max, hist->blueluminance.brightness, hist->blueluminance.contrast);
		}
		else if(channelmode_selected==1)
		{
			transfer_mode=8;
			update_slider_labels(hist->bluealpha.threshold_min, hist->bluealpha.threshold_max, hist->bluealpha.brightness, hist->bluealpha.contrast);
		}	
	}

	updating_labels=0;
	hist->set_transfer_mode(transfer_mode);
	emit tfunc_rebuild_histogram_signal();
	update();
}
void QtTransferFunctionWidget::channel_button_event(int id)
{
	updating_labels=1;
	channel_selected = id;

	if(channel_selected==0)
	{			
		if(channelmode_selected==0)
		{
			transfer_mode=1;
			update_slider_labels(hist->luminance.threshold_min, hist->luminance.threshold_max, hist->luminance.brightness, hist->luminance.contrast);
		}
		else if(channelmode_selected==1)
		{
			transfer_mode=2;
			update_slider_labels(hist->alpha.threshold_min, hist->alpha.threshold_max, hist->alpha.brightness, hist->alpha.contrast);
		}
	}
	else if(channel_selected==1)
	{	
		if(channelmode_selected==0)
		{
			transfer_mode=3;
			update_slider_labels(hist->redluminance.threshold_min, hist->redluminance.threshold_max, hist->redluminance.brightness, hist->redluminance.contrast);
		}
		else if(channelmode_selected==1)
		{
			transfer_mode=6;
			update_slider_labels(hist->redalpha.threshold_min, hist->redalpha.threshold_max, hist->redalpha.brightness, hist->redalpha.contrast);
		}	
	}
	else if(channel_selected==2)
	{	
		if(channelmode_selected==0)
		{
			transfer_mode=4;
			update_slider_labels(hist->greenluminance.threshold_min, hist->greenluminance.threshold_max, hist->greenluminance.brightness, hist->greenluminance.contrast);
		}
		else if(channelmode_selected==1)
		{
			transfer_mode=7;
			update_slider_labels(hist->greenalpha.threshold_min, hist->greenalpha.threshold_max, hist->greenalpha.brightness, hist->greenalpha.contrast);
		}	
	}
	else if(channel_selected==3)
	{
		if(channelmode_selected==0)
		{
			transfer_mode=5;
			update_slider_labels(hist->blueluminance.threshold_min, hist->blueluminance.threshold_max, hist->blueluminance.brightness, hist->blueluminance.contrast);
		}
		else if(channelmode_selected==1)
		{
			transfer_mode=8;
			update_slider_labels(hist->bluealpha.threshold_min, hist->bluealpha.threshold_max, hist->bluealpha.brightness, hist->bluealpha.contrast);
		}	
	}

	updating_labels=0;
	hist->set_transfer_mode(transfer_mode);
	emit tfunc_rebuild_histogram_signal();
	update();
}

void QtTransferFunctionWidget::transferfun_action(void)
{
	hist->tfunc_enable=!hist->tfunc_enable;
	rebuild();
	update();
}
void QtTransferFunctionWidget::histoview_action(void)
{
	hist->hist_enable=!hist->hist_enable;
	rebuild();
	update();
}
void QtTransferFunctionWidget::cumfreview_action(void)
{
	hist->cumfreq_enable=!hist->cumfreq_enable;
	rebuild();
	update();
}
void QtTransferFunctionWidget::tfunc_histogram_slot(double* histo)
{
	for(int i=0; i<256; i++)
	{
		hist->data[i] = histo[i];			
	}
	
	emit tfunc_rebuild_histogram_signal();
	update();
}
void QtTransferFunctionWidget::histogram_action(int* histo)
{
	for(int i=0; i<256; i++)
	{
		hist->data[i] = histo[i];
	}

	//hist->draw_transfer();
	update();
}
void QtTransferFunctionWidget::emit_transfer_function(void)
{

}

void QtTransferFunctionWidget::update_slider_labels(int tmin, int tmax, int b, int c)
{
	threshmin = tmin;
	threshmax = tmax;
	brightness = b;
	contrast = c;

	QString s;
	QString text;

	threshmin_slider->setValue(tmin);
	text = "Min. Threshold:	";
	s.setNum(tmin);
	text += s;
	threshmin_lb->setText (text);

	threshmax_slider->setValue(tmax);
	text = "Max Threshold:	";
	s.setNum(tmax);
	text += s;
	threshmax_lb->setText (text);

	brightness_slider->setValue(200-b);
	text.setNum(b);
	text += "%";
	brightness_lb->setText (text);

	contrast_slider->setValue(200-c);
	text.setNum(c);
	text += "%";
	contrast_lb->setText (text);
}
void QtTransferFunctionWidget::update_tfuncs(void)
{
		if(channel_selected==0)
		{
			if(channelmode_selected==0)
			{
				hist->redluminance.threshold_min = hist->greenluminance.threshold_min = hist->blueluminance.threshold_min = hist->luminance.threshold_min = threshmin;
				hist->redluminance.threshold_max = hist->greenluminance.threshold_max = hist->blueluminance.threshold_max = hist->luminance.threshold_max = threshmax;
				hist->redluminance.brightness = hist->greenluminance.brightness = hist->blueluminance.brightness = hist->luminance.brightness = brightness;
				hist->redluminance.contrast = hist->greenluminance.contrast = hist->blueluminance.contrast = hist->luminance.contrast = contrast; 
			}
			if(channelmode_selected==1)
			{
				hist->redalpha.threshold_min = hist->greenalpha.threshold_min = hist->bluealpha.threshold_min = hist->alpha.threshold_min = threshmin;
				hist->redalpha.threshold_max = hist->greenalpha.threshold_max = hist->bluealpha.threshold_max = hist->alpha.threshold_max = threshmax;
				hist->redalpha.brightness = hist->greenalpha.brightness = hist->bluealpha.brightness = hist->alpha.brightness = brightness;
				hist->redalpha.contrast = hist->greenalpha.contrast = hist->bluealpha.contrast = hist->alpha.contrast = contrast;
			}
		}
		if(channel_selected==1)
		{
			if(channelmode_selected==0)
			{
				hist->redluminance.threshold_min = threshmin;
				hist->redluminance.threshold_max = threshmax;
				hist->redluminance.brightness = brightness;
				hist->redluminance.contrast = contrast;
			}
			if(channelmode_selected==1)
			{
				hist->redalpha.threshold_min = threshmin;
				hist->redalpha.threshold_max = threshmax;
				hist->redalpha.brightness = brightness;
				hist->redalpha.contrast = contrast;
			}
		}
		if(channel_selected==2)
		{
			if(channelmode_selected==0)
			{
				hist->greenluminance.threshold_min = threshmin;
				hist->greenluminance.threshold_max = threshmax;
				hist->greenluminance.brightness = brightness;
				hist->greenluminance.contrast = contrast;
			}
			if(channelmode_selected==1)	
			{
				hist->greenalpha.threshold_min = threshmin;
				hist->greenalpha.threshold_max = threshmax;
				hist->greenalpha.brightness = brightness;
				hist->greenalpha.contrast = contrast;
			}
		}
		if(channel_selected==3)
		{
			if(channelmode_selected==0)
			{
				hist->blueluminance.threshold_min = threshmin;
				hist->blueluminance.threshold_max = threshmax;
				hist->blueluminance.brightness = brightness;
				hist->blueluminance.contrast = contrast;
			}
			if(channelmode_selected==1)
			{
				hist->bluealpha.threshold_min = threshmin;
				hist->bluealpha.threshold_max = threshmax;
				hist->bluealpha.brightness = brightness;
				hist->bluealpha.contrast = contrast;
			}
		}

	hist->emit_tfunc_current();
}
void QtTransferFunctionWidget::threshmin_action(int j)
{
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

	printf("threshmin: %d\n", threshmin);

	if(updating_labels==0)
	{
		update_tfuncs();
	}
}
void QtTransferFunctionWidget::threshmax_action(int j)
{
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

	printf("threshmax: %d\n", threshmax);

	if(updating_labels==0)
	{
		update_tfuncs();
	}
}
void QtTransferFunctionWidget::brightness_action(int i)
{
	//update our slider label
	brightness = 200-i;
	QString text;
	text.setNum(brightness);
	text += "%";
	brightness_lb->setText (text);

	if(updating_labels==0)
	{
		update_tfuncs();
	}
}
void QtTransferFunctionWidget::contrast_action(int i)
{
	//update our slider label
	contrast = 200-i;
	QString text;
	text.setNum(contrast);
	text += "%";
	contrast_lb->setText (text);

	if(updating_labels==0)
	{
		update_tfuncs();
	}
}


void QtTransferFunctionWidget::updatehisto_action(void)
{
	int toggle = 0;
	
	if(updatehistcb->checkState()==Qt::Checked) toggle = 1;
	else toggle = 0;

	emit tfunc_histoupdate_signal(toggle);
}
void QtTransferFunctionWidget::logscalehistcb_action(void)
{	
	if(logscalehistcb->checkState()==Qt::Checked) hist->logscale = 1;
	else hist->logscale = 0;

	hist->update();
}
void QtTransferFunctionWidget::tfunc_updateAll_slot(TfuncState lum, TfuncState alp, TfuncState redlum, TfuncState redalp, TfuncState greenlum, TfuncState greenalp, TfuncState bluelum, TfuncState bluealpha)
{
	hist->luminance = lum;
	hist->alpha = alp;
	hist->redluminance = redlum;
	hist->redalpha = redalp;
	hist->greenluminance = greenlum;
	hist->greenalpha = greenalp;
	hist->blueluminance = bluelum;
	hist->bluealpha = bluealpha;

	channel_button_event(channel_selected);
	hist->draw_transfer();
}

void QtTransferFunctionWidget::resetbutton_event(void)
{

}
void QtTransferFunctionWidget::applybutton_event(void)
{

}
void QtTransferFunctionWidget::closebutton_event(void)
{

}
