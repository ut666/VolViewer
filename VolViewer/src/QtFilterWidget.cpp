/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "QtFilterWidget.h"
//Added by qt3to4:
#include <QPixmap>


QtFilterWidget::QtFilterWidget(QWidget* parent, const char* name)
: QWidget( parent, name )
{
	filter_dialogue=NULL;
	currentworkingdir="";
	setAcceptDrops(true);
	channels.resize(3);
	channels[0]=true;
	channels[1]=false;
	channels[2]=false;

	axis.resize(3);
	axis[0]=true;
	axis[1]=true;
	axis[2]=true;

}
QtFilterWidget::~QtFilterWidget()
{
}

void QtFilterWidget::rebuild(void)
{
}
void QtFilterWidget::reset(void)
{
}
void QtFilterWidget::dragndrop_slot(QString sFileName)
{	
}
void QtFilterWidget::dragEnterEvent(QDragEnterEvent* event)
{
	event->accept(Q3TextDrag::canDecode(event));
}
void QtFilterWidget::dropEvent(QDropEvent* event)
{
	Q3StrList URIs;

	if ( Q3UriDrag::decode(event, URIs) ) 
	{
		QString text = Q3UriDrag::uriToLocalFile(URIs.getFirst());
		dragndrop_slot(text);
	}
}
void QtFilterWidget::create(void)
{
/*	Q3ButtonGroup *loadsave_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "File:", this );

		load_pb = new QPushButton(QIcon(QPixmap("icons/fileopen.xpm")), "Open", loadsave_grp, "open");
		connect( load_pb, SIGNAL( clicked () ), this, SLOT( load_event() ) );

		save_pb = new QPushButton(QIcon(QPixmap("icons/filesave.xpm")), "Save", loadsave_grp, "save");
		connect( save_pb, SIGNAL( clicked () ), this, SLOT( save_event() ) );
*/

	/*Q3ButtonGroup *reset_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Reset:", this );

		reset_pb = new QPushButton("Reset", reset_grp);
		connect( reset_pb, SIGNAL( pressed() ), this, SLOT( reset_button_event() ) );
*/
	Q3BoxLayout *topSub = new Q3BoxLayout ( 0, Q3BoxLayout::LeftToRight, 0, 5, "vboxlayout");


		Q3BoxLayout *vboxlayoutsubLEFT = new Q3BoxLayout ( 0, Q3BoxLayout::TopToBottom, 0, 5, "vboxlayout");
	
			operators_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Voxels:", this );
				
				QPushButton* collapseI_pb = new QPushButton("Collapse", operators_grp, "collapseI");
				collapseI_pb->setToolTip("Convert colour voxels values to greyscale.");
				connect( collapseI_pb, SIGNAL( clicked () ), this, SLOT( collapse_event() ) );

				QPushButton* invert_pb = new QPushButton("Invert", operators_grp, "invert");
				invert_pb->setToolTip("Invert the voxels intensity values.");
				connect( invert_pb, SIGNAL( clicked () ), this, SLOT( invert_event() ) );

				QPushButton* thresh_pb = new QPushButton("Threshold", operators_grp, "thresh");
				thresh_pb->setToolTip("Threshold the voxels by the current opacity threshold.");
				connect( thresh_pb, SIGNAL( clicked () ), this, SLOT( thresh_event() ) );

				QPushButton* binary_pb = new QPushButton("Binarize", operators_grp, "binary");
				binary_pb->setToolTip("Convert the voxels to a binary mask.");
				connect( binary_pb, SIGNAL( clicked () ), this, SLOT( binary_event() ) );

				QPushButton* count_pb = new QPushButton("Count", operators_grp, "count");
				count_pb->setToolTip("Count the number of voxels in a channel.");
				connect( count_pb, SIGNAL( clicked () ), this, SLOT( count_event() ) );

				QPushButton* clear_pb = new QPushButton("Clear", operators_grp, "clear");
				clear_pb->setToolTip("Clear the voxels of a specific data channel.");
				connect( clear_pb, SIGNAL( clicked () ), this, SLOT( clear_event() ) );

			channelsop_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Channels:", this );

				QPushButton* copyto_pb = new QPushButton("Copy ...", channelsop_grp, "copy");
				copyto_pb->setToolTip("Copy data from channel A to channel B.");
				connect( copyto_pb, SIGNAL( clicked () ), this, SLOT( copyto_event() ) );

				QPushButton* addto_pb = new QPushButton("Append ...", channelsop_grp, "add");
				addto_pb->setToolTip("Append data from channel A to channel B.");
				connect( addto_pb, SIGNAL( clicked () ), this, SLOT( addto_event() ) );
				
				QPushButton* swap_pb = new QPushButton("Swap ...", channelsop_grp, "swap");
				swap_pb->setToolTip("Swap data from channel A to channel B.");
				connect( swap_pb, SIGNAL( clicked () ), this, SLOT( swap_event() ) );

				QPushButton* replacewith_pb = new QPushButton("Replace With ...", channelsop_grp, "replace");
				replacewith_pb->setToolTip("Replace data from channel A if CONDITION is met in channel B with data from channel C.");
				connect( replacewith_pb, SIGNAL( clicked () ), this, SLOT( replacewith_event() ) );

				QPushButton* setop_pb = new QPushButton("Set Operators", channelsop_grp, "set");
				setop_pb->setToolTip("Set operators between two channels.");
				connect( setop_pb, SIGNAL( clicked () ), this, SLOT( setoperator_event() ) );

				QPushButton* arithop_pb = new QPushButton("Arithmetic Operators", channelsop_grp, "set");
				arithop_pb->setToolTip("Arithmetic operators between two channels.");
				connect( arithop_pb, SIGNAL( clicked () ), this, SLOT( artihoperator_event() ) );

			mortho_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Morphological:", this );

				QPushButton* dilate_pb = new QPushButton("Dilate", mortho_grp, "dilate");
				connect( dilate_pb, SIGNAL( clicked () ), this, SLOT( dilate_event() ) );

				QPushButton* erode_pb = new QPushButton("Erode", mortho_grp, "erode");
				connect( erode_pb, SIGNAL( clicked () ), this, SLOT( erode_event() ) );	

				QPushButton* morpopen_pb = new QPushButton("Open", mortho_grp, "open");
				connect( morpopen_pb, SIGNAL( clicked () ), this, SLOT( morpopen_event() ) );	

				QPushButton* morpclose_pb = new QPushButton("Close", mortho_grp, "close");
				connect( morpclose_pb, SIGNAL( clicked () ), this, SLOT( morpclose_event() ) );	

		vboxlayoutsubLEFT->setAlignment(Qt::AlignTop);
		vboxlayoutsubLEFT->addWidget(operators_grp, 0, Qt::AlignTop);		
		vboxlayoutsubLEFT->addWidget(channelsop_grp, 0, Qt::AlignTop);
//		vboxlayoutsubLEFT->addWidget(mortho_grp, 0, Qt::AlignTop);

		Q3BoxLayout *vboxlayoutsubRIGHT = new Q3BoxLayout ( 0, Q3BoxLayout::TopToBottom, 0, 5, "vboxlayout");

			easyfilters_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "3D Filters:", this );

				QPushButton* rmvnoise_pb = new QPushButton("Remove Noise", easyfilters_grp, "removnoise");
				rmvnoise_pb->setToolTip("Remove noise from the volume.");
				connect( rmvnoise_pb, SIGNAL( clicked () ), this, SLOT( removenoise_event() ) );

				QPushButton* sharpen_pb = new QPushButton("Enhance Edges", easyfilters_grp, "sharpen");
				sharpen_pb->setToolTip("Ehance the edges in the volume.");
				connect( sharpen_pb, SIGNAL( clicked () ), this, SLOT( sharpen_event() ) );

				QPushButton* smooth_pb = new QPushButton("Smooth", easyfilters_grp, "sharpen");
				smooth_pb->setToolTip("Smooth the volume.");
				connect( smooth_pb, SIGNAL( clicked () ), this, SLOT( smooth_event() ) );

				QPushButton* despekle_pb = new QPushButton("Despekle", easyfilters_grp, "despek");
				despekle_pb->setToolTip("Remove speckles from the volume.");
				connect( despekle_pb, SIGNAL( clicked () ), this, SLOT( despekle_event() ) );
			
			filters_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Filters:", this );

				QPushButton* median_pb = new QPushButton("Median Filter", filters_grp, "median");
				connect( median_pb, SIGNAL( clicked () ), this, SLOT( median_event() ) );
				
				QPushButton* aniso_pb = new QPushButton("Anisotropic Diffusion", filters_grp, "aniso");
				connect( aniso_pb, SIGNAL( clicked () ), this, SLOT( peronamalik_event() ) );

				QPushButton* bilateral_pb = new QPushButton("Bi-Lateral Filter", filters_grp, "bilat");
				connect( bilateral_pb, SIGNAL( clicked () ), this, SLOT( bilateral_event() ) );

				QPushButton* mean_pb = new QPushButton("Mean Filter", filters_grp, "mean");
				connect( mean_pb, SIGNAL( clicked () ), this, SLOT( mean_event() ) );
				mean_pb->hide();

				QPushButton* gaussian_pb = new QPushButton("Gaussian Filter", filters_grp, "gaussian");
				connect( gaussian_pb, SIGNAL( clicked () ), this, SLOT( gaussian_event() ) );

				QPushButton* unsharp_pb = new QPushButton("Unsharp Mask", filters_grp, "unsharp");
				connect( unsharp_pb, SIGNAL( clicked () ), this, SLOT( unsharp_event() ) );
				//unsharp_pb->hide();
				
				QPushButton* sobel_pb = new QPushButton("Sobel Edges", filters_grp, "sobel");
				connect( sobel_pb, SIGNAL( clicked () ), this, SLOT( sobel_event() ) );
				sobel_pb->hide();

				QPushButton* sieve_pb = new QPushButton("Sieve", filters_grp, "sieve");
				connect( sieve_pb, SIGNAL( clicked () ), this, SLOT( sieve_event() ) );
				sieve_pb->hide();

			histogram_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Histogram:", this );
			
				QPushButton* lvls_pb = new QPushButton("Histogram Equalization", histogram_grp, "level");
				connect( lvls_pb, SIGNAL( clicked () ), this, SLOT( autolevels_event() ) );

				QPushButton* contra_pb = new QPushButton("Stretch Contrast", histogram_grp, "contra");
				connect( contra_pb, SIGNAL( clicked () ), this, SLOT( contraststretch_event() ) );
			histogram_grp->hide();

		vboxlayoutsubRIGHT->setAlignment(Qt::AlignTop);
		vboxlayoutsubRIGHT->addWidget(mortho_grp, 0, Qt::AlignTop);
		vboxlayoutsubRIGHT->addWidget(easyfilters_grp, 0, Qt::AlignTop);		
		vboxlayoutsubRIGHT->addWidget(filters_grp, 0, Qt::AlignTop);
		vboxlayoutsubRIGHT->addWidget(histogram_grp, 0, Qt::AlignTop);

		Q3BoxLayout *vboxlayoutsubRIGHT2 = new Q3BoxLayout ( 0, Q3BoxLayout::TopToBottom, 0, 5, "vboxlayout2");

