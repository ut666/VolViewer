/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include <qlayout.h>
#include <q3frame.h>
#include <qnamespace.h>
#include <qsplitter.h>
#include <qpushbutton.h>

#include "QtGLObjWin.h"
#include "QtGLWidget.h"

#include <QPixmap>
#include <QLabel>

QtGLObjectWindow::QtGLObjectWindow( QWidget* parent, const char* name, QString abbsapppath)
    : QWidget( parent, name )
{
	apppath = abbsapppath;
	//===================================================
	// Style helpers
	//===================================================
	QFont font("Monospace");
	font.setPixelSize(14);
	font.setStyleHint(QFont::TypeWriter);

	QPalette pal;
	pal.setColor(QPalette::Background, Qt::white);
	pal.setColor(QPalette::Button, Qt::white);

	int height = 32;

	//===================================================
	 //Qtframe to put our GLWidgets inside.
	
	glvoxelframe = new QFrame( this, "glvoxelframe" );
	glvoxelframe->setFrameStyle( Q3Frame::Sunken | Q3Frame::Panel );
	glvoxelframe->setLineWidth(1);

	//===================================================
	// Create OpenGL widget and put it inside the fame

	//===================================================
	//voxel view
	//===================================================    
	QGLFormat glformat;
	glformat = QGLFormat(QGL::DoubleBuffer|QGL::DepthBuffer|QGL::AlphaChannel|QGL::DirectRendering|QGL::StereoBuffers);

	glvoxel = NULL;
	glvoxel	  = new QtGLWidget(glformat, glvoxelframe, "glvoxel", 0);
	
	QBoxLayout* voxel_layout = new QBoxLayout(glvoxelframe, QBoxLayout::TopToBottom, 0, 0,"flayout1");
	voxel_layout->setMargin(0);
	voxel_layout->setSpacing(0);
	voxel_layout->addWidget(glvoxel);
	voxel_layout->setResizeMode(QLayout::FreeResize);

	//===================================================
	//	OUR 3 SLICE WIDGETS XY, XZ, YZ,
	//===================================================
	slice_arrayXY=0;
	slice_arrayXZ=0;
	slice_arrayYZ=0;

		//===================================================
		// XY SLICES
		//===================================================
		// XY PIXMAP LABEL
		xylb = new QtSliceFrame(this);
		xylb->mode = 0;
		//===================================================
		// XY SLIDER
		slider_xy = new QSlider ( 0, 0, 1, 0, Qt::Vertical, this, "zsl" );
		slider_xy->setTickmarks( QSlider::Left );
		//slider_xy->setTracking(TRUE);
		//slider_xy->setStyle(new QMotifPlusStyle );
		connect( slider_xy, SIGNAL(valueChanged(int)), this, SLOT(makeXYPixmapEvent(int)) );
		//===================================================
		// XY SLIDER LABEL
 		//slider_xy_lb = new QLabel( "   0", this );
		//slider_xy_lb->setFont( QFont("System", 12 ) );
		//===================================================
		// XY SLIDER LAYOUT (XY SLIDER + XY SLIDER LABEL)
		QBoxLayout* slider_xy_layout = new QBoxLayout(QBoxLayout::TopToBottom, -1, "slider_xy_layout");
  		slider_xy_layout->setMargin(0);
		slider_xy_layout->setSpacing (0);
		slider_xy_layout->addWidget( slider_xy);
		//slider_xy_layout->addWidget( slider_xy_lb);
		//===================================================
		// XY LAYOUT (XY LABEL + XY SLIDER + XY SLIDER LABEL)
		QBoxLayout * slicexy_layout = new QBoxLayout (QBoxLayout::LeftToRight, -1, "slicexy_layout");
  		slicexy_layout->setMargin(0);
		slicexy_layout->setSpacing (0);
		slicexy_layout->addWidget( xylb,0);
		slicexy_layout->addLayout( slider_xy_layout,0);
		//===================================================
		QFrame *xyframe = new QFrame(this);
		xyframe->setFrameShape(QFrame::StyledPanel);
		xyframe->setLayout(slicexy_layout);
		xydock = new QDockWidget(tr("XY"), this);
		xydock->setAllowedAreas(Qt::AllDockWidgetAreas);
		xydock->setWidget(xyframe);
		xydock->setObjectName("xydock");
		
		//===================================================

		//===================================================
		// XZ SLICES
		//===================================================
		// XZ PIXMAP LABEL
		xzlb = new QtSliceFrame(this);
		xzlb->mode = 1;
		//===================================================
		// XZ SLIDER  
		slider_xz = new QSlider ( 0, 0, 1, 0, Qt::Vertical, this, "ysl" );
		slider_xz->setTickmarks( QSlider::Left );
		slider_xz->setTracking(TRUE);
		connect( slider_xz, SIGNAL(valueChanged(int)), this, SLOT(makeXZPixmapEvent(int)) );
		//===================================================
		// XZ SLIDER LABEL
//		slider_xz_lb = new QLabel(this, "   0" );
//		slider_xz_lb->setFont( QFont( "System", 12 ) );
		//===================================================
		// XZ SLIDER LAYOUT (XZ SLIDER + XZ SLIDER LABEL)
		QBoxLayout* slider_xz_layout = new QBoxLayout( QBoxLayout::TopToBottom, -1, "slider_xz_layout");
  		slider_xz_layout->setMargin(0);
		slider_xz_layout->setSpacing (0);
		slider_xz_layout->addWidget( slider_xz);
//		slider_xz_layout->addWidget( slider_xz_lb);
		//===================================================
		// XZ LAYOUT (XZ LABEL + XZ SLIDER LAYOUT)
		QBoxLayout * slicexz_layout = new QBoxLayout (QBoxLayout::LeftToRight, -1, "slicexz_layout");
 		slicexz_layout->setMargin(0);
		slicexz_layout->setSpacing (0);
		slicexz_layout->addWidget( xzlb,0);
		slicexz_layout->addLayout( slider_xz_layout,0);
		//===================================================
		QFrame *xzframe = new QFrame(this);
		xzframe->setFrameShape(QFrame::StyledPanel);
		xzframe->setLayout(slicexz_layout);
		xzdock = new QDockWidget(tr("XZ"), this);
		xzdock->setAllowedAreas(Qt::AllDockWidgetAreas);
		xzdock->setWidget(xzframe);
		xzdock->setObjectName("xzdock");
		//===================================================

		//===================================================
		// YZ SLICES
		//===================================================
		  // YZ PIXMAP LABEL
		yzlb = new QtSliceFrame(this);
		yzlb->mode = 2;
		//===================================================
		// YZ SLIDER
		slider_yz = new QSlider ( 0, 0, 1, 0, Qt::Vertical, this, "xsl" );
		slider_yz->setTickmarks( QSlider::Left );
		slider_yz->setTracking(TRUE);
		connect( slider_yz, SIGNAL(valueChanged(int)), this, SLOT(makeYZPixmapEvent(int)) );
		//===================================================
		// YZ SLIDER LABEL
	//	slider_yz_lb = new QLabel( "   0", this );
	//	slider_yz_lb->setFont( QFont( "System", 12 ) );
		//===================================================
		// YZ SLIDER LAYOUT (YZ SLIDER + YZ SLIDER LABEL)
		QBoxLayout* slider_yz_layout = new QBoxLayout( QBoxLayout::TopToBottom, -1, "slider_yz_layout");
 		slider_yz_layout->setMargin(0);
		slider_yz_layout->setSpacing (0);
		slider_yz_layout->addWidget( slider_yz);
	//	slider_yz_layout->addWidget( slider_yz_lb);
		//===================================================
		// YZ LAYOUT (YZ LABEL + YZ SLIDER LAYOUT)
		QBoxLayout * sliceyz_layout = new QBoxLayout (QBoxLayout::LeftToRight, -1, "sliceyz_layout");
		sliceyz_layout->setMargin(0);
		sliceyz_layout->setSpacing (0);
		sliceyz_layout->addWidget( yzlb,0);
		sliceyz_layout->addLayout( slider_yz_layout,0);
		//===================================================
		QFrame *yzframe = new QFrame(this);
		yzframe->setFrameShape(QFrame::StyledPanel);
		yzframe->setLayout(sliceyz_layout);
		yzdock = new QDockWidget(tr("YZ"), this);
		yzdock->setAllowedAreas(Qt::AllDockWidgetAreas);
		yzdock->setWidget(yzframe);
		yzdock->setObjectName("yzdock");
		//===================================================

	//===================================================
	// Slice Operator widgets
	//===================================================
	QPushButton* zoominXYbtn = new QPushButton(QIcon(QPixmap(apppath+"/Icons/zoomin.png")), "", this, "zoominXYbtn");
	zoominXYbtn->setFixedSize(24,24);
	connect( zoominXYbtn, SIGNAL( clicked () ), xylb, SLOT( sliceframe_zoomin_slot() ) );
	QPushButton* zoomoutXYbtn = new QPushButton(QIcon(QPixmap(apppath+"/Icons/zoomout.png")),"", this, "zoomoutXYbtn");
	zoomoutXYbtn->setFixedSize(24,24);
	connect( zoomoutXYbtn, SIGNAL( clicked () ), xylb, SLOT( sliceframe_zoomout_slot() ) );
	QPushButton* resetzoomXYbtn = new QPushButton(QIcon(QPixmap(apppath+"/Icons/zoomreset.png")),"", this, "resetzoomXYbtn");
	resetzoomXYbtn->setFixedSize(24,24);
	connect( resetzoomXYbtn, SIGNAL( clicked () ), xylb, SLOT( sliceframe_zoomreset_slot() ) );
	QPushButton* sectioncursorXYbtn = new QPushButton(QIcon(QPixmap(apppath+"/Icons/sectioncursor.png")),"", this, "sectioncursorXYbtn");
	sectioncursorXYbtn->setFixedSize(24,24);
	sectioncursorXYbtn->setToggleButton(true);
	sectioncursorXYbtn->setOn(true);
	connect( sectioncursorXYbtn, SIGNAL( clicked () ), xylb, SLOT( sliceframe_sectioncursor_slot() ) );
	QPushButton* sectioninvertXYbtn = new QPushButton(QIcon(QPixmap(apppath+"/Icons/sectioninvert.png")),"", this, "sectioninvertXYbtn");
	sectioninvertXYbtn->setFixedSize(24,24);
	sectioninvertXYbtn->setToggleButton(true);
	sectioninvertXYbtn->setOn(false);
	connect( sectioninvertXYbtn, SIGNAL( clicked () ), xylb, SLOT( sliceframe_sectioninvert_slot() ) );

	// XY SLIDER LABEL
	slider_xy_lb = new QLabel( "XY-section | 0", this );
	slider_xy_lb->setFont(font);

	//Our zoom buttons
	QBoxLayout * topbXYlayout = new QBoxLayout ( 0, QBoxLayout::LeftToRight, 0, -1, "blayout3");
	topbXYlayout->setAlignment(Qt::AlignRight);
	topbXYlayout->addWidget(slider_xy_lb, 1, Qt::AlignLeft);
	topbXYlayout->addWidget(zoominXYbtn, 1);
	topbXYlayout->addWidget(zoomoutXYbtn, 1);
	topbXYlayout->addWidget(resetzoomXYbtn, 1);
	topbXYlayout->addWidget(sectioncursorXYbtn, 1);
	topbXYlayout->addWidget(sectioninvertXYbtn, 1);

	QFrame *topbXYframe = new QFrame(this);
	topbXYframe->setFrameShape(QFrame::StyledPanel);
	topbXYframe->setLayout(topbXYlayout);
	
	xydock->setTitleBarWidget(topbXYframe);

	//===================================================

	QPushButton* zoominYZbtn = new QPushButton(QIcon(QPixmap(apppath+"/Icons/zoomin.png")), "", this, "zoominYZbtn");
	zoominYZbtn->setFixedSize(24,24);
	connect( zoominYZbtn, SIGNAL( clicked () ), yzlb, SLOT( sliceframe_zoomin_slot() ) );
	QPushButton* zoomoutYZbtn = new QPushButton(QIcon(QPixmap(apppath+"/Icons/zoomout.png")),"", this, "zoomoutYZbtn");
	zoomoutYZbtn->setFixedSize(24,24);
	connect( zoomoutYZbtn, SIGNAL( clicked () ), yzlb, SLOT( sliceframe_zoomout_slot() ) );
	QPushButton* resetzoomYZbtn = new QPushButton(QIcon(QPixmap(apppath+"/Icons/zoomreset.png")),"", this, "resetzoomYZbtn");
	resetzoomYZbtn->setFixedSize(24,24);
	connect( resetzoomYZbtn, SIGNAL( clicked () ), yzlb, SLOT( sliceframe_zoomreset_slot() ) );
	QPushButton* sectioncursorYZbtn = new QPushButton(QIcon(QPixmap(apppath+"/Icons/sectioncursor.png")),"", this, "sectioncursorYZbtn");
	sectioncursorYZbtn->setFixedSize(24,24);
	sectioncursorYZbtn->setToggleButton(true);
	sectioncursorYZbtn->setOn(true);
	connect( sectioncursorYZbtn, SIGNAL( clicked () ), yzlb, SLOT( sliceframe_sectioncursor_slot() ) );
	QPushButton* sectioninvertYZbtn = new QPushButton(QIcon(QPixmap(apppath+"/Icons/sectioninvert.png")),"", this, "sectioninvertYZbtn");
	sectioninvertYZbtn->setToggleButton(true);
	sectioninvertYZbtn->setOn(false);
	sectioninvertYZbtn->setFixedSize(24,24);
	connect( sectioninvertYZbtn, SIGNAL( clicked () ), yzlb, SLOT( sliceframe_sectioninvert_slot() ) );

	// YZ SLIDER LABEL
	slider_yz_lb = new QLabel( "YZ-section | 0", this );
	slider_yz_lb->setFont(font);

	//Our zoom buttons
	QBoxLayout * topbYZlayout = new QBoxLayout ( 0, QBoxLayout::LeftToRight, 0, -1, "topbYZlayout");
	topbYZlayout->setAlignment(Qt::AlignRight);
	topbYZlayout->addWidget(slider_yz_lb, 1, Qt::AlignLeft);
	topbYZlayout->addWidget(zoominYZbtn, 1);
	topbYZlayout->addWidget(zoomoutYZbtn, 1);
	topbYZlayout->addWidget(resetzoomYZbtn, 1);
	topbYZlayout->addWidget(sectioncursorYZbtn, 1);
	topbYZlayout->addWidget(sectioninvertYZbtn, 1);

	QFrame *topbYZframe = new QFrame(this);
	topbYZframe->setFrameShape(QFrame::StyledPanel);
	topbYZframe->setLayout(topbYZlayout);
	
	yzdock->setTitleBarWidget(topbYZframe);

	//===================================================

	QPushButton* zoominXZbtn = new QPushButton(QIcon(QPixmap(apppath+"/Icons/zoomin.png")), "", this, "zoominXZbtn");
	zoominXZbtn->setFixedSize(24,24);
	connect( zoominXZbtn, SIGNAL( clicked () ), xzlb, SLOT( sliceframe_zoomin_slot() ) );
	QPushButton* zoomoutXZbtn = new QPushButton(QIcon(QPixmap(apppath+"/Icons/zoomout.png")),"", this, "zoomoutXZbtn");
	zoomoutXZbtn->setFixedSize(24,24);
	connect( zoomoutXZbtn, SIGNAL( clicked () ), xzlb, SLOT( sliceframe_zoomout_slot() ) );
	QPushButton* resetzoomXZbtn = new QPushButton(QIcon(QPixmap(apppath+"/Icons/zoomreset.png")),"", this, "resetzoomXZbtn");
	resetzoomXZbtn->setFixedSize(24,24);
	connect( resetzoomXZbtn, SIGNAL( clicked () ), xzlb, SLOT( sliceframe_zoomreset_slot() ) );
	QPushButton* sectioncursorXZbtn = new QPushButton(QIcon(QPixmap(apppath+"/Icons/sectioncursor.png")),"", this, "sectioncursorXZbtn");
	sectioncursorXZbtn->setFixedSize(24,24);
	sectioncursorXZbtn->setToggleButton(true);
	sectioncursorXZbtn->setOn(true);
	connect( sectioncursorXZbtn, SIGNAL( clicked () ), xzlb, SLOT( sliceframe_sectioncursor_slot() ) );
	QPushButton* sectioninvertXZbtn = new QPushButton(QIcon(QPixmap(apppath+"/Icons/sectioninvert.png")),"", this, "sectioninvertXZbtn");
	sectioninvertXZbtn->setToggleButton(true);
	sectioninvertXZbtn->setOn(false);
	sectioninvertXZbtn->setFixedSize(24,24);
	connect( sectioninvertXZbtn, SIGNAL( clicked () ), xzlb, SLOT( sliceframe_sectioninvert_slot() ) );
	
	// XZ SLIDER LABEL
	slider_xz_lb = new QLabel( "XZ-section | 0", this );
	slider_xz_lb->setFont(font);

	//Our zoom buttons
	QBoxLayout * topbXZlayout = new QBoxLayout ( 0, QBoxLayout::LeftToRight, 0, -1, "topbYZlayout");
	topbXZlayout->setAlignment(Qt::AlignRight);
	topbXZlayout->addWidget(slider_xz_lb, 1, Qt::AlignLeft);
	topbXZlayout->addWidget(zoominXZbtn, 1);
	topbXZlayout->addWidget(zoomoutXZbtn, 1);
	topbXZlayout->addWidget(resetzoomXZbtn, 1);
	topbXZlayout->addWidget(sectioncursorXZbtn, 1);
	topbXZlayout->addWidget(sectioninvertXZbtn, 1);

	QFrame *topbXZframe = new QFrame(this);
	topbXZframe->setFrameShape(QFrame::StyledPanel);
	topbXZframe->setLayout(topbXZlayout);
	
	xzdock->setTitleBarWidget(topbXZframe);

	//===================================================

 /*   topbdock = new QDockWidget(tr("Slices"), this);
	topbdock->setAllowedAreas(Qt::AllDockWidgetAreas);
	topbdock->setWidget(topbframe);
	topbdock->setFixedHeight(48);
	topbdock->setFeatures(QDockWidget::NoDockWidgetFeatures);*/

	//===================================================
	// Cursor info Widgets
	//===================================================
	cursor_label = new QLabel( this, "cursor_label", Qt::Widget );
	cursor_label->setBackgroundColor(Qt::white);
	cursor_label->setAutoFillBackground(true);
	cursor_label->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
	cursor_label->setMaximumHeight(32);
	cursor_label->setText(" | <b>cursor:</b> [x:0 y:0 z:0][r:0 g:0 b:0] ");
	cursor_label->setFont(font);	
	
    cursordock = new QDockWidget(tr("Cursor"), this);
    cursordock->setAllowedAreas(Qt::AllDockWidgetAreas);
	cursordock->setWidget(cursor_label);
	cursordock->setFeatures(QDockWidget::DockWidgetVerticalTitleBar|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetClosable);
	cursordock->setAutoFillBackground(true);
	cursordock->setPalette(pal);
	cursordock->setFixedHeight(height);
	cursordock->setObjectName("cursordock");

	//===================================================
	//File info Widgets
	//===================================================
	info_label = new QLabel( this, "cursor_label", Qt::Widget );
	info_label->setBackgroundColor(Qt::white);
	info_label->setAutoFillBackground(true);
	info_label->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
	info_label->setMaximumHeight(32);
	info_label->setText(" <b>data:</b> [0x0x0x0] | <b>file:</b> [none]");
	info_label->setFont(font);	

	infodock = new QDockWidget(tr("File"), this);
    infodock->setAllowedAreas(Qt::AllDockWidgetAreas);
	infodock->setWidget(info_label);
	infodock->setFeatures(QDockWidget::DockWidgetVerticalTitleBar|QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetClosable);
	infodock->setBackgroundColor(Qt::white);
	infodock->setAutoFillBackground(true);
	pal = infodock->palette();
	pal.setColor(QPalette::Background, Qt::white);
	pal.setColor(QPalette::Button, Qt::white);
	infodock->setPalette(pal);
	infodock->setFixedHeight(height);
	infodock->setObjectName("infodock");

	QWidget* titlebar0 = new QWidget();
	infodock->setTitleBarWidget(titlebar0);
	QWidget* titlebar1 = new QWidget();
	cursordock->setTitleBarWidget(titlebar1);

	sections_toggle=0;
	create();
}
QtGLObjectWindow::~QtGLObjectWindow()
{	
	glvoxel=NULL;

	//lets be clean
	destroy();
}
void QtGLObjectWindow::destroy(void)
{
//	xylb->slice_pixmap = new QPixmap(100, 100);
//	xylb->slice_pixmap->fill(Qt::black);

//	xzlb->slice_pixmap = new QPixmap(100, 100);
//	xzlb->slice_pixmap->fill(Qt::black);

//	yzlb->slice_pixmap = new QPixmap(100, 100);
//	yzlb->slice_pixmap->fill(Qt::black);

//	if(slice_arrayXY!=NULL) delete[] slice_arrayXY;
//	if(slice_arrayXZ!=NULL) delete[] slice_arrayXZ;
//	if(slice_arrayYZ!=NULL) delete[] slice_arrayYZ;
}
void QtGLObjectWindow::create(void)
{
//	printf("Trying to create slice frames!\n");
	
	if(glvoxel->volobject.texwidth==1&&glvoxel->volobject.texheight==1&&glvoxel->volobject.texdepth==1) return;
	if(glvoxel->volobject.texture3d==NULL) return;
	if(glvoxel->volrender.volume_render_mode==4) return;

//	printf("Creating slice frames!\n");
	
//	progressbar = new Q3ProgressDialog("Creating 2D Slices...", "Cancel", 4, 0, "progress", TRUE);		
//	progressbar->setCaption("Please Wait");
	
	//update our progress bar
//    progressbar->setProgress(1);

	//XY SLICE
	//======================================
	//create an array to store our XY slices
	if(slice_arrayXY) delete[] slice_arrayXY;
	slice_arrayXY = new unsigned char[3*glvoxel->volobject.texwidth*glvoxel->volobject.texheight];
	
	for(int i=0; i<3*glvoxel->volobject.texwidth*glvoxel->volobject.texheight; i++) slice_arrayXY[i] = 0;

	//set its slider according to what we have read in
	if(slider_xy)
	{
		slider_xy->setMaxValue(glvoxel->volobject.texdepth-1);
		slider_xy->setTickInterval(glvoxel->volobject.texdepth/8);
		slider_xy->setTracking(true);
		slider_xy->setValue(glvoxel->volobject.texdepth/2.0);
	}

	//let frame obj now about it
	makeXYPixmapEvent(glvoxel->volobject.texdepth/2.0);
	glvoxel->volrender.z_slice = glvoxel->volobject.texdepth/2.0;
	xylb->z = glvoxel->volobject.texdepth/2.0;
	xzlb->z = glvoxel->volobject.texdepth/2.0;
	yzlb->z = glvoxel->volobject.texdepth/2.0;
	xylb->zres = glvoxel->volobject.texdepth;
	xzlb->zres = glvoxel->volobject.texdepth;
	yzlb->zres = glvoxel->volobject.texdepth;

	//update our progress bar
//    progressbar->setProgress(2);

	//XZ SLICE
	//======================================
	//create an array to store our XY slices
	if(slice_arrayXZ) delete[] slice_arrayXZ;
	slice_arrayXZ = new unsigned char[3*glvoxel->volobject.texwidth*glvoxel->volobject.texdepth];
	for(int i=0; i<3*glvoxel->volobject.texwidth*glvoxel->volobject.texdepth; i++) slice_arrayXZ[i] = 0;
		
	//cout<<"slice size: "<<4*glvoxel->volobject.texwidth*glvoxel->volobject.texdepth<<endl;

	//set its slider according to what we have read in
	if(slider_xz)
	{
		slider_xz->setMaxValue(glvoxel->volobject.texheight-1);
		slider_xz->setTickInterval(glvoxel->volobject.texheight/8);
		slider_xz->setTracking(true);
		slider_xz->setValue(glvoxel->volobject.texheight/2.0);
	}

	//let frame obj now about it
	makeXZPixmapEvent((int)(glvoxel->volobject.texheight/2.0));
	glvoxel->volrender.y_slice = glvoxel->volobject.texheight/2.0;
	xylb->y = glvoxel->volobject.texheight/2.0;
	xzlb->y = glvoxel->volobject.texheight/2.0;
	yzlb->y = glvoxel->volobject.texheight/2.0;	
	xylb->yres = glvoxel->volobject.texheight;
	xzlb->yres = glvoxel->volobject.texheight;
	yzlb->yres = glvoxel->volobject.texheight;

	//update our progress bar
//    progressbar->setProgress(3);

	//YZ SLICE
	//======================================
	//create an array to store our XY slices
	if(slice_arrayYZ) delete[] slice_arrayYZ;
	slice_arrayYZ = new unsigned char[3*glvoxel->volobject.texheight*glvoxel->volobject.texdepth];
	for(int i=0; i<3*glvoxel->volobject.texheight*glvoxel->volobject.texdepth; i++) slice_arrayYZ[i] = 0;
		
	//set its slider according to what we have read in
	if(slider_yz)
	{
		slider_yz->setMaxValue(glvoxel->volobject.texwidth-1);
		slider_yz->setTickInterval(glvoxel->volobject.texwidth/8);
		slider_yz->setTracking(true);
		slider_yz->setValue(glvoxel->volobject.texwidth/2.0);
	}

	//let frame obj now about it
	makeYZPixmapEvent((int)(glvoxel->volobject.texwidth/2.0));	
	glvoxel->volrender.x_slice = glvoxel->volobject.texwidth/2.0;
	xylb->x = glvoxel->volobject.texwidth/2.0;
	xzlb->x = glvoxel->volobject.texwidth/2.0;
	yzlb->x = glvoxel->volobject.texwidth/2.0;	
	xylb->xres = glvoxel->volobject.texwidth;
	xzlb->xres = glvoxel->volobject.texwidth;
	yzlb->xres = glvoxel->volobject.texwidth;

//	makeXYPixmapEvent(glvoxel->volobject.texdepth/2.0);
//	makeXZPixmapEvent(glvoxel->volobject.texheight/2.0);
//	makeYZPixmapEvent(glvoxel->volobject.texwidth/2.0);

	xzlb->initPixmap(xzlb->slice_pixmap);
	xylb->initPixmap(xylb->slice_pixmap);
	yzlb->initPixmap(yzlb->slice_pixmap);

	//update our progress bar
/*    progressbar->setProgress(4);
	delete progressbar;
	progressbar=NULL;*/
}
void QtGLObjectWindow::save_sectionimage_slot(QString fn, int axis, int index)
{
	if(axis==0)
	{
		makeYZPixmapEvent(index);		
		if(!yzlb->slice_pixmap->isNull()) yzlb->slice_pixmap->save(fn, "PNG");
	}
	if(axis==1)
	{
		makeXZPixmapEvent(index);		
		if(!xzlb->slice_pixmap->isNull()) xzlb->slice_pixmap->save(fn, "PNG");
	}
	if(axis==2)
	{
		makeXYPixmapEvent(index);		
		if(!xylb->slice_pixmap->isNull()) xylb->slice_pixmap->save(fn, "PNG");
	}
}
void QtGLObjectWindow::makeXYPixmapEvent(int z)
{ 
	//check we have some data
	if(glvoxel->volobject.texwidth==1 || glvoxel->volobject.texheight==1 || glvoxel->volobject.texdepth==1) return;
	if(z>glvoxel->volobject.texdepth-1)	z = glvoxel->volobject.texdepth-1;
	if(z<0)	z = 0;
	if(glvoxel->volobject.texture3d==NULL) return;

	//index helpers
	int index_2D;
	int index_3D;

	//save the slice #
	glvoxel->volrender.z_slice = z;
	xylb->z = z;
	xzlb->z = z;
	yzlb->z = z;
	
	int r,g,b;
	int result_r, result_g, result_b;
	
	//get the slice the user has asked for
	for(int x=0; x<glvoxel->volobject.texwidth; x++)
	{
		for(int y=0; y<glvoxel->volobject.texheight; y++)
		{
			index_2D = 3*(((y)*glvoxel->volobject.texwidth)+(x));

			result_r = r = glvoxel->volobject.get_value(x,y,z,0);
			result_g = g = glvoxel->volobject.get_value(x,y,z,1);
			result_b = b = glvoxel->volobject.get_value(x,y,z,2);
			
			//if(glvoxel->volobject.is_greyscale) result_g = result_b = result_r;

 			slice_arrayXY[index_2D+0] = (unsigned char) result_r;
			slice_arrayXY[index_2D+1] = (unsigned char) result_g;
			slice_arrayXY[index_2D+2] = (unsigned char) result_b;
		}
	}

	//upload our slice to gfx card
	glBindTexture (GL_TEXTURE_2D, glvoxel->volrender.SLICEZ); 
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
	//glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, glvoxel->volobject.texwidth, glvoxel->volobject.texheight, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)slice_arrayXY);

	//create a QImage using our RAW RGB data
	QImage xy_image = QImage(slice_arrayXY, glvoxel->volobject.texwidth, glvoxel->volobject.texheight, QImage::Format_RGB888);

	xylb->resizeX = (glvoxel->volobject.xsize/glvoxel->volobject.maxsize);
	xylb->resizeY = (glvoxel->volobject.ysize/glvoxel->volobject.maxsize);

//	printf("resizedslice: %f, %f\n", xy_image.width()*xylb->resizeX, xy_image.height()*xylb->resizeY);
	
	if(xylb->resizeX!=xylb->resizeY)
	{	
		/*printf("XY slice: %d, %d\n", xy_image.width(), xy_image.height());
		printf("XY resize: %f, %f\n", xylb->resizeX, xylb->resizeY);*/

		float scaleX = xylb->resizeX;
		float scaleY = xylb->resizeY;

		if(xylb->resizeX<1.0)
		{
			scaleX = 1.0;
			scaleY = xylb->resizeY * 1.0/xylb->resizeX;
		}
		else if(xylb->resizeY<1.0)
		{
			scaleY = 1.0;
			scaleX = xylb->resizeX * 1.0/xylb->resizeY;
		}
		//printf("XY scaler: %f, %f\n", scaleX, scaleY);

		xy_image = xy_image.scaled(xy_image.width()*scaleX, xy_image.height()*scaleX, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		//printf("XY scaled slice: %d, %d\n", xy_image.width(), xy_image.height());

		xylb->resizeX= scaleX;
		xylb->resizeY= scaleY;
	}
	else
	{
		xylb->resizeX=1.0;
		xylb->resizeY=1.0;
	}

	//create a QPixmap using our QImage
	xy_pixmap.convertFromImage(xy_image);

	//if we are successful update the label
    if ( !xy_pixmap.isNull() ) 
	{ 
		//xylb->draw();
		xzlb->draw();
		yzlb->draw();
		xylb->updatePixmap( &xy_pixmap );
    }
	//else display an error message in the frame
    else 
	{
//		xylb->setText( "ERROR: makeXYPixmap()" );
    }
	
	//Our slider label value
	QString s;
	QString n;
	n.setNum(z);

	s = "XY-section | ";
	s += n;
	slider_xy_lb->setText(s);

	if(slider_xy) slider_xy->setValue(z);
}
void QtGLObjectWindow::makeXZPixmapEvent(int y)
{
	//check we have some data
	if(glvoxel->volobject.texwidth==1 || glvoxel->volobject.texheight==1 || glvoxel->volobject.texdepth==1) return;
	if(y>glvoxel->volobject.texheight-1)	y = glvoxel->volobject.texheight-1;
	if(y<0)	y = 0;
	if(glvoxel->volobject.texture3d==NULL) return;

	//index helpers
	int index_2D;
	int index_3D;

	//save the slice #
	glvoxel->volrender.y_slice = y;
	xylb->y = y;
	xzlb->y = y;
	yzlb->y = y;

	int r,g,b;
	int result_r, result_g, result_b;
	//get the slice the user has asked for
	for(int x=0; x<glvoxel->volobject.texwidth; x++)
	{
		for(int z=0; z<glvoxel->volobject.texdepth; z++)
		{
			index_2D = 3*((z*glvoxel->volobject.texwidth)+x);
		
			result_r = r = glvoxel->volobject.get_value(x,y,z,0);
			result_g = g = glvoxel->volobject.get_value(x,y,z,1);
			result_b = b = glvoxel->volobject.get_value(x,y,z,2);
						
			//if(glvoxel->volobject.is_greyscale) result_g = result_b = result_r;

			slice_arrayXZ[index_2D+0] = result_r;
			slice_arrayXZ[index_2D+1] = result_g;
			slice_arrayXZ[index_2D+2] = result_b;
		}
	}

	//upload our slice to gfx card
	glBindTexture (GL_TEXTURE_2D, glvoxel->volrender.SLICEY); 
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
//	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, glvoxel->volobject.texwidth, glvoxel->volobject.texdepth, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)slice_arrayXZ);

	//create a QImage using our RAW RGB data
	QImage xz_image = QImage(slice_arrayXZ, glvoxel->volobject.texwidth, glvoxel->volobject.texdepth, QImage::Format_RGB888);
	
	xzlb->resizeX = (glvoxel->volobject.xsize/glvoxel->volobject.maxsize);
	xzlb->resizeY = (glvoxel->volobject.zsize/glvoxel->volobject.maxsize);

	if(xzlb->resizeX!=xzlb->resizeY)
	{
		//printf("XZ slice: %d, %d\n", xz_image.width(), xz_image.height());
		//printf("XZ resize: %f, %f\n", xzlb->resizeX, xzlb->resizeY);

		float scaleX = xzlb->resizeX;
		float scaleY = xzlb->resizeY;

		if(xzlb->resizeX<1.0)
		{
			scaleX = 1.0;
			scaleY = xzlb->resizeY * 1.0/xzlb->resizeX;
		}
		else if(xzlb->resizeY<1.0)
		{
			scaleY = 1.0;
			scaleX = xzlb->resizeX * 1.0/xzlb->resizeY;
		}
		//printf("XZ scaler: %f, %f\n", scaleX, scaleY);

		xz_image = xz_image.scaled(xz_image.width()*scaleX, xz_image.height()*scaleY, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		//printf("XZ scaled slice: %d, %d\n", xz_image.width(), xz_image.height());
		
		xzlb->resizeX= scaleX;
		xzlb->resizeY= scaleY;
	}
	else
	{
		xzlb->resizeX=1.0;
		xzlb->resizeY=1.0;
	}

	//printf("sliceXZ WxH: %d, %d\n", xz_image.width(), xz_image.height());
	//xz_image = xz_image.scaled(xz_image.width()*(glvoxel->volobject.xsize/glvoxel->volobject.maxsize), xz_image.height()*(glvoxel->volobject.zsize/glvoxel->volobject.maxsize), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	//printf("sliceXZ WxH: %d, %d\n", xz_image.width(), xz_image.height());

	//create a QPixmap using our QImage
	xz_pixmap.convertFromImage(xz_image);

	//if we are successful update the label
	if ( !xz_pixmap.isNull() ) 
	{
		xylb->draw();
		//xzlb->draw();
		yzlb->draw();
		xzlb->updatePixmap( &xz_pixmap );
	}
 	//else display an error message in the frame
	else 
	{
//		xzlb->setText( "ERROR: makeXZPixmap()" );
	}

	//Our slider label value
	//We need to make sure our label is always same size
	//or our window will reshape due to new size...
	QString s;
	QString n;
	n.setNum(y);

	s = "XZ-section | ";
	s += n;
	slider_xz_lb->setText(s);
	if(slider_xz) slider_xz->setValue(y);
}
void QtGLObjectWindow::makeYZPixmapEvent(int x)
{
	//check we have some data
	if(glvoxel->volobject.texwidth==1 || glvoxel->volobject.texheight==1 || glvoxel->volobject.texdepth==1) return;
	if(x>glvoxel->volobject.texwidth-1)	x = glvoxel->volobject.texwidth-1;
	if(x<0)	x = 0;
	if(glvoxel->volobject.texture3d==NULL) return;

	//index helpers
	int index_2D;
	int index_3D;

	//save the slice #
	glvoxel->volrender.x_slice = x;
	xylb->x = x;
	xzlb->x = x;
	yzlb->x = x;

	int r,g,b;
	int result_r, result_g, result_b;
	//get the slice the user has asked for
	for(int y=0; y<glvoxel->volobject.texheight; y++)
	{
		for(int z=0; z<glvoxel->volobject.texdepth; z++)
		{
			index_2D = 3*((z*glvoxel->volobject.texheight)+y);

			result_r = r = glvoxel->volobject.get_value(x,y,z,0);
			result_g = g = glvoxel->volobject.get_value(x,y,z,1);
			result_b = b = glvoxel->volobject.get_value(x,y,z,2);

			//if(glvoxel->volobject.is_greyscale) result_g = result_b = result_r;

			slice_arrayYZ[index_2D+0] = (unsigned char) result_r;
			slice_arrayYZ[index_2D+1] = (unsigned char) result_g;
			slice_arrayYZ[index_2D+2] = (unsigned char) result_b;
		}
	}
	
	//upload our slice to gfx card
	glBindTexture (GL_TEXTURE_2D, glvoxel->volrender.SLICEX); 
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
//	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, glvoxel->volobject.texheight, glvoxel->volobject.texdepth, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)slice_arrayYZ);

	//create a QImage using our RAW RGB data
	QImage yz_image = QImage(slice_arrayYZ, glvoxel->volobject.texheight, glvoxel->volobject.texdepth, QImage::Format_RGB888);

	yzlb->resizeX = (glvoxel->volobject.ysize/glvoxel->volobject.maxsize);
	yzlb->resizeY = (glvoxel->volobject.zsize/glvoxel->volobject.maxsize);

	if(yzlb->resizeX!=yzlb->resizeY)
	{
		//printf("YZ slice: %d, %d\n", yz_image.width(), yz_image.height());
		//printf("YZ resize: %f, %f\n", yzlb->resizeX, yzlb->resizeY);

		float scaleX = yzlb->resizeX;
		float scaleY = yzlb->resizeY;

		if(yzlb->resizeX<1.0)
		{
			scaleX = 1.0;
			scaleY = yzlb->resizeY * 1.0/yzlb->resizeX;
		}
		else if(yzlb->resizeY<1.0)
		{
			scaleY = 1.0;
			scaleX = yzlb->resizeX * 1.0/yzlb->resizeY;
		}
		//printf("YZ scaler: %f, %f\n", scaleX, scaleY);

		yz_image = yz_image.scaled(yz_image.width()*scaleX, yz_image.height()*scaleY, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		//printf("YZ scaled slice: %d, %d\n", yz_image.width(), yz_image.height());

		yzlb->resizeX= scaleX;
		yzlb->resizeY= scaleY;
	}
	else
	{
		yzlb->resizeX=1.0;
		yzlb->resizeY=1.0;
	}

	//create a QPixmap using our QImage
	yz_pixmap.convertFromImage(yz_image);

	//if we are successful update the label
    if ( !yz_pixmap.isNull() ) 
	{
		xylb->draw();
		xzlb->draw();
		//yzlb->draw();
		yzlb->updatePixmap( &yz_pixmap );
   }
 	//else display an error message in the frame
    else 
	{
//		yzlb->setText( "ERROR: makeYZPixmap()" );
    }

	//Our slider label value
	//We need to make sure our label is always same size
	//or our window will reshape due to new size...
	QString s;
	QString n;
	n.setNum(x);

	s = "YZ-section | ";
	s += n;
	slider_yz_lb->setText(s);
	if(slider_yz) slider_yz->setValue(x);
}
void QtGLObjectWindow::saveimageEvent(QString filename, int mode)
{
	//check we have some data
	if(glvoxel->volobject.texwidth==1 || glvoxel->volobject.texheight==1 || glvoxel->volobject.texdepth==1) return;
	if(filename.isEmpty()) return;

	if(filename[int(filename.length()-4)]!='.') filename += ".png";
	
	if(mode==1)
	{
		//save our rendering window
		QImage im = glvoxel->volrender.render_framebuffer(false);
		QPixmap pm;
		pm.convertFromImage(im);
		im.reset();

		if(!pm.isNull()) pm.save(filename, "PNG");
	}
	else if(mode==2)
	{
		//save XY slice
		if(!xy_pixmap.isNull()) xy_pixmap.save( filename, "PNG" );
	}
	else if(mode==3)
	{
		//save XZ slice
		if(!xz_pixmap.isNull()) xz_pixmap.save( filename, "PNG" );
	}
	else if(mode==4)
	{
		//save YZ slice
		if(!yz_pixmap.isNull()) yz_pixmap.save( filename, "PNG" );
	}
	else
	{
		return;
	}
}
void QtGLObjectWindow::frames_redraw_slot(void)
{	
	//check we have some data
	if(glvoxel->volobject.texwidth==1 || glvoxel->volobject.texheight==1 || glvoxel->volobject.texdepth==1) return;

	makeXZPixmapEvent(glvoxel->volrender.y_slice);
	makeXYPixmapEvent(glvoxel->volrender.z_slice);
	makeYZPixmapEvent(glvoxel->volrender.x_slice);
//	glvoxel->redraw_event();

/*	//=============================================================
	//update histogram
	//=============================================================
	int histogram[256];
	int numb_voxels = glvoxel->volobject.texwidth*glvoxel->volobject.texheight*glvoxel->volobject.texdepth;

	//build histogram
	int i;
	for(i=0; i<256; i++)
	{
		histogram[i]=0;
	}
	for(i=0; i<numb_voxels; i++)
	{
		int val = glvoxel->volobject.get_value(i, 2);

		if(val>255) val = 255;
		if(val<0) val = 0;

		histogram[val]++;
	}

	emit histogram_signal(&histogram[0]);*/
}
void QtGLObjectWindow::filter_maxflow_brush_slot(float size)
{
	printf("brush: %f\n", size);

	xylb->brushsize = size;
	xzlb->brushsize = size;
	yzlb->brushsize = size;

	xylb->updateLabels();
	xzlb->updateLabels();
	yzlb->updateLabels();
}
void QtGLObjectWindow::sliceframe_addlabel_slot(int drawmode, Vector v)
{
	int index;
	if(drawmode==1)
	{
		float brushsize = v.w;
		for(int x=-brushsize/2.0; x<brushsize/2.0; x++)
		{
			for(int y=-brushsize/2.0; y<brushsize/2.0; y++)
			{
				index = glvoxel->volobject.get_index1D(v.x+x, v.y+y, v.z);
				glvoxel->maxflowbg_indices.push_back(index);				
				index = glvoxel->volobject.get_index1D(v.x+x, v.y, v.z+y);
				glvoxel->maxflowbg_indices.push_back(index);				
				index = glvoxel->volobject.get_index1D(v.x, v.y+x, v.z+y);
				glvoxel->maxflowbg_indices.push_back(index);				
			}
		}
	}
	else if(drawmode==2)
	{
		float brushsize = v.w;
		for(int x=-brushsize/2.0; x<brushsize/2.0; x++)
		{
			for(int y=-brushsize/2.0; y<brushsize/2.0; y++)
			{
				index = glvoxel->volobject.get_index1D(v.x+x, v.y+y, v.z);
				glvoxel->maxflowfg_indices.push_back(index);				
				index = glvoxel->volobject.get_index1D(v.x+x, v.y, v.z+y);
				glvoxel->maxflowfg_indices.push_back(index);				
				index = glvoxel->volobject.get_index1D(v.x, v.y+x, v.z+y);
				glvoxel->maxflowfg_indices.push_back(index);				
			}
		}
	}

	xylb->sliceframe_addlabel_slot(v);
	xzlb->sliceframe_addlabel_slot(v);
	yzlb->sliceframe_addlabel_slot(v);

	xylb->updateLabels();
	xzlb->updateLabels();
	yzlb->updateLabels();
}
void QtGLObjectWindow::filter_maxflow_buildlabels_slot(void)
{
	//printf("BUILD MAXFLOW LABELS\n");
	/*
	int index;
	Vector v;
	
	glvoxel->maxflowfg_indices.clear();
	glvoxel->maxflowbg_indices.clear();

	//FOREGROUND LABELS
	for(int i=0; i<xylb->fg_mflowLabels.size(); i++)
	{
		for(int j=0; j<xylb->fg_mflowLabels[i].size(); j++)
		{
			v = xylb->fg_mflowLabels[i][j];

			float brushsize = v.w;
			for(int x=-brushsize/2.0; x<brushsize/2.0; x++)
			{
				for(int y=-brushsize/2.0; y<brushsize/2.0; y++)
				{
					index = glvoxel->volobject.get_index1D(v.x+x, v.y+y, v.z);
					glvoxel->maxflowfg_indices.push_back(index);				
				}
			}		

//			index = glvoxel->volobject.get_index1D(v.x, v.y, v.z);
//			glvoxel->maxflowfg_indices.push_back(index);
		}
	}
	for(int i=0; i<xzlb->fg_mflowLabels.size(); i++)
	{
		for(int j=0; j<xzlb->fg_mflowLabels[i].size(); j++)
		{
			v = xzlb->fg_mflowLabels[i][j];

			float brushsize = v.w;
			for(int x=-brushsize/2.0; x<brushsize/2.0; x++)
			{
				for(int z=-brushsize/2.0; z<brushsize/2.0; z++)
				{
					index = glvoxel->volobject.get_index1D(v.x+x, v.y, v.z+z);
					glvoxel->maxflowfg_indices.push_back(index);				
				}
			}		
			
//			index = glvoxel->volobject.get_index1D(v.x, v.y, v.z);
//			glvoxel->maxflowfg_indices.push_back(index);
		}
	}
	for(int i=0; i<yzlb->fg_mflowLabels.size(); i++)
	{
		for(int j=0; j<yzlb->fg_mflowLabels[i].size(); j++)
		{
			v = yzlb->fg_mflowLabels[i][j];

			float brushsize = v.w;
			for(int y=-brushsize/2.0; y<brushsize/2.0; y++)
			{
				for(int z=-brushsize/2.0; z<brushsize/2.0; z++)
				{
					index = glvoxel->volobject.get_index1D(v.x, v.y+y, v.z+z);
					glvoxel->maxflowfg_indices.push_back(index);				
				}
			}		
			
//			index = glvoxel->volobject.get_index1D(v.x, v.y, v.z);
//			glvoxel->maxflowfg_indices.push_back(index);
		}
	}

	//BACKGROUND LABELS
	for(int i=0; i<xylb->bg_mflowLabels.size(); i++)
	{
		for(int j=0; j<xylb->bg_mflowLabels[i].size(); j++)
		{
			v = xylb->bg_mflowLabels[i][j];

			float brushsize = v.w;
			for(int x=-brushsize/2.0; x<brushsize/2.0; x++)
			{
				for(int y=-brushsize/2.0; y<brushsize/2.0; y++)
				{
					index = glvoxel->volobject.get_index1D(v.x+x, v.y+y, v.z);
					glvoxel->maxflowbg_indices.push_back(index);				
				}
			}		
			
//			index = glvoxel->volobject.get_index1D(v.x, v.y, v.z);
//			glvoxel->maxflowbg_indices.push_back(index);
		}
	}
	for(int i=0; i<xzlb->bg_mflowLabels.size(); i++)
	{
		for(int j=0; j<xzlb->bg_mflowLabels[i].size(); j++)
		{
			v = xzlb->bg_mflowLabels[i][j];

			float brushsize = v.w;
			for(int x=-brushsize/2.0; x<brushsize/2.0; x++)
			{
				for(int z=-brushsize/2.0; z<brushsize/2.0; z++)
				{
					index = glvoxel->volobject.get_index1D(v.x+x, v.y, v.z+z);
					glvoxel->maxflowbg_indices.push_back(index);				
				}
			}					
//			index = glvoxel->volobject.get_index1D(v.x, v.y, v.z);
//			glvoxel->maxflowbg_indices.push_back(index);
		}
	}
	for(int i=0; i<yzlb->bg_mflowLabels.size(); i++)
	{
		for(int j=0; j<yzlb->bg_mflowLabels[i].size(); j++)
		{
			v = yzlb->bg_mflowLabels[i][j];

			float brushsize = v.w;
			for(int y=-brushsize/2.0; y<brushsize/2.0; y++)
			{
				for(int z=-brushsize/2.0; z<brushsize/2.0; z++)
				{
					index = glvoxel->volobject.get_index1D(v.x, v.y+y, v.z+z);
					glvoxel->maxflowbg_indices.push_back(index);				
				}
			}		
//			index = glvoxel->volobject.get_index1D(v.x, v.y, v.z);
//			glvoxel->maxflowbg_indices.push_back(index);
		}
	}*/
}
void QtGLObjectWindow::filter_maxflow_clear_slot(void)
{
	xylb->clearLabels();
	xzlb->clearLabels();
	yzlb->clearLabels();

	glvoxel->maxflowfg_indices.clear();
	glvoxel->maxflowbg_indices.clear();

	frames_redraw_slot();
}
void QtGLObjectWindow::filter_maxflow_dimmode_slot(int d)
{
	printf("Dimension mode: %d\n", d);

	xylb->dimension_mode = d;
	xzlb->dimension_mode = d;
	yzlb->dimension_mode = d;
}
void QtGLObjectWindow::filter_maxflow_open_slot(void)
{
	printf("open\n");
	QString fn = QFileDialog::getOpenFileName("", "*.msr", this );

	if(!fn.isEmpty())
	{
		ifstream input_file; 
		input_file.open(fn.latin1());

		if(!input_file.is_open()) return;

		string label;
		Vector v;
		int load_mode =-1;

		Tokenizer token(input_file);
		token.eolIsSignificant(true);

		//until oef is reached step through each token
		while(!input_file.eof())
		{
			if(token.ttype()==TT_WORD)
			{
				//get the first string lable
				label = token.sval();
				
				if(label=="OBJECT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_STRING)
					{
						label = token.sval();
						QString name = label.data();

						if(name=="xylb->fg_mflowLabels") load_mode = 1;
						else if(name=="xzlb->fg_mflowLabels") load_mode = 2;
						else if(name=="yzlb->fg_mflowLabels") load_mode = 3;
						else if(name=="xylb->bg_mflowLabels") load_mode = 4;
						else if(name=="xzlb->bg_mflowLabels") load_mode = 5;
						else if(name=="yzlb->bg_mflowLabels") load_mode = 6;
						else load_mode = -1;

						//printf("load_mode:%d\n", load_mode);
					}
				}
				else if(label=="VERT")
				{		
					//skip the equal sign
					token.nextToken(); 
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.x = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.y = token.nval();
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.z = token.nval();					
					token.nextToken();
					if(token.ttype()==TT_NUMBER) v.w = token.nval();					
					//v.w = 0.0;

					if(load_mode==1) xylb->fg_mflowLabels[v.z].push_back(v);
					else if(load_mode==2) xzlb->fg_mflowLabels[v.y].push_back(v); 
					else if(load_mode==3) yzlb->fg_mflowLabels[v.x].push_back(v);
					else if(load_mode==4) xylb->bg_mflowLabels[v.z].push_back(v);
					else if(load_mode==5) xzlb->bg_mflowLabels[v.y].push_back(v);
					else if(load_mode==6) yzlb->bg_mflowLabels[v.x].push_back(v);						
				}
				//else we dont know what it is, go to next label
				else
				{
					token.nextToken();
				}
			}
			else token.nextToken();
		}

		input_file.close();
	}

	frames_redraw_slot();
}
void QtGLObjectWindow::filter_maxflow_save_slot(void)
{
	printf("save\n");
	QString fn = QFileDialog::getSaveFileName("", "*.msr", this );
	printf("SAVNG FILE\n");

	if(!fn.isEmpty())
	{
		int numb_objects=0;
		if(!xylb->fg_mflowLabels.empty()) numb_objects++;
		if(!xzlb->fg_mflowLabels.empty()) numb_objects++;
		if(!yzlb->fg_mflowLabels.empty()) numb_objects++;
		if(!xylb->bg_mflowLabels.empty()) numb_objects++;
		if(!xzlb->bg_mflowLabels.empty()) numb_objects++;
		if(!yzlb->bg_mflowLabels.empty()) numb_objects++;

		Vector v;
		ofstream fout;
		fout.open(fn.latin1());

		fout<<"MSR_VERSION = 1.2"<<endl;
		fout<<"ORIGINALDATA = PATH '"<<" "<<"'"<<endl;
		fout<<"SCALE = "<<1<<" "<<1<<" "<<1<<endl;
		fout<<"ALGORITHM = 'VolViewer - MaxFlowLabels'"<<endl;
		fout<<"OBJECTCOUNT = "<<numb_objects<<endl;
		
		if(!xylb->fg_mflowLabels.empty()) 
		{
			int numbvert=0;
			for(int i=0; i<xylb->fg_mflowLabels.size(); i++)
				numbvert +=xylb->fg_mflowLabels[i].size();

			fout<<"OBJECT = 'xylb->fg_mflowLabels'"<<endl;
			fout<<"RENDERTYPE = POINTS"<<endl;
			fout<<"VERTCOUNT = "<<numbvert<<endl;		
			for(int i=0; i<xylb->fg_mflowLabels.size(); i++)
			{
				for(int j=0; j<xylb->fg_mflowLabels[i].size(); j++)
				{
					v = xylb->fg_mflowLabels[i][j];
					fout<<"VERT = "<<v.x<<" "<<v.y<<" "<<v.z<<" "<<v.w<<endl;
				}
			}
		}
		if(!xzlb->fg_mflowLabels.empty()) 
		{
			int numbvert=0;
			for(int i=0; i<xzlb->fg_mflowLabels.size(); i++)
				numbvert +=xzlb->fg_mflowLabels[i].size();

			fout<<"OBJECT = 'xzlb->fg_mflowLabels'"<<endl;
			fout<<"RENDERTYPE = POINTS"<<endl;
			fout<<"VERTCOUNT = "<<numbvert<<endl;		
			for(int i=0; i<xzlb->fg_mflowLabels.size(); i++)
			{
				for(int j=0; j<xzlb->fg_mflowLabels[i].size(); j++)
				{
					v = xzlb->fg_mflowLabels[i][j];
					fout<<"VERT = "<<v.x<<" "<<v.y<<" "<<v.z<<endl;
				}
			}
		}
		if(!yzlb->fg_mflowLabels.empty()) 
		{
			int numbvert=0;
			for(int i=0; i<yzlb->fg_mflowLabels.size(); i++)
				numbvert +=yzlb->fg_mflowLabels[i].size();

			fout<<"OBJECT = 'yzlb->fg_mflowLabels'"<<endl;
			fout<<"RENDERTYPE = POINTS"<<endl;
			fout<<"VERTCOUNT = "<<numbvert<<endl;		
			for(int i=0; i<yzlb->fg_mflowLabels.size(); i++)
			{
				for(int j=0; j<yzlb->fg_mflowLabels[i].size(); j++)
				{
					v = yzlb->fg_mflowLabels[i][j];
					fout<<"VERT = "<<v.x<<" "<<v.y<<" "<<v.z<<endl;
				}
			}
		}
		if(!xylb->bg_mflowLabels.empty()) 
		{
			int numbvert=0;
			for(int i=0; i<xylb->bg_mflowLabels.size(); i++)
				numbvert +=xylb->bg_mflowLabels[i].size();

			fout<<"OBJECT = 'xylb->bg_mflowLabels'"<<endl;
			fout<<"RENDERTYPE = POINTS"<<endl;
			fout<<"VERTCOUNT = "<<numbvert<<endl;		
			for(int i=0; i<xylb->bg_mflowLabels.size(); i++)
			{
				for(int j=0; j<xylb->bg_mflowLabels[i].size(); j++)
				{
					v = xylb->bg_mflowLabels[i][j];
					fout<<"VERT = "<<v.x<<" "<<v.y<<" "<<v.z<<endl;
				}
			}
		}
		if(!xzlb->bg_mflowLabels.empty()) 
		{
			int numbvert=0;
			for(int i=0; i<xzlb->bg_mflowLabels.size(); i++)
				numbvert +=xzlb->bg_mflowLabels[i].size();

			fout<<"OBJECT = 'xzlb->bg_mflowLabels'"<<endl;
			fout<<"RENDERTYPE = POINTS"<<endl;
			fout<<"VERTCOUNT = "<<numbvert<<endl;		
			for(int i=0; i<xzlb->bg_mflowLabels.size(); i++)
			{
				for(int j=0; j<xzlb->bg_mflowLabels[i].size(); j++)
				{
					v = xzlb->bg_mflowLabels[i][j];
					fout<<"VERT = "<<v.x<<" "<<v.y<<" "<<v.z<<endl;
				}
			}
		}
		if(!yzlb->bg_mflowLabels.empty()) 
		{
			int numbvert=0;
			for(int i=0; i<yzlb->bg_mflowLabels.size(); i++)
				numbvert +=yzlb->bg_mflowLabels[i].size();

			fout<<"OBJECT = 'yzlb->bg_mflowLabels'"<<endl;
			fout<<"RENDERTYPE = POINTS"<<endl;
			fout<<"VERTCOUNT = "<<numbvert<<endl;		
			for(int i=0; i<yzlb->bg_mflowLabels.size(); i++)
			{
				for(int j=0; j<yzlb->bg_mflowLabels[i].size(); j++)
				{
					v = yzlb->bg_mflowLabels[i][j];
					fout<<"VERT = "<<v.x<<" "<<v.y<<" "<<v.z<<endl;
				}
			}
		}

		fout.close();
	}
}