#ifdef OPENCL_SUPPORT
			gpufilters_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "GPU-Filters:", this );

				QPushButton* median_pb0 = new QPushButton("Median Filter", gpufilters_grp, "median");
				connect( median_pb0, SIGNAL( clicked () ), this, SLOT( gpu_median_event() ) );
				
				QPushButton* aniso_pb0 = new QPushButton("Anisotropic Diffusion", gpufilters_grp, "aniso");
				connect( aniso_pb0, SIGNAL( clicked () ), this, SLOT( gpu_peronamalik_event() ) );

				QPushButton* bilateral_pb0 = new QPushButton("Bi-Lateral Filter", gpufilters_grp, "bilat");
				connect( bilateral_pb0, SIGNAL( clicked () ), this, SLOT( gpu_bilateral_event() ) );

				QPushButton* gaussian_pb0 = new QPushButton("Gaussian Filter", gpufilters_grp, "gaussian");
				connect( gaussian_pb0, SIGNAL( clicked () ), this, SLOT( gpu_gaussian_event() ) );

				QPushButton* dilate_pb0 = new QPushButton("Dilate", gpufilters_grp, "dilate");
				connect( dilate_pb0, SIGNAL( clicked () ), this, SLOT( gpu_dilate_event() ) );

				QPushButton* erode_pb0 = new QPushButton("Erode", gpufilters_grp, "erode");
				connect( erode_pb0, SIGNAL( clicked () ), this, SLOT( gpu_erode_event() ) );	

				QPushButton* smoothdil_pb0 = new QPushButton("Smoothed Dilate", gpufilters_grp, "sdil");
				connect( smoothdil_pb0, SIGNAL( clicked () ), this, SLOT( gpu_smoothdil_event() ) );
			gpufilters_grp->hide();
#endif

			regions_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Segementation:", this );
				
				QPushButton* ffill_pb = new QPushButton("Flood Fill Seeded", regions_grp, "ffill");
				connect( ffill_pb, SIGNAL( clicked () ), this, SLOT( ffill_event() ) );
				ffill_pb->hide();

				QPushButton* smoothdil_pb = new QPushButton("Smoothed Dilate", regions_grp, "sdil");
				connect( smoothdil_pb, SIGNAL( clicked () ), this, SLOT( smoothdil_event() ) );

				QPushButton* maxflow_pb = new QPushButton("Max Flow", regions_grp, "sdil");
				connect( maxflow_pb, SIGNAL( clicked () ), this, SLOT( maxflow_event() ) );

#ifdef OPENCL_SUPPORT
		vboxlayoutsubRIGHT2->addWidget(gpufilters_grp, 0, Qt::AlignTop);
#endif
		vboxlayoutsubRIGHT2->addWidget(regions_grp, 0, Qt::AlignTop);

	topSub->addLayout(vboxlayoutsubLEFT, 0);
	topSub->addLayout(vboxlayoutsubRIGHT, 0);
	topSub->addLayout(vboxlayoutsubRIGHT2, 0);

	Q3BoxLayout *vboxlayoutRIGHT = new Q3BoxLayout ( 0, Q3BoxLayout::TopToBottom, 0, 5, "vboxlayout");
	vboxlayoutRIGHT->addLayout(topSub, 0);

	Q3BoxLayout *topmost= new Q3BoxLayout ( this, Q3BoxLayout::LeftToRight, 10, 5, "topmost");
	topmost->addLayout(vboxlayoutRIGHT, 10);
}
#ifdef OPENCL_SUPPORT

void QtFilterWidget::gpu_smoothdil_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_GPUSMOOTHDIL;
    filter_dialogue = new QDialog(this, "Smooth Dilate", 0);
		
		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
			
		QStringList chan_items;
		chan_items += "Red";
		chan_items += "Green";
		chan_items += "Blue";

		Q3BoxLayout* srcdsst_layout = new Q3BoxLayout(0, Q3BoxLayout::LeftToRight, 4, 4, "filter_layout");

			Q3ButtonGroup* src_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
			QLabel* src_lb = new QLabel( "Source: ", src_grp );
			src_combobox = new QComboBox(src_grp, "src_combobox");
			src_combobox->insertItems(0, chan_items);
			src_combobox->setCurrentItem(0);

			Q3ButtonGroup* dst_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
			QLabel* dst_lb = new QLabel( "Destination: ", dst_grp );
			dst_combobox = new QComboBox(dst_grp, "dst_combobox");
			dst_combobox->insertItems(0, chan_items);
			dst_combobox->setCurrentItem(1);

			srcdsst_layout->addWidget(src_grp);
			srcdsst_layout->addWidget(dst_grp);

		Q3BoxLayout* seed_layout = new Q3BoxLayout(0, Q3BoxLayout::LeftToRight, 4, 4, "filter_layout");

			Q3ButtonGroup* seed_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Seed Point", this );
			QLabel* seed_lb = new QLabel( "Intensity Window: ", seed_grp );
			seedptwindow_le = new QLineEdit("10", seed_grp);
			QValidator *seedptwindow_int_validator = new QIntValidator(1, 255, this);
			seedptwindow_le->setValidator(seedptwindow_int_validator);

			QLabel* minsize_lb = new QLabel( "Minimum Size: ", seed_grp );
			minsize_le = new QLineEdit("1000", seed_grp);
			QValidator *minsize_int_validator = new QIntValidator(1, 100*100*100, this);
			minsize_le->setValidator(minsize_int_validator);
		
		seed_layout->addWidget(seed_grp);

		Q3BoxLayout* gauss_layout = new Q3BoxLayout(0, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
			
			Q3ButtonGroup *kernel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "Gaussian", this);

				QLabel* kernel_size_lb = new QLabel("Kernel Width", kernel_butgroup);
				kernel_size_le = new QLineEdit("5", kernel_butgroup);
				QValidator *int_validator = new QIntValidator(1, 128, this);
				kernel_size_le->setValidator(int_validator);

				QLabel* kernel_strength_lb = new QLabel("Kernel Strength", kernel_butgroup);
				kernel_strength_le = new QLineEdit("1.5", kernel_butgroup);
				QValidator *float_validator = new QDoubleValidator(0.0, 64.0, 2, this);
				kernel_strength_le->setValidator(float_validator);
				
				QLabel* gaussthresh_lb = new QLabel( "Threshold Window: ", kernel_butgroup );
				gaussthresh_le = new QLineEdit("150", kernel_butgroup);
				QValidator *gaussthresh_int_validator = new QIntValidator(1, 255, this);
				gaussthresh_le->setValidator(seedptwindow_int_validator);
	
		gauss_layout->addWidget(kernel_butgroup);

		Q3BoxLayout* preview_layout = new Q3BoxLayout(0, Q3BoxLayout::LeftToRight, 4, 4, "preview_layout");

			Q3ButtonGroup *preview_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "Preview", this);
				previewSections_pb = new QPushButton("Preview Sections", preview_butgroup);
				previewSections_pb->setToggleButton(true);
				previewSections_pb->setOn(true);
			
				previewVolume_pb = new QPushButton("Preview Volume", preview_butgroup);
				previewVolume_pb->setToggleButton(true);
				previewVolume_pb->setOn(false);

		preview_layout->addWidget(preview_butgroup);

		Q3BoxLayout* addto_layout = new Q3BoxLayout(0, Q3BoxLayout::LeftToRight, 4, 4, "filter_layout");

			Q3ButtonGroup* addto_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
			QLabel* addto_lb = new QLabel( "Add to: ", addto_grp );
			with_combobox = new QComboBox(addto_grp, "src_combobox");
			with_combobox->insertItems(0, chan_items);
			with_combobox->setCurrentItem(2);

		Q3ButtonGroup *add_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			QPushButton* addto_pb = new QPushButton("Add to", add_butgroup, "addto");
			connect( addto_pb, SIGNAL( clicked () ), this, SLOT( filtersmdiladd_event() ) );

		addto_layout->add(addto_grp);
		addto_layout->add(add_butgroup);
		
		Q3ButtonGroup *goabort_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			QPushButton* filtergo_pb = new QPushButton("Go", goabort_butgroup, "go");
			connect( filtergo_pb, SIGNAL( clicked () ), this, SLOT( gpu_filtersmdilgo_event() ) );
			QPushButton* filterabort_pb = new QPushButton("ABORT", goabort_butgroup, "abort");
			connect( filterabort_pb, SIGNAL( clicked () ), this, SLOT( filtersmdilabort_event() ) );
		
		Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			QPushButton* filterclose_pb = new QPushButton("Close", okcancel_butgroup, "close");
			connect( filterclose_pb, SIGNAL( clicked () ), this, SLOT( filtersmdilclose_event() ) );
	
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addLayout(srcdsst_layout);
	filter_layout->addLayout(seed_layout);	
	filter_layout->addLayout(gauss_layout);	
	filter_layout->addLayout(preview_layout);	
	filter_layout->addLayout(addto_layout);	
	filter_layout->addWidget(goabort_butgroup);	
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();

//	emit filter_signal(filter_selected, 0, 0, 0, channels);
}
void QtFilterWidget::gpu_filtersmdilgo_event(void)
{
	printf("GPU DIL: GO\n");
	QString txt;

	int srcchan = src_combobox->currentIndex();
	int dstchan = dst_combobox->currentIndex();
	
	txt = seedptwindow_le->text();
	int seed_window = txt.toInt();
	
	txt = minsize_le->text();
	int minsize = txt.toInt();

	txt = kernel_size_le->text();
	int gaussian_width = txt.toInt();

	txt = kernel_strength_le->text();
	float gaussian_strength = txt.toFloat();

	txt = gaussthresh_le->text();
	int gaussian_window = txt.toInt();

	int toggle_sections=0;
	if(previewSections_pb->isOn()) toggle_sections=1;

	int toggle_volume=0;
	if(previewVolume_pb->isOn()) toggle_volume=1;

	emit filter_gpu_smthdilate_signal(srcchan, dstchan, seed_window, minsize, gaussian_width, gaussian_strength, gaussian_window, toggle_sections, toggle_volume);
}

void QtFilterWidget::gpu_dilate_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_GPUDILATE;

    filter_dialogue = new QDialog(this, "Dilate", 0);
		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
				//channels.resize(3);
				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
			Q3ButtonGroup *axis_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Axis:", this );
			connect(axis_grp, SIGNAL(clicked(int)), this, SLOT(axis_button_event(int)));
			axis_grp->setExclusive(false);
				//axis.resize(3);
				xaxisbutton = new QPushButton("X axis", axis_grp);
				xaxisbutton->setCheckable(TRUE);
				xaxisbutton->setChecked(axis[0]);
				//axis[0] = true;
				yaxisbutton = new QPushButton("Y axis", axis_grp);
				yaxisbutton->setCheckable(true);
				yaxisbutton->setChecked(axis[1]);
				//axis[1] = true;
				zaxisbutton = new QPushButton("Z axis", axis_grp);
				zaxisbutton->setCheckable(true);
				zaxisbutton->setChecked(axis[2]);
				//axis[2] = true;
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );
				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(axis_grp);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}

void QtFilterWidget::gpu_erode_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_GPUERODE;
	
    filter_dialogue = new QDialog(this, "Erode", 0);
		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
				//channels.resize(3);
				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
			Q3ButtonGroup *axis_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Axis:", this );
			connect(axis_grp, SIGNAL(clicked(int)), this, SLOT(axis_button_event(int)));
			axis_grp->setExclusive(false);
				//axis.resize(3);
				xaxisbutton = new QPushButton("X axis", axis_grp);
				xaxisbutton->setCheckable(TRUE);
				xaxisbutton->setChecked(axis[0]);
				//axis[0] = true;
				yaxisbutton = new QPushButton("Y axis", axis_grp);
				yaxisbutton->setCheckable(true);
				yaxisbutton->setChecked(axis[1]);
				//axis[1] = true;
				zaxisbutton = new QPushButton("Z axis", axis_grp);
				zaxisbutton->setCheckable(true);
				zaxisbutton->setChecked(axis[2]);
				//axis[2] = true;
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );
				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(axis_grp);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}
void QtFilterWidget::gpu_peronamalik_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_GPUPERONAMALIK;

	QStringList chan_items;
	chan_items += "Red";
	chan_items += "Green";
	chan_items += "Blue";

    filter_dialogue = new QDialog(this, "Anisotropic Diffusion", 0);
	Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");

			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
			//channels.resize(3);

				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				//connect( red, SIGNAL( pressed() ), this, SLOT( red_button_event() ) );

				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				//connect( green, SIGNAL( pressed() ), this, SLOT( green_button_event() ) );

				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
				//connect( blue, SIGNAL( pressed() ), this, SLOT( blue_button_event() ) );
				
			Q3ButtonGroup *dim_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			dim_butgroup->setExclusive(true);
			
				twod_pb = new QPushButton("2D", dim_butgroup, "ok");
				twod_pb->setCheckable(true);
				twod_pb->setChecked(false);
				
				threed_pb = new QPushButton("3D", dim_butgroup, "cancel");
				threed_pb->setCheckable(true);
				threed_pb->setChecked(true);


		Q3ButtonGroup* param_butgroup = new Q3ButtonGroup( 6, Qt::Horizontal, "", 0);
		
			QLabel* wparam_lb = new QLabel("lambda: ", param_butgroup);
			maxflow_wparam_le = new QLineEdit("", param_butgroup);
			maxflow_wparam_le->setText("5.0");

			QLabel* vparam_lb = new QLabel("dt: ", param_butgroup);
			maxflow_vparam_le = new QLineEdit("", param_butgroup);
			maxflow_vparam_le->setText("0.1");

			QLabel* kparam_lb = new QLabel("iterations: ", param_butgroup);
			maxflow_kparam_le = new QLineEdit("", param_butgroup);
			maxflow_kparam_le->setText("10");

	Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);

		QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
		connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );

		QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
		connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );

	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(dim_butgroup);
	filter_layout->addWidget(param_butgroup);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}
void QtFilterWidget::gpu_median_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_GPUMEDIAN;
    filter_dialogue = new QDialog(this, "Median", 0);

	QValidator *int_validator = new QIntValidator(1, 32, this);
	QValidator *float_validator = new QDoubleValidator(0.0, 32.0, 2, this);
 
		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");

			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
			//channels.resize(3);

				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				//connect( red, SIGNAL( pressed() ), this, SLOT( red_button_event() ) );

				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				//connect( green, SIGNAL( pressed() ), this, SLOT( green_button_event() ) );

				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
				//connect( blue, SIGNAL( pressed() ), this, SLOT( blue_button_event() ) );
				
			Q3ButtonGroup *dim_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			dim_butgroup->setExclusive(true);
			
				twod_pb = new QPushButton("2D", dim_butgroup, "ok");
				twod_pb->setCheckable(true);
				twod_pb->setChecked(false);
				
				threed_pb = new QPushButton("3D", dim_butgroup, "cancel");
				threed_pb->setCheckable(true);
				threed_pb->setChecked(true);

			Q3ButtonGroup *kernel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			
				QLabel* kernel_size_lb = new QLabel("Kernel Width", kernel_butgroup);
				kernel_size_le = new QLineEdit("1", kernel_butgroup);
				kernel_size_le->setValidator(int_validator);
				
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);

				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );

				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
				
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(dim_butgroup);
	filter_layout->addWidget(kernel_butgroup);
	filter_layout->addWidget(okcancel_butgroup);	

	filter_dialogue->show();
}
void QtFilterWidget::gpu_bilateral_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_GPUBILATERAL;
    filter_dialogue = new QDialog(this, "Bi-Lateral Filter", 0);

	QValidator *int_validator = new QIntValidator(1, 32, this);
	QValidator *float_validator = new QDoubleValidator(0.0, 32.0, 2, this);

		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");

			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
				//channels.resize(3);
				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;

			Q3ButtonGroup *kernel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			
				QLabel* kernel_size_lb = new QLabel("Spatial", kernel_butgroup);
				kernel_size_le = new QLineEdit("1", kernel_butgroup);
				kernel_size_le->setValidator(int_validator);

				QLabel* kernel_strength_lb = new QLabel("Range", kernel_butgroup);
				kernel_strength_le = new QLineEdit("0.1", kernel_butgroup);
				kernel_strength_le->setValidator(float_validator);
				
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);

				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );

				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(kernel_butgroup);
	filter_layout->addWidget(okcancel_butgroup);	

	filter_dialogue->show();
}
void QtFilterWidget::gpu_gaussian_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_GPUGAUSSIAN;
    filter_dialogue = new QDialog(this, "Gaussian Smooth", 0);
	
	QValidator *int_validator = new QIntValidator(1, 32, this);
	QValidator *float_validator = new QDoubleValidator(0.0, 32.0, 2, this);

		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");

			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
				//channels.resize(3);
				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
			
			Q3ButtonGroup *dim_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			dim_butgroup->setExclusive(true);
			
				twod_pb = new QPushButton("2D", dim_butgroup, "ok");
				twod_pb->setCheckable(true);
				twod_pb->setChecked(false);
				
				threed_pb = new QPushButton("3D", dim_butgroup, "cancel");
				threed_pb->setCheckable(true);
				threed_pb->setChecked(true);

			Q3ButtonGroup *kernel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			
				QLabel* kernel_size_lb = new QLabel("Kernel Width", kernel_butgroup);
				kernel_size_le = new QLineEdit("3", kernel_butgroup);
				kernel_size_le->setValidator(int_validator);

				QLabel* kernel_strength_lb = new QLabel("Kernel Strength", kernel_butgroup);
				kernel_strength_le = new QLineEdit("0.5", kernel_butgroup);
				kernel_strength_le->setValidator(float_validator);
				
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);

				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );

				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(dim_butgroup);
	filter_layout->addWidget(kernel_butgroup);
	filter_layout->addWidget(okcancel_butgroup);	

	filter_dialogue->show();
}
#endif
void QtFilterWidget::bilateral_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_BILATERAL;
    filter_dialogue = new QDialog(this, "Bi-Lateral Filter", 0);

	QValidator *int_validator = new QIntValidator(1, 32, this);
	QValidator *float_validator = new QDoubleValidator(0.0, 32.0, 2, this);

		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");

			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
				//channels.resize(3);
				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;

			Q3ButtonGroup *kernel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			
				QLabel* kernel_size_lb = new QLabel("Spatial", kernel_butgroup);
				kernel_size_le = new QLineEdit("1", kernel_butgroup);
				kernel_size_le->setValidator(int_validator);

				QLabel* kernel_strength_lb = new QLabel("Range", kernel_butgroup);
				kernel_strength_le = new QLineEdit("0.1", kernel_butgroup);
				kernel_strength_le->setValidator(float_validator);
				
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);

				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );

				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(kernel_butgroup);
	filter_layout->addWidget(okcancel_butgroup);	

	filter_dialogue->show();
}
void QtFilterWidget::gaussian_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_GAUSSIAN;
    filter_dialogue = new QDialog(this, "Gaussian Smooth", 0);
	
	QValidator *int_validator = new QIntValidator(1, 32, this);
	QValidator *float_validator = new QDoubleValidator(0.0, 32.0, 2, this);

		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");

			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
				//channels.resize(3);
				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
			
			Q3ButtonGroup *dim_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			dim_butgroup->setExclusive(true);
			
				twod_pb = new QPushButton("2D", dim_butgroup, "ok");
				twod_pb->setCheckable(true);
				twod_pb->setChecked(false);
				
				threed_pb = new QPushButton("3D", dim_butgroup, "cancel");
				threed_pb->setCheckable(true);
				threed_pb->setChecked(true);

			Q3ButtonGroup *kernel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			
				QLabel* kernel_size_lb = new QLabel("Kernel Width", kernel_butgroup);
				kernel_size_le = new QLineEdit("3", kernel_butgroup);
				kernel_size_le->setValidator(int_validator);

				QLabel* kernel_strength_lb = new QLabel("Kernel Strength", kernel_butgroup);
				kernel_strength_le = new QLineEdit("0.5", kernel_butgroup);
				kernel_strength_le->setValidator(float_validator);
				
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);

				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );

				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(dim_butgroup);
	filter_layout->addWidget(kernel_butgroup);
	filter_layout->addWidget(okcancel_butgroup);	

	filter_dialogue->show();
}
void QtFilterWidget::replacewith_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_REPLACE;

	QStringList chan_items;
	chan_items += "Red";
	chan_items += "Green";
	chan_items += "Blue";

	QStringList repop_items;
	repop_items += "non zero";
	repop_items += "zero";

	filter_dialogue = new QDialog(this, "Copy Channel to...", 0);
	Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
	
	Q3BoxLayout* srcdsst_layout = new Q3BoxLayout(0, Q3BoxLayout::LeftToRight, 4, 4, "filter_layout");
			
			Q3ButtonGroup* src_grp = new Q3ButtonGroup( 4, Qt::Horizontal, "", this );
				QLabel* src_lb = new QLabel( "If ", src_grp );
				src_combobox = new QComboBox(src_grp, "src_combobox");
				src_combobox->insertItems(0, chan_items);
				src_combobox->setCurrentItem(0);
				QLabel* is_lb = new QLabel( "is ", src_grp );
				repop_combobox = new QComboBox(src_grp, "src_combobox");
				repop_combobox->insertItems(0, repop_items);
				repop_combobox->setCurrentItem(0);

			Q3ButtonGroup* dst_grp = new Q3ButtonGroup( 4, Qt::Horizontal, "", this );
				QLabel* repin_lb = new QLabel( "replace in ", src_grp );
				in_combobox = new QComboBox(src_grp, "src_combobox");
				in_combobox->insertItems(0, chan_items);
				in_combobox->setCurrentItem(1);
				QLabel* with_lb = new QLabel( "with ", src_grp );
				with_combobox = new QComboBox(src_grp, "src_combobox");
				with_combobox->insertItems(0, chan_items);
				with_combobox->setCurrentItem(2);

		srcdsst_layout->addWidget(src_grp);
		srcdsst_layout->addWidget(dst_grp);

	Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);

		QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
		connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );

		QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
		connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );

	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addLayout(srcdsst_layout);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}

void QtFilterWidget::addto_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_ADD;

	QStringList chan_items;
	chan_items += "Red";
	chan_items += "Green";
	chan_items += "Blue";

    filter_dialogue = new QDialog(this, "Copy Channel to...", 0);
	Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");

		Q3BoxLayout* srcdsst_layout = new Q3BoxLayout(0, Q3BoxLayout::LeftToRight, 4, 4, "filter_layout");

			Q3ButtonGroup* src_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
			QLabel* src_lb = new QLabel( "Source: ", src_grp );
			src_combobox = new QComboBox(src_grp, "src_combobox");
			src_combobox->insertItems(0, chan_items);
			src_combobox->setCurrentItem(0);

			Q3ButtonGroup* dst_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
			QLabel* dst_lb = new QLabel( "Destination: ", dst_grp );
			dst_combobox = new QComboBox(dst_grp, "dst_combobox");
			dst_combobox->insertItems(0, chan_items);
			dst_combobox->setCurrentItem(1);

		srcdsst_layout->addWidget(src_grp);
		srcdsst_layout->addWidget(dst_grp);

	Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);

		QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
		connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );

		QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
		connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );

	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addLayout(srcdsst_layout);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}
void QtFilterWidget::count_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_COUNT;

    filter_dialogue = new QDialog(this, "Clear", 0);
		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
				//channels.resize(3);
				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );
				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}
void QtFilterWidget::swap_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_SWAP;

	QStringList chan_items;
	chan_items += "Red";
	chan_items += "Green";
	chan_items += "Blue";

    filter_dialogue = new QDialog(this, "Swap Channels...", 0);
	Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");

		Q3BoxLayout* srcdsst_layout = new Q3BoxLayout(0, Q3BoxLayout::LeftToRight, 4, 4, "filter_layout");

			Q3ButtonGroup* src_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
			QLabel* src_lb = new QLabel( "Source A: ", src_grp );
			src_combobox = new QComboBox(src_grp, "src_combobox");
			src_combobox->insertItems(0, chan_items);
			src_combobox->setCurrentItem(0);

			Q3ButtonGroup* dst_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
			QLabel* dst_lb = new QLabel( "Source B: ", dst_grp );
			dst_combobox = new QComboBox(dst_grp, "dst_combobox");
			dst_combobox->insertItems(0, chan_items);
			dst_combobox->setCurrentItem(1);

		srcdsst_layout->addWidget(src_grp);
		srcdsst_layout->addWidget(dst_grp);

	Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);

		QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
		connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );

		QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
		connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );

	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addLayout(srcdsst_layout);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}
void QtFilterWidget::copyto_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_COPY;

	QStringList chan_items;
	chan_items += "Red";
	chan_items += "Green";
	chan_items += "Blue";

    filter_dialogue = new QDialog(this, "Copy Channel to...", 0);
	Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");

		Q3BoxLayout* srcdsst_layout = new Q3BoxLayout(0, Q3BoxLayout::LeftToRight, 4, 4, "filter_layout");

			Q3ButtonGroup* src_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
			QLabel* src_lb = new QLabel( "Source: ", src_grp );
			src_combobox = new QComboBox(src_grp, "src_combobox");
			src_combobox->insertItems(0, chan_items);
			src_combobox->setCurrentItem(0);

			Q3ButtonGroup* dst_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
			QLabel* dst_lb = new QLabel( "Destination: ", dst_grp );
			dst_combobox = new QComboBox(dst_grp, "dst_combobox");
			dst_combobox->insertItems(0, chan_items);
			dst_combobox->setCurrentItem(1);

		srcdsst_layout->addWidget(src_grp);
		srcdsst_layout->addWidget(dst_grp);

	Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);

		QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
		connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );

		QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
		connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );

	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addLayout(srcdsst_layout);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}
void QtFilterWidget::peronamalik_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_PERONAMALIK;

	QStringList chan_items;
	chan_items += "Red";
	chan_items += "Green";
	chan_items += "Blue";

    filter_dialogue = new QDialog(this, "Anisotropic Diffusion", 0);
	Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");

			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
			//channels.resize(3);

				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				//connect( red, SIGNAL( pressed() ), this, SLOT( red_button_event() ) );

				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				//connect( green, SIGNAL( pressed() ), this, SLOT( green_button_event() ) );

				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
				//connect( blue, SIGNAL( pressed() ), this, SLOT( blue_button_event() ) );
				
			Q3ButtonGroup *dim_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			dim_butgroup->setExclusive(true);
			
				twod_pb = new QPushButton("2D", dim_butgroup, "ok");
				twod_pb->setCheckable(true);
				twod_pb->setChecked(false);
				
				threed_pb = new QPushButton("3D", dim_butgroup, "cancel");
				threed_pb->setCheckable(true);
				threed_pb->setChecked(true);


		Q3ButtonGroup* param_butgroup = new Q3ButtonGroup( 6, Qt::Horizontal, "", 0);
		
			QLabel* wparam_lb = new QLabel("lambda: ", param_butgroup);
			maxflow_wparam_le = new QLineEdit("", param_butgroup);
			maxflow_wparam_le->setText("5.0");

			QLabel* vparam_lb = new QLabel("dt: ", param_butgroup);
			maxflow_vparam_le = new QLineEdit("", param_butgroup);
			maxflow_vparam_le->setText("0.1");

			QLabel* kparam_lb = new QLabel("iterations: ", param_butgroup);
			maxflow_kparam_le = new QLineEdit("", param_butgroup);
			maxflow_kparam_le->setText("10");

	Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);

		QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
		connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );

		QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
		connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );

	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(dim_butgroup);
	filter_layout->addWidget(param_butgroup);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}

void QtFilterWidget::maxflow_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_MAXFLOW;

	QStringList chan_items;
	chan_items += "Red";
	chan_items += "Green";
	chan_items += "Blue";

	QDoubleValidator *float_validator = new QDoubleValidator(0.0, 1.0, 4, this);
	QDoubleValidator *float_validator2 = new QDoubleValidator(1.0, 1000.0, 4, this);

    filter_dialogue = new QDialog(this, "Max Flow", 0);
	Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");

		Q3BoxLayout* srcdsst_layout = new Q3BoxLayout(0, Q3BoxLayout::LeftToRight, 4, 4, "filter_layout");
			
			Q3ButtonGroup* src_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
			QLabel* src_lb = new QLabel( "Source: ", src_grp );
			src_combobox = new QComboBox(src_grp, "src_combobox");
			src_combobox->insertItems(0, chan_items);
			src_combobox->setCurrentItem(0);

			Q3ButtonGroup* dst_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
			QLabel* dst_lb = new QLabel( "Destination: ", dst_grp );
			dst_combobox = new QComboBox(dst_grp, "dst_combobox");
			dst_combobox->insertItems(0, chan_items);
			dst_combobox->setCurrentItem(1);
	
		srcdsst_layout->addWidget(src_grp);
		srcdsst_layout->addWidget(dst_grp);

		Q3ButtonGroup* param_butgroup = new Q3ButtonGroup( 6, Qt::Horizontal, "", 0);
		
			QLabel* wparam_lb = new QLabel("w: ", param_butgroup);
			maxflow_wparam_le = new QLineEdit("", param_butgroup);
			maxflow_wparam_le->setValidator(float_validator);
			maxflow_wparam_le->setText("1.0");

			QLabel* vparam_lb = new QLabel("v: ", param_butgroup);
			maxflow_vparam_le = new QLineEdit("", param_butgroup);
			maxflow_vparam_le->setValidator(float_validator);
			maxflow_vparam_le->setText("0.45");

			QLabel* kparam_lb = new QLabel("k: ", param_butgroup);
			maxflow_kparam_le = new QLineEdit("", param_butgroup);
			maxflow_kparam_le->setValidator(float_validator2);
			maxflow_kparam_le->setText("32.0");

		Q3ButtonGroup* param2_butgroup = new Q3ButtonGroup( 6, Qt::Horizontal, "Brush: ", 0);

			Q3ButtonGroup* dimmode_butgroup = new Q3ButtonGroup( 6, Qt::Horizontal, "", param2_butgroup);

				maxflow_brush2d = new QPushButton("2D", dimmode_butgroup);
				maxflow_brush2d->setCheckable(TRUE);
				maxflow_brush2d->setChecked(FALSE);
				connect( maxflow_brush2d, SIGNAL( pressed() ), this, SLOT( maxflow_2d_button_event() ) );

				maxflow_brush3d = new QPushButton("3D", dimmode_butgroup);
				maxflow_brush3d->setCheckable(TRUE);
				maxflow_brush3d->setChecked(TRUE);
				connect( maxflow_brush3d, SIGNAL( pressed() ), this, SLOT( maxflow_3d_button_event() ) );
			
			Q3ButtonGroup* size_butgroup = new Q3ButtonGroup( 6, Qt::Horizontal, "", param2_butgroup);

				QLabel* brushparam_lb = new QLabel("Size: ", size_butgroup);
				maxflow_brushparam_le = new QLineEdit("", size_butgroup);
				maxflow_brushparam_le->setValidator(float_validator2);
				maxflow_brushparam_le->setText("1.0");
				connect( maxflow_brushparam_le, SIGNAL( textChanged(const QString&) ), this, SLOT( maxflow_brushparam_event(const QString&) ) );
	//			connect( maxflow_brushparam_le, SIGNAL( textEdited(const QString&) ), this, SLOT( maxflow_brushparam_event(const QString&) ) );
 		
		Q3ButtonGroup* labels_grp = new Q3ButtonGroup( 2, Qt::Horizontal, "Labels:", 0, "labels_grp");

			Q3ButtonGroup* opensave_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "File:", labels_grp, "opensave_grp");
			opensave_grp->setExclusive( FALSE );
			opensave_grp->setAlignment(Qt::AlignLeft);
			//opensave_grp->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));

				QPushButton* open = new QPushButton(QIcon(QPixmap("icons/fileopen.xpm")), "Open", opensave_grp, "open");
				connect( open, SIGNAL( clicked () ), this, SLOT( load_event() ) );
				//open->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
		
				QPushButton* save = new QPushButton(QIcon(QPixmap("icons/filesave.xpm")), "Save", opensave_grp, "save");
				connect( save, SIGNAL( clicked () ), this, SLOT( save_event() ) );
				//save->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));

			Q3ButtonGroup* bgfg_layout = new Q3ButtonGroup( 2, Qt::Horizontal, "Draw Mode:", labels_grp);

				maxflow_bg = new QPushButton("Background", bgfg_layout);
				maxflow_bg->setCheckable(TRUE);
				maxflow_bg->setChecked(FALSE);
				connect( maxflow_bg, SIGNAL( pressed() ), this, SLOT( maxflow_bg_button_event() ) );

				maxflow_fg = new QPushButton("Object", bgfg_layout);
				maxflow_fg->setCheckable(TRUE);
				maxflow_fg->setChecked(FALSE);
				connect( maxflow_fg, SIGNAL( pressed() ), this, SLOT( maxflow_fg_button_event() ) );

				QPushButton* maxflow_clear = new QPushButton("CLEAR", bgfg_layout);
				maxflow_clear->setCheckable(TRUE);
				maxflow_clear->setChecked(FALSE);
				maxflow_clear->setToggleButton(FALSE);
				connect( maxflow_clear, SIGNAL( pressed() ), this, SLOT( maxflow_clear_button_event() ) );

		/*Q3BoxLayout* seedpnt_layout = new Q3BoxLayout(0, Q3BoxLayout::LeftToRight, 4, 4, "filter_layout");
			Q3ButtonGroup* seed_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Seed Point", this );
			QLabel* seed_lb = new QLabel( "Intensity Window: ", seed_grp );
			seedptwindow_le = new QLineEdit("10", seed_grp);
			QValidator *seedptwindow_int_validator = new QIntValidator(1, 255, this);
			seedptwindow_le->setValidator(seedptwindow_int_validator);
		seedpnt_layout->addWidget(seed_grp);*/
/*
		Q3BoxLayout* addto_layout = new Q3BoxLayout(0, Q3BoxLayout::LeftToRight, 4, 4, "filter_layout");

			Q3ButtonGroup* addto_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
			QLabel* addto_lb = new QLabel( "Add to: ", addto_grp );
			with_combobox = new QComboBox(addto_grp, "src_combobox");
			with_combobox->insertItems(0, chan_items);
			with_combobox->setCurrentItem(2);

		Q3ButtonGroup *add_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			QPushButton* addto_pb = new QPushButton("Add to", add_butgroup, "addto");
			connect( addto_pb, SIGNAL( clicked () ), this, SLOT( filtersmdiladd_event() ) );

		addto_layout->add(addto_grp);
		addto_layout->add(add_butgroup);
*/
	Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);

		QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
		connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );

		QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
		connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );

	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addLayout(srcdsst_layout);
	filter_layout->addWidget(param_butgroup);
	filter_layout->addWidget(param2_butgroup);	
	filter_layout->addWidget(labels_grp);
//	filter_layout->addWidget(bgfg_layout);
//	filter_layout->addLayout(addto_layout);	
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}
void QtFilterWidget::maxflow_2d_button_event(void)
{
//	printf("2d\n");
	if(!maxflow_brush2d->isChecked())
	{
//		printf("not checked\n");
		maxflow_brush2d->setFocus();
		maxflow_brush3d->setChecked(FALSE);
		emit filter_maxflow_dimmode_signal(0);
	}
	else
	{
//		printf("checked\n");
		emit filter_maxflow_dimmode_signal(1);
	}
}
void QtFilterWidget::maxflow_3d_button_event(void)
{
//	printf("3d\n");
	if(!maxflow_brush3d->isChecked())
	{
//		printf("not checked\n");
		maxflow_brush3d->setFocus();
		maxflow_brush2d->setChecked(FALSE);
		emit filter_maxflow_dimmode_signal(1);
	}
	else
	{
//		printf("checked\n");
		emit filter_maxflow_dimmode_signal(0);
	}
}
void QtFilterWidget::maxflow_bg_button_event(void)
{
	if(!maxflow_bg->isChecked())
	{
		maxflow_fg->setChecked(FALSE);
		emit filter_maxflow_interact_signal(1);
	}
	else
	{
		emit filter_maxflow_interact_signal(-1);
	}
}
void QtFilterWidget::maxflow_fg_button_event(void)
{
	if(!maxflow_fg->isChecked())
	{
		maxflow_bg->setChecked(FALSE);
		emit filter_maxflow_interact_signal(2);
	}
	else
	{
		emit filter_maxflow_interact_signal(-1);
	}

}
void QtFilterWidget::maxflow_clear_button_event(void)
{
	emit filter_maxflow_clear_signal();
}
void QtFilterWidget::ffill_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_FLOODFILL;

	QStringList chan_items;
	chan_items += "Red";
	chan_items += "Green";
	chan_items += "Blue";

    filter_dialogue = new QDialog(this, "Seeded Flood Fill", 0);
	Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");

		Q3BoxLayout* srcdsst_layout = new Q3BoxLayout(0, Q3BoxLayout::LeftToRight, 4, 4, "filter_layout");
			
			Q3ButtonGroup* src_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
			QLabel* src_lb = new QLabel( "Source: ", src_grp );
			src_combobox = new QComboBox(src_grp, "src_combobox");
			src_combobox->insertItems(0, chan_items);
			src_combobox->setCurrentItem(0);

			Q3ButtonGroup* dst_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
			QLabel* dst_lb = new QLabel( "Destination: ", dst_grp );
			dst_combobox = new QComboBox(dst_grp, "dst_combobox");
			dst_combobox->insertItems(0, chan_items);
			dst_combobox->setCurrentItem(1);
		
		srcdsst_layout->addWidget(src_grp);
		srcdsst_layout->addWidget(dst_grp);

		Q3BoxLayout* seedpnt_layout = new Q3BoxLayout(0, Q3BoxLayout::LeftToRight, 4, 4, "filter_layout");
			Q3ButtonGroup* seed_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Seed Point", this );
			QLabel* seed_lb = new QLabel( "Intensity Window: ", seed_grp );
			seedptwindow_le = new QLineEdit("10", seed_grp);
			QValidator *seedptwindow_int_validator = new QIntValidator(1, 255, this);
			seedptwindow_le->setValidator(seedptwindow_int_validator);
		seedpnt_layout->addWidget(seed_grp);

	Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);

		QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
		connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );

		QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
		connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );

	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addLayout(srcdsst_layout);
	filter_layout->addLayout(seedpnt_layout);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}
void QtFilterWidget::artihoperator_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_ARITHM;

	QStringList chan_items;
	chan_items += "Red";
	chan_items += "Green";
	chan_items += "Blue";

    filter_dialogue = new QDialog(this, "Set Operators.", 0);
	Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");

		Q3BoxLayout* srcdsst_layout = new Q3BoxLayout(0, Q3BoxLayout::LeftToRight, 4, 4, "filter_layout");
			Q3ButtonGroup* src_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
			QLabel* src_lb = new QLabel( "A: ", src_grp );
			src_combobox = new QComboBox(src_grp, "src_combobox");
			src_combobox->insertItems(0, chan_items);
			src_combobox->setCurrentItem(0);

			Q3ButtonGroup* dst_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
			QLabel* dst_lb = new QLabel( "B: ", dst_grp );
			dst_combobox = new QComboBox(dst_grp, "dst_combobox");
			dst_combobox->insertItems(0, chan_items);
			dst_combobox->setCurrentItem(1);

			Q3ButtonGroup* operator_grp = new Q3ButtonGroup( 2, Qt::Horizontal, "", this );
			operator_grp->setExclusive(true);

				QPixmap addIcon = QPixmap( "icons/add.png" );
				addbutton		= new QPushButton(addIcon, "",	operator_grp );
				addbutton->setToolTip("A + B");
				addbutton->setCheckable(true);
				addbutton->setChecked(true);
				addbutton->setFixedSize(48,48);

				QPixmap subIcon = QPixmap( "icons/remove.png" );
				subbutton		= new QPushButton(subIcon, "",	operator_grp );
				subbutton->setToolTip("A - B");
				subbutton->setCheckable(true);
				subbutton->setChecked(false);
				subbutton->setFixedSize(48,48);

		srcdsst_layout->addWidget(src_grp);
		srcdsst_layout->addWidget(dst_grp);

	Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);

		QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
		connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );

		QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
		connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );

	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addLayout(srcdsst_layout);
	filter_layout->addWidget(operator_grp);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}
void QtFilterWidget::setoperator_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_SETOPERATOR;

	QStringList chan_items;
	chan_items += "Red";
	chan_items += "Green";
	chan_items += "Blue";

    filter_dialogue = new QDialog(this, "Set Operators.", 0);
	Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");

		Q3BoxLayout* srcdsst_layout = new Q3BoxLayout(0, Q3BoxLayout::LeftToRight, 4, 4, "filter_layout");
			Q3ButtonGroup* src_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
			QLabel* src_lb = new QLabel( "A: ", src_grp );
			src_combobox = new QComboBox(src_grp, "src_combobox");
			src_combobox->insertItems(0, chan_items);
			src_combobox->setCurrentItem(0);

			Q3ButtonGroup* dst_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
			QLabel* dst_lb = new QLabel( "B: ", dst_grp );
			dst_combobox = new QComboBox(dst_grp, "dst_combobox");
			dst_combobox->insertItems(0, chan_items);
			dst_combobox->setCurrentItem(1);

			Q3ButtonGroup* operator_grp = new Q3ButtonGroup( 2, Qt::Horizontal, "", this );
			operator_grp->setExclusive(true);

				QPixmap anandIcon = QPixmap( "icons/A_AND_B.png" );
				aandbbutton		= new QPushButton(anandIcon, "",	operator_grp );
				aandbbutton->setToolTip("A and B");
				aandbbutton->setCheckable(true);
				aandbbutton->setChecked(true);

				QPixmap andiffbIcon = QPixmap( "icons/A_DIFF_B.png" );
				adiffbbutton		= new QPushButton(andiffbIcon, "",	operator_grp );
				adiffbbutton->setToolTip("A diff B");
				adiffbbutton->setCheckable(true);
				adiffbbutton->setChecked(false);

				QPixmap anotbIcon = QPixmap( "icons/A_NOT_B.png" );
				anotbbutton		= new QPushButton(anotbIcon, "",	operator_grp );
				anotbbutton->setToolTip("A not B");
				anotbbutton->setCheckable(true);
				anotbbutton->setChecked(false);

				QPixmap bnotaIcon = QPixmap( "icons/B_NOT_A.png" );
				bnotabutton		= new QPushButton(bnotaIcon, "",	operator_grp );
				bnotabutton->setToolTip("B not A");
				bnotabutton->setCheckable(true);
				bnotabutton->setChecked(false);

		srcdsst_layout->addWidget(src_grp);
		srcdsst_layout->addWidget(dst_grp);

	Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);

		QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
		connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );

		QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
		connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );

	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addLayout(srcdsst_layout);
	filter_layout->addWidget(operator_grp);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}

void QtFilterWidget::median_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_MEADIAN;
    filter_dialogue = new QDialog(this, "Median", 0);

	QValidator *int_validator = new QIntValidator(1, 32, this);
	QValidator *float_validator = new QDoubleValidator(0.0, 32.0, 2, this);
 
		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");

			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
			//channels.resize(3);

				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				//connect( red, SIGNAL( pressed() ), this, SLOT( red_button_event() ) );

				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				//connect( green, SIGNAL( pressed() ), this, SLOT( green_button_event() ) );

				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
				//connect( blue, SIGNAL( pressed() ), this, SLOT( blue_button_event() ) );
				
			Q3ButtonGroup *dim_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			dim_butgroup->setExclusive(true);
			
				twod_pb = new QPushButton("2D", dim_butgroup, "ok");
				twod_pb->setCheckable(true);
				twod_pb->setChecked(false);
				
				threed_pb = new QPushButton("3D", dim_butgroup, "cancel");
				threed_pb->setCheckable(true);
				threed_pb->setChecked(true);

			Q3ButtonGroup *kernel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			
				QLabel* kernel_size_lb = new QLabel("Kernel Width", kernel_butgroup);
				kernel_size_le = new QLineEdit("1", kernel_butgroup);
				kernel_size_le->setValidator(int_validator);
				
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);

				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );

				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
				
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(dim_butgroup);
	filter_layout->addWidget(kernel_butgroup);
	filter_layout->addWidget(okcancel_butgroup);	

	filter_dialogue->show();
}
void QtFilterWidget::mean_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_MEAN;
    filter_dialogue = new QDialog(this, "Median", 0);

	QValidator *int_validator = new QIntValidator(1, 32, this);
	QValidator *float_validator = new QDoubleValidator(0.0, 32.0, 2, this);
 
		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
			
			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
			//channels.resize(3);

				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				///connect( red, SIGNAL( pressed() ), this, SLOT( red_button_event() ) );

				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				//connect( green, SIGNAL( pressed() ), this, SLOT( green_button_event() ) );

				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
				//connect( blue, SIGNAL( pressed() ), this, SLOT( blue_button_event() ) );

			Q3ButtonGroup *dim_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			dim_butgroup->setExclusive(true);
			
				twod_pb = new QPushButton("2D", dim_butgroup, "ok");
				twod_pb->setCheckable(true);
				twod_pb->setChecked(false);
				
				threed_pb = new QPushButton("3D", dim_butgroup, "cancel");
				threed_pb->setCheckable(true);
				threed_pb->setChecked(true);

			Q3ButtonGroup *kernel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			
				QLabel* kernel_size_lb = new QLabel("Kernel Width", kernel_butgroup);
				kernel_size_le = new QLineEdit("1", kernel_butgroup);
				kernel_size_le->setValidator(int_validator);
				
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);

				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );

				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(dim_butgroup);
	filter_layout->addWidget(kernel_butgroup);
	filter_layout->addWidget(okcancel_butgroup);	

	filter_dialogue->show();
}
void QtFilterWidget::unsharp_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_UNSHARP;
    filter_dialogue = new QDialog(this, "Unsharp Mask", 0);
	
	QValidator *int_validator = new QIntValidator(1, 32, this);
	QValidator *float_validator = new QDoubleValidator(0.0, 32.0, 2, this);

		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");

			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
			//channels.resize(3);

				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				//connect( red, SIGNAL( pressed() ), this, SLOT( red_button_event() ) );

				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				//connect( green, SIGNAL( pressed() ), this, SLOT( green_button_event() ) );

				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
				//connect( blue, SIGNAL( pressed() ), this, SLOT( blue_button_event() ) );
			
			Q3ButtonGroup *dim_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			dim_butgroup->setExclusive(true);
			
				twod_pb = new QPushButton("2D", dim_butgroup, "ok");
				twod_pb->setCheckable(true);
				twod_pb->setChecked(false);
				
				threed_pb = new QPushButton("3D", dim_butgroup, "cancel");
				threed_pb->setCheckable(true);
				threed_pb->setChecked(true);

			Q3ButtonGroup *kernel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			
				QLabel* kernel_size_lb = new QLabel("Kernel Width", kernel_butgroup);
				kernel_size_le = new QLineEdit("3", kernel_butgroup);
				kernel_size_le->setValidator(int_validator);
			
				QLabel* kernel_strength_lb = new QLabel("Kernel Strength", kernel_butgroup);
				kernel_strength_le = new QLineEdit("0.5", kernel_butgroup);
				kernel_strength_le->setValidator(float_validator);
				
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);

				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );

				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(dim_butgroup);
	filter_layout->addWidget(kernel_butgroup);
	filter_layout->addWidget(okcancel_butgroup);	

	filter_dialogue->show();
}
void QtFilterWidget::sieve_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_SIEVE;
    filter_dialogue = new QDialog(this, "Sieve", 0);
	//QValidator *int_validator = new QIntValidator(0, 100, this);

	QStringList chan_items;
	chan_items += "Red";
	chan_items += "Green";
	chan_items += "Blue";

    filter_dialogue = new QDialog(this, "Sieve Aglorithm", 0);
	Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");

			Q3BoxLayout* srcdsst_layout = new Q3BoxLayout(0, Q3BoxLayout::LeftToRight, 4, 4, "filter_layout");
				
				Q3ButtonGroup* src_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
				QLabel* src_lb = new QLabel( "Source: ", src_grp );
				src_combobox = new QComboBox(src_grp, "src_combobox");
				src_combobox->insertItems(0, chan_items);
				src_combobox->setCurrentItem(0);

				Q3ButtonGroup* dst_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
				QLabel* dst_lb = new QLabel( "Destination: ", dst_grp );
				dst_combobox = new QComboBox(dst_grp, "dst_combobox");
				dst_combobox->insertItems(0, chan_items);
				dst_combobox->setCurrentItem(1);
			
			srcdsst_layout->addWidget(src_grp);
			srcdsst_layout->addWidget(dst_grp);
				
			Q3ButtonGroup *kernel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			
			QLabel* kernel_size_lb = new QLabel("Region Size:", kernel_butgroup);
				kernel_size_le = new QLineEdit("1000", kernel_butgroup);
//				kernel_size_le->setValidator(int_validator);
				
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);

				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );

				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addLayout(srcdsst_layout);
	filter_layout->addWidget(kernel_butgroup);
	filter_layout->addWidget(okcancel_butgroup);	

	filter_dialogue->show();
}
void QtFilterWidget::filterok_event(void)
{
	filter_dialogue->accept();

	vector<int> intparams;
	vector<double> doubparams;
	QString txt;

	if(filter_selected==FILTER_MEAN)
	{
		if(twod_pb->isChecked()) intparams.push_back(0);
		else intparams.push_back(1);

		txt = kernel_size_le->text();
		intparams.push_back(txt.toInt());
	}
	else if(filter_selected==FILTER_MEADIAN)
	{
		if(twod_pb->isChecked()) intparams.push_back(0);
		else intparams.push_back(1);

		txt = kernel_size_le->text();
		intparams.push_back(txt.toInt());
	}
	else if(filter_selected==FILTER_GPUGAUSSIAN)
	{
		if(twod_pb->isChecked()) intparams.push_back(0);
		else intparams.push_back(1);

		txt = kernel_size_le->text();
		intparams.push_back(txt.toInt());

		txt = kernel_strength_le->text();
		doubparams.push_back(txt.toDouble());
	}
	else if(filter_selected==FILTER_BILATERAL)
	{
		txt = kernel_size_le->text();
		intparams.push_back(txt.toInt());

		txt = kernel_strength_le->text();
		doubparams.push_back(txt.toDouble());
	}
	else if(filter_selected==FILTER_GPUBILATERAL)
	{
		txt = kernel_size_le->text();
		intparams.push_back(txt.toInt());

		txt = kernel_strength_le->text();
		doubparams.push_back(txt.toDouble());
	}
	else if(filter_selected==FILTER_GAUSSIAN)
	{
		if(twod_pb->isChecked()) intparams.push_back(0);
		else intparams.push_back(1);

		txt = kernel_size_le->text();
		intparams.push_back(txt.toInt());

		txt = kernel_strength_le->text();
		doubparams.push_back(txt.toDouble());
	}
	else if(filter_selected==FILTER_UNSHARP)
	{
		//if(twod_pb->isChecked()) intparams.push_back(0);
		//else intparams.push_back(1);
		intparams.push_back(1);

		txt = kernel_size_le->text();
		intparams.push_back(txt.toInt());

		txt = kernel_strength_le->text();
		doubparams.push_back(txt.toDouble());
	}
	else if(filter_selected==FILTER_PERONAMALIK)
	{
		if(twod_pb->isChecked()) intparams.push_back(0);
		else intparams.push_back(1);

		txt = maxflow_wparam_le->text();
		doubparams.push_back(txt.toDouble());

		txt = maxflow_vparam_le->text();
		doubparams.push_back(txt.toDouble());

		txt = maxflow_kparam_le->text();
		intparams.push_back(txt.toInt());
	}
	else if(filter_selected==FILTER_GPUPERONAMALIK)
	{
		if(twod_pb->isChecked()) intparams.push_back(0);
		else intparams.push_back(1);

		txt = maxflow_wparam_le->text();
		doubparams.push_back(txt.toDouble());

		txt = maxflow_vparam_le->text();
		doubparams.push_back(txt.toDouble());

		txt = maxflow_kparam_le->text();
		intparams.push_back(txt.toInt());
	}
	else if(filter_selected==FILTER_MAXFLOW)
	{
		emit filter_maxflow_interact_signal(-1);
		emit filter_maxflow_buildlabels_signal();

		int srcchan = src_combobox->currentIndex();
		intparams.push_back(srcchan);

		int dstchan = dst_combobox->currentIndex();
		intparams.push_back(dstchan);

		txt = maxflow_wparam_le->text();
		doubparams.push_back(txt.toDouble());

		txt = maxflow_vparam_le->text();
		doubparams.push_back(txt.toDouble());

		txt = maxflow_kparam_le->text();
		doubparams.push_back(txt.toDouble());
	}
	else if(filter_selected==FILTER_SOBEL)
	{
	//	if(twod_pb->isChecked()) intparams.push_back(0);
	//	else intparams.push_back(1);
	}
	else if(filter_selected==FILTER_SIEVE)
	{
		int srcchan = src_combobox->currentIndex();
		intparams.push_back(srcchan);

		int dstchan = dst_combobox->currentIndex();
		intparams.push_back(dstchan);

		txt = kernel_size_le->text();
		int size = txt.toInt();
		intparams.push_back(size);
	}
	else if(filter_selected==FILTER_COLLAPSE)
	{
	}
	else if(filter_selected==FILTER_INVERT)
	{
	}
	else if(filter_selected==FILTER_THRESHOLD)
	{
	}
	else if(filter_selected==FILTER_BINARIZE)
	{
	}
	else if(filter_selected==FILTER_CLEAR)
	{
	}
	else if(filter_selected==FILTER_COUNT)
	{
	}
	else if(filter_selected==FILTER_OPEN)
	{
		for(int i=0; i<axis.size(); i++)
			channels.push_back(axis[i]);
	}
	else if(filter_selected==FILTER_CLOSE)
	{
		for(int i=0; i<axis.size(); i++)
			channels.push_back(axis[i]);
	}
	else if(filter_selected==FILTER_GPUDILATE)
	{
		for(int i=0; i<axis.size(); i++)
			channels.push_back(axis[i]);
	}
	else if(filter_selected==FILTER_DILATE)
	{
		for(int i=0; i<axis.size(); i++)
			channels.push_back(axis[i]);
	}
	else if(filter_selected==FILTER_ERODE)
	{
		for(int i=0; i<axis.size(); i++)
			channels.push_back(axis[i]);
	}
	else if(filter_selected==FILTER_COPY)
	{
		int srcchan = src_combobox->currentIndex();
		intparams.push_back(srcchan);

		int dstchan = dst_combobox->currentIndex();
		intparams.push_back(dstchan);
	}
	else if(filter_selected==FILTER_SWAP)
	{
		int srcchan = src_combobox->currentIndex();
		intparams.push_back(srcchan);

		int dstchan = dst_combobox->currentIndex();
		intparams.push_back(dstchan);
	}
	else if(filter_selected==FILTER_REPLACE)
	{
		int if_chan = src_combobox->currentIndex();
		intparams.push_back(if_chan);
		int op_chan = repop_combobox->currentIndex();
		intparams.push_back(op_chan);

		int in_chan = in_combobox->currentIndex();
		intparams.push_back(in_chan);
		int with_chan = with_combobox->currentIndex();
		intparams.push_back(with_chan);
	}
	else if(filter_selected==FILTER_ADD)
	{
		int srcchan = src_combobox->currentIndex();
		intparams.push_back(srcchan);

		int dstchan = dst_combobox->currentIndex();
		intparams.push_back(dstchan);
	}
	else if(filter_selected==FILTER_SETOPERATOR)
	{
		int a_chan = src_combobox->currentIndex();
		intparams.push_back(a_chan);
		int b_chan = dst_combobox->currentIndex();
		intparams.push_back(b_chan);

		int operatormode = -1;
		if(anotbbutton->isChecked()) operatormode=0;
		if(bnotabutton->isChecked()) operatormode=1;
		if(adiffbbutton->isChecked()) operatormode=2;
		if(aandbbutton->isChecked()) operatormode=3;
		intparams.push_back(operatormode);
	}
	else if(filter_selected==FILTER_ARITHM)
	{
		int a_chan = src_combobox->currentIndex();
		intparams.push_back(a_chan);
		int b_chan = dst_combobox->currentIndex();
		intparams.push_back(b_chan);
		
		int operatormode = -1;
		if(addbutton->isChecked()) operatormode=0;
		if(subbutton->isChecked()) operatormode=1;
		intparams.push_back(operatormode);
	}
	else if(filter_selected==FILTER_STRETCH)
	{
//		if(twod_pb->isChecked()) intparams.push_back(0);
//		else intparams.push_back(1);
	}
	else if(filter_selected==FILTER_EQUALIZE)
	{
///		if(twod_pb->isChecked()) intparams.push_back(0);
//		else intparams.push_back(1);
	}
	else if(filter_selected==FILTER_FLOODFILL)
	{
		int srcchan = src_combobox->currentIndex();
		intparams.push_back(srcchan);

		int dstchan = dst_combobox->currentIndex();
		intparams.push_back(dstchan);

		txt = seedptwindow_le->text();
		int window = txt.toInt();
		intparams.push_back(window);
	}
	else if(filter_selected==FILTER_SMOOTHDIL)
	{
	}

	emit filter_signal(filter_selected, channels, intparams, doubparams);
	delete filter_dialogue;
	filter_dialogue=NULL;
}
void QtFilterWidget::filtersmdiladd_event(void)
{
	vector<int> intparams;
	vector<double> doubparams;

	filter_selected=FILTER_ADD;

	int srcchan = dst_combobox->currentIndex();
	intparams.push_back(srcchan);

	int dstchan = with_combobox->currentIndex();
	intparams.push_back(dstchan);

	emit filter_signal(filter_selected, channels, intparams, doubparams);
}

void QtFilterWidget::smoothdil_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_SMOOTHDIL;
    filter_dialogue = new QDialog(this, "Smooth Dilate", 0);
		
		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
			
		QStringList chan_items;
		chan_items += "Red";
		chan_items += "Green";
		chan_items += "Blue";

		Q3BoxLayout* srcdsst_layout = new Q3BoxLayout(0, Q3BoxLayout::LeftToRight, 4, 4, "filter_layout");

			Q3ButtonGroup* src_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
			QLabel* src_lb = new QLabel( "Source: ", src_grp );
			src_combobox = new QComboBox(src_grp, "src_combobox");
			src_combobox->insertItems(0, chan_items);
			src_combobox->setCurrentItem(0);

			Q3ButtonGroup* dst_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
			QLabel* dst_lb = new QLabel( "Destination: ", dst_grp );
			dst_combobox = new QComboBox(dst_grp, "dst_combobox");
			dst_combobox->insertItems(0, chan_items);
			dst_combobox->setCurrentItem(1);

			srcdsst_layout->addWidget(src_grp);
			srcdsst_layout->addWidget(dst_grp);

		Q3BoxLayout* seed_layout = new Q3BoxLayout(0, Q3BoxLayout::LeftToRight, 4, 4, "filter_layout");

			Q3ButtonGroup* seed_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Seed Point", this );
			QLabel* seed_lb = new QLabel( "Intensity Window: ", seed_grp );
			seedptwindow_le = new QLineEdit("10", seed_grp);
			QValidator *seedptwindow_int_validator = new QIntValidator(1, 255, this);
			seedptwindow_le->setValidator(seedptwindow_int_validator);

			QLabel* minsize_lb = new QLabel( "Minimum Size: ", seed_grp );
			minsize_le = new QLineEdit("1000", seed_grp);
			QValidator *minsize_int_validator = new QIntValidator(1, 100*100*100, this);
			minsize_le->setValidator(minsize_int_validator);
		
		seed_layout->addWidget(seed_grp);

		Q3BoxLayout* gauss_layout = new Q3BoxLayout(0, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
			
			Q3ButtonGroup *kernel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "Gaussian", this);

				QLabel* kernel_size_lb = new QLabel("Kernel Width", kernel_butgroup);
				kernel_size_le = new QLineEdit("5", kernel_butgroup);
				QValidator *int_validator = new QIntValidator(1, 128, this);
				kernel_size_le->setValidator(int_validator);

				QLabel* kernel_strength_lb = new QLabel("Kernel Strength", kernel_butgroup);
				kernel_strength_le = new QLineEdit("1.5", kernel_butgroup);
				QValidator *float_validator = new QDoubleValidator(0.0, 64.0, 2, this);
				kernel_strength_le->setValidator(float_validator);
				
				QLabel* gaussthresh_lb = new QLabel( "Threshold Window: ", kernel_butgroup );
				gaussthresh_le = new QLineEdit("150", kernel_butgroup);
				QValidator *gaussthresh_int_validator = new QIntValidator(1, 255, this);
				gaussthresh_le->setValidator(seedptwindow_int_validator);
	
		gauss_layout->addWidget(kernel_butgroup);

		Q3BoxLayout* preview_layout = new Q3BoxLayout(0, Q3BoxLayout::LeftToRight, 4, 4, "preview_layout");
			Q3ButtonGroup *preview_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "Preview", this);
				previewSections_pb = new QPushButton("Preview Sections", preview_butgroup);
				previewSections_pb->setToggleButton(true);
				previewSections_pb->setOn(true);
			
				previewVolume_pb = new QPushButton("Preview Volume", preview_butgroup);
				previewVolume_pb->setToggleButton(true);
				previewVolume_pb->setOn(false);

		preview_layout->addWidget(preview_butgroup);

		Q3BoxLayout* addto_layout = new Q3BoxLayout(0, Q3BoxLayout::LeftToRight, 4, 4, "filter_layout");

			Q3ButtonGroup* addto_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "", this );
			QLabel* addto_lb = new QLabel( "Add to: ", addto_grp );
			with_combobox = new QComboBox(addto_grp, "src_combobox");
			with_combobox->insertItems(0, chan_items);
			with_combobox->setCurrentItem(2);

		Q3ButtonGroup *add_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			QPushButton* addto_pb = new QPushButton("Add to", add_butgroup, "addto");
			connect( addto_pb, SIGNAL( clicked () ), this, SLOT( filtersmdiladd_event() ) );

		addto_layout->add(addto_grp);
		addto_layout->add(add_butgroup);

		Q3ButtonGroup *goabort_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			QPushButton* filtergo_pb = new QPushButton("Go", goabort_butgroup, "go");
			connect( filtergo_pb, SIGNAL( clicked () ), this, SLOT( filtersmdilgo_event() ) );
			QPushButton* filterabort_pb = new QPushButton("ABORT", goabort_butgroup, "abort");
			connect( filterabort_pb, SIGNAL( clicked () ), this, SLOT( filtersmdilabort_event() ) );
		
		Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
			QPushButton* filterclose_pb = new QPushButton("Close", okcancel_butgroup, "close");
			connect( filterclose_pb, SIGNAL( clicked () ), this, SLOT( filtersmdilclose_event() ) );
	
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addLayout(srcdsst_layout);
	filter_layout->addLayout(seed_layout);	
	filter_layout->addLayout(gauss_layout);	
	filter_layout->addLayout(preview_layout);	
	filter_layout->addLayout(addto_layout);	
	filter_layout->addWidget(goabort_butgroup);	
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();

//	emit filter_signal(filter_selected, 0, 0, 0, channels);
}
void QtFilterWidget::filtersmdilgo_event(void)
{
	QString txt;

	int srcchan = src_combobox->currentIndex();
	int dstchan = dst_combobox->currentIndex();
	
	txt = seedptwindow_le->text();
	int seed_window = txt.toInt();
	
	txt = minsize_le->text();
	int minsize = txt.toInt();

	txt = kernel_size_le->text();
	int gaussian_width = txt.toInt();

	txt = kernel_strength_le->text();
	float gaussian_strength = txt.toFloat();

	txt = gaussthresh_le->text();
	int gaussian_window = txt.toInt();

	int toggle_sections=0;
	if(previewSections_pb->isOn()) toggle_sections=1;

	int toggle_volume=0;
	if(previewVolume_pb->isOn()) toggle_volume=1;

	emit filter_smthdilate_signal(srcchan, dstchan, seed_window, minsize, gaussian_width, gaussian_strength, gaussian_window, toggle_sections, toggle_volume);
}
void QtFilterWidget::filtersmdilabort_event(void)
{
	emit filter_smthdilateabort_signal();

}
void QtFilterWidget::filtersmdilclose_event(void)
{
	filter_dialogue->accept();
	emit filter_smthdilateabort_signal();
	delete filter_dialogue;
	filter_dialogue = NULL;
}
void QtFilterWidget::thresh_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_THRESHOLD;

    filter_dialogue = new QDialog(this, "Threshold", 0);
		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
				//channels.resize(3);
				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );
				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}

void QtFilterWidget::erode_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_ERODE;
	
    filter_dialogue = new QDialog(this, "Erode", 0);
		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
				//channels.resize(3);
				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
			Q3ButtonGroup *axis_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Axis:", this );
			connect(axis_grp, SIGNAL(clicked(int)), this, SLOT(axis_button_event(int)));
			axis_grp->setExclusive(false);
				//axis.resize(3);
				xaxisbutton = new QPushButton("X axis", axis_grp);
				xaxisbutton->setCheckable(TRUE);
				xaxisbutton->setChecked(axis[0]);
				//axis[0] = true;
				yaxisbutton = new QPushButton("Y axis", axis_grp);
				yaxisbutton->setCheckable(true);
				yaxisbutton->setChecked(axis[1]);
				//axis[1] = true;
				zaxisbutton = new QPushButton("Z axis", axis_grp);
				zaxisbutton->setCheckable(true);
				zaxisbutton->setChecked(axis[2]);
				//axis[2] = true;
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );
				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(axis_grp);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}
void QtFilterWidget::morpopen_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_OPEN;

    filter_dialogue = new QDialog(this, "Open", 0);
		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
				//channels.resize(3);
				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
			Q3ButtonGroup *axis_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Axis:", this );
			connect(axis_grp, SIGNAL(clicked(int)), this, SLOT(axis_button_event(int)));
			axis_grp->setExclusive(false);
				//axis.resize(3);
				xaxisbutton = new QPushButton("X axis", axis_grp);
				xaxisbutton->setCheckable(TRUE);
				xaxisbutton->setChecked(axis[0]);
				//axis[0] = true;
				yaxisbutton = new QPushButton("Y axis", axis_grp);
				yaxisbutton->setCheckable(true);
				yaxisbutton->setChecked(axis[1]);
				//axis[1] = true;
				zaxisbutton = new QPushButton("Z axis", axis_grp);
				zaxisbutton->setCheckable(true);
				zaxisbutton->setChecked(axis[2]);
				//axis[2] = true;
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );
				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(axis_grp);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}
void QtFilterWidget::morpclose_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_CLOSE;

    filter_dialogue = new QDialog(this, "Close", 0);
		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
				//channels.resize(3);
				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
			Q3ButtonGroup *axis_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Axis:", this );
			connect(axis_grp, SIGNAL(clicked(int)), this, SLOT(axis_button_event(int)));
			axis_grp->setExclusive(false);
				//axis.resize(3);
				xaxisbutton = new QPushButton("X axis", axis_grp);
				xaxisbutton->setCheckable(TRUE);
				xaxisbutton->setChecked(axis[0]);
				//axis[0] = true;
				yaxisbutton = new QPushButton("Y axis", axis_grp);
				yaxisbutton->setCheckable(true);
				yaxisbutton->setChecked(axis[1]);
				//axis[1] = true;
				zaxisbutton = new QPushButton("Z axis", axis_grp);
				zaxisbutton->setCheckable(true);
				zaxisbutton->setChecked(axis[2]);
				//axis[2] = true;
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );
				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(axis_grp);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}

void QtFilterWidget::dilate_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_DILATE;

    filter_dialogue = new QDialog(this, "Dilate", 0);
		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
				//channels.resize(3);
				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
			Q3ButtonGroup *axis_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Axis:", this );
			connect(axis_grp, SIGNAL(clicked(int)), this, SLOT(axis_button_event(int)));
			axis_grp->setExclusive(false);
				//axis.resize(3);
				xaxisbutton = new QPushButton("X axis", axis_grp);
				xaxisbutton->setCheckable(TRUE);
				xaxisbutton->setChecked(axis[0]);
				//axis[0] = true;
				yaxisbutton = new QPushButton("Y axis", axis_grp);
				yaxisbutton->setCheckable(true);
				yaxisbutton->setChecked(axis[1]);
				//axis[1] = true;
				zaxisbutton = new QPushButton("Z axis", axis_grp);
				zaxisbutton->setCheckable(true);
				zaxisbutton->setChecked(axis[2]);
				//axis[2] = true;
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );
				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(axis_grp);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}
void QtFilterWidget::binary_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_BINARIZE;

    filter_dialogue = new QDialog(this, "Binarize", 0);
		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
				//channels.resize(3);
				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );
				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}
void QtFilterWidget::clear_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_CLEAR;

    filter_dialogue = new QDialog(this, "Clear", 0);
		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
				//channels.resize(3);
				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );
				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}
void QtFilterWidget::sobel_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_SOBEL;
    filter_dialogue = new QDialog(this, "Sobel Edges", 0);
		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
				//channels.resize(3);
				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );
				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}
void QtFilterWidget::autolevels_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_EQUALIZE;
    filter_dialogue = new QDialog(this, "Histogram Equalization", 0);
		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
				//channels.resize(3);
				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(channels[2]);
				blue->setChecked(false);
				//channels[2] = false;
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );
				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}
void QtFilterWidget::contraststretch_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_STRETCH;
    filter_dialogue = new QDialog(this, "Contrast Stretch", 0);
		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
				//channels.resize(3);
				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );
				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}
void QtFilterWidget::invert_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_INVERT;
    filter_dialogue = new QDialog(this, "Invert", 0);
		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
				//channels.resize(3);
				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( filterok_event() ) );
				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}
void QtFilterWidget::despekle_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_MEADIAN;
    filter_dialogue = new QDialog(this, "Despekle", 0);
		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
				//channels.resize(3);
				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( despekleOK_event() ) );
				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}
void QtFilterWidget::removenoise_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_PERONAMALIK;
    filter_dialogue = new QDialog(this, "Remove Noise", 0);
		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
				//channels.resize(3);
				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( removenoiseOK_event() ) );
				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}
void QtFilterWidget::sharpen_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_UNSHARP;
    filter_dialogue = new QDialog(this, "Sharpen", 0);
		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
				//channels.resize(3);
				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( sharpenOK_event() ) );
				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}
void QtFilterWidget::smooth_event(void)
{
	if(filter_dialogue!=NULL) return;
	filter_selected=FILTER_GAUSSIAN;
    filter_dialogue = new QDialog(this, "Smooth", 0);
		Q3BoxLayout* filter_layout = new Q3BoxLayout(filter_dialogue, Q3BoxLayout::TopToBottom, 4, 4, "filter_layout");
			Q3ButtonGroup *toggle_grp = new Q3ButtonGroup( 1, Qt::Vertical, "Channels:", this );
			connect(toggle_grp, SIGNAL(clicked(int)), this, SLOT(channel_button_event(int)));
			toggle_grp->setExclusive(true);
				//channels.resize(3);
				red = new QPushButton("Red", toggle_grp);
				red->setCheckable(TRUE);
				red->setChecked(channels[0]);
				//channels[0] = true;
				green = new QPushButton("Green", toggle_grp);
				green->setCheckable(true);
				green->setChecked(channels[1]);
				//channels[1] = false;
				blue = new QPushButton("Blue", toggle_grp);
				blue->setCheckable(true);
				blue->setChecked(channels[2]);
				//channels[2] = false;
			Q3ButtonGroup *okcancel_butgroup = new Q3ButtonGroup( 2, Qt::Horizontal, "", this);
				QPushButton* filterok_pb = new QPushButton("OK", okcancel_butgroup, "ok");
				connect( filterok_pb, SIGNAL( clicked () ), this, SLOT( smoothOK_event() ) );
				QPushButton* filtercancel_pb = new QPushButton("Cancel", okcancel_butgroup, "cancel");
				connect( filtercancel_pb, SIGNAL( clicked () ), this, SLOT( filtercancel_event() ) );
	connect(filter_dialogue, SIGNAL(rejected()), this, SLOT( filtercancel_event() ) );
	filter_layout->addWidget(toggle_grp);
	filter_layout->addWidget(okcancel_butgroup);	
	filter_dialogue->show();
}
void QtFilterWidget::despekleOK_event(void)
{
	filter_dialogue->accept();
	delete filter_dialogue;
	filter_dialogue=NULL;

	vector<int> intparams;
	vector<double> doubparams;

	//filter type
	filter_selected = FILTER_MEADIAN;

	//2d - 3d mode
	intparams.push_back(1);
	
	//kernel size
	intparams.push_back(1);

	emit filter_signal(filter_selected, channels, intparams, doubparams);
}
void QtFilterWidget::removenoiseOK_event(void)
{
	filter_dialogue->accept();
	delete filter_dialogue;
	filter_dialogue=NULL;

	vector<int> intparams;
	vector<double> doubparams;

	//filter type
	filter_selected = FILTER_PERONAMALIK;

	//2d/3d mode
	intparams.push_back(1);

	//Lambda
	doubparams.push_back(5.0);

	//dT
	doubparams.push_back(0.1);

	//number of interations
	intparams.push_back(5);

	emit filter_signal(filter_selected, channels, intparams, doubparams);
}
void QtFilterWidget::sharpenOK_event(void)
{
	filter_dialogue->accept();
	delete filter_dialogue;
	filter_dialogue=NULL;

	filter_selected=FILTER_UNSHARP;

	vector<int> intparams;
	vector<double> doubparams;

	//2d / 3d mode
	intparams.push_back(1);

	//kernel size
	intparams.push_back(3);

	//gaussian strength
	doubparams.push_back(2.0);
	emit filter_signal(filter_selected, channels, intparams, doubparams);
}
void QtFilterWidget::smoothOK_event(void)
{
	filter_dialogue->accept();
	delete filter_dialogue;
	filter_dialogue=NULL;

	filter_selected=FILTER_GAUSSIAN;

	vector<int> intparams;
	vector<double> doubparams;
	
	//2d / 3d mode
	intparams.push_back(1);

	//kernel size
	intparams.push_back(3);

	//gaussian strength
	doubparams.push_back(0.5);
	emit filter_signal(filter_selected, channels, intparams, doubparams);
}

void QtFilterWidget::collapse_event(void)
{
	filter_selected=FILTER_COLLAPSE;

	vector<int> intparams;
	vector<double> doubparams;
 
	emit filter_signal(filter_selected, channels, intparams, doubparams);
}
void QtFilterWidget::filtercancel_event(void)
{
	filter_dialogue->accept();
	delete filter_dialogue;

	emit filter_maxflow_interact_signal(-1);

	filter_dialogue = NULL;
}
void QtFilterWidget::exit(void)
{
}
void QtFilterWidget::reset_button_event(void)
{
}
void QtFilterWidget::axis_button_event(int selected)
{
	printf("axis_button_event\n");

	if(xaxisbutton->isChecked())
	{
		printf("x on\n");
		axis[0]=true;
	}
	else
	{
		printf("x off\n");
		axis[0]=false;
	}

	if(yaxisbutton->isChecked())
	{
		printf("y on\n");
		axis[1]=true;
	}
	else
	{
		printf("y off\n");
		axis[1]=false;
	}

	if(zaxisbutton->isChecked())
	{
		printf("z on\n");
		axis[2]=true;
	}
	else
	{
		printf("z off\n");
		axis[2]=false;
	}
}
void QtFilterWidget::channel_button_event(int selected)
{
	if(selected==0)
	{
		channels[0] = true;
		channels[1] = false;
		channels[2] = false;
	}
	else if(selected==1)
	{
		channels[0] = false;
		channels[1] = true;
		channels[2] = false;
	}
	else if(selected==2)
	{
		channels[0] = false;
		channels[1] = false;
		channels[2] = true;
	}
}
void QtFilterWidget::load_event(void)
{
	emit filter_maxflow_open_signal();
}
void QtFilterWidget::save_event(void)
{
	emit filter_maxflow_save_signal();
}
void QtFilterWidget::easymode(bool state)
{
	if(state==true)
	{
		easyfilters_grp->show();

		channelsop_grp->hide();
		mortho_grp->hide();
		filters_grp->hide();
		histogram_grp->hide();
		regions_grp->hide();
#ifdef OPENCL_SUPPORT
		gpufilters_grp->hide();
#endif
	}
	else
	{
		easyfilters_grp->hide();

		channelsop_grp->show();
		mortho_grp->show();
		filters_grp->show();
		regions_grp->show();


		histogram_grp->show();
#ifdef OPENCL_SUPPORT
		gpufilters_grp->show();
#endif
	}

	adjustSize();
}
void QtFilterWidget::maxflow_brushparam_event(const QString & txt)
{
//	printf("maxflow_brushparam_event\n");
	float size = txt.toFloat();
	emit filter_maxflow_brush_signal(size);
}