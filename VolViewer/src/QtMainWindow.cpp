/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "QtMainWindow.h"

QtMainWindow::QtMainWindow(QString abbsapppath)
    : QMainWindow( 0, "", Qt::WDestructiveClose )
{
	printf("=====================================\n");
	printf("LAUNCHING: VolViewer v3.0rev%s\n", svn.revision.latin1());
	printf("=====================================\n");

	setCaption( "VolViewer v3.0 rev"+ svn.revision);

#ifdef BIOPTONICS_SUPPORT
	setCaption( "BioptonicsViewer v3.0 rev"+ svn.revision);
#endif

	appPath = abbsapppath;

	auto_centre=1;
	main_width = 494*2;
	main_height = 383*2;
	channel_mode = 0;
	view_orientation = -1;
	window_main_screen = 0;
	maximised = 0;
	timelapsehelper=0;
	promptChannels = true;

	transferfunction_editor_tabdlg	= 0;	
	volume_editor_tabdlg			= 0;
	clipping_editor_tabdlg			= 0;
	view_editor_tabdlg				= 0;
	light_editor_tabdlg				= 0;
	stereo_editor_tabdlg			= 0;
	movie_editor_tabdlg				= 0;
	measure_editor_tabdlg			= 0;
	filter_editor_tabdlg			= 0;
	crop_editor_tabdlg				= 0;
	renderview_editor_tabdlg		= 0;
	datasetview_editro_tabdlg		= 0; 


	qfilesystemwatcher = new QFileSystemWatcher(this);
	qfilesystemwatcher->addPath(appPath+"/settings.ini");

	//=================================================
	//GL window
	// -------------------------------------------------------------------------	
	glwindow = new QtGLObjectWindow( this, "editor", abbsapppath);
	glwindow->setFocus();
	setCentralWidget(glwindow->glvoxelframe);

	//addDockWidget(Qt::RightDockWidgetArea, glwindow->topbdock);
	addDockWidget(Qt::RightDockWidgetArea, glwindow->xydock);
	addDockWidget(Qt::RightDockWidgetArea, glwindow->yzdock);
	addDockWidget(Qt::RightDockWidgetArea, glwindow->xzdock);

	addDockWidget(Qt::BottomDockWidgetArea, glwindow->infodock);
	addDockWidget(Qt::BottomDockWidgetArea, glwindow->cursordock);

	glwindow->glvoxel->volrender.appPath = abbsapppath;

	//========================================================================
	//fileMenu = menuBar()->addMenu(tr("&File"));
	 	 
			 QAction* rawopenAct = new QAction(tr("&Volume (RAW)"), this);
			 rawopenAct->setShortcut(tr("Ctrl+V"));
			 rawopenAct->setStatusTip(tr("Open a RAW Volume image."));
			 connect(rawopenAct, SIGNAL(triggered()), this, SLOT(openraw_event()));
			 
			 QAction* stackopenAct = new QAction(tr("Image &Stack (BMP/PNG/JPG/TIFF)"), this);
			 stackopenAct->setShortcut(tr("Ctrl+S"));
			 stackopenAct->setStatusTip(tr("Open a stack of BMP/PNG/JPG/TIFF images."));
			 connect(stackopenAct, SIGNAL(triggered()), this, SLOT(openImage_event()));
			 
			#ifdef BROWSER5D_SUPPORT
				QAction* timediropenAct = new QAction(tr("&Directory of Stacks (BMP/PNG/JPG/TIFF)"), this);
				timediropenAct->setShortcut(tr("Ctrl+D"));
				timediropenAct->setStatusTip(tr("Open a directory of BMP/PNG/JPG/TIFF stacks."));
				connect(timediropenAct, SIGNAL(triggered()), this, SLOT(openTimelapse_event()));
			#endif
 
			 #ifdef OMERO_SUPPORT
				 QAction* omeroimgopenAct = new QAction(tr("Open OMERO &Image"), this);
				 omeroimgopenAct->setShortcut(tr("Ctrl+I"));
				 omeroimgopenAct->setStatusTip(tr("Opens an Image from an OMERO server."));
				 connect(omeroimgopenAct, SIGNAL(triggered()), this, SLOT(openOmeroImage_event()));
			#endif

		 openMenu =  new QMenu("Open Menu", this);//fileMenu->addMenu(tr("&Open"));
		 openMenu->setObjectName("openMenu");
		 openMenu->addAction(rawopenAct);
		 openMenu->addAction(stackopenAct);
 		 #ifdef BROWSER5D_SUPPORT
  			openMenu->addAction(timediropenAct);
		 #endif
		 #ifdef OMERO_SUPPORT
			openMenu->addAction(omeroimgopenAct);
		 #endif
		 openMenu->setTearOffEnabled(true);
		 openMenu->setTitle("Open Menu");

			 QAction* rawsaveAct = new QAction(tr("&Volume (RAW)"), this);
			 rawsaveAct->setShortcut(tr("Alt+V"));
			 rawsaveAct->setStatusTip(tr("Save a RAW Volume image."));
			 connect(rawsaveAct, SIGNAL(triggered()), this, SLOT(saveraw_event()));

			 QAction* pngsaveAct = new QAction(tr("Image &Stack (PNG)"), this);
			 pngsaveAct->setShortcut(tr("Alt+S"));
			 pngsaveAct->setStatusTip(tr("Save a PNG stack."));
			 connect(pngsaveAct, SIGNAL(triggered()), this, SLOT(savepng_event()));
			 
#ifdef OMERO_SUPPORT
			 QAction* omerosaveAct = new QAction(tr("OMERO Image"), this);
			 omerosaveAct->setShortcut(tr("Alt+O"));
			 omerosaveAct->setStatusTip(tr("Save to an OMERO server."));
			 connect(omerosaveAct, SIGNAL(triggered()), this, SLOT(saveOmeroImage_event()));
#endif

			 QAction* tiffsaveAct = new QAction(tr("multi-page &TIFF"), this);
			 tiffsaveAct->setShortcut(tr("Alt+T"));
			 tiffsaveAct->setStatusTip(tr("Save as a multi-page TIFF."));
			 connect(tiffsaveAct, SIGNAL(triggered()), this, SLOT(savetiff_event()));

			 QAction* rendersaveAct = new QAction(tr("Image &Grab (PNG)"), this);
			 rendersaveAct->setShortcut(tr("Alt+G"));
			 rendersaveAct->setStatusTip(tr("Save a render or section image."));
			 connect(rendersaveAct, SIGNAL(triggered()), this, SLOT(saveImage_event()));

		 saveMenu =  new QMenu("Save Menu", this); //fileMenu->addMenu(tr("&Save"));
		 saveMenu->setObjectName("saveMenu");
		 saveMenu->addAction(rawsaveAct);
		 saveMenu->addAction(pngsaveAct);
#ifdef OMERO_SUPPORT
		 saveMenu->addAction(omerosaveAct);
#endif
		 //saveMenu->addAction(rendersaveAct);
		 //saveMenu->setTearOffEnabled(true);
		 saveMenu->setTitle("Save Menu");

	//========================================================================

	viewMenu = new QMenu("View Menu", this);
	viewMenu->setObjectName("viewMenu");
	viewMenu->setTitle("View Menu");
	viewMenu->setTearOffEnabled(true);

			 QAction* saveviewAct = new QAction(tr("Save Layout"), this);
			 saveviewAct->setStatusTip(tr("Saves the layout as default"));
			 saveviewAct->setCheckable(false);
			 connect(saveviewAct, SIGNAL(triggered()), this, SLOT(minibutton_savelayout_event()));

			 QAction* restoreviewAct = new QAction(tr("Restore Layout"), this);
			 restoreviewAct->setStatusTip(tr("Restore the default layout"));
			 restoreviewAct->setCheckable(false);
			 connect(restoreviewAct, SIGNAL(triggered()), this, SLOT(minibutton_restorelayout_event()));

			 QAction* viewResetAllAct = new QAction(tr("Reset All"), this);
			 //viewResetAllAct->setShortcut(tr("Alt+1"));
			 viewResetAllAct->setStatusTip(tr("Reset All (Zoom/Translation/Orientation)."));
			 connect(viewResetAllAct, SIGNAL(triggered()), this, SLOT(resetviewall_event()));

			 QAction* viewResetZoomAct = new QAction(tr("Reset Zoom"), this);
			// viewFrontAct->setShortcut(tr("Alt+1"));
			 viewResetZoomAct->setStatusTip(tr("Reset the Zoom."));
			 connect(viewResetZoomAct, SIGNAL(triggered()), this, SLOT(resetviewzoom_event()));

			 QAction* viewTranslationAct = new QAction(tr("Reset Translation"), this);
			 //viewFrontAct->setShortcut(tr("Alt+1"));
			 viewTranslationAct->setStatusTip(tr("Reset Translation."));
			 connect(viewTranslationAct, SIGNAL(triggered()), this, SLOT(resetviewtrans_event()));

			 QAction* viewBottomAct = new QAction(tr("Bottom"), this);
			 viewBottomAct->setShortcut(tr("Alt+1"));
			 viewBottomAct->setStatusTip(tr("View Volume image from Bottom."));
			 connect(viewBottomAct, SIGNAL(triggered()), this, SLOT(bottom_event()));

			 QAction* viewTopAct = new QAction(tr("Top"), this);
			 viewTopAct->setShortcut(tr("Alt+2"));
			 viewTopAct->setStatusTip(tr("View Volume image from Top."));
			 connect(viewTopAct, SIGNAL(triggered()), this, SLOT(top_event()));

			 QAction* viewBackAct = new QAction(tr("Back"), this);
			 viewBackAct->setShortcut(tr("Alt+3"));
			 viewBackAct->setStatusTip(tr("View Volume image from Back."));
			 connect(viewBackAct, SIGNAL(triggered()), this, SLOT(back_event()));
			 
			 QAction* viewFrontAct = new QAction(tr("Front"), this);
			 viewFrontAct->setShortcut(tr("Alt+4"));
			 viewFrontAct->setStatusTip(tr("View Volume image from Front."));
			 connect(viewFrontAct, SIGNAL(triggered()), this, SLOT(front_event()));

			 QAction* viewLeftAct = new QAction(tr("Left"), this);
			 viewLeftAct->setShortcut(tr("Alt+5"));
			 viewLeftAct->setStatusTip(tr("View Volume image from Left."));
			 connect(viewLeftAct, SIGNAL(triggered()), this, SLOT(left_event()));

			 QAction* viewRightAct = new QAction(tr("Right"), this);
			 viewRightAct->setShortcut(tr("Alt+6"));
			 viewRightAct->setStatusTip(tr("View Volume image from Right."));
			 connect(viewRightAct, SIGNAL(triggered()), this, SLOT(right_event()));

	viewMenu->addAction(saveviewAct);
	viewMenu->addAction(restoreviewAct);
	viewMenu->addSeparator();
	viewMenu->addAction(viewResetAllAct);
	viewMenu->addAction(viewResetZoomAct);
	viewMenu->addAction(viewTranslationAct);
	viewMenu->addSeparator();
	viewMenu->addAction(viewBottomAct);
	viewMenu->addAction(viewTopAct);
	viewMenu->addAction(viewBackAct);
	viewMenu->addAction(viewFrontAct);
	viewMenu->addAction(viewLeftAct);
	viewMenu->addAction(viewRightAct);

	//========================================================================
	QMenu *renderinqualMenu = new QMenu("Render Quality Menu", this);
	renderinqualMenu->setObjectName("renderinqualMenu");
	renderinqualMenu->setTearOffEnabled(true);
	renderinqualMenu->setTitle("Render Quality Menu");

		QAction* hqAct = new QAction(tr("High Quality"), this);
		QPixmap hqIcon = QPixmap( appPath+"/Icons/HQ.png" );
		hqAct->setStatusTip(tr("Change Rendering to High Quality Preset."));
		hqAct->setIcon(hqIcon);
		connect(hqAct, SIGNAL(triggered()), this, SLOT(minibutton_hq_event()));

		QAction* mqAct = new QAction(tr("Medium Quality"), this);
		QPixmap mqIcon = QPixmap( appPath+"/Icons/mq.png" );
		mqAct->setStatusTip(tr("Change Rendering to Medium Quality Preset."));
		mqAct->setIcon(mqIcon);
		connect(mqAct, SIGNAL(triggered()), this, SLOT(minibutton_mq_event()));

		QAction* lqAct = new QAction(tr("Low Quality"), this);
		QPixmap lqIcon = QPixmap( appPath+"/Icons/lq.png" );
		lqAct->setStatusTip(tr("Change Rendering to Low Quality Preset."));
		lqAct->setIcon(lqIcon);
		connect(lqAct, SIGNAL(triggered()), this, SLOT(minibutton_lq_event()));

	renderinqualMenu->addAction(hqAct);
	renderinqualMenu->addAction(mqAct);
	renderinqualMenu->addAction(lqAct);

	//============================================================================

	QPixmap intenIcon = QPixmap( appPath+"/Icons/I.png" );
    intentb	= new QToolButton( intenIcon, "inten", QString::null,
			   this, SLOT(minibutton_I_event()), 0, "Intensity/RGB Channel Toggle" );
	intentb->setTextLabel("Intensity/RGB Channel Toggle");
	intentb->setToggleButton(true);
	intentb->setOn(true);

	QPixmap redIcon = QPixmap( appPath+"/Icons/red.png" );
    redtb = new QToolButton( redIcon, "red", QString::null,
			   this, SLOT(minibutton_red_event()), 0, "red" );
	redtb->setTextLabel("Red Channel Toggle");
	redtb->setToggleButton(true);
	redtb->setOn(true);

	QPixmap greenIcon = QPixmap( appPath+"/Icons/green.png" );
    greentb	= new QToolButton( greenIcon, "green", QString::null,
			   this, SLOT(minibutton_green_event()), 0, "Green Channel Toggle" );
	greentb->setTextLabel("Green Channel Toggle");
	greentb->setToggleButton(true);
	greentb->setOn(true);

	QPixmap blueIcon = QPixmap( appPath+"/Icons/blue.png" );
    bluetb	= new QToolButton( blueIcon, "blue", QString::null,
			   this, SLOT(minibutton_blue_event()), 0, "Blue Channel Toggle" );
	bluetb->setTextLabel("Blue Channel Toggle");
	bluetb->setToggleButton(true);
	bluetb->setOn(true);

	QPixmap surfacesIcon = QPixmap( appPath+"/Icons/s.png" );
    QToolButton * surfacestb
	= new QToolButton( surfacesIcon, "Surfaces Display Toggle", QString::null,
			   this, SLOT(minibutton_surfaces_event()), 0, "surfaces" );
	surfacestb->setTextLabel("Surfaces Display Toggle");
	surfacestb->setToggleButton(true);
	surfacestb->setOn(true);

	//===============================================================
	QPixmap mipIcon = QPixmap( appPath+"/Icons/mip.png" );
    QToolButton * miptb
	= new QToolButton( mipIcon, "mip", QString::null,
			   this, SLOT(minibutton_mip_event()), 0, "Maximum Intensity Projection Toggle" );
	miptb->setTextLabel("Maximum Intensity Projection Toggle");
	miptb->setToggleButton(true);

	QPixmap bgIcon = QPixmap( appPath+"/Icons/bg.png" );
    bgtb = new QToolButton( bgIcon, "Background Colour Options", QString::null,
			   this, SLOT(minibutton_bg_event()), 0, "bg" );
	bgtb->setTextLabel("Background Colour Options");

	//===============================================================
	QMenu *toggleMenu = new QMenu("Toggle Menu", this);
	toggleMenu->setObjectName("toggleMenu");
	toggleMenu->setTearOffEnabled(true);
	toggleMenu->setTitle("Toggle Menu");

			 boundingboxAct = new QAction(tr("Bounding Box (ON/OFF)"), this);
			 QPixmap bbIcon = QPixmap( appPath+"/Icons/bb.png" );
			 boundingboxAct->setStatusTip(tr("Enable or Disable the object Bounding Box."));
			 boundingboxAct->setCheckable(true);
			 boundingboxAct->setIcon(bbIcon);
			 connect(boundingboxAct, SIGNAL(triggered()), this, SLOT(minibutton_bb_event()));

 			 lightAct = new QAction(tr("Light (ON/OFF)"), this);
			 QPixmap lightIcon = QPixmap( appPath+"/Icons/l.png" );
			 lightAct->setStatusTip(tr("Enable or Disable the display of the Light."));
			 lightAct->setCheckable(true);
			 lightAct->setIcon(lightIcon);
			 connect(lightAct, SIGNAL(triggered()), this, SLOT(minibutton_lights_event()));

 			 axisAct = new QAction(tr("Axis (ON/OFF)"), this);
			 QPixmap axisIcon = QPixmap( appPath+"/Icons/a.png" );
			 axisAct->setStatusTip(tr("Enable or Disable the display of the Axis."));
			 axisAct->setCheckable(true);
			 axisAct->setIcon(axisIcon);
			 connect(axisAct, SIGNAL(triggered()), this, SLOT(minibutton_axis_event()));

			 scalebarAct = new QAction(tr("Scalebar (ON/OFF)"), this);
			 QPixmap scalebarIcon = QPixmap( appPath+"/Icons/scalebar.png" );
			 scalebarAct->setStatusTip(tr("Enable or Disable the display of the Scalebar."));
			 scalebarAct->setCheckable(true);
			 scalebarAct->setIcon(scalebarIcon);
			 connect(scalebarAct, SIGNAL(triggered()), this, SLOT(toggle_scalebar_event()));

			 fpsAct = new QAction(tr("FPS (ON/OFF)"), this);
			 QPixmap fpsIcon = QPixmap( appPath+"/Icons/fps.png" );
			 fpsAct->setStatusTip(tr("Enable or Disable the display of the Frames per Second."));
			 fpsAct->setCheckable(true);
			 fpsAct->setIcon(fpsIcon);
			 connect(fpsAct, SIGNAL(triggered()), this, SLOT(minibutton_fps_event()));
	
	//toggleMenu->addAction(fullscreenAct);
	toggleMenu->addAction(boundingboxAct);
	toggleMenu->addAction(lightAct);
	toggleMenu->addAction(axisAct);
	toggleMenu->addAction(scalebarAct);
	//toggleMenu->addAction(fpsAct);

	//===============================================================
	QPixmap infoIcon = QPixmap( appPath+"/Icons/info.png" );
    QToolButton * infotb
	= new QToolButton( infoIcon, "Info Display Toggle", QString::null,
			   this, SLOT(minibutton_info_event()), 0, "info" );
	infotb->setTextLabel("Info Display Toggle");
 	infotb->setToggleButton(true);

	QPixmap threeDIcon = QPixmap( appPath+"/Icons/3D.png" );
    QToolButton * threeDb
	= new QToolButton( threeDIcon, "3D", QString::null,
			   this, SLOT(minibutton_stereo_event()), 0, "3D" );
	threeDb->setTextLabel("Stereo Display Options");

	//===============================================================================================

	QPixmap cursorIcon = QPixmap( appPath+"/Icons/sectioncursor.png" );
    QToolButton * cursorb
		= new QToolButton( cursorIcon, "3D Cursor", QString::null,
			   this, SLOT(minibutton_cursor_event()), 0, "3D Cursor" );
	cursorb->setTextLabel("Toggle the 3D Cursor");
	cursorb->setToggleButton(true);

	/*
	//QIcon(QPixmap(apppath+"/Icons/sectioncursor.png"))
	cursorAct = new QAction(tr("Cursor (ON/OFF)"), this);
	QPixmap cursorIcon = QPixmap( appPath+"/Icons/c.png" );
	cursorAct->setStatusTip(tr("Enable or Disable the display of the Cursor."));
	cursorAct->setCheckable(true);
	cursorAct->setIcon(cursorIcon);
	connect(cursorAct, SIGNAL(triggered()), this, SLOT(minibutton_cursor_event()));*/

	//===============================================================================================
		QPixmap viewIcon = QPixmap( appPath+"/Icons/view.png" );
		QToolButton * viewb = new QToolButton(this);
		viewb->setIcon(viewIcon);
	//( viewIcon, "view", QString::null,
	//			   this, SLOT(), 0, "Viewing Direction Options" );
		viewb->setTextLabel("Viewing Direction Options");
		viewb->setMenu(viewMenu);
		viewb->setPopupMode(QToolButton::InstantPopup);

		QPixmap toggleIcon = QPixmap( appPath+"/Icons/onoff.png" );
		//QToolButton * toggleb
		QToolButton * toggleb = new QToolButton(this);
		toggleb->setIcon(toggleIcon);
	//	= new QToolButton( toggleIcon, "view", QString::null,
	//			   this, SLOT(), 0, "Viewing Direction Options" );
		toggleb->setTextLabel("Viewing Direction Options");
		toggleb->setMenu(toggleMenu);
		toggleb->setPopupMode(QToolButton::InstantPopup);

		/*
		QPixmap renderIcon = QPixmap( appPath+"/Icons/quality.png" );
		//QToolButton * renderqualb
		//= new QToolButton( renderIcon, "qual", QString::null,
		//		   this, SLOT(), 0, "Render Quality Options" );
		QToolButton * renderqualb = new QToolButton(this);
		renderqualb->setIcon(renderIcon);
		renderqualb->setTextLabel("Render Quality Options");
		renderqualb->setMenu(renderinqualMenu);
		renderqualb->setPopupMode(QToolButton::InstantPopup);
		*/

	//===============================================================================================
    /*QToolButton * filtertb = new QToolButton(renderIcon, "filter", QString::null, this, SLOT(minibutton_filter_event()), 0, "Filter" );
	filtertb->setTextLabel("Filter");
	filtertb->setToggleButton(false);

    QToolButton * labeltb = new QToolButton(toggleIcon, "label", QString::null, this, SLOT(minibutton_label_event()), 0, "Label" );
	labeltb->setTextLabel("Label");
	labeltb->setToggleButton(false);

    QToolButton * vertfindertb = new QToolButton(renderIcon, "vertfinder", QString::null, this, SLOT(minibutton_vertfinder_event()), 0, "VertFinder" );
	vertfindertb->setTextLabel("VertFinder");
	vertfindertb->setToggleButton(false);*/
	
//	QPixmap emptyIcon = QPixmap( "Icons/empty.png" );

	QPixmap addIcon = QPixmap( appPath+"/Icons/add.png" );
	QPixmap removeIcon = QPixmap( appPath+"/Icons/remove.png" );
    QToolButton * vertfindertb = new QToolButton(addIcon, "label", QString::null, this, SLOT(minibutton_vertfinder_event()), 0, "VertFinder" );
	vertfindertb->setTextLabel("Timelapse folder +");
	vertfindertb->setToggleButton(false);
    QToolButton * labeltb = new QToolButton(removeIcon, "label", QString::null, this, SLOT(minibutton_label_event()), 0, "Label" );
	labeltb->setTextLabel("Timelapse folder -");
	labeltb->setToggleButton(false);

 	QPixmap screenshotIcon = QPixmap( appPath+"/Icons/camera.png" );
    QToolButton * screenshottb = new QToolButton(screenshotIcon, "screenshot", QString::null, this, SLOT(save_renderScreenShot_event()), 0, "Label" );
	screenshottb->setTextLabel("Save Screenshot of Rendering Window");
	screenshottb->setToggleButton(false);
	//===============================================================================================
	QPalette pal;
	pal.setColor(QPalette::Button, QColor("white"));
		
    buttonTools = new QToolBar("ToolBar", this);
	buttonTools->setObjectName("buttonTools");
	buttonTools->setLabel( "View Menu" );
	buttonTools->setIconSize(QSize(24,24));
	buttonTools->setMovable(true);
	buttonTools->setOrientation(Qt::Vertical);
	buttonTools->setToolButtonStyle(Qt::ToolButtonIconOnly); //Qt::ToolButtonIconOnly
	buttonTools->setAutoFillBackground(true);
	buttonTools->setPalette(pal);

	buttonTools->addSeparator();
	buttonTools->addWidget(intentb);
	buttonTools->addWidget(redtb);
	buttonTools->addWidget(greentb);
	buttonTools->addWidget(bluetb);
	buttonTools->addWidget(surfacestb);
	buttonTools->addSeparator();
	buttonTools->addWidget(miptb);
	buttonTools->addWidget(bgtb);
	buttonTools->addWidget(threeDb);
	buttonTools->addWidget(cursorb);
    buttonTools->addSeparator();
	//buttonTools->addWidget(filtertb);
	//buttonTools->addWidget(labeltb);
    
//	buttonTools->addWidget(vertfindertb);
//	buttonTools->addWidget(labeltb);
//	buttonTools->addSeparator();

	buttonTools->addWidget(screenshottb);
	buttonTools->addSeparator();

	//buttonTools->addWidget(renderqualb);
	buttonTools->addWidget(viewb);
	buttonTools->addWidget(toggleb);
	buttonTools->addSeparator();

	addToolBar(Qt::LeftToolBarArea, buttonTools);

	//==================================================
		QPixmap loadIcon = QPixmap( appPath+"/Icons/load.png" );
		QToolButton * loadb = new QToolButton(this);
		loadb->setIcon(loadIcon);
//		= new QToolButton( loadIcon, "load", QString::null,
//				   this, SLOT(), 0, "Load Menu..." );
		loadb->setTextLabel("Load data...");
		loadb->setMenu(openMenu);
		loadb->setPopupMode(QToolButton::InstantPopup);

		//-------------------------------------------------------

		QPixmap saveIcon = QPixmap( appPath+"/Icons/save.png" );
		QToolButton * saveb  = new QToolButton(this);
//		= new QToolButton( saveIcon, "save", QString::null,
//				   this, SLOT(), 0, "Save Menu..." );
		saveb->setIcon(saveIcon);
		saveb->setTextLabel("Save data...");
		saveb->setMenu(saveMenu);
		saveb->setPopupMode(QToolButton::InstantPopup);

		//-------------------------------------------------------

		QPixmap viewprefIcon = QPixmap( appPath+"/Icons/spanner.png" );
		QToolButton * viewpref
		= new QToolButton( viewprefIcon, "Render Settings", QString::null,
				   this, SLOT(viewpref_event()), 0, "Render Settings" );
		viewpref->setTextLabel("Render Settings");

		//-------------------------------------------------------

		QPixmap isosurfIcon = QPixmap( appPath+"/Icons/surface.png" );
		QToolButton * isosurf
		= new QToolButton( isosurfIcon, "Surfaces", QString::null,
				   this, SLOT(volume_editor_event()), 0, "Surfaces" );
		isosurf->setTextLabel("Surfaces");

		//-------------------------------------------------------

		QPixmap clippingIcon = QPixmap( appPath+"/Icons/cut.png" );
		clipping = new QToolButton( clippingIcon, "Clipping Planes", QString::null, 
												  this, SLOT(transform_editor_event()), 0, "Clipping Planes" );
		clipping->setTextLabel("Clipping Planes");
	
		//-------------------------------------------------------

		QPixmap tfuncIcon = QPixmap( appPath+"/Icons/function.png" );
		QToolButton * tfunc
		= new QToolButton( tfuncIcon, "Transfer Functions", QString::null,
				   this, SLOT(transfer_editor_event()), 0, "Transfer Functions" );
		tfunc->setTextLabel("Transfer Functions");

		//-------------------------------------------------------

		QPixmap lightprefIcon = QPixmap( appPath+"/Icons/light.png" );
		QToolButton * lightpref
		= new QToolButton( lightprefIcon, "Light & Shading", QString::null,
				   this, SLOT(lightpref_event()), 0, "Light/Shader Settings" );

		lightpref->setTextLabel("Light & Shading");

		//-------------------------------------------------------

		QPixmap movieprefIcon = QPixmap( appPath+"/Icons/movie.png" );
		QToolButton * moviepref
		= new QToolButton( movieprefIcon , "Movie Output", QString::null,
				   this, SLOT(moviepref_event()), 0, "movie Preferences" );

		moviepref->setTextLabel("Movie Output");

		//-------------------------------------------------------

		QPixmap measureprefIcon = QPixmap( appPath+"/Icons/ruler.png" );
		measurepref
		= new QToolButton( measureprefIcon , "Measure", QString::null,
				   this, SLOT(measurepref_event()), 0, "Measuring Tool" );

		measurepref->setTextLabel("Measure");
	
		//-------------------------------------------------------

		QPixmap filterprefIcon = QPixmap( appPath+"/Icons/filter.png" );
		QToolButton * filterpref
		= new QToolButton( filterprefIcon , "Filters", QString::null,
				   this, SLOT(filterpref_event()), 0, "Filter Tool" );

		filterpref->setTextLabel("Filter");
		

		QPixmap cropprefIcon = QPixmap( appPath+"/Icons/crop.png" );
		croppref
		= new QToolButton( cropprefIcon, "Crop", QString::null,
				   this, SLOT(croppref_event()), 0, "Crop Tool" );

		croppref->setTextLabel("Crop");

		//-------------------------------------------------------
    
		QPixmap renderviewprefIcon = QPixmap( appPath+"/Icons/bookmarks.png" );
		QToolButton * renderviewpref
		= new QToolButton( renderviewprefIcon , "Visual Bookmarks", QString::null,
				   this, SLOT(renderviewpref_event()), 0, "View Tool" );

		renderviewpref->setTextLabel("Visual Bookmarks");
				
		//-------------------------------------------------------

		QPixmap cameraviewprefIcon = QPixmap( appPath+"/Icons/VideoCamera.png" );
		cameraviewpref
		= new QToolButton( cameraviewprefIcon , "Camera Settings", QString::null,
				   this, SLOT(cameraviewpref_event()), 0, "View Tool" );

		cameraviewpref->setTextLabel("Camera Settings");

		//-------------------------------------------------------

		QString abouticonpath = "/Icons/about.png";

#ifdef BIOPTONICS_SUPPORT
		abouticonpath = "/Icons/biooptonics_logo.png";
#endif
		QPixmap aboutIcon = QPixmap( appPath+abouticonpath);
		QToolButton * abouttbtn
		= new QToolButton( aboutIcon , "About", QString::null,
				   this, SLOT(about_event()), 0, "About" );

		abouttbtn->setTextLabel("About");

		//-------------------------------------------------------
               /* QPixmap tfunctionIcon = QPixmap( "icons/simple/function.png" );
                QToolButton * tfunctionpref
                = new QToolButton( tfunctionIcon , "Measure", QString::null,
                                   this, SLOT(tfunction_event()), 0, "TransferFunction" );

                tfunctionpref->setTextLabel("Tfunc");*/
                //-------------------------------------------------------
                //-------------------------------------------------------

	fileTools = new QToolBar("File Menu", this);
	fileTools->setObjectName("fileTools");
	fileTools->setLabel( "File Menu" );
	fileTools->setIconSize(QSize(32, 32));
	fileTools->setMovable(true);
	fileTools->setAutoFillBackground(true);
	fileTools->setPalette(pal);
	fileTools->setToolButtonStyle(Qt::ToolButtonIconOnly);
	fileTools->addSeparator();
	fileTools->addWidget(loadb);
	fileTools->addWidget(saveb);
	fileTools->addSeparator();
    
    prefTools = new QToolBar("Tools Menu", this);
	prefTools->setObjectName("prefTools");
	prefTools->setLabel( "Tools Menu" );
	prefTools->setIconSize(QSize(32, 32));
	prefTools->setAutoFillBackground(true);
	prefTools->setPalette(pal);
	prefTools->setMovable(true);
	prefTools->setToolButtonStyle(Qt::ToolButtonIconOnly);
	prefTools->addSeparator();
	prefTools->addWidget(tfunc);
	prefTools->addSeparator();
	prefTools->addWidget(croppref);
	prefTools->addSeparator();
	prefTools->addWidget(clipping);
	prefTools->addSeparator();
	prefTools->addWidget(measurepref);
	prefTools->addSeparator();
	prefTools->addWidget(isosurf);
	prefTools->addSeparator();
	prefTools->addWidget(filterpref);
	prefTools->addSeparator();

    settingsTools = new QToolBar("Settings Menu", this);
	settingsTools->setObjectName("settingsTools");
	settingsTools->setLabel( "Settings Menu" );
	settingsTools->setIconSize(QSize(32, 32));
	settingsTools->setAutoFillBackground(true);
	settingsTools->setPalette(pal);
	settingsTools->setMovable(true);
	settingsTools->setToolButtonStyle(Qt::ToolButtonIconOnly);
	settingsTools->addSeparator();
	settingsTools->addWidget(renderviewpref);
	settingsTools->addSeparator();
	settingsTools->addWidget(moviepref);
	settingsTools->addSeparator();
	settingsTools->addWidget(cameraviewpref);
	settingsTools->addSeparator();
	settingsTools->addWidget(lightpref);
	settingsTools->addSeparator();
	settingsTools->addWidget(viewpref);
	settingsTools->addSeparator();
    /*settingsTools->addWidget(tfunctionpref);
    settingsTools->addSeparator();*/

	QWidget* spacer = new QWidget();
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	aboutTool = new QToolBar("About Menu", this);
	aboutTool->setObjectName("aboutTool");
	aboutTool->setLabel( "About Menu" );
	aboutTool->setIconSize(QSize(32, 32));
	aboutTool->setAutoFillBackground(true);
	aboutTool->setPalette(pal);
#ifdef BIOPTONICS_SUPPORT
	aboutTool->setIconSize(QSize(85, 32));
#endif
	aboutTool->setMovable(true);
	aboutTool->setToolButtonStyle(Qt::ToolButtonIconOnly);
	//aboutTool->addSeparator();
	aboutTool->addWidget(spacer);
	aboutTool->addWidget(abouttbtn);
	
	addToolBar(Qt::TopToolBarArea, fileTools);
	addToolBar(Qt::TopToolBarArea, prefTools);
	addToolBar(Qt::TopToolBarArea, settingsTools);
	addToolBar(Qt::TopToolBarArea, aboutTool);

	// -------------------------------------------------------------------------
	// create our EDITOR TAB dialogues,
	// -------------------------------------------------------------------------
	transferfunction_editor_tabdlg	= new QtTransferFunctionEditorDialogue(this, "QtTransferFunction", false);	
	volume_editor_tabdlg			= new QtVolumeEditorDialogue(this, "QtVolumeEditor", false);
	clipping_editor_tabdlg			= new QtClippingEditorDialogue(this, "QtClippingEditor", false);
	view_editor_tabdlg				= new QtViewEditorDialogue(this, "QtViewEditor", false);
	light_editor_tabdlg				= new QtLightEditorDialogue(this, "QtLightEditor", false);
	stereo_editor_tabdlg			= new QtStereoEditorDialogue(this, "QtStereoEditor", false);
	movie_editor_tabdlg				= new QtMovieEditorDialogue(this, "QtMovieEditor", false);
	measure_editor_tabdlg			= new QtMeasuringEditorDialogue(this, "QtMeasureEditor", false, 0, appPath);
	filter_editor_tabdlg			= new QtFilterEditorDialogue(this, "QtFilterEditor", false);
	crop_editor_tabdlg				= new QtCropEditorDialogue(this, "QtCropEditor", false, 0, appPath);
	renderview_editor_tabdlg		= new QtRenderViewDialogue(this, "QtRenderViewEditor", false);
	datasetview_editro_tabdlg		= new QtDatasetViewDialogue(this, "QtDatasetViewEditor", false); 
	camera_editor_tabdlg			= new QtCameraEditorDialogue(this, "QtCameraEditorDialogue", false); 

#ifdef LIBTIFF_SUPPORT
    transferfunc16bit_dialogue = new QtTransferFuncDialogue(this, "QtTransferFuncDialogue", false);
#endif

	glwindow->glvoxel->volrender.editor = transferfunction_editor_tabdlg;

	//we need this to allow for intensity and image space calculations
	measure_editor_tabdlg->measuring_tab->volobj = &glwindow->glvoxel->volobject;
	
	// -------------------------------------------------------------------------
	//connect our SIGNALS and SLOTS between our widgets
	// -------------------------------------------------------------------------
	connect_signals_and_slots();
	minibutton_restorelayout_event();

	// load our settings.ini file
	//load_inifile();

/*	
	//do we have a default filename to load?
	if(!default_filename.isEmpty())
	{
		glwindow->glvoxel->argc_mode = 1;
		glwindow->glvoxel->volobject.file_name = default_filename;		
	}
*/
	glwindow->glvoxel->redraw_event();
	
	// -------------------------------------------------------------------------

	//let our script engine know about the mainwindow
	volscript.mw = this;
	//volscript.parse_script("open_image_stack('E:/-=VOLUME DATA=-/-=TEST GALLERY=-/TEST/Teddy PNG/Slice000.png') \n wait_for_quit() \n save_projection('blah\\render.png') \n set_auto_close()");
	//volscript.execute_script_now();
	
#ifdef OPENCL_SUPPORT
	glwindow->glvoxel->updateCL_UI();
#endif

}

QtMainWindow::~QtMainWindow()
{
}

void QtMainWindow::connect_signals_and_slots(void)
{
	//printf("connect_signals_and_slots\n");

	// -------------------------------------------------------------------------
	//THIS
	// -------------------------------------------------------------------------
	connect(this, SIGNAL(measure_setscale2_signal(float,float,float)), 		measure_editor_tabdlg->measuring_tab, SLOT(measure_setscale2_slot(float,float,float)));
	connect(this, SIGNAL(measure_setrescale_signal(float,float,float)), 	measure_editor_tabdlg->measuring_tab, SLOT(measure_setrescale_slot(float,float,float)));
	connect(this, SIGNAL(measure_setspacing_signal(float,float,float)), 	measure_editor_tabdlg->measuring_tab, SLOT(measure_setspacing_slot(float,float,float)));
	connect(this, SIGNAL(measure_setoriginalfn_signal(QString)), 			measure_editor_tabdlg->measuring_tab, SLOT(measure_setoriginalfn_slot(QString)));
	
//**************************************************************************************************************************************************

	connect(this, SIGNAL(currentworkingdir_signal(QString)), 				renderview_editor_tabdlg->renviewtab, SLOT(renderview_setcurrentworkingdir_slot(QString)));
	connect(this, SIGNAL(currentworkingdir_signal(QString)), 				measure_editor_tabdlg->measuring_tab, SLOT(measure_setcurrentworkingdir_slot(QString)));
	connect(this, SIGNAL(currentworkingdir_signal(QString)), 				volume_editor_tabdlg->isosurface_tab, SLOT(isosurface_setcurrentworkingdir_slot(QString)));
	connect(this, SIGNAL(currentworkingdir_signal(QString)), 				transferfunction_editor_tabdlg->tranfer_function_tab->hist, SLOT(hist_setcurrentworkingdir_slot(QString)));
	connect(this, SIGNAL(currentworkingdir_signal(QString)), 				clipping_editor_tabdlg->cliptab, SLOT(cutplane_setcurrentworkingdir_slot(QString)));
	connect(this, SIGNAL(currentworkingdir_signal(QString)), 				glwindow->glvoxel, SLOT(setcurrentworkingdir_slot(QString)));

	#ifdef LIBTIFF_SUPPORT
		connect(this, SIGNAL(currentworkingdir_signal(QString)), 				transferfunc16bit_dialogue->tranfer_func_tab, SLOT(tfunc16_setcurrentworkingdir_slot(QString)));
	
	//	connect(transferfunc16bit_dialogue->tranfer_func_tab, 		SIGNAL(tfunc_brightcontrast_signal(int, int, int)),													glwindow->glvoxel, SLOT(tfunc_brightcontrast_shader_slot(int, int, int)));	
 		connect(transferfunc16bit_dialogue->tranfer_func_tab,		SIGNAL(tfunc_remap16bit_signal(int, int, const vector<Vector>&, int, int, const vector<Vector>&)),  glwindow->glvoxel, SLOT(tfunc_remap16bit_slot(int, int, const vector<Vector>&, int, int, const vector<Vector>&)));	
 	//	connect(transferfunc16bit_dialogue->tranfer_func_tab,		SIGNAL(tfunc_invert_signal(int, int)), 																glwindow->glvoxel, SLOT(tfunc_invert_slot(int, int)));

		connect(glwindow->glvoxel, SIGNAL(tfunc_16bit_histogram_signal(double*, double*)), 	transferfunc16bit_dialogue->tranfer_func_tab, SLOT(tfunc_16bit_histogram_slot(double*, double*)));
	#endif

//**************************************************************************************************************************************************
	
	//VOLIO
	QObject::connect(glwindow->glvoxel->volio.watcher, SIGNAL(finished()), this, SLOT(uploadTextureSlot()));

	// -------------------------------------------------------------------------
	//Transform editor signals
	// -------------------------------------------------------------------------
	connect(clipping_editor_tabdlg->cliptab, SIGNAL(cutplane_redclip_signal(void)),			glwindow->glvoxel, SLOT(cutplane_redclip_slot(void)));
	connect(clipping_editor_tabdlg->cliptab, SIGNAL(cutplane_blueclip_signal(void)),			glwindow->glvoxel, SLOT(cutplane_blueclip_slot(void)));
	connect(clipping_editor_tabdlg->cliptab, SIGNAL(cutplane_greenclip_signal(void)),			glwindow->glvoxel, SLOT(cutplane_greenclip_slot(void)));
	connect(clipping_editor_tabdlg->cliptab, SIGNAL(cutplane_isoclip_signal(void)),			glwindow->glvoxel, SLOT(cutplane_isoclip_slot(void)));
	connect(clipping_editor_tabdlg->cliptab, SIGNAL(cutplane_resetall_signal(void)), 			glwindow->glvoxel, SLOT(cutplane_resetall_slot(void)));
	connect(clipping_editor_tabdlg->cliptab, SIGNAL(cutplane_resetcurrent_signal(void)), 		glwindow->glvoxel, SLOT(cutplane_resetcurrent_slot(void)));
	connect(clipping_editor_tabdlg->cliptab, SIGNAL(cutplane_invert_signal(int)), 				glwindow->glvoxel, SLOT(cutplane_invert_slot(int)));
	connect(clipping_editor_tabdlg->cliptab, SIGNAL(cutplane_showall_signal(int)), 			glwindow->glvoxel, SLOT(cutplane_showall_slot(int)));
	connect(clipping_editor_tabdlg->cliptab, SIGNAL(cutplane_load_signal(QString)), 			glwindow->glvoxel, SLOT(cutplane_load_slot(QString)));
	connect(clipping_editor_tabdlg->cliptab, SIGNAL(cutplane_save_signal(QString)), 			glwindow->glvoxel, SLOT(cutplane_save_slot(QString)));
	connect(clipping_editor_tabdlg->cliptab, SIGNAL(cutplane_realign_signal(void)), 			glwindow->glvoxel, SLOT(cutplane_realign_slot(void)));
	connect(clipping_editor_tabdlg->cliptab, SIGNAL(cutplane_invcrop_signal(void)), 			glwindow->glvoxel, SLOT(cutplane_invcrop_slot(void)));
	connect(clipping_editor_tabdlg->cliptab, SIGNAL(cutplane_recentre_signal(void)), 			glwindow->glvoxel, SLOT(cutplane_recentre_slot(void)));
	connect(clipping_editor_tabdlg->cliptab, SIGNAL(cutplane_crop_signal(void)), 				glwindow->glvoxel, SLOT(cutplane_crop_slot(void)));
	connect(clipping_editor_tabdlg->cliptab, SIGNAL(cutplane_interact_clicked_signal(int)),	glwindow->glvoxel, SLOT(cutplane_interact_clicked_slot(int)));
	connect(clipping_editor_tabdlg->cliptab, SIGNAL(cutplane_unselect_signal(void)), 			glwindow->glvoxel, SLOT(cutplane_unselect_slot(void)));
	connect(clipping_editor_tabdlg->cliptab, SIGNAL(cutplane_rendermode_signal(int)), 			glwindow->glvoxel, SLOT(cutplane_rendermode_slot(int)));
	connect(clipping_editor_tabdlg->cliptab, SIGNAL(cutplane_selected_signal(int)), 			glwindow->glvoxel, SLOT(cutplane_selected_slot(int)));
	connect(clipping_editor_tabdlg->cliptab, SIGNAL(cutplane_applyall_drawmode_signal(void)),	glwindow->glvoxel, SLOT(cutplane_applyall_drawmode_slot(void)));
	connect(clipping_editor_tabdlg->cliptab, SIGNAL(cutplane_curveangle_signal(int, int)),		glwindow->glvoxel, SLOT(cutplane_curveangle_slot(int, int)));
	connect(clipping_editor_tabdlg->cliptab, SIGNAL(cutplane_curvecoords_signal(bool)),		glwindow->glvoxel, SLOT(cutplane_curvecoords_slot(bool)));
	connect(clipping_editor_tabdlg->cliptab, SIGNAL(cutplane_curvetrans_signal(bool)),		glwindow->glvoxel, SLOT(cutplane_curvetrans_slot(bool)));


	// -------------------------------------------------------------------------
	//Volume editor signals
	// -------------------------------------------------------------------------
	connect(volume_editor_tabdlg->isosurface_tab, SIGNAL(isosurface_new_signal(QString , int , int , int, Vector, bool*, int )), 			 glwindow->glvoxel, SLOT(isosurface_new_slot(QString , int , int , int , Vector, bool*, int ))); 
	connect(volume_editor_tabdlg->isosurface_tab, SIGNAL(isosurface_get_signal(QString , int* , int* , int* , Vector*, int*, int*, int* )),  glwindow->glvoxel, SLOT(isosurface_get_slot(QString , int* , int* ,int* ,  Vector*, int*, int*, int* ))); 
	connect(volume_editor_tabdlg->isosurface_tab, SIGNAL(isosurface_del_signal(QString )), 													 glwindow->glvoxel, SLOT(isosurface_del_slot(QString ))); 
	connect(volume_editor_tabdlg->isosurface_tab, SIGNAL(isosurface_update_signal(QString, QString , int , int, int , Vector, bool*, int )), glwindow->glvoxel, SLOT(isosurface_update_slot(QString, QString , int, int , int , Vector, bool*, int ))); 
	connect(volume_editor_tabdlg->isosurface_tab, SIGNAL(isosurface_load_signal(QString)),													 glwindow->glvoxel, SLOT(isosurface_load_slot(QString)));
	connect(volume_editor_tabdlg->isosurface_tab, SIGNAL(isosurface_save_signal(void)), 													 glwindow->glvoxel, SLOT(isosurface_save_slot(void)));	
	connect(volume_editor_tabdlg->isosurface_tab, SIGNAL(isosurface_crop_signal(int)), 													 glwindow->glvoxel, SLOT(isosurface_crop_slot(int)));	

	// -------------------------------------------------------------------------
	// View Editor
	// -------------------------------------------------------------------------
	connect(view_editor_tabdlg->rendersettings_tab, SIGNAL(rendersettings_framebuffer_lod_signal(int)),				glwindow->glvoxel, SLOT(rendersettings_framebuffer_lod_slot(int)) );
	connect(view_editor_tabdlg->rendersettings_tab, SIGNAL(rendersettings_framebuffer_signal(int)), 				glwindow->glvoxel, SLOT(rendersettings_framebuffer_slot(int)) );
	connect(view_editor_tabdlg->rendersettings_tab, SIGNAL(rendersettings_postprocess_signal(int)), 				glwindow->glvoxel, SLOT(rendersettings_postprocess_slot(int)) );
	//connect(view_editor_tabdlg->rendersettings_tab, SIGNAL(rendersettings_silhouettes_signal(int)), 				glwindow->glvoxel, SLOT(rendersettings_silhouettes_slot(int)) );
	connect(view_editor_tabdlg->rendersettings_tab, SIGNAL(rendersettings_channelmode_signal(int,int,int,int)),		glwindow->glvoxel, SLOT(rendersettings_channelmode_slot(int,int,int,int)) );
	connect(view_editor_tabdlg->rendersettings_tab, SIGNAL(rendersettings_numbslices_signal(int)), 					glwindow->glvoxel, SLOT(rendersettings_numbslices_slot(int)) );
	connect(view_editor_tabdlg->rendersettings_tab, SIGNAL(rendersettings_numbsliceslod_signal(int)), 				glwindow->glvoxel, SLOT(rendersettings_numbsliceslod_slot(int)) );
	connect(view_editor_tabdlg->rendersettings_tab, SIGNAL(rendersettings_numbsliceslodtoggle_signal(int)), 		glwindow->glvoxel, SLOT(rendersettings_numbsliceslodtoggle_slot(int)) );
	connect(view_editor_tabdlg->rendersettings_tab, SIGNAL(rendersettings_framebuffertoggle_signal(int)), 			glwindow->glvoxel, SLOT(rendersettings_framebuffertoggle_slot(int)) );
	connect(view_editor_tabdlg->rendersettings_tab, SIGNAL(rendersettings_postprocesstoggle_signal(int)), 			glwindow->glvoxel, SLOT(rendersettings_postprocesstoggle_slot(int)) );
	connect(view_editor_tabdlg->rendersettings_tab, SIGNAL(rendersettings_maxtexturesize_signal(int)), 				glwindow->glvoxel, SLOT(rendersettings_maxtexturesize_slot(int)) );
	connect(view_editor_tabdlg->rendersettings_tab, SIGNAL(rendersettings_renderquality_signal(float)), 			glwindow->glvoxel, SLOT(rendersettings_renderquality_slot(float)) );
	connect(view_editor_tabdlg->rendersettings_tab, SIGNAL(rendersettings_renderqualitylod_signal(float)), 			glwindow->glvoxel, SLOT(rendersettings_renderqualitylod_slot(float)) );

#ifdef OPENCL_SUPPORT
	connect(view_editor_tabdlg->rendersettings_tab, SIGNAL(rendersettings_openclplat_signal(int)), 			glwindow->glvoxel, SLOT(rendersettings_openclplat_slot(int)) );
#endif

	// -------------------------------------------------------------------------
	// Lighting Editor
	// -------------------------------------------------------------------------
	connect(light_editor_tabdlg->lightettings_tab, SIGNAL(light_azimuth_signal(float)), 										glwindow->glvoxel, SLOT(light_azimuth_slot(float)) );
	connect(light_editor_tabdlg->lightettings_tab, SIGNAL(light_altitude_signal(float)), 										glwindow->glvoxel, SLOT(light_altitude_slot(float)) );
	connect(light_editor_tabdlg->lightettings_tab, SIGNAL(light_colour_signal(Vector,Vector,Vector)), 							glwindow->glvoxel, SLOT(light_colour_slot(Vector,Vector,Vector)) );
	connect(light_editor_tabdlg->lightettings_tab, SIGNAL(light_volshader_signal(int)), 										glwindow->glvoxel, SLOT(light_volshader_slot(int)) );
	connect(light_editor_tabdlg->lightettings_tab, SIGNAL(light_gooch_lighting_signal(Vector, Vector, int, int, int, int)), 	glwindow->glvoxel, SLOT(light_gooch_lighting_slot(Vector, Vector, int, int, int, int)) );

	connect(light_editor_tabdlg->lightettings_tab, SIGNAL(light_depthcue_signal(Vector, int)), 	glwindow->glvoxel, SLOT(light_depthcue_slot(Vector, int)) );
	connect(light_editor_tabdlg->lightettings_tab, SIGNAL(light_depthpeal_signal(int, int)), 	glwindow->glvoxel, SLOT(light_depthpeal_slot(int, int)) );

	// -------------------------------------------------------------------------
	// Stereo Editor
	// -------------------------------------------------------------------------
	connect(stereo_editor_tabdlg->stereosettings_tab, SIGNAL(stereo_onoff_signal(int)), 		glwindow->glvoxel, SLOT(stereo_onoff_slot(int)) );
	connect(stereo_editor_tabdlg->stereosettings_tab, SIGNAL(stereo_rendermode_signal(int)), 	glwindow->glvoxel, SLOT(stereo_rendermode_slot(int)) );
	connect(stereo_editor_tabdlg->stereosettings_tab, SIGNAL(stereo_seperation_signal(float)), 	glwindow->glvoxel, SLOT(stereo_seperation_slot(float)) );
	//connect(stereo_editor_tabdlg->stereosettings_tab, SIGNAL(stereo_zscreen_signal(int)), 	glwindow->glvoxel, SLOT(stereo_zscreen_signal(int)) );

	// -------------------------------------------------------------------------
	// Camera Editor
	// -------------------------------------------------------------------------
	connect(camera_editor_tabdlg->camerasettings_tab, SIGNAL(camerasettings_fov_signal(int)),							glwindow->glvoxel, SLOT(camerasettings_fov_slot(int)) );
	connect(camera_editor_tabdlg->camerasettings_tab, SIGNAL(camerasettings_mode_signal(int)),							glwindow->glvoxel, SLOT(camerasettings_mode_slot(int)) );
	connect(camera_editor_tabdlg->camerasettings_tab, SIGNAL(camerasettings_step_signal(int)),							glwindow->glvoxel, SLOT(camerasettings_step_slot(int)) );
	
	
	// -------------------------------------------------------------------------
	// Measure Editor
	// -------------------------------------------------------------------------
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_start_signal(void)),								glwindow->glvoxel, SLOT(measure_start_slot(void)) );
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_stop_signal(void)),								glwindow->glvoxel, SLOT(measure_stop_slot(void)) );
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_edit_signal(void)),								glwindow->glvoxel, SLOT(measure_edit_slot(void)) );
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_pause_signal(void)),								glwindow->glvoxel, SLOT(measure_pause_slot(void)));

	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_modify_start_signal(void)), glwindow->glvoxel, SLOT(measure_modify_start_slot(void)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_modify_stop_signal(void)), glwindow->glvoxel, SLOT(measure_modify_stop_slot(void)));

	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_add_item_signal(MeasureObject, int)), 			glwindow->glvoxel, SLOT(measure_add_item_slot(MeasureObject, int)) );
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_update_item_signal(int, MeasureObject, int)),	glwindow->glvoxel, SLOT(measure_update_item_slot(int, MeasureObject, int)) );

	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_delete_item_signal(int)),							glwindow->glvoxel, SLOT(measure_delete_item_slot(int)) );
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_update_current_signal(int)),						glwindow->glvoxel, SLOT(measure_update_current_slot(int)) );
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_update_current_signal(MeasureObject)),				glwindow->glvoxel, SLOT(measure_update_current_slot(MeasureObject)) );

	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_update_current_drawmode_signal(int)), 				glwindow->glvoxel, SLOT(measure_update_current_drawmode_slot(int)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_update_current_editmode_signal(int)), 				glwindow->glvoxel, SLOT(measure_update_current_editmode_slot(int)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_update_current_viewmode_signal(int)), 				glwindow->glvoxel, SLOT(measure_update_current_viewmode_slot(int)));

	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_setscale_signal(float, float, float)), 			this,			   SLOT(measure_setscale_slot(float, float, float)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_setcutplane_signal(int, float, float, float)),		glwindow->glvoxel, SLOT(measure_setcutplane_slot(int, float, float, float)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_enableunselect_signal(void)),		clipping_editor_tabdlg->cliptab,   SLOT(measure_enableunselect_slot(void)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_disableunselect_signal(void)),		clipping_editor_tabdlg->cliptab,   SLOT(measure_disableunselect_slot(void)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_setcutplane_signal(int, float, float, float)),		clipping_editor_tabdlg->cliptab, SLOT(measure_setcutplane_slot(int, float, float, float)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_clearcurrent_signal(void)),							glwindow->glvoxel, SLOT(measure_clearcurrent_slot(void)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_clearall_signal(void)),							glwindow->glvoxel, SLOT(measure_clearall_slot(void)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_cancel_signal(void)),								glwindow->glvoxel, SLOT(measure_cancel_slot(void)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_update_visible_signal(int, bool)),					glwindow->glvoxel, SLOT(measure_update_visible_slot(int, bool)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(cutplane_resetall_signal(void)), 							glwindow->glvoxel, SLOT(cutplane_resetall_slot(void)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(cutplane_solid_signal(bool)), 								glwindow->glvoxel, SLOT(cutplane_solid_slot(bool)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(cutplane_flip_signal(bool)), 								glwindow->glvoxel, SLOT(cutplane_flip_slot(bool)));

	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_labels_signal(int, int, int, int, int, int, int)), glwindow->glvoxel, SLOT(measure_labels_slot(int, int, int, int, int, int, int)));
	
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_rmv_currentvert_signal(MeasureObject, int)),					glwindow->glvoxel, SLOT(measure_rmv_currentvert_slot(MeasureObject, int)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_rmv_currentface_signal(int)), glwindow->glvoxel, SLOT(measure_rmv_currentface_slot(int)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_rmv_currentedge_signal(int)), glwindow->glvoxel, SLOT(measure_rmv_currentedge_slot(int)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_rmv_currentlabel_signal(int,bool)), glwindow->glvoxel, SLOT(measure_rmv_currentlabel_slot(int,bool)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_selected_vertindex_signal(int)),					glwindow->glvoxel, SLOT(measure_selected_vertindex_slot(int)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_selected_faceindex_signal(int)), glwindow->glvoxel, SLOT(measure_selected_faceindex_slot(int)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_selected_edgeindex_signal(int)), glwindow->glvoxel, SLOT(measure_selected_edgeindex_slot(int)));

	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measureCursorOn_signal(void)), this, SLOT(measureCursorOn_slot(void)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measureCursorOff_signal(void)), this, SLOT(measureCursorOff_slot(void)));

	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_facealign_orient_signal(bool)), glwindow->glvoxel, SLOT(measure_facealign_orient_slot(bool)));
	
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_section_toggle_signal(bool)), glwindow->glvoxel, SLOT(measure_section_toggle_slot(bool)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_section_trans_signal(int)), glwindow->glvoxel, SLOT(measure_section_trans_slot(int)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_section_thickn_signal(int)), glwindow->glvoxel, SLOT(measure_section_thickn_slot(int)));
	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(measure_facealign_updateview_signal(bool)), glwindow->glvoxel, SLOT(measure_facealign_updateview_slot(bool)));

	connect(measure_editor_tabdlg->measuring_tab, SIGNAL(meausre_setuprendering_signal(void)), glwindow->glvoxel, SLOT(meausre_setuprendering_slot(void)));
	
	// -------------------------------------------------------------------------
	// Movie Editor
	// -------------------------------------------------------------------------
	connect(movie_editor_tabdlg->moviesettings_tab, SIGNAL(movie_orthosectmovie_signal(QWidget*, int, int, QString)), 										glwindow->glvoxel, SLOT(movie_orthosectmovie_slot(QWidget*, int, int, QString)));

	connect(movie_editor_tabdlg->moviesettings_tab, SIGNAL(movie_rockmovie_signal(QWidget*, int, int, QString, float)), 						glwindow->glvoxel, SLOT(movie_rockmovie_slot(QWidget*, int, int, QString, float)));
	connect(movie_editor_tabdlg->moviesettings_tab, SIGNAL(movie_rotmovie_signal(QWidget*, int, int, QString, int, int, int, float)), 						glwindow->glvoxel, SLOT(movie_rotmovie_slot(QWidget*, int, int, QString, int, int, int, float)));
	//connect(movie_editor_tabdlg->moviesettings_tab, SIGNAL(movie_fademovie_signal(QWidget*, int, int, QString, int, int, int, float, int, int, int)), 		glwindow->glvoxel, SLOT(movie_fademovie_slot(QWidget*, int, int, QString, int, int, int, float, int, int, int)));
	connect(movie_editor_tabdlg->moviesettings_tab, SIGNAL(movie_clipmovie_signal(QWidget*, int, int, QString, int, int, int, float, int, int)), 			glwindow->glvoxel, SLOT(movie_clipmovie_slot(QWidget*, int, int, QString, int, int, int, float, int, int)));
	//connect(movie_editor_tabdlg->moviesettings_tab, SIGNAL(movie_crossfademovie_signal(QWidget*, int, int, QString, int, int, int, float, int, int, int)), 	glwindow->glvoxel, SLOT(movie_crossfademovie_slot(QWidget*, int, int, QString, int, int, int, float, int, int, int)));
	connect(movie_editor_tabdlg->moviesettings_tab, SIGNAL(movie_renderviewmovie_signal(QWidget*, int, int, QString)), 	glwindow->glvoxel, SLOT(movie_renderviewmovie_slot(QWidget*, int, int, QString)));

	

	// -------------------------------------------------------------------------
	//Renderview editor
	// -------------------------------------------------------------------------
	connect(renderview_editor_tabdlg->renviewtab, SIGNAL(renderview_saveview_signal(void)), 	glwindow->glvoxel, SLOT(renderview_saveview_slot(void)));
	connect(renderview_editor_tabdlg->renviewtab, SIGNAL(renderview_updateview_signal(int)), 	glwindow->glvoxel, SLOT(renderview_updateview_slot(int)));

	connect(renderview_editor_tabdlg->renviewtab, SIGNAL(renderview_setview_signal(VolumeRenderView)), 	glwindow->glvoxel, SLOT(renderview_setview_slot(VolumeRenderView)));
	connect(renderview_editor_tabdlg->renviewtab, SIGNAL(renderview_renderview_signal(VolumeRenderView)), 	glwindow->glvoxel, SLOT(renderview_renderview_slot(VolumeRenderView)));
	connect(renderview_editor_tabdlg->renviewtab, SIGNAL(renderview_interpolate_signal(vector< VolumeRenderView >)), 	glwindow->glvoxel, SLOT(renderview_interpolate_slot(vector< VolumeRenderView >)));

	// -------------------------------------------------------------------------
	//Datasetview editor
	// -------------------------------------------------------------------------

	connect(datasetview_editro_tabdlg->datasetviewtab, SIGNAL(datasetview_loadFolder_signal(QString)), 	glwindow->glvoxel, SLOT(datasetview_loadFolder_slot(QString)));
	connect(datasetview_editro_tabdlg->datasetviewtab, SIGNAL(datasetview_updatethumbnail_signal(int)), 	glwindow->glvoxel, SLOT(datasetview_updatethumbnail_slot(int)));
	connect(datasetview_editro_tabdlg->datasetviewtab, SIGNAL(datasetview_saveSettings_signal(QString, QString)), 	this, SLOT(datasetview_saveSettings_slot(QString, QString)));
	connect(datasetview_editro_tabdlg->datasetviewtab, SIGNAL(datasetview_loadSettings_signal(QString, QString)), 	this, SLOT(datasetview_loadSettings_slot(QString, QString)));
	connect(datasetview_editro_tabdlg->datasetviewtab, SIGNAL(datasetview_edit_MSR_signal(QString, QString, int)), 	this, SLOT(datasetview_edit_MSR_slot(QString, QString, int)));
	connect(datasetview_editro_tabdlg->datasetviewtab, SIGNAL(datasetview_align_signal(DatasetView)), 				this, SLOT(datasetview_align_slot(DatasetView)));
	connect(datasetview_editro_tabdlg->datasetviewtab, SIGNAL(datasetview_computestacks_signal(QString, DatasetView)), 	this, SLOT(datasetview_computestacks_slot(QString, DatasetView)));
	
	// -------------------------------------------------------------------------
	//Filter editor
	// -------------------------------------------------------------------------
	connect(filter_editor_tabdlg->filtertab, SIGNAL(filter_signal(int, vector<bool>, vector<int>, vector<double>)),	glwindow->glvoxel,	SLOT(filter_slot(int, vector<bool>, vector<int>, vector<double>)));
	connect(filter_editor_tabdlg->filtertab, SIGNAL(filter_smthdilate_signal(int,int,int,int,int,float,int,int,int)),		glwindow->glvoxel,	SLOT(filter_smthdilate_slot(int,int,int,int,int,float,int,int,int)));

#ifdef OPENCL_SUPPORT
	connect(filter_editor_tabdlg->filtertab, SIGNAL(filter_gpu_smthdilate_signal(int,int,int,int,int,float,int,int,int)),		glwindow->glvoxel,	SLOT(filter_gpu_smthdilate_slot(int,int,int,int,int,float,int,int,int)));
#endif

	connect(filter_editor_tabdlg->filtertab, SIGNAL(filter_smthdilateabort_signal(void)),							glwindow->glvoxel,	SLOT(filter_smthdilateabort_slot(void)));
	connect(filter_editor_tabdlg->filtertab, SIGNAL(filter_maxflow_interact_signal(int)),							glwindow->glvoxel,	SLOT(filter_maxflow_interact_slot(int)));
	connect(filter_editor_tabdlg->filtertab, SIGNAL(filter_maxflow_buildlabels_signal(void)),						glwindow,	SLOT(filter_maxflow_buildlabels_slot(void)));
	connect(filter_editor_tabdlg->filtertab, SIGNAL(filter_maxflow_clear_signal(void)),								glwindow,	SLOT(filter_maxflow_clear_slot(void)));
	connect(filter_editor_tabdlg->filtertab, SIGNAL(filter_maxflow_open_signal(void)),								glwindow,	SLOT(filter_maxflow_open_slot(void)));
	connect(filter_editor_tabdlg->filtertab, SIGNAL(filter_maxflow_save_signal(void)),								glwindow,	SLOT(filter_maxflow_save_slot(void)));
	//connect(filter_editor_tabdlg->filtertab, SIGNAL(filter_maxflow_brush_signal(float)),								glwindow->glvoxel,	SLOT(filter_maxflow_brush_slot(float)));
	connect(filter_editor_tabdlg->filtertab, SIGNAL(filter_maxflow_brush_signal(float)),								glwindow,			SLOT(filter_maxflow_brush_slot(float)));	
	connect(filter_editor_tabdlg->filtertab, SIGNAL(filter_maxflow_dimmode_signal(int)),								glwindow,			SLOT(filter_maxflow_dimmode_slot(int)));

	// -------------------------------------------------------------------------
	//SliceFrame image
	// -------------------------------------------------------------------------
	connect(glwindow->xzlb, SIGNAL(sliceframe_pixel_signal(int,int,int,int)), glwindow->glvoxel, SLOT(sliceframe_pixel_slot(int,int,int,int)));
	connect(glwindow->yzlb, SIGNAL(sliceframe_pixel_signal(int,int,int,int)), glwindow->glvoxel, SLOT(sliceframe_pixel_slot(int,int,int,int)));
	connect(glwindow->xylb, SIGNAL(sliceframe_pixel_signal(int,int,int,int)), glwindow->glvoxel, SLOT(sliceframe_pixel_slot(int,int,int,int)));
	connect(glwindow->xzlb, SIGNAL(sliceframe_pixel_signal(int,int,int,int)), this, SLOT(sliceframe_pixel_slot(int,int,int,int)));
	connect(glwindow->yzlb, SIGNAL(sliceframe_pixel_signal(int,int,int,int)), this, SLOT(sliceframe_pixel_slot(int,int,int,int)));
	connect(glwindow->xylb, SIGNAL(sliceframe_pixel_signal(int,int,int,int)), this, SLOT(sliceframe_pixel_slot(int,int,int,int)));
	
	connect(glwindow->xylb, SIGNAL(sliceframe_selected_signal(int)), 											glwindow->xzlb, 	SLOT(sliceframe_selected_slot(int)));
	connect(glwindow->xylb, SIGNAL(sliceframe_selected_signal(int)),											glwindow->yzlb, 	SLOT(sliceframe_selected_slot(int)));
	connect(glwindow->xzlb, SIGNAL(sliceframe_selected_signal(int)),											glwindow->xylb, 	SLOT(sliceframe_selected_slot(int)));
	connect(glwindow->xzlb, SIGNAL(sliceframe_selected_signal(int)),											glwindow->yzlb, 	SLOT(sliceframe_selected_slot(int)));
	connect(glwindow->yzlb, SIGNAL(sliceframe_selected_signal(int)),											glwindow->xzlb, 	SLOT(sliceframe_selected_slot(int)));
	connect(glwindow->yzlb, SIGNAL(sliceframe_selected_signal(int)),											glwindow->xylb, 	SLOT(sliceframe_selected_slot(int)));

	connect(glwindow->xylb, SIGNAL(sliceframe_addlabel_signal(int, Vector)),										glwindow, 	SLOT(sliceframe_addlabel_slot(int, Vector)));
	connect(glwindow->xzlb, SIGNAL(sliceframe_addlabel_signal(int, Vector)),										glwindow, 	SLOT(sliceframe_addlabel_slot(int, Vector)));
	connect(glwindow->yzlb, SIGNAL(sliceframe_addlabel_signal(int, Vector)),										glwindow, 	SLOT(sliceframe_addlabel_slot(int, Vector)));


/*	connect(glwindow->xylb, SIGNAL(sliceframe_addlabels_signal(int, int, int, int, int)),						glwindow->xzlb, 	SLOT(sliceframe_addlabels_slot(int, int, int, int, int)));
	connect(glwindow->xylb, SIGNAL(sliceframe_addlabels_signal(int, int, int, int, int)),						glwindow->yzlb, 	SLOT(sliceframe_addlabels_slot(int, int, int, int, int)));
	connect(glwindow->xylb, SIGNAL(sliceframe_addlabels_signal(int, int, int, int, int)),						glwindow->xylb, 	SLOT(sliceframe_addlabels_slot(int, int, int, int, int)));

	connect(glwindow->xzlb, SIGNAL(sliceframe_addlabels_signal(int, int, int, int, int)),						glwindow->xylb, 	SLOT(sliceframe_addlabels_slot(int, int, int, int, int)));
	connect(glwindow->xzlb, SIGNAL(sliceframe_addlabels_signal(int, int, int, int, int)),						glwindow->yzlb, 	SLOT(sliceframe_addlabels_slot(int, int, int, int, int)));
	connect(glwindow->xzlb, SIGNAL(sliceframe_addlabels_signal(int, int, int, int, int)),						glwindow->xzlb, 	SLOT(sliceframe_addlabels_slot(int, int, int, int, int)));

	connect(glwindow->yzlb, SIGNAL(sliceframe_addlabels_signal(int, int, int, int, int)),						glwindow->xylb, 	SLOT(sliceframe_addlabels_slot(int, int, int, int, int)));
	connect(glwindow->yzlb, SIGNAL(sliceframe_addlabels_signal(int, int, int, int, int)),						glwindow->xzlb, 	SLOT(sliceframe_addlabels_slot(int, int, int, int, int)));
	connect(glwindow->yzlb, SIGNAL(sliceframe_addlabels_signal(int, int, int, int, int)),						glwindow->yzlb, 	SLOT(sliceframe_addlabels_slot(int, int, int, int, int)));
*/
	// -------------------------------------------------------------------------
	//Crop Editor
	// -------------------------------------------------------------------------
	connect(crop_editor_tabdlg->croptab, SIGNAL(crop_start_signal(void)), glwindow->glvoxel, SLOT(crop_start_slot(void)));
	connect(crop_editor_tabdlg->croptab, SIGNAL(crop_end_signal(vector<bool>)),  glwindow->glvoxel, SLOT(crop_end_slot(vector<bool>)));
	connect(crop_editor_tabdlg->croptab, SIGNAL(crop_recentre_signal(void)),  glwindow->glvoxel, SLOT(cutplane_recentre_slot(void)));
	connect(crop_editor_tabdlg->croptab, SIGNAL(crop_invsel_signal(int)),  glwindow->glvoxel, SLOT(crop_invsel_slot(int)));
	connect(crop_editor_tabdlg->croptab, SIGNAL(crop_reset_signal(void)),  glwindow->glvoxel, SLOT(crop_reset_slot(void)));

	// -------------------------------------------------------------------------
	//Transfer Function Editor
	// -------------------------------------------------------------------------
	connect(transferfunction_editor_tabdlg->tranfer_function_tab,		SIGNAL(tfunc_rebuild_histogram_signal()),										glwindow->glvoxel, SLOT(tfunc_rebuild_histogram_slot()));		
 	connect(transferfunction_editor_tabdlg->tranfer_function_tab,		SIGNAL(tfunc_histoupdate_signal(int)), 											glwindow->glvoxel, SLOT(tfunc_histoupdate_slot(int)));

	// -------------------------------------------------------------------------
	//Histogram Widget
	// -------------------------------------------------------------------------
	connect(transferfunction_editor_tabdlg->tranfer_function_tab->hist, SIGNAL(hist_tfunc_signal(int, TfuncState)), glwindow->glvoxel, SLOT(hist_tfunc_slot(int, TfuncState)));
	connect(transferfunction_editor_tabdlg->tranfer_function_tab->hist, SIGNAL(hist_redraw_signal(void)), glwindow->glvoxel, SLOT(redraw_event(void)));
	
	// -------------------------------------------------------------------------
	// GLWidget
	// -------------------------------------------------------------------------
	connect(glwindow->glvoxel, SIGNAL(arc_mode_signal(void)), 						this, SLOT(arc_mode_slot(void)));
	connect(glwindow->glvoxel, SIGNAL(frames_redraw_signal(void)), 					glwindow, SLOT(frames_redraw_slot(void)));
	connect(glwindow->glvoxel, SIGNAL(dragndrop_signal(QString)), 					this, SLOT(dragndrop_slot(QString)));
	connect(glwindow->glvoxel, SIGNAL(isosurface_addnew_signal(QString, Vector)),	volume_editor_tabdlg->isosurface_tab, SLOT(addnew_event(QString, Vector)));
	connect(glwindow->glvoxel, SIGNAL(single_channel_signal(void)), 				this, SLOT(single_channel_slot(void)));
	

	connect(glwindow->glvoxel, SIGNAL(measure_updatecurrent_signal(MeasureObject)),					measure_editor_tabdlg->measuring_tab, SLOT(measure_updatecurrent_slot(MeasureObject)));
	connect(glwindow->glvoxel, SIGNAL(measure_point_signal(Vector)),								measure_editor_tabdlg->measuring_tab, SLOT(measure_point_slot(Vector)));
	connect(glwindow->glvoxel, SIGNAL(measure_editpoint_signal(Vector)),							measure_editor_tabdlg->measuring_tab, SLOT(measure_editpoint_slot(Vector)));
	connect(glwindow->glvoxel, SIGNAL(measure_editpoint_select_signal(int)),						measure_editor_tabdlg->measuring_tab, SLOT(measure_editpoint_select_slot(int)));
	connect(glwindow->glvoxel, SIGNAL(measure_savecutplane_signal(int, float, float, float)),		measure_editor_tabdlg->measuring_tab, SLOT(measure_savecutplane_slot(int, float, float, float)));
	connect(glwindow->glvoxel, SIGNAL(measure_close_signal(void)),									measure_editor_tabdlg->measuring_tab, SLOT(measure_close_slot( void)));
	connect(glwindow->glvoxel, SIGNAL(measure_clearcurrent_signal(void)),							measure_editor_tabdlg->measuring_tab, SLOT(measure_clearcurrent_slot( void)));
	connect(glwindow->glvoxel, SIGNAL(measure_editgrppoint_signal(int, Vector)),					measure_editor_tabdlg->measuring_tab, SLOT(measure_editgrppoint_slot(int, Vector)));
	connect(glwindow->glvoxel, SIGNAL(measure_addvertface_signal(int)),								measure_editor_tabdlg->measuring_tab, SLOT(measure_addvertface_slot(int)));
	connect(glwindow->glvoxel, SIGNAL(measure_rmvvertface_signal(int)),								measure_editor_tabdlg->measuring_tab, SLOT(measure_rmvvertface_slot(int)));
	connect(glwindow->glvoxel, SIGNAL(measure_endvertface_signal(void)),							measure_editor_tabdlg->measuring_tab, SLOT(measure_endvertface_slot(void)));
	connect(glwindow->glvoxel, SIGNAL(measure_edge_select_signal(int)),								measure_editor_tabdlg->measuring_tab, SLOT(measure_edge_select_slot(int)));
	connect(glwindow->glvoxel, SIGNAL(measure_face_select_signal(int)),								measure_editor_tabdlg->measuring_tab, SLOT(measure_face_select_slot(int)));

	connect(glwindow->glvoxel, SIGNAL(cutplane_unselect_signal(void)),				clipping_editor_tabdlg->cliptab, SLOT(cutplane_unselect_slot(void)));
	connect(glwindow->glvoxel, SIGNAL(cutplane_selectedupdate_signal(int,int)),		clipping_editor_tabdlg->cliptab, SLOT(cutplane_selectedupdate_slot(int,int)));
	connect(glwindow->glvoxel, SIGNAL(cutplane_selected_signal(int,int)),			clipping_editor_tabdlg->cliptab, SLOT(cutplane_selected_slot(int,int)));
	connect(glwindow->glvoxel, SIGNAL(cutplane_enableplanepanel_signal(void)),		clipping_editor_tabdlg->cliptab, SLOT(cutplane_enableplanepanel_slot(void)));
	//connect(glwindow->glvoxel, SIGNAL(openImageDir_signal(QString)),				clipping_editor_tabdlg->cliptab, SLOT(openImageDir(QString)));
	connect(glwindow->glvoxel, SIGNAL(save_sectionimage_signal(QString, int, int)),	glwindow, SLOT(save_sectionimage_slot(QString, int, int)));
	connect(glwindow->glvoxel, SIGNAL(timelapseEvent(void)),	this, SLOT(timelapseEvent(void)));

	connect(glwindow->glvoxel, SIGNAL(update_channels_signal(void)),	this, SLOT(update_channels_slot(void)));
	connect(glwindow->glvoxel, SIGNAL(updatestatus_cursor_signal(void)),	this, SLOT(updatestatus_cursor_slot(void)));

	connect(glwindow->glvoxel, SIGNAL(renderview_updateview_signal(int, QPixmap*, VolumeRenderView)), renderview_editor_tabdlg->renviewtab, SLOT(renderview_updateview_slot(int, QPixmap*, VolumeRenderView)));
	connect(glwindow->glvoxel, SIGNAL(renderview_addview_signal(QPixmap*, VolumeRenderView)), renderview_editor_tabdlg->renviewtab, SLOT(renderview_addview_slot(QPixmap*, VolumeRenderView)));
	connect(glwindow->glvoxel, SIGNAL(renderview_addsavedview_signal(QPixmap*)), renderview_editor_tabdlg->renviewtab, SLOT(renderview_addsavedview_slot(QPixmap*)));
	connect(glwindow->glvoxel, SIGNAL(renderview_getallviews_signal(void)), renderview_editor_tabdlg->renviewtab, SLOT(renderview_getallviews_slot(void)));
	
	connect(glwindow->glvoxel, SIGNAL(datasetview_setThumbnail_signal(int, QPixmap)), datasetview_editro_tabdlg->datasetviewtab, SLOT(datasetview_setThumbnail_slot(int, QPixmap)));

	connect(glwindow->glvoxel, SIGNAL(mw_openImageDir_sinal(QString)), this ,SLOT(mw_openImageDir_slot(QString)));
	connect(glwindow->glvoxel, SIGNAL(mw_initImageDir_sinal(QString)), this ,SLOT(mw_initImageDir_slot(QString)));
//**************************************************************************************************************************************************

	connect(glwindow->glvoxel, SIGNAL(tfunc_histogram_signal(double*)), 	transferfunction_editor_tabdlg->tranfer_function_tab, SLOT(tfunc_histogram_slot(double*)));
	connect(glwindow->glvoxel, SIGNAL(tfunc_updateAll_signal(TfuncState, TfuncState, TfuncState, TfuncState, TfuncState, TfuncState, TfuncState, TfuncState)), 	transferfunction_editor_tabdlg->tranfer_function_tab, SLOT(tfunc_updateAll_slot(TfuncState, TfuncState, TfuncState, TfuncState, TfuncState, TfuncState, TfuncState, TfuncState)));
	
	connect(glwindow->glvoxel, SIGNAL(sliceframe_drawmode_signal(int)),			glwindow->xylb,	SLOT(sliceframe_drawmode_slot(int)));
	connect(glwindow->glvoxel, SIGNAL(sliceframe_drawmode_signal(int)),			glwindow->xzlb,	SLOT(sliceframe_drawmode_slot(int)));
	connect(glwindow->glvoxel, SIGNAL(sliceframe_drawmode_signal(int)),			glwindow->yzlb,	SLOT(sliceframe_drawmode_slot(int)));

	connect(glwindow->glvoxel, SIGNAL(camerasettings_setmode_signal(int,int)),		camera_editor_tabdlg->camerasettings_tab,	SLOT(camerasettings_setmode_slot(int,int)));
	
	connect(glwindow->glvoxel, SIGNAL(camerasettings_toggleUI_signal(bool)),		this,	SLOT(camerasettings_toggleUI_slot(bool)));
	
#ifdef OPENCL_SUPPORT
	connect(glwindow->glvoxel, SIGNAL(rendersettings_setopenclplat_signal(int, vector<string>)),			view_editor_tabdlg->rendersettings_tab,	SLOT(rendersettings_setopenclplat_slot(int, vector<string>)));
#endif

	// -------------------------------------------------------------------------
	// QFilesystemwatcher
	// -------------------------------------------------------------------------
	connect(qfilesystemwatcher, SIGNAL(fileChanged(QString)), this, SLOT(fileChanged(QString)));
}

void QtMainWindow::datasetview_edit_MSR_slot(QString src, QString dst, int index)
{
	printf("EDIT\n");
	measure_editor_tabdlg->measuring_tab->dataset_event(src, dst, index);
}
void QtMainWindow::datasetview_computestacks_slot(QString savedn, DatasetView dsview)
{
	printf("datasetview_computestacks_slot\n");

	if(dsview.datasubfolders.empty()) return;

	QString savesubdir = dsview.datafolder + "/" + "VVData/" + dsview.datasubfolders[0] + "/";
	glwindow->glvoxel->volrender.volrenderview.save(savesubdir+"view.vrv");
/*
	//Compute the bounding box
	Vector dbBB_min, dbBB_max;
	dbBB_min = Vector(8192,8192,8192);
	dbBB_max = Vector(-8192,-8192,-8192);
	for(int i=0; i<dsview.datasubfolders.size(); i++)
	{
		QString dn = dsview.datafolder + "/" + dsview.datasubfolders[i] + "/";
		printf("datadir: %s\n", dn.latin1());

		QString loadsubdir = dsview.datafolder + "/" + "VVData/" + dsview.datasubfolders[i] + "/";
		glwindow->glvoxel->volrender.volrenderview.load(loadsubdir+"view.vrv");

		mw_openImageDir_slot(dn);

		Vector trans = Vector(glwindow->glvoxel->volrender.volrenderview.tx, glwindow->glvoxel->volrender.volrenderview.ty, glwindow->glvoxel->volrender.volrenderview.tz);
		Vector bbmin = glwindow->glvoxel->volrender.volrenderview.bb_v7+trans;
		Vector bbmax = glwindow->glvoxel->volrender.volrenderview.bb_v2+trans;
			
		if(dbBB_min.x>bbmin.x)
		{
			dbBB_min.x = bbmin.x;
		}
		if(dbBB_min.y>bbmin.y)
		{
			dbBB_min.y = bbmin.y;
		}
		if(dbBB_min.z>bbmin.z)
		{
			dbBB_min.z = bbmin.z;
		}

		if(dbBB_max.x<bbmax.x)
		{
			dbBB_max.x = bbmax.x;
		}
		if(dbBB_max.y<bbmax.y)
		{
			dbBB_max.y = bbmax.y;
		}
		if(dbBB_max.z<bbmax.z)
		{
			dbBB_max.z = bbmax.z;
		}
	}
*/
	int numbslices = glwindow->glvoxel->volobject.maxres/4.0f;

	//Save the slices
	float zdist = 1.0;//dbBB_max.z - dbBB_min.z;
	zdist /= (float) numbslices;

	Vector currcentre;
	int xmin, xmax, ymin, ymax;
	int r,g,b,x,y;
	QRgb pixel;

	for(int ii=0; ii<dsview.datasubfolders.size(); ii++)
	{
		QString loaddatadir = dsview.datafolder + "/" + dsview.datasubfolders[ii] + "/";
		printf("loaddatadir: %s\n", loaddatadir.latin1());

		QString loadsubdir = dsview.datafolder + "/" + "VVData/" + dsview.datasubfolders[ii] + "/";
		glwindow->glvoxel->volrender.volrenderview.load(loadsubdir+"view.vrv");

		//Vector trans = Vector(glwindow->glvoxel->volrender.volrenderview.tx, glwindow->glvoxel->volrender.volrenderview.ty, glwindow->glvoxel->volrender.volrenderview.tz);

		mw_openImageDir_slot(loaddatadir);

		QString savedatadir = savedn + "/" + dsview.datasubfolders[ii] + "/";
		printf("savedatadir: %s\n", loaddatadir.latin1());
		
		QDir dir(savedatadir);
		if (!dir.exists())
		{
			dir.mkpath(savedatadir);
		}
/*
		Vector centre = (dbBB_max+dbBB_min)/2.0f;
//		currcentre = Vector(0.0, 0.0, 0.0);

//		Vector newcentre = -centre;

		printf("centre: %f, %f, %f\n", centre.x, centre.y, centre.z);
		
		glwindow->glvoxel->volrender.compute_boundingboxDS(dbBB_min, dbBB_max);
		
		glwindow->glvoxel->volrender.volrenderview.zoom = 0.0;

		//translate to centre of BB
		glwindow->glvoxel->volrender.volrenderview.tx = -centre.x+glwindow->glvoxel->volrender.volrenderview.tx;
		glwindow->glvoxel->volrender.volrenderview.ty = -centre.y+glwindow->glvoxel->volrender.volrenderview.ty;
		glwindow->glvoxel->volrender.volrenderview.tz = -centre.z+glwindow->glvoxel->volrender.volrenderview.tz;
*/
		glwindow->glvoxel->volrender.old_background_colour = Qt::black;
		glwindow->glvoxel->volrender.background_colour = Qt::black;

		minibutton_I_event();

		if(glwindow->glvoxel->volrender.scalebar_toggle) glwindow->glvoxel->volrender.scalebar_toggle=0;

		glwindow->glvoxel->volrender.volrenderview.plane_rendermode[0] = 2;
		glwindow->glvoxel->volrender.volrenderview.cutting_planes.selected[0] = 1;

		if(ii!=0) glwindow->glvoxel->volrender.volrenderview.cutting_planes.render_selected = 0;
		//Set view bottom.
		bottom_event();

		//return;
		//system("pause");

		for(int i=0; i<numbslices; i++)
		{
			QString filename = savedatadir + "Slice";
			QString extension = ".png";
			QString framenumber;		
			framenumber.setNum(i+100000);
			filename += framenumber;
			filename += extension;
			
			glwindow->glvoxel->cutplane_planeslider_slot(zdist);
			
			glwindow->glvoxel->volrender.r_channel = 1;
			if(glwindow->glvoxel->volrender.g_channel) glwindow->glvoxel->volrender.g_channel =0;
			if(glwindow->glvoxel->volrender.b_channel) glwindow->glvoxel->volrender.b_channel =0;
			glwindow->glvoxel->redraw_event();
			QImage redim = glwindow->glvoxel->volrender.render_framebuffer(false);

			glwindow->glvoxel->volrender.g_channel = 1;
			if(glwindow->glvoxel->volrender.r_channel) glwindow->glvoxel->volrender.r_channel =0;
			if(glwindow->glvoxel->volrender.b_channel) glwindow->glvoxel->volrender.b_channel =0;
			glwindow->glvoxel->redraw_event();
			QImage greenim = glwindow->glvoxel->volrender.render_framebuffer(false);

			glwindow->glvoxel->volrender.b_channel = 1;
			if(glwindow->glvoxel->volrender.r_channel) glwindow->glvoxel->volrender.r_channel =0;
			if(glwindow->glvoxel->volrender.g_channel) glwindow->glvoxel->volrender.g_channel =0;
			glwindow->glvoxel->redraw_event();
			QImage blueim = glwindow->glvoxel->volrender.render_framebuffer(false);

			//crop the images by the yellow bounding box from our cutting plane
			if(i==0 && ii==0)
			{
				y = blueim.height()/2.0;		
				for(x=blueim.width()/2.0; x>=0; x--)
				{
					pixel = blueim.pixel(x,y);
					r = qRed(pixel);
					g = qGreen(pixel);
					b = qBlue(pixel);

					if(r!=0 || g!=0)
					{
						xmin=x+1;
						x = -1;
					}
				}
				for(x=blueim.width()/2.0; x<blueim.width(); x++)
				{
					pixel = blueim.pixel(x,y);
					r = qRed(pixel);
					g = qGreen(pixel);
					b = qBlue(pixel);

					if(r!=0 || g!=0)
					{
						xmax=x;
						x = blueim.width();
					}
				}
				x = blueim.width()/2.0;		
				for(y=blueim.height()/2.0; y>=0; y--)
				{
					pixel = blueim.pixel(x,y);
					r = qRed(pixel);
					g = qGreen(pixel);
					b = qBlue(pixel);

					if(r!=0 || g!=0)
					{
						ymin=y+1;
						y = -1;
					}
				}
				for(y=blueim.height()/2.0; y<blueim.height(); y++)
				{
					pixel = blueim.pixel(x,y);
					r = qRed(pixel);
					g = qGreen(pixel);
					b = qBlue(pixel);

					if(r!=0 || g!=0)
					{
						ymax=y;
						y = blueim.height();
					}
				}
				printf("crop: %d, %d, %d, %d\n", xmin, ymin, xmax, ymax);
			}

			QImage redimcropped = redim.copy(xmin, ymin, xmax-xmin, ymax-ymin);
			QImage greenimcropped = greenim.copy(xmin, ymin, xmax-xmin, ymax-ymin);
			QImage blueimcropped = blueim.copy(xmin, ymin, xmax-xmin, ymax-ymin);

			for(int x=0; x<redimcropped.width(); x++)
			{
				for(int y=0; y<redimcropped.height(); y++)
				{
					pixel = redimcropped.pixel(x,y);
					r = qRed(pixel);
					pixel = greenimcropped.pixel(x,y);
					g = qGreen(pixel);
					pixel = blueimcropped.pixel(x,y);
					b = qBlue(pixel);

					pixel = qRgb(r,g,b);
					redimcropped.setPixel(x, y, pixel);
				}
			}

			QPixmap pm;
			pm.convertFromImage(redimcropped);
			if(!pm.isNull())
			{
				printf("saving: %s\n", filename.latin1());
				pm.save(filename, "PNG");	
			}

			redim.reset();
			greenim.reset();
			blueim.reset();

			redimcropped.reset();
			greenimcropped.reset();
			blueimcropped.reset();
		}

		glwindow->glvoxel->volrender.volrenderview.cutting_planes.reset_all();

	}
	glwindow->glvoxel->volrender.volrenderview.cutting_planes.render_selected = 1;
	//minibutton_I_event();
}
void QtMainWindow::datasetview_align_slot(DatasetView dsview)
{
	if(dsview.datasubfolders.empty()) return;

	QString savesubdir = dsview.datafolder + "/" + "VVData/" + dsview.datasubfolders[0] + "/";
	glwindow->glvoxel->volrender.volrenderview.save(savesubdir+"view.vrv");

	glwindow->glvoxel->volrender.slice_draw_mode = 0;

	//=========================================
	//Compute centroids of measure objects
	//=========================================
	vector< Vector > centroids;
	for(int i=0; i<dsview.datasubfolders.size(); i++)
	{
		QString savedir = dsview.datafolder + "/" + "VVData/";
		QString savesubdir = dsview.datafolder + "/" + "VVData/" + dsview.datasubfolders[i] + "/";
		
		measure_editor_tabdlg->measuring_tab->open_file(savesubdir+"align.msr");
		
		if(!measure_editor_tabdlg->measuring_tab->measure_objects.measure_objects.empty())
		{
			Vector t0_v0, t0_v1, c;
			t0_v0 = measure_editor_tabdlg->measuring_tab->measure_objects.measure_objects[i].vertices[0];
			t0_v1 = measure_editor_tabdlg->measuring_tab->measure_objects.measure_objects[i].vertices[1];

			t0_v0 = t0_v0.mult_matrix(glwindow->glvoxel->volrender.volrenderview.arcball.Transform.M);
			t0_v1 = t0_v1.mult_matrix(glwindow->glvoxel->volrender.volrenderview.arcball.Transform.M);

			c = (t0_v0 + t0_v1)*0.5f;

			centroids.push_back(c);
		}
	}
	measure_editor_tabdlg->measuring_tab->clear_all();

	//=========================================
	//Save the transformations to align the data
	//=========================================
	VolumeRenderView vrv0;
	for(int i=0; i<centroids.size(); i++)
	{
		QString savedir = dsview.datafolder + "/" + "VVData/";
		QString savesubdir = dsview.datafolder + "/" + "VVData/" + dsview.datasubfolders[i] + "/";

		QString savesubdir0 = dsview.datafolder + "/" + "VVData/" + dsview.datasubfolders[0] + "/";
		vrv0.load(savesubdir0+"view.vrv");

		if(!centroids.empty())
		{
			Vector c0 = centroids[0];
			Vector c1 = centroids[i];

			Vector diff = c0 - c1;
		//	diff = diff.mult_matrix(glwindow->glvoxel->volrender.volrenderview.arcball.Transform.M);

			vrv0.at = diff;
		}

		vrv0.save(savesubdir+"view.vrv");
	}

	//=========================================
	//Compute the bounding box for the whole series
	//=========================================
	Vector dbBB_min, dbBB_max;
	dbBB_min = Vector(8192,8192,8192);
	dbBB_max = Vector(-8192,-8192,-8192);
	//vrv0 = glwindow->glvoxel->volrender.volrenderview;
	for(int i=0; i<dsview.datasubfolders.size(); i++)
	{
		QString dn = dsview.datafolder + "/" + dsview.datasubfolders[i] + "/";

		QString loadsubdir = dsview.datafolder + "/" + "VVData/" + dsview.datasubfolders[i] + "/";
		vrv0.load(loadsubdir+"view.vrv");

		Vector trans = vrv0.at;
		Vector bbmin = glwindow->glvoxel->volrender.volrenderview.bb_v7+trans;
		Vector bbmax = glwindow->glvoxel->volrender.volrenderview.bb_v2+trans;
			
		if(dbBB_min.x>bbmin.x)
		{
			dbBB_min.x = bbmin.x;
		}
		if(dbBB_min.y>bbmin.y)
		{
			dbBB_min.y = bbmin.y;
		}
		if(dbBB_min.z>bbmin.z)
		{
			dbBB_min.z = bbmin.z;
		}

		if(dbBB_max.x<bbmax.x)
		{
			dbBB_max.x = bbmax.x;
		}
		if(dbBB_max.y<bbmax.y)
		{
			dbBB_max.y = bbmax.y;
		}
		if(dbBB_max.z<bbmax.z)
		{
			dbBB_max.z = bbmax.z;
		}
	}
		Vector bbmax = dbBB_max;
		Vector bbmin = dbBB_min;

		QString loadsubdir = dsview.datafolder + "/" + "VVData/" + dsview.datasubfolders[0] + "/";
		VolumeRenderView vvr;
		vvr.load(loadsubdir+"view.vrv");
		Vector c = (dbBB_max+dbBB_min)/2.0f;
		Vector size = dbBB_max-dbBB_min;
		//vvr.cutting_planes.init(c, size);
		vvr.bb_v1 = Vector(dbBB_min.x, dbBB_max.y, dbBB_max.z);
		vvr.bb_v2 = Vector(dbBB_max.x, dbBB_max.y, dbBB_max.z);
		vvr.bb_v3 = Vector(dbBB_min.x, dbBB_min.y, dbBB_max.z);
		vvr.bb_v4 = Vector(dbBB_max.x, dbBB_min.y, dbBB_max.z);
		vvr.bb_v5  = Vector(dbBB_min.x, dbBB_max.y, dbBB_min.z);
		vvr.bb_v6 = Vector(dbBB_max.x, dbBB_max.y, dbBB_min.z);
		vvr.bb_v7 = Vector(dbBB_min.x, dbBB_min.y, dbBB_min.z);
		vvr.bb_v8 = Vector(dbBB_max.x, dbBB_min.y, dbBB_min.z);
		vvr.save(loadsubdir+"view.vrv");

		for(int i=0; i<dsview.datasubfolders.size(); i++)
		{
			QString dn = dsview.datafolder + "/" + dsview.datasubfolders[i] + "/";
			//printf("datadir: %s\n", dn.latin1());

			QString loadsubdir = dsview.datafolder + "/" + "VVData/" + dsview.datasubfolders[i] + "/";
			vrv0.load(loadsubdir+"view.vrv");
		
			vrv0 = vvr;
			Vector c0 = centroids[0];
			Vector c1 = centroids[i];
			Vector diff = c0 - c1;
			//diff = diff.mult_matrix(glwindow->glvoxel->volrender.volrenderview.arcball.Transform.M);

			vrv0.at = diff;

			vrv0.cutting_planes.translate_by_vec(-diff);

			vrv0.bb_v1 -= diff;
			vrv0.bb_v2 -= diff;
			vrv0.bb_v3 -= diff;
			vrv0.bb_v4 -= diff;
			vrv0.bb_v5 -= diff;
			vrv0.bb_v6 -= diff;
			vrv0.bb_v7 -= diff;
			vrv0.bb_v8 -= diff;

			vrv0.save(loadsubdir+"view.vrv");
		}

	centroids.clear();
}
void QtMainWindow::datasetview_saveSettings_slot(QString dn, QString subdn)
{
	QString savedir = dn + "/" + "VVData/";
	QString savesubdir = dn + "/" + "VVData/" + subdn + "/";

	QDir dir(savesubdir);
	if (!dir.exists())
	{
		dir.mkpath(savesubdir);
	}
}
void QtMainWindow::datasetview_loadSettings_slot(QString dn, QString subdn)
{
	QString savedir = dn + "/" + "VVData/";
	QString savesubdir = dn + "/" + "VVData/" + subdn + "/";
	QString savesubdir2 = dn + "/" + subdn + "/";

	QDir dir(savesubdir);
	if (!dir.exists())
	{
		dir.mkpath(savesubdir);
	}

	printf("Loading: %s\n", savesubdir.latin1());

	QFile* file = new QFile(savesubdir+"view.vrv");
	if(file->exists())
	{
		glwindow->glvoxel->volrender.volrenderview.load(savesubdir+"view.vrv");
	}

	/*QFile* file2 = new QFile(savesubdir2+"measure.msr");
	if(file2->exists())
	{
		measure_editor_tabdlg->measuring_tab->clear_all();
		measure_editor_tabdlg->measuring_tab->open_file(savesubdir2+"measure.msr");
	}*/

	/*QFile* file3 = new QFile(savesubdir+"clip.slc");
	if(file3->exists())
	{
		glwindow->glvoxel->volrender.volrenderview.cutting_planes.load(savesubdir+"clip.slc");	
	}*/
}
void QtMainWindow::fileChanged(QString fn)
{
	load_inifile();
}
void QtMainWindow::updatestatus_cursor_slot(void)
{
	int r,g,b;

	int x = glwindow->glvoxel->volrender.cursor3D_texturespace.x;
	int y = glwindow->glvoxel->volrender.cursor3D_texturespace.x;
	int z = glwindow->glvoxel->volrender.cursor3D_texturespace.x;

	if(x>=glwindow->glvoxel->volobject.texwidth)  x=glwindow->glvoxel->volobject.texwidth-1;
	if(y>=glwindow->glvoxel->volobject.texheight) y=glwindow->glvoxel->volobject.texheight-1;
	if(z>=glwindow->glvoxel->volobject.texdepth)  z=glwindow->glvoxel->volobject.texdepth-1;
	if(x<0) x=0;
	if(y<0) y=0;
	if(z<0) z=0;

	QString s;
	QString text;
	text = " | <b>cursor:</b> [x:";
	s.setNum(x);
	text += s;
	if(x<10) text += " ";
	if(x<100) text += " ";
	if(x<1000) text += " ";
	text += " y:";
	s.setNum(y);
	text += s;
	if(y<10) text += " ";
	if(y<100) text += " ";
	if(y<1000) text += " ";
	text += " z:";
	s.setNum(z);
	text += s;
	if(z<10) text += " ";
	if(z<100) text += " ";
	if(z<1000) text += " ";
	text += "] ";

	int index = glwindow->glvoxel->volobject.get_index1D(x, y, z);
	int rval = (int) glwindow->glvoxel->volobject.texture3d[3*index+0];
	int gval = (int) glwindow->glvoxel->volobject.texture3d[3*index+1];
	int bval = (int) glwindow->glvoxel->volobject.texture3d[3*index+2];

	text += "[r:";
	s.setNum(rval);
	text += s;
//	if(rval<10) text += " ";
//	if(rval<100) text += " ";
	s.setNum(gval);
	text += " g:";
	text += s;
//	if(gval<10) text += " ";
//	if(gval<100) text += " ";
	s.setNum(bval);
	text += " b:";
	text += s;
//	if(bval<10) text += " ";
//	if(bval<100) text += " ";
	text += "] ";

	glwindow->cursor_label->setText(text);

	emit frames_redraw_signal();
}
void QtMainWindow::sliceframe_pixel_slot(int x,int y,int z, int mode)
{
	if(!glwindow->glvoxel->volobject.texture3d) return;

	QString s;
	QString text;
	text = " | <b>cursor:</b> [x:";
	s.setNum(x);
	text += s;
/*	if(x<10) text += " ";
	if(x<100) text += " ";
	if(x<1000) text += " ";*/
	text += " y:";
	s.setNum(y);
	text += s;
/*	if(y<10) text += " ";
	if(y<100) text += " ";
	if(y<1000) text += " ";*/
	text += " z:";
	s.setNum(z);
	text += s;
/*	if(z<10) text += " ";
	if(z<100) text += " ";
	if(z<1000) text += " ";*/
	text += "] ";

	int index = glwindow->glvoxel->volobject.get_index1D(x, y, z);
	int rval = (int) glwindow->glvoxel->volobject.texture3d[3*index+0];
	int gval = (int) glwindow->glvoxel->volobject.texture3d[3*index+1];
	int bval = (int) glwindow->glvoxel->volobject.texture3d[3*index+2];

	text += "[r:";
	s.setNum(rval);
	text += s;
	//if(rval<10) text += " ";
//	if(rval<100) text += " ";
	s.setNum(gval);
	text += " g:";
	text += s;
	//if(gval<10) text += " ";
	//if(gval<100) text += " ";
	s.setNum(bval);
	text += " b:";
	text += s;
	//if(bval<10) text += " ";
	//if(bval<100) text += " ";
	text += "] ";

	glwindow->cursor_label->setText(text);

	//printf("xyz: %d, %d, %d\n", x, y, z);

	/*if(mode==2)
	{
		glwindow->makeYZPixmapEvent(x);
		glwindow->makeXZPixmapEvent(glwindow->glvoxel->volobject.texheight-y);
		glwindow->makeXYPixmapEvent(glwindow->glvoxel->volobject.texdepth-z);
	}
	else
	{*/
		if(glwindow->glvoxel->maxflow_mode==-1) glwindow->makeYZPixmapEvent(x);
		if(glwindow->glvoxel->maxflow_mode==-1) glwindow->makeXZPixmapEvent(y);
		if(glwindow->glvoxel->maxflow_mode==-1) glwindow->makeXYPixmapEvent(z);
	//}

	emit frames_redraw_signal();
}	
void QtMainWindow::render0_event()
{
/*    render_mode->setItemChecked( render0_id, TRUE );
    render_mode->setItemChecked( render1_id, FALSE );
    render_mode->setItemChecked( render2_id, FALSE );
*/
	glwindow->glvoxel->volrender.volume_render_mode = 1;
	glwindow->glvoxel->volrender.load_TextureMap();
	view_editor_tabdlg->rebuild_tabs();
	glwindow->glvoxel->update();
	glwindow->glvoxel->redraw_event();
	update();
}

void QtMainWindow::render1_event()
{
/*    render_mode->setItemChecked( render0_id, FALSE );
    render_mode->setItemChecked( render1_id, TRUE );
    render_mode->setItemChecked( render2_id, FALSE );
*/
	glwindow->glvoxel->volrender.volume_render_mode = 5;
	glwindow->glvoxel->volrender.load_TextureMap();
	view_editor_tabdlg->rebuild_tabs();
	glwindow->glvoxel->update();
	glwindow->glvoxel->redraw_event();
	update();
}
void QtMainWindow::render2_event()
{
/*    render_mode->setItemChecked( render0_id, FALSE);
    render_mode->setItemChecked( render1_id, FALSE);
    render_mode->setItemChecked( render2_id, TRUE);
*/
	glwindow->glvoxel->volrender.volume_render_mode = 2;
	glwindow->glvoxel->volrender.load_TextureMap();
	view_editor_tabdlg->rebuild_tabs();
	glwindow->glvoxel->update();
	glwindow->glvoxel->redraw_event();
	update();
}
void QtMainWindow::render3_event()
{
 /*   render_mode->setItemChecked( render0_id, FALSE);
    render_mode->setItemChecked( render1_id, FALSE);
    render_mode->setItemChecked( render2_id, TRUE);
*/
	//pick_numb_bricks();

	glwindow->glvoxel->volrender.volume_render_mode = 4;
	glwindow->glvoxel->volrender.load_TextureMap();
	glwindow->glvoxel->update();
	glwindow->glvoxel->redraw_event();
	update();
}

void QtMainWindow::blend_mode1_event()
{
 /*   blend->setItemChecked( blend_id, TRUE );
    blend->setItemChecked( max_blend_id, FALSE );
    blend->setItemChecked( min_blend_id, FALSE );
*/
	glwindow->glvoxel->volrender.blend_mode(1);
	glwindow->glvoxel->update();
	glwindow->glvoxel->redraw_event();
	update();
}
void QtMainWindow::blend_mode2_event()
{
  /*  blend->setItemChecked( blend_id, FALSE );
    blend->setItemChecked( max_blend_id, TRUE );
    blend->setItemChecked( min_blend_id, FALSE );
*/
	glwindow->glvoxel->volrender.blend_mode(2);
	glwindow->glvoxel->update();
	glwindow->glvoxel->redraw_event();
	update();
}
void QtMainWindow::blend_mode3_event()
{
 /*   blend->setItemChecked( blend_id, FALSE );
    blend->setItemChecked( max_blend_id, FALSE );
    blend->setItemChecked( min_blend_id, TRUE );
*/
	glwindow->glvoxel->volrender.blend_mode(3);
	glwindow->glvoxel->update();
	glwindow->glvoxel->redraw_event();
	update();
}
void QtMainWindow::blend_mode4_event()
{
	/* 
		blend->setItemChecked( blend_id, FALSE );
		blend->setItemChecked( max_blend_id, FALSE );
		blend->setItemChecked( min_blend_id, FALSE );
		blend->setItemChecked( atten_blend_id, TRUE );
	*/

	glwindow->glvoxel->volrender.blend_mode(4);
	glwindow->glvoxel->update();
	glwindow->glvoxel->redraw_event();
	update();
}
void QtMainWindow::toggle_scalebar_event(void)
{
	//toggle our scalebar
	glwindow->glvoxel->volrender.scalebar_toggle = !glwindow->glvoxel->volrender.scalebar_toggle;

	glwindow->glvoxel->redraw_event();
	update();
}
void QtMainWindow::togglemoveLOD_event(void)
{
	/*
	glwindow->glvoxel->moveLOD_toggle = !glwindow->glvoxel->moveLOD_toggle;

	if(glwindow->glvoxel->moveLOD_toggle) view->setItemChecked( movelod_id, TRUE );
	else view->setItemChecked( movelod_id, FALSE);
	glwindow->glvoxel->update();
	glwindow->glvoxel->redraw_event();
	update();
	*/
}
void QtMainWindow::openImageR_event(void)
{
	channel_mode = 1;
	channel_dlg->close(1);
}
void QtMainWindow::openImageG_event(void)
{
	channel_mode = 2;
	channel_dlg->close(1);
}
void QtMainWindow::openImageB_event(void)
{
	channel_mode = 3;
	channel_dlg->close(1);
}
void QtMainWindow::openImageRGBA_event(void)
{
	channel_mode = 4;
	channel_dlg->close();
}
void QtMainWindow::openImageI_event(void)
{
	channel_mode = 5;
	channel_dlg->close(1);
}
void QtMainWindow::openImageChannelMode(int chanmode)
{
	promptChannels=false;
	channel_mode = chanmode;
	glwindow->glvoxel->update();
	update();
}
void QtMainWindow::openImagechannelmsgbox_event()
{
	if(promptChannels)
	{
		channel_dlg = new QDialog(this, Qt::WindowTitleHint | Qt::WindowSystemMenuHint |  Qt::WindowStaysOnTopHint);
		channel_dlg->setModal(true);
		channel_dlg->setWindowTitle("Select Channel Mode");

		Q3BoxLayout* channel_layout = new Q3BoxLayout(channel_dlg, Q3BoxLayout::LeftToRight, 4, 4, "channel_layout");
		QPushButton *rgba = new QPushButton("RGBA", channel_dlg);
		QPushButton *r	  = new QPushButton("R",	channel_dlg);
		QPushButton *g	  = new QPushButton("G",	channel_dlg);
		QPushButton *b	  = new QPushButton("B",	channel_dlg);

		connect(r,		SIGNAL(clicked()), SLOT(openImageR_event()));
		connect(g,		SIGNAL(clicked()), SLOT(openImageG_event()));
		connect(b,		SIGNAL(clicked()), SLOT(openImageB_event()));
		connect(rgba,		SIGNAL(clicked()), SLOT(openImageRGBA_event()));

		channel_layout->addWidget(rgba, 1);
		channel_layout->addWidget(r, 1);
		channel_layout->addWidget(g, 1);
		channel_layout->addWidget(b, 1);

		channel_dlg->setMinimumWidth(384);
		//updateStatusBar();

		channel_dlg->exec();
		glwindow->glvoxel->update();
		update();
	}
	promptChannels=true;
}
void QtMainWindow::update_info_labels(void)
{
	//filename
	QString text = "Filename: ";
	text += glwindow->glvoxel->volobject.file_name;

	//resolution
	QString s;
	text = " Resolution: ";
	s.setNum(glwindow->glvoxel->volobject.texwidth);
	text += s;
	text += "x";
	s.setNum(glwindow->glvoxel->volobject.texheight);
	text += s;
	text += "x";
	s.setNum(glwindow->glvoxel->volobject.texdepth);
	text += s;

	//data type
	text += " Data Type: UCHAR";
	glwindow->info_label->setText(text);
}
void QtMainWindow::update_channels_slot(void)
{
		intentb->setOn(true);
		intentb->setDisabled(false);

		if(glwindow->glvoxel->volobject.has_red==1)
		{
			glwindow->glvoxel->volrender.r_channel = true;
			redtb->setOn(true);
			redtb->setDisabled(false);
			transferfunction_editor_tabdlg->tranfer_function_tab->red_pb->setEnabled(true);
		}
		else
		{
			glwindow->glvoxel->volrender.r_channel = false;
			redtb->setOn(false);
			redtb->setDisabled(true);
			transferfunction_editor_tabdlg->tranfer_function_tab->red_pb->setEnabled(false);
		}

		if(glwindow->glvoxel->volobject.has_green==true)
		{
			glwindow->glvoxel->volrender.g_channel = true;
			greentb->setOn(true);
			greentb->setDisabled(false);
			transferfunction_editor_tabdlg->tranfer_function_tab->green_pb->setEnabled(true);
		}
		else
		{
			glwindow->glvoxel->volrender.g_channel = false;
			greentb->setOn(false);
			greentb->setDisabled(true);
			transferfunction_editor_tabdlg->tranfer_function_tab->green_pb->setEnabled(false);
		}

		if(glwindow->glvoxel->volobject.has_blue==true)
		{
			glwindow->glvoxel->volrender.b_channel = true;
			bluetb->setOn(true);
			bluetb->setDisabled(false);
			transferfunction_editor_tabdlg->tranfer_function_tab->blue_pb->setEnabled(true);
		}
		else
		{
			glwindow->glvoxel->volrender.b_channel = false;
			bluetb->setOn(false);
			bluetb->setDisabled(true);
			transferfunction_editor_tabdlg->tranfer_function_tab->blue_pb->setEnabled(false);
		}

		frames_redraw_signal();
}
#ifdef OMERO_SUPPORT
void QtMainWindow::saveOmeroImage_event(void)
{
	cout<<"OMERO: SaveImage started"<<endl;
	QtOMEROLoader* qtomeroloader = new QtOMEROLoader(this, 0, &omeroLoader, appPath);
	qtomeroloader->imgID_lineedit->setDisabled(true);
	connect(qtomeroloader->connect_button, SIGNAL(pressed()), this, SLOT(saveOmeroImage_SaveDataevent()));

	qtomeroloader->show();
}
void QtMainWindow::saveOmeroImage_Save(int dsid, string name, string descp)
{
	int xsize = glwindow->glvoxel->volobject.texwidth;
	int ysize = glwindow->glvoxel->volobject.texheight;
	int zsize = glwindow->glvoxel->volobject.texdepth;
	int csize = 3;
	int tsize = 1;

	double physX = glwindow->glvoxel->volobject.xscale/glwindow->glvoxel->volobject.x_resize;
	double physY = glwindow->glvoxel->volobject.yscale/glwindow->glvoxel->volobject.y_resize;
	double physZ = glwindow->glvoxel->volobject.zscale/glwindow->glvoxel->volobject.z_resize;

	if(omeroLoader.connect()==-1)
	{
		printf("ERROR: Failed to connect to OMERO server.\n");
		return;
	}

	//QString des = glwindow->glvoxel->volobject.file_name + "\n" +QString::fromStdString(descp);
	//descp = des.ascii();
	omeroLoader.saveImage(dsid, glwindow->glvoxel->volobject.file_name.ascii(), descp, xsize, ysize, zsize, csize, tsize, physX, physY, physZ, glwindow->glvoxel->volobject.texture3d);
}
void QtMainWindow::saveOmeroImage_Save(string name, string descp)
{
	int xsize = glwindow->glvoxel->volobject.texwidth;
	int ysize = glwindow->glvoxel->volobject.texheight;
	int zsize = glwindow->glvoxel->volobject.texdepth;
	int csize = 3;
	int tsize = 1;

	double physX = glwindow->glvoxel->volobject.xscale/glwindow->glvoxel->volobject.x_resize;
	double physY = glwindow->glvoxel->volobject.yscale/glwindow->glvoxel->volobject.y_resize;
	double physZ = glwindow->glvoxel->volobject.zscale/glwindow->glvoxel->volobject.z_resize;

	if(omeroLoader.connect()==-1)
	{
		printf("ERROR: Failed to connect to OMERO server.\n");
		return;
	}
//	QString des = glwindow->glvoxel->volobject.file_name + "\n" + QString::fromStdString(descp);
//	descp = des.ascii();
	omeroLoader.saveImage(glwindow->glvoxel->volobject.file_name.ascii(), descp, xsize, ysize, zsize, csize, tsize, physX, physY, physZ, glwindow->glvoxel->volobject.texture3d);
}
void QtMainWindow::saveOmeroImage_SaveDataevent(void)
{
	int xsize = glwindow->glvoxel->volobject.texwidth;
	int ysize = glwindow->glvoxel->volobject.texheight;
	int zsize = glwindow->glvoxel->volobject.texdepth;
	int csize = 3;
	int tsize = 1;

	double physX = glwindow->glvoxel->volobject.xscale/glwindow->glvoxel->volobject.x_resize;
	double physY = glwindow->glvoxel->volobject.yscale/glwindow->glvoxel->volobject.y_resize;
	double physZ = glwindow->glvoxel->volobject.zscale/glwindow->glvoxel->volobject.z_resize;

	if(omeroLoader.connect()==-1)
	{
		printf("ERROR: Failed to connect to OMERO server.\n");
		return;
	}
	omeroLoader.saveImage(glwindow->glvoxel->volobject.file_name.ascii(), "", xsize, ysize, zsize, csize, tsize, physX, physY, physZ, glwindow->glvoxel->volobject.texture3d);
	omeroLoader.disconnect();
}
void QtMainWindow::openOmeroImage_Load(void)
{
	glwindow->glvoxel->volio.load_OMERO(&omeroLoader);

	glwindow->glvoxel->volrender.volrenderview.zoom = 0.0;

	//glwindow->glvoxel->volobject.x_resize = glwindow->glvoxel->volobject.y_resize = glwindow->glvoxel->volobject.z_resize = 1.0;
	//glwindow->glvoxel->volobject.xsize = glwindow->glvoxel->volobject.xscale = omeroLoader.xphysize;
	//glwindow->glvoxel->volobject.ysize = glwindow->glvoxel->volobject.yscale = omeroLoader.yphysize;
	//glwindow->glvoxel->volobject.zsize = glwindow->glvoxel->volobject.zscale = omeroLoader.zphysize;

//	glwindow->glvoxel->volobject.maxsize =  max((float)glwindow->glvoxel->volobject.xsize, (float)glwindow->glvoxel->volobject.ysize);
//	glwindow->glvoxel->volobject.maxsize =  max((float)glwindow->glvoxel->volobject.maxsize, (float)glwindow->glvoxel->volobject.zsize);

	Vector res = Vector((glwindow->glvoxel->volobject.xsize/glwindow->glvoxel->volobject.maxsize), (glwindow->glvoxel->volobject.ysize/glwindow->glvoxel->volobject.maxsize), (glwindow->glvoxel->volobject.zsize/glwindow->glvoxel->volobject.maxsize));
	emit measure_setrescale_signal(glwindow->glvoxel->volobject.x_resize, glwindow->glvoxel->volobject.y_resize, glwindow->glvoxel->volobject.z_resize);
	emit measure_setscale2_signal(glwindow->glvoxel->volobject.xscale, glwindow->glvoxel->volobject.yscale, glwindow->glvoxel->volobject.zscale);
	emit measure_setspacing_signal(res.x, res.y, res.z);

	glwindow->glvoxel->volrender.volume_toggle=1;

	if(omeroLoader.cdim==1)
	{
		glwindow->glvoxel->volobject.is_greyscale=1;
		glwindow->glvoxel->volrender.r_channel = glwindow->glvoxel->volobject.has_red = 1;
		glwindow->glvoxel->volrender.g_channel = glwindow->glvoxel->volobject.has_green = 0;
		glwindow->glvoxel->volrender.b_channel = glwindow->glvoxel->volobject.has_blue = 0;
	}
	else if(omeroLoader.cdim==2)
	{
		glwindow->glvoxel->volobject.is_greyscale=0;
		glwindow->glvoxel->volrender.r_channel = glwindow->glvoxel->volobject.has_red = 1;
		glwindow->glvoxel->volrender.g_channel = glwindow->glvoxel->volobject.has_green = 1;
		glwindow->glvoxel->volrender.b_channel = glwindow->glvoxel->volobject.has_blue = 0;
	}
	else
	{
		glwindow->glvoxel->volobject.is_greyscale=0;
		glwindow->glvoxel->volrender.r_channel = glwindow->glvoxel->volobject.has_red = 1;
		glwindow->glvoxel->volrender.g_channel = glwindow->glvoxel->volobject.has_green = 1;
		glwindow->glvoxel->volrender.b_channel = glwindow->glvoxel->volobject.has_blue = 1;
	}

	update_channels_slot();

	QString imgid;
	imgid.setNum(omeroLoader.imageID);
	glwindow->glvoxel->volobject.file_name = "omero.imageID."+imgid;
	glwindow->glvoxel->volobject.file_type = 1;
	last_working_dir = "";

	//reset our transfer dialogue
	volume_editor_tabdlg->reset();

	//update our histogram
	volume_editor_tabdlg->rebuild_tabs();
	view_editor_tabdlg->rebuild_tabs();

	//update how many slices we discretize the volume with
	view_editor_tabdlg->rendersettings_tab->rebuild();

	//load our data to opengl
	glwindow->glvoxel->volrender.compute_boundingbox();
	glwindow->glvoxel->volrender.load_TextureMap();

	//setup our slice arrays and slice sliders
	glwindow->create();
	updateStatusBar();

	glwindow->glvoxel->redraw_event();
}

void QtMainWindow::openOmeroImage_LoadDataevent(void)
{
	cout<<"OMERO: LoadImage started"<<endl;
	//connect to OMERO server
	if(omeroLoader.connect()==-1)
	{
		printf("ERROR: Failed to connect to OMERO server.\n");
		return;
	}	
	
	openOmeroImage_Load();

	omeroLoader.disconnect();

	cout<<"OMERO: LoadImage finished"<<endl;
}
void QtMainWindow::openOmeroImage_event(void)
{
	QtOMEROLoader* qtomeroloader = new QtOMEROLoader(this, 0, &omeroLoader, appPath);
	connect(qtomeroloader->connect_button, SIGNAL(pressed()), this, SLOT(openOmeroImage_LoadDataevent()));

	qtomeroloader->show();
}
#endif

void QtMainWindow::runScript(QString s)
{
	QFile file(s);
	
	if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
		printf("ERROR: Could not open the file: %s\n", watchfile.data());

	QTextStream stream( &file );

	QStringList commandList;
	QString lines;
	
	while( !stream.atEnd() )
	{
		lines += stream.readLine()+"\n";
	}
	
	file.close();

	volscript.parse_script(lines);
	volscript.execute_script_now();
}
void QtMainWindow::watchfileChanged(QString s)
{	
	QFile file(watchfile);
	
	if( !file.open( QIODevice::ReadOnly | QIODevice::Text ) )
		printf("ERROR: Could not open the file: %s\n", watchfile.data());

	QTextStream stream( &file );

	QStringList commandList;
	QString lines;

	
	while( !stream.atEnd() )
	{
		lines += stream.readLine()+"\n";
//		printf("line: %s\n", line.latin1());
//		commandList.append( line );
	}
	
	file.close();

//	commandline_parse(commandList);

	volscript.parse_script(lines);
	volscript.execute_script_now();
}

void QtMainWindow::openTimelapse_event(void)
{
	/*
	QFileDialog* fd = new QFileDialog( this );
	fd->setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);

	QString dn = fd->getExistingDirectory(this, "Please select a Timelapse Directory", last_working_dir);
	last_working_dir = dn;
	
	QDir tldir = QDir(dn);
	tldir.setFilter(QDir::Dirs | QDir::NoSymLinks | QDir::NoDotAndDotDot);
	timelapsedirs = tldir.entryList();

	if(!timelapsedirs.isEmpty())
	{
		glwindow->glvoxel->volrender.timelapsefolder = 0;

		timelapsehelper=0;
		QString temp = last_working_dir;
		openImageDir(last_working_dir+"/"+timelapsedirs.at(0)+"/");
		last_working_dir = temp;

		glwindow->glvoxel->volrender.timelapsefolder = 1;
	}*/

	datasetview_editro_tabdlg->datasetviewtab->open();
	datasetview_editro_tabdlg->show();
}
void QtMainWindow::openImage_event(void)
{		
	Q3FileDialog* fd = new Q3FileDialog( this );
	QString fn = fd->getOpenFileName(last_working_dir, "*.png *.bmp *.jpg *.jpeg *.tif *.tiff", this );

	openImageDir(fn);
}
void QtMainWindow::uploadTextureSlot(void)
{
	glwindow->glvoxel->volrender.load_TextureMap();
}
void QtMainWindow::mw_initImageDir_slot(QString dn)
{
	QDir dir = QDir(dn);
	dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
	QStringList list = dir.entryList();

	//printf("found: %d file entries\n", (int)list.size());

	if(list.empty())
	{
		printf("ERROR: Found zero files in %s\n", dn.latin1());
		return;
	}
	QString fn = dn+list.at(0);

	//printf("filename: %s\n", fn.latin1());

	//if we actually got something back,
	//then load up the data in that dir
	if ( !dn.isEmpty() )
	{
		glwindow->glvoxel->volobject.file_name = dn;
		glwindow->glvoxel->volobject.file_type = 0;
		last_working_dir = dn;

		emit measure_setoriginalfn_signal(dn);		
		emit currentworkingdir_signal(dn);
		
		if(glwindow->glvoxel->volrender.volume_render_mode!=4)	//NOT BRICKS
		{
			//printf("NOT BRICKS\n");
			channel_mode=4;

			//QTime t2;
			//t2.start();
			glwindow->glvoxel->volio.load_PNG(fn, channel_mode);
			//printf("sub-time: %d msec.\n", t2.elapsed());
		}
		
		update_info_labels();
	}
	else //if we dont have a proper filename, stop
	{
		glwindow->glvoxel->volobject.file_name = "";
		glwindow->glvoxel->volobject.file_type = -1;
		printf("ERROR: filename/directory name is empty!\n");

	    //statusBar->message("Error Loading, No Filename!", 4000);	
		return;
	}

	channel_mode=0;
	Vector res = Vector((glwindow->glvoxel->volobject.xsize/glwindow->glvoxel->volobject.maxsize), (glwindow->glvoxel->volobject.ysize/glwindow->glvoxel->volobject.maxsize), (glwindow->glvoxel->volobject.zsize/glwindow->glvoxel->volobject.maxsize));
	emit measure_setrescale_signal(glwindow->glvoxel->volobject.x_resize, glwindow->glvoxel->volobject.y_resize, glwindow->glvoxel->volobject.z_resize);
	emit measure_setscale2_signal(glwindow->glvoxel->volobject.xscale, glwindow->glvoxel->volobject.yscale, glwindow->glvoxel->volobject.zscale);
	emit measure_setspacing_signal(res.x, res.y, res.z);

	glwindow->glvoxel->volrender.volume_toggle=1;
	glwindow->glvoxel->volrender.r_channel = glwindow->glvoxel->volrender.g_channel = glwindow->glvoxel->volrender.b_channel = 0;
	//if(glwindow->glvoxel->volobject.has_red==1) 
		
	glwindow->glvoxel->volrender.r_channel = glwindow->glvoxel->volobject.has_red;
	glwindow->glvoxel->volrender.g_channel = glwindow->glvoxel->volobject.has_green;
	glwindow->glvoxel->volrender.b_channel = glwindow->glvoxel->volobject.has_blue;
	
	//if(glwindow->glvoxel->volobject.has_green==1) glwindow->glvoxel->volrender.g_channel = 1;
	//if(glwindow->glvoxel->volobject.has_blue==1) glwindow->glvoxel->volrender.b_channel = 1;
	//if(glwindow->glvoxel->volrender.g_channel==1 || glwindow->glvoxel->volrender.b_channel==1) glwindow->glvoxel->volobject.is_greyscale==0;

	if(glwindow->glvoxel->volobject.is_greyscale==1)
	{
		transferfunction_editor_tabdlg->tranfer_function_tab->red_pb->setEnabled(false);
		transferfunction_editor_tabdlg->tranfer_function_tab->green_pb->setEnabled(false);
		transferfunction_editor_tabdlg->tranfer_function_tab->blue_pb->setEnabled(false);

		intentb->setOn(true);
		redtb->setOn(false);
		greentb->setOn(false);
		bluetb->setOn(false);
	}
	else
	{
		transferfunction_editor_tabdlg->tranfer_function_tab->red_pb->setEnabled(glwindow->glvoxel->volobject.has_red);
		transferfunction_editor_tabdlg->tranfer_function_tab->green_pb->setEnabled(glwindow->glvoxel->volobject.has_green);
		transferfunction_editor_tabdlg->tranfer_function_tab->blue_pb->setEnabled(glwindow->glvoxel->volobject.has_blue);

		intentb->setOn(glwindow->glvoxel->volobject.is_greyscale);
		redtb->setOn(glwindow->glvoxel->volobject.has_red);
		greentb->setOn(glwindow->glvoxel->volobject.has_green);
		bluetb->setOn(glwindow->glvoxel->volobject.has_blue);	
	}

	//reset our transfer dialogue
	//volume_editor_tabdlg->reset();

	//glwindow->glvoxel->mcubes.clear();
	//volume_editor_tabdlg->isosurface_tab->del_all();	

	//update our histogram
	volume_editor_tabdlg->rebuild_tabs();
	view_editor_tabdlg->rebuild_tabs();

	//update how many slices we discretize the volume with
	view_editor_tabdlg->rendersettings_tab->rebuild();

	//load our data to opengl
	//glwindow->glvoxel->volrender.compute_boundingbox();
	glwindow->glvoxel->volrender.load_TextureMap();

	//setup our slice arrays and slice sliders
	glwindow->create();	
	updateStatusBar();

	glwindow->glvoxel->redraw_event();
}
void QtMainWindow::mw_openImageDir_slot(QString dn)
{
	//printf("SLOT LOADING: Image Dir: %s\n", dn.latin1());

	QDir dir = QDir(dn);
	dir.setFilter(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
	QStringList list = dir.entryList();

	//printf("found: %d file entries\n", (int)list.size());

	if(list.empty())
	{
		printf("ERROR: Found zero files in %s\n", dn.latin1());
		return;
	}
	QString fn = dn+list.at(0);

	//printf("filename: %s\n", fn.latin1());

	//if we actually got something back,
	//then load up the data in that dir
	if ( !dn.isEmpty() )
	{
		glwindow->glvoxel->volobject.file_name = dn;
		glwindow->glvoxel->volobject.file_type = 0;
		last_working_dir = dn;

		emit measure_setoriginalfn_signal(dn);		
		emit currentworkingdir_signal(dn);
		
		if(glwindow->glvoxel->volrender.volume_render_mode!=4)	//NOT BRICKS
		{
			//printf("NOT BRICKS\n");
			channel_mode=4;

			//QTime t2;
			//t2.start();
			glwindow->glvoxel->volio.load_PNG(fn, channel_mode);
			//printf("sub-time: %d msec.\n", t2.elapsed());
 
		}
		
		update_info_labels();
	}
	else //if we dont have a proper filename, stop
	{
		glwindow->glvoxel->volobject.file_name = "";
		glwindow->glvoxel->volobject.file_type = -1;
		printf("ERROR: filename/directory name is empty!\n");

	    //statusBar->message("Error Loading, No Filename!", 4000);	
		return;
	}

	channel_mode=0;
	Vector res = Vector((glwindow->glvoxel->volobject.xsize/glwindow->glvoxel->volobject.maxsize), (glwindow->glvoxel->volobject.ysize/glwindow->glvoxel->volobject.maxsize), (glwindow->glvoxel->volobject.zsize/glwindow->glvoxel->volobject.maxsize));
	emit measure_setrescale_signal(glwindow->glvoxel->volobject.x_resize, glwindow->glvoxel->volobject.y_resize, glwindow->glvoxel->volobject.z_resize);
	emit measure_setscale2_signal(glwindow->glvoxel->volobject.xscale, glwindow->glvoxel->volobject.yscale, glwindow->glvoxel->volobject.zscale);
	emit measure_setspacing_signal(res.x, res.y, res.z);

	glwindow->glvoxel->volrender.volume_toggle=1;
	glwindow->glvoxel->volrender.r_channel = glwindow->glvoxel->volrender.g_channel = glwindow->glvoxel->volrender.b_channel = 0;
	//if(glwindow->glvoxel->volobject.has_red==1) 
		
	glwindow->glvoxel->volrender.r_channel = glwindow->glvoxel->volobject.has_red;
	glwindow->glvoxel->volrender.g_channel = glwindow->glvoxel->volobject.has_green;
	glwindow->glvoxel->volrender.b_channel = glwindow->glvoxel->volobject.has_blue;
	
	//if(glwindow->glvoxel->volobject.has_green==1) glwindow->glvoxel->volrender.g_channel = 1;
	//if(glwindow->glvoxel->volobject.has_blue==1) glwindow->glvoxel->volrender.b_channel = 1;
	//if(glwindow->glvoxel->volrender.g_channel==1 || glwindow->glvoxel->volrender.b_channel==1) glwindow->glvoxel->volobject.is_greyscale==0;

	if(glwindow->glvoxel->volobject.is_greyscale==1)
	{
		transferfunction_editor_tabdlg->tranfer_function_tab->red_pb->setEnabled(false);
		transferfunction_editor_tabdlg->tranfer_function_tab->green_pb->setEnabled(false);
		transferfunction_editor_tabdlg->tranfer_function_tab->blue_pb->setEnabled(false);

		intentb->setOn(true);
		redtb->setOn(false);
		greentb->setOn(false);
		bluetb->setOn(false);
	}
	else
	{
		transferfunction_editor_tabdlg->tranfer_function_tab->red_pb->setEnabled(glwindow->glvoxel->volobject.has_red);
		transferfunction_editor_tabdlg->tranfer_function_tab->green_pb->setEnabled(glwindow->glvoxel->volobject.has_green);
		transferfunction_editor_tabdlg->tranfer_function_tab->blue_pb->setEnabled(glwindow->glvoxel->volobject.has_blue);

		intentb->setOn(glwindow->glvoxel->volobject.is_greyscale);
		redtb->setOn(glwindow->glvoxel->volobject.has_red);
		greentb->setOn(glwindow->glvoxel->volobject.has_green);
		bluetb->setOn(glwindow->glvoxel->volobject.has_blue);	
	}

	//reset our transfer dialogue
	//volume_editor_tabdlg->reset();

	//glwindow->glvoxel->mcubes.clear();
	//volume_editor_tabdlg->isosurface_tab->del_all();	

	//update our histogram
	volume_editor_tabdlg->rebuild_tabs();
	view_editor_tabdlg->rebuild_tabs();

	//update how many slices we discretize the volume with
	view_editor_tabdlg->rendersettings_tab->rebuild();

	//load our data to opengl
	glwindow->glvoxel->volrender.compute_boundingbox();
	glwindow->glvoxel->volrender.load_TextureMap();

	//setup our slice arrays and slice sliders
	glwindow->create();
	updateStatusBar();

	glwindow->glvoxel->redraw_event();
}

void QtMainWindow::openImageDir(QString fn)
{
	QString dn = fn;
	last_working_dir = fn;
	printf("LOADING: Image Dir: %s\n", dn.latin1());

	//remove the filename of first file to get only dirname
	int pos = dn.findRev(QString("/").latin1(), -1, false);
	dn.replace(pos+1, dn.length()-pos+1, "");

	//QTime t;
	//t.start();

	printf("LOADING: Image Dir: %s\n", dn.latin1());

	//if we actually got something back,
	//then load up the data in that dir
	if ( !dn.isEmpty() )
	{
		glwindow->glvoxel->volobject.file_name = dn;
		glwindow->glvoxel->volobject.file_type = 0;
		last_working_dir = dn;

		emit measure_setoriginalfn_signal(dn);		
		emit currentworkingdir_signal(dn);
		
		if(glwindow->glvoxel->volrender.volume_render_mode!=4)	//NOT BRICKS
		{
			//printf("NOT BRICKS\n");

			//choose which channel mode
			openImagechannelmsgbox_event();

			for(int i=0; i<glwindow->glvoxel->volrender.brick_textures.size(); i++)
				glwindow->glvoxel->volrender.brick_textures[i].clear();

			glwindow->glvoxel->volrender.brick_textures.clear();
			glwindow->glvoxel->volrender.brick_centres.clear();
			glwindow->glvoxel->volrender.empty_brick.clear();

			//QTime t2;
			//t2.start();
			glwindow->glvoxel->volio.load_PNG(fn, channel_mode);
			//printf("sub-time: %d msec.\n", t2.elapsed());
 
		}
		else		//BRICKS
		{
			printf("BRICKS\n");
			
			if(glwindow->glvoxel->volobject.texture3d!=0) delete glwindow->glvoxel->volobject.texture3d;
			
			//choose which channel mode
			openImagechannelmsgbox_event();

			for(int i=0; i<glwindow->glvoxel->volrender.brick_textures.size(); i++)
				glwindow->glvoxel->volrender.brick_textures[i].clear();
				
			glwindow->glvoxel->volrender.brick_textures.clear();
			glwindow->glvoxel->volrender.brick_centres.clear();
			glwindow->glvoxel->volrender.empty_brick.clear();

			glwindow->glvoxel->volio.init_subimageloading(dn, channel_mode);
			glwindow->glvoxel->volobject.texture3d = new unsigned char[1*1*1];
		}

		//have we selected something sensible?
		if(channel_mode==0)
		{
 			//statusBar->message("Error Loading, Error with Data!", 4000);	
			printf("ERROR: no channel_mode has been selected!\n");
			return;
		}
		
		update_info_labels();
	}
	else //if we dont have a proper filename, stop
	{
		glwindow->glvoxel->volobject.file_name = "";
		glwindow->glvoxel->volobject.file_type = -1;
		printf("ERROR: filename/directory name is empty!\n");

	    //statusBar->message("Error Loading, No Filename!", 4000);	
		return;
	}

	channel_mode=0;
	Vector res = Vector((glwindow->glvoxel->volobject.xsize/glwindow->glvoxel->volobject.maxsize), (glwindow->glvoxel->volobject.ysize/glwindow->glvoxel->volobject.maxsize), (glwindow->glvoxel->volobject.zsize/glwindow->glvoxel->volobject.maxsize));
	emit measure_setrescale_signal(glwindow->glvoxel->volobject.x_resize, glwindow->glvoxel->volobject.y_resize, glwindow->glvoxel->volobject.z_resize);
	emit measure_setscale2_signal(glwindow->glvoxel->volobject.xscale, glwindow->glvoxel->volobject.yscale, glwindow->glvoxel->volobject.zscale);
	emit measure_setspacing_signal(res.x, res.y, res.z);

	glwindow->glvoxel->volrender.volume_toggle=1;
	glwindow->glvoxel->volrender.r_channel = glwindow->glvoxel->volrender.g_channel = glwindow->glvoxel->volrender.b_channel = 0;
	//if(glwindow->glvoxel->volobject.has_red==1) 
		
	glwindow->glvoxel->volrender.r_channel = glwindow->glvoxel->volobject.has_red;
	glwindow->glvoxel->volrender.g_channel = glwindow->glvoxel->volobject.has_green;
	glwindow->glvoxel->volrender.b_channel = glwindow->glvoxel->volobject.has_blue;
	
	//if(glwindow->glvoxel->volobject.has_green==1) glwindow->glvoxel->volrender.g_channel = 1;
	//if(glwindow->glvoxel->volobject.has_blue==1) glwindow->glvoxel->volrender.b_channel = 1;
	//if(glwindow->glvoxel->volrender.g_channel==1 || glwindow->glvoxel->volrender.b_channel==1) glwindow->glvoxel->volobject.is_greyscale==0;

	update_channels_slot();

	//reset our transfer dialogue
	volume_editor_tabdlg->reset();

	//glwindow->glvoxel->mcubes.clear();
	//volume_editor_tabdlg->isosurface_tab->del_all();	

	//update our histogram
	volume_editor_tabdlg->rebuild_tabs();
	view_editor_tabdlg->rebuild_tabs();

	//update how many slices we discretize the volume with
	view_editor_tabdlg->rendersettings_tab->rebuild();

	if(glwindow->glvoxel->volrender.volume_render_mode!=4)
	{
		//load our data to opengl
		glwindow->glvoxel->volrender.compute_boundingbox();
	}

	glwindow->glvoxel->volrender.load_TextureMap();

#ifdef LIBTIFF_SUPPORT
	if(glwindow->glvoxel->volobject.is16bit)
	{
		int oldvallum = transferfunc16bit_dialogue->tranfer_func_tab->histogrampixmap_luminance->remaponfly;
		int oldvalopac = transferfunc16bit_dialogue->tranfer_func_tab->histogrampixmap_opacity->remaponfly;

		transferfunc16bit_dialogue->tranfer_func_tab->histogrampixmap_luminance->remaponfly = 0;
		transferfunc16bit_dialogue->tranfer_func_tab->histogrampixmap_opacity->remaponfly = 0;

		transferfunction_editor_tabdlg->hide();
		transferfunction_editor_tabdlg->tranfer_function_tab->resetall_button_action();
		transferfunc16bit_dialogue->tranfer_func_tab->reset();
		emit tfunc_16bit_histogram_signal(&glwindow->glvoxel->volobject.histogram16bit[0], &glwindow->glvoxel->volobject.histogram[0]);
	
		int minthresh=0;
		for(int i=0; i<65536; i++)
		{
			if(glwindow->glvoxel->volobject.histogram16bit[i]!=0.0)
			{
				minthresh = i;
				break;
			}
		}
		int maxthresh=0;
		for(int i=65535; i>=0; i--)
		{
			if(glwindow->glvoxel->volobject.histogram16bit[i]!=0.0)
			{
				maxthresh = i;
				break;
			}
		}

		//printf("min / max: %d, %d\n", minthresh, maxthresh);
		transferfunc16bit_dialogue->tranfer_func_tab->threshmin_slider->setValue(minthresh);
		transferfunc16bit_dialogue->tranfer_func_tab->threshmax_slider->setValue(maxthresh);
		transferfunc16bit_dialogue->tranfer_func_tab->threshmin_action(minthresh);
		transferfunc16bit_dialogue->tranfer_func_tab->threshmax_action(maxthresh);
		transferfunc16bit_dialogue->tranfer_func_tab->histogrampixmap_luminance->remaponfly = 1;
		transferfunc16bit_dialogue->tranfer_func_tab->histogrampixmap_opacity->remaponfly = 1;

		transferfunc16bit_dialogue->tranfer_func_tab->tfunc_remapslot();

		transferfunc16bit_dialogue->tranfer_func_tab->histogrampixmap_luminance->remaponfly = oldvallum;
		transferfunc16bit_dialogue->tranfer_func_tab->histogrampixmap_opacity->remaponfly = oldvalopac;
	}
	else
	{
		transferfunc16bit_dialogue->hide();
	}
#endif

//	if(glwindow->glvoxel->volrender.volume_render_mode!=4)
	if(timelapsehelper==0)
	{
		glwindow->glvoxel->volrender.timelapsefoldersize = timelapsedirs.size();

		if(view_orientation==1) front_event();
		else if(view_orientation==2) back_event();
		else if(view_orientation==3) left_event();
		else if(view_orientation==4) right_event();
		else if(view_orientation==5) top_event();
		else if(view_orientation==6) bottom_event();
	}

	//setup our slice arrays and slice sliders
	glwindow->create();
	updateStatusBar();

	glwindow->glvoxel->redraw_event();

	//printf("LOADING: Total Time: %d msec.\n", t.elapsed());
}
void QtMainWindow::openwlz_event(void)
{
	Q3FileDialog* fd = new Q3FileDialog( this );
	//statusBar->message("Loading WLZ Volume...");
	QString fn = fd->getOpenFileName(last_working_dir, "*.wlz", this );

	//remove the filename of first file to get only dirname
	//and save a copy of last working dir
	QString fn2 = fn; 
	int pos = fn2.findRev(QString("/").latin1(), -1, false);
	last_working_dir = fn2.replace(pos+1, fn2.length()-pos+1, "");

	//if we actually got something back,
	//then load up the data in that dir
	if ( !fn.isEmpty() )
	{
		glwindow->glvoxel->volobject.file_name = fn;
		glwindow->glvoxel->volobject.file_type = 2;

		//choose which channel mode
		openImagechannelmsgbox_event();

		//if we have a good channel mode, try loading
//		if(channel_mode!=0) glwindow->glvoxel->volio.load_WLZ(fn, channel_mode);

		//have we selected something sensible?
		if(channel_mode==0)
		{
 			//statusBar->message("Error Loading, Error with Data!", 4000);	
			return;
		}
		
		//all good, let the user know about it.
	    //statusBar->message("Done", 1000);
	    //statusBar->message("Ready", 2000);
		update_info_labels();
	}
	else //got no filename, then stop
	{
		glwindow->glvoxel->volobject.file_name = "";
		glwindow->glvoxel->volobject.file_type = -1;
	    //statusBar->message("Error Loading, No Filename!", 2000);	
		return;
	}

	channel_mode=0;

	glwindow->glvoxel->volrender.r_channel = glwindow->glvoxel->volrender.g_channel = glwindow->glvoxel->volrender.b_channel = false;
	view_editor_tabdlg->rendersettings_tab->r_toggle = view_editor_tabdlg->rendersettings_tab->g_toggle = view_editor_tabdlg->rendersettings_tab->b_toggle = false;
	
	glwindow->glvoxel->volrender.volume_toggle=1;
	glwindow->glvoxel->volrender.r_channel = glwindow->glvoxel->volrender.g_channel = glwindow->glvoxel->volrender.b_channel = false;
	if(glwindow->glvoxel->volio.red==true) glwindow->glvoxel->volrender.r_channel = true;
	if(glwindow->glvoxel->volio.green==true) glwindow->glvoxel->volrender.g_channel = true;
	if(glwindow->glvoxel->volio.blue==true) glwindow->glvoxel->volrender.b_channel = true;

	update_channels_slot();

	//reset our transfer dialogue
	volume_editor_tabdlg->reset();

	glwindow->glvoxel->mcubes.clear();
	volume_editor_tabdlg->isosurface_tab->del_all();	

	//update our ui
	volume_editor_tabdlg->rebuild_tabs();
	view_editor_tabdlg->rebuild_tabs();

	//load our data to opengl
	glwindow->glvoxel->volrender.compute_boundingbox();
	glwindow->glvoxel->volrender.load_TextureMap();

	//setup our slice arrays and slice sliders
	glwindow->create();
}

void QtMainWindow::openRawFile(QString fn)
{
	emit measure_setoriginalfn_signal(fn);
	
	//remove the filename of first file to get only dirname
	//and save a copy of last working dir
	QString fn2 = fn; 
	int pos = fn2.findRev(QString("/").latin1(), -1, false);
	last_working_dir = fn2.replace(pos+1, fn2.length()-pos+1, "");
	
	emit currentworkingdir_signal(last_working_dir);

	fn.truncate(fn.length()-3);                            
	fn += "dat";

	//if we actually got something back,
	//then load up the data in that dir
	if ( !fn.isEmpty() )
	{
		glwindow->glvoxel->volobject.file_name = fn;
		glwindow->glvoxel->volobject.file_type = 1;

		//choose which channel mode
		openImagechannelmsgbox_event();

		//if we have a good channel mode, try loading
		if(channel_mode!=0)
		{
			glwindow->glvoxel->volio.load_RAW(fn, channel_mode);
		}

		//have we selected something sensible?
		if(channel_mode==0)
		{
 			//statusBar->message("Error Loading, Error with Data!", 4000);	
			return;
		}

	    //statusBar->message("Done", 1000);
	    //statusBar->message("Ready", 2000);
		update_info_labels();
	}
	else //got no filename, then stop
	{
		glwindow->glvoxel->volobject.file_name = "";
		glwindow->glvoxel->volobject.file_type = -1;
	    //statusBar->message("Error Loading, No Filename!", 2000);	
		return;
	}

	channel_mode=0;

	emit measure_setrescale_signal(glwindow->glvoxel->volobject.x_resize, glwindow->glvoxel->volobject.y_resize, glwindow->glvoxel->volobject.z_resize);
	emit measure_setscale2_signal(glwindow->glvoxel->volobject.xscale, glwindow->glvoxel->volobject.yscale, glwindow->glvoxel->volobject.zscale);

	glwindow->glvoxel->volrender.volume_toggle=1;
	glwindow->glvoxel->volrender.r_channel = glwindow->glvoxel->volrender.g_channel = glwindow->glvoxel->volrender.b_channel = false;
	if(glwindow->glvoxel->volio.red==true) glwindow->glvoxel->volrender.r_channel = true;
	if(glwindow->glvoxel->volio.green==true) glwindow->glvoxel->volrender.g_channel = true;
	if(glwindow->glvoxel->volio.blue==true) glwindow->glvoxel->volrender.b_channel = true;
	
	update_channels_slot();

	float maxsize =  max(glwindow->glvoxel->volobject.xsize, glwindow->glvoxel->volobject.ysize);
	maxsize =  max(maxsize, glwindow->glvoxel->volobject.zsize);
	Vector res = Vector((glwindow->glvoxel->volobject.xsize/maxsize), (glwindow->glvoxel->volobject.ysize/maxsize), (glwindow->glvoxel->volobject.zsize/maxsize));
	emit measure_setspacing_signal(res.x, res.y, res.z);

	//update our histogram
	glwindow->glvoxel->mcubes.clear();
	
	glwindow->glvoxel->mcubes.clear();
	volume_editor_tabdlg->isosurface_tab->del_all();	

	//load our data to opengl
	glwindow->glvoxel->volrender.compute_boundingbox();
	glwindow->glvoxel->volrender.load_TextureMap();

	if(view_orientation==1) front_event();
	else if(view_orientation==2) back_event();
	else if(view_orientation==3) left_event();
	else if(view_orientation==4) right_event();
	else if(view_orientation==5) top_event();
	else if(view_orientation==6) bottom_event();
	
	//save a copy of original data
//	glwindow->glvoxel->undo_volobject = glwindow->glvoxel->sourceobject = glwindow->glvoxel->volobject;

	view_editor_tabdlg->rebuild_tabs();

#ifdef LIBTIFF_SUPPORT
	glwindow->glvoxel->volobject.is16bit = false;
	transferfunc16bit_dialogue->hide();
#endif

	//setup our slice arrays and slice sliders
	glwindow->create();
	//statusBar->showMessage(glwindow->glvoxel->volobject.file_name, 0);
	updateStatusBar();
}
void QtMainWindow::openraw_event(void)
{
	Q3FileDialog* fd = new Q3FileDialog( this );
	//statusBar->message("Loading RAW Volume...");
	QString fn = fd->getOpenFileName(last_working_dir, "*.dat", this );
	
	openRawFile(fn);
}
void QtMainWindow::togglestereo_event(void)
{
	glwindow->glvoxel->volrender.stereo_toggle = !glwindow->glvoxel->volrender.stereo_toggle;

	if(glwindow->glvoxel->volrender.stereo_toggle==1)
	{
		if(glwindow->glvoxel->volrender.stereo_mode!=0) glwindow->glvoxel->volrender.background_colour=glwindow->glvoxel->volrender.old_background_colour;
	}
	else
	{
		if(glwindow->glvoxel->volrender.stereo_mode!=0) glwindow->glvoxel->volrender.background_colour=Qt::black;
	}
//  if(glwindow->glvoxel->volrender.stereo_toggle) render->setItemChecked( stereo_id, TRUE );
//	else render->setItemChecked( stereo_id, FALSE);

	glwindow->glvoxel->update();
	glwindow->glvoxel->redraw_event();
	update();
}

void QtMainWindow::togglecursor_event()
{
	glwindow->glvoxel->volrender.cursor_toggle = !glwindow->glvoxel->volrender.cursor_toggle;

	glwindow->glvoxel->update();
	glwindow->glvoxel->redraw_event();
	update();
}
void QtMainWindow::togglefps_event()
{
	glwindow->glvoxel->volrender.fps_toggle = !glwindow->glvoxel->volrender.fps_toggle;

//    if(glwindow->glvoxel->volrender.fps_toggle) render->setItemChecked( fps_id, TRUE );
//	else render->setItemChecked( fps_id, FALSE);

	glwindow->glvoxel->update();
	glwindow->glvoxel->redraw_event();
	update();
}
void QtMainWindow::togglelights_event(void)
{
	glwindow->glvoxel->volrender.lights_toggle = !glwindow->glvoxel->volrender.lights_toggle;

 /*   if(glwindow->glvoxel->volrender.lights_toggle) render->setItemChecked( lights_id, TRUE );
	else render->setItemChecked( lights_id, FALSE);
*/
	glwindow->glvoxel->update();
	glwindow->glvoxel->redraw_event();
	update();
}

void QtMainWindow::toggleaxis_event(void)
{
	glwindow->glvoxel->volrender.axis_toggle = !glwindow->glvoxel->volrender.axis_toggle;

 /*   if(glwindow->glvoxel->volrender.axis_toggle) render->setItemChecked( axis_id, TRUE );
	else render->setItemChecked( axis_id, FALSE);
*/
	glwindow->glvoxel->update();
	glwindow->glvoxel->redraw_event();
	update();
}

void QtMainWindow::toggleBB_event(void)
{
	glwindow->glvoxel->volrender.BB_toggle = !glwindow->glvoxel->volrender.BB_toggle;

    if(glwindow->glvoxel->volrender.BB_toggle)
	{
		QColorDialog* coldialogue=0;
		glwindow->glvoxel->volrender.bb_colour = coldialogue->getColor(glwindow->glvoxel->volrender.bb_colour, this, "Choose a colour");
	//	render->setItemChecked( bb_id, TRUE );
	}
	//else render->setItemChecked( bb_id, FALSE);

	glwindow->glvoxel->update();
	glwindow->glvoxel->redraw_event();
	update();
}
void QtMainWindow::togglesurfaces_event(void)
{
	glwindow->glvoxel->volrender.isosurface_toggle = !glwindow->glvoxel->volrender.isosurface_toggle;

//    if(glwindow->glvoxel->volrender.isosurface_toggle) render->setItemChecked( surf_id, TRUE );
//	else render->setItemChecked( surf_id, FALSE);

	glwindow->glvoxel->update();
	glwindow->glvoxel->redraw_event();
	update();
}
void QtMainWindow::togglevoxels_event(void)
{
	glwindow->glvoxel->volrender.volume_toggle = !glwindow->glvoxel->volrender.volume_toggle;

	glwindow->glvoxel->update();
	glwindow->glvoxel->redraw_event();
	update();
}

void QtMainWindow::slicemode0_event(void)
{
 /*   slice->setItemChecked(slicemode0_id, TRUE );
    slice->setItemChecked(slicemode1_id, FALSE );
    slice->setItemChecked(slicemode2_id, FALSE );
    slice->setItemChecked(slicemode3_id, FALSE );
*/
	glwindow->glvoxel->volrender.slice_draw_mode = 0;

	glwindow->xylb->slices = 0;
	glwindow->xzlb->slices = 0;
	glwindow->yzlb->slices = 0;

	glwindow->frames_redraw_slot();
	glwindow->glvoxel->update();
	glwindow->glvoxel->redraw_event();
	update();
}
void QtMainWindow::slicemode1_event(void)
{
 /*   slice->setItemChecked(slicemode0_id, FALSE );
    slice->setItemChecked(slicemode1_id, TRUE );
    slice->setItemChecked(slicemode2_id, FALSE );
    slice->setItemChecked(slicemode3_id, FALSE );
*/
	glwindow->glvoxel->volrender.slice_draw_mode = 1;

	glwindow->xylb->slices = 1;
	glwindow->xzlb->slices = 1;
	glwindow->yzlb->slices = 1;

	glwindow->frames_redraw_slot();
	glwindow->glvoxel->update();
	glwindow->glvoxel->redraw_event();
	update();
}
void QtMainWindow::slicemode2_event(void)
{
  /*  slice->setItemChecked(slicemode0_id, FALSE );
    slice->setItemChecked(slicemode1_id, FALSE );
    slice->setItemChecked(slicemode2_id, TRUE );
    slice->setItemChecked(slicemode3_id, FALSE );
*/
	glwindow->glvoxel->volrender.slice_draw_mode = 2;

	glwindow->xylb->slices = 1;
	glwindow->xzlb->slices = 1;
	glwindow->yzlb->slices = 1;

	glwindow->frames_redraw_slot();
	glwindow->glvoxel->update();
	glwindow->glvoxel->redraw_event();
	update();
}
void QtMainWindow::slicemode3_event(void)
{
/*    slice->setItemChecked(slicemode0_id, FALSE );
    slice->setItemChecked(slicemode1_id, FALSE );
    slice->setItemChecked(slicemode2_id, FALSE );
    slice->setItemChecked(slicemode3_id, TRUE );
*/	
	glwindow->glvoxel->volrender.slice_draw_mode = 3;

	glwindow->xylb->slices = 1;
	glwindow->xzlb->slices = 1;
	glwindow->yzlb->slices = 1;

	glwindow->frames_redraw_slot();
	glwindow->glvoxel->update();
	glwindow->glvoxel->redraw_event();
	update();
}
void QtMainWindow::savetiff_event(void)
{
	QString fn = QFileDialog::getExistingDirectory ( this, "Please pick a save directory.", last_working_dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	//remove the filename of first file to get only dirname
	//and save a copy of last working dir
	QString fn2 = fn; 
	//int pos = fn2.findRev(QString("/").latin1(), -1, false);
	last_working_dir = fn2 + "/"; //fn2.replace(pos+1, fn2.length()-pos+1, "");
  	
	printf("last_working_dir: %s\n",last_working_dir.latin1());
	//statusBar->message("Saving PNG Volume Slices...", 1000);
  
	if ( !fn.isEmpty() ) 
	{			
		//glwindow->glvoxel->volio.save_TIFFstack(fn);
	    //statusBar->message("Done", 1000);
	    //statusBar->message("Ready", 2000);
	}
	else
	{
	    //statusBar->message("Error Saving", 2000);		
	}

	if(glwindow->glvoxel->volobject.texwidth&&glwindow->glvoxel->volobject.texheight&&glwindow->glvoxel->volobject.texdepth==1) return;
	if(glwindow->glvoxel->volobject.texture3d==NULL) return;

	//save a copy of original data
	//glwindow->glvoxel->sourceobject = glwindow->glvoxel->volobject;

	glwindow->glvoxel->update();
	update();
}
void QtMainWindow::savepng_event(void)
{
//	QString fn = Q3FileDialog::getSaveFileName(last_working_dir, "*.png", this );
 
/*	QString fn = QFileDialog::getSaveFileName(this, tr("Open File"),
                                                "/home/jana",
                                                tr("Images (*.png *.xpm *.jpg)"));
*/
	QString fn = QFileDialog::getExistingDirectory ( this, "Please pick a save directory.", last_working_dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	//remove the filename of first file to get only dirname
	//and save a copy of last working dir
	QString fn2 = fn; 
	//int pos = fn2.findRev(QString("/").latin1(), -1, false);
	last_working_dir = fn2 + "/"; //fn2.replace(pos+1, fn2.length()-pos+1, "");
  	
	printf("last_working_dir: %s\n",last_working_dir.latin1());
	//statusBar->message("Saving PNG Volume Slices...", 1000);
  
	if ( !fn.isEmpty() ) 
	{			
		glwindow->glvoxel->volio.save_PNG(fn);
	    //statusBar->message("Done", 1000);
	    //statusBar->message("Ready", 2000);
	}
	else
	{
	    //statusBar->message("Error Saving", 2000);		
	}

	if(glwindow->glvoxel->volobject.texwidth&&glwindow->glvoxel->volobject.texheight&&glwindow->glvoxel->volobject.texdepth==1) return;
	if(glwindow->glvoxel->volobject.texture3d==NULL) return;

	//save a copy of original data
	//glwindow->glvoxel->sourceobject = glwindow->glvoxel->volobject;

	glwindow->glvoxel->update();
	update();
}

void QtMainWindow::saveraw_event(void)
{
	QString fn = Q3FileDialog::getSaveFileName(last_working_dir, "*.dat", this );
  	
	//remove the filename of first file to get only dirname
	//and save a copy of last working dir
	QString fn2 = fn; 
	int pos = fn2.findRev(QString("/").latin1(), -1, false);
	last_working_dir = fn2.replace(pos+1, fn2.length()-pos+1, "");
		
	//statusBar->message("Saving RAW Volume...", 1000);
  
	if ( !fn.isEmpty() ) 
	{			
		glwindow->glvoxel->volio.save_RAW(fn);	     
	    //statusBar->message("Done", 1000);
	    //statusBar->message("Ready", 2000);
	}
	else
	{
	    //statusBar->message("Error Saving", 2000);		
	}

	if(glwindow->glvoxel->volobject.texwidth&&glwindow->glvoxel->volobject.texheight&&glwindow->glvoxel->volobject.texdepth==1) return;
	if(glwindow->glvoxel->volobject.texture3d==NULL) return;

	//save a copy of original data
	//glwindow->glvoxel->sourceobject = glwindow->glvoxel->volobject;

	glwindow->glvoxel->update();
	update();
}
void QtMainWindow::saveobj_event(void)
{	
	QDir qdir("");
	QString dirstring = qdir.filePath("Data");
	QString fn = Q3FileDialog::getSaveFileName(dirstring, "*.obj", this );
  	
	//statusBar->message("Saving OBJ Surface...", 1000);
  
	if ( !fn.isEmpty() ) 
	{	
		//glwindow->glvoxel->volio.save_OBJ(fn);	     
	    //statusBar->message("Done", 1000);
	    //statusBar->message("Ready", 2000);
	}
	else
	{
	    //statusBar->message("Error Saving", 2000);		
	}

	glwindow->glvoxel->update();
	update();
}
void QtMainWindow::save_renderScreenShot_event(void)
{
	QString fn = Q3FileDialog::getSaveFileName(last_working_dir, "*.png", this );
	QString fn2 = fn; 

	int image_mode = 1;
	int pos = fn2.findRev(QString("/").latin1(), -1, false);
	
	last_working_dir = fn2.replace(pos+1, fn2.length()-pos+1, "");
	glwindow->saveimageEvent(fn, image_mode);

	glwindow->glvoxel->update();
	update();
}
void QtMainWindow::saveImage_event(void)
{
 	//set rendering of off, 
	//we want all those precious cpu cycles	
	QString fn = Q3FileDialog::getSaveFileName(last_working_dir, "*.png", this );
  
	//remove the filename of first file to get only dirname
	//and save a copy of last working dir
	QString fn2 = fn; 
	int pos = fn2.findRev(QString("/").latin1(), -1, false);
	last_working_dir = fn2.replace(pos+1, fn2.length()-pos+1, "");

	//statusBar->message("Saving PNG image...", 1000);
  
	if ( !fn.isEmpty() ) 
	{	
		saveImagemsgbox_event();

		if(image_mode==0) 
		{
			//statusBar->message("Error Saving", 2000);
			return;
		}
		else
		{
			glwindow->saveimageEvent(fn, image_mode);
					
			//statusBar->message("Done", 1000);
			//statusBar->message("Ready", 2000);
		}
	}
	else
	{
	    //statusBar->message("Error Saving", 2000);
		return;
	}

	glwindow->glvoxel->update();
	update();
}
void QtMainWindow::saveImagemsgbox_event()
{
    image_dlg = new QDialog(this, "Choose Slice", TRUE);
	image_dlg->setModal(true);

	Q3BoxLayout* image_layout	= new Q3BoxLayout(image_dlg, Q3BoxLayout::LeftToRight, 4, 4, "image_layout");
	QPushButton *ren			= new QPushButton("RENDERING",	image_dlg);
	QPushButton *xy				= new QPushButton("XY",	image_dlg);
	QPushButton *xz				= new QPushButton("XZ",	image_dlg);
	QPushButton *yz				= new QPushButton("YZ",	image_dlg);

	connect(ren,	SIGNAL(clicked()), SLOT(saveImageREN_event()));
	connect(xy,		SIGNAL(clicked()), SLOT(saveImageXY_event()));
	connect(xz,		SIGNAL(clicked()), SLOT(saveImageXZ_event()));
	connect(yz,		SIGNAL(clicked()), SLOT(saveImageYZ_event()));

	image_layout->addWidget(ren, 1);
	image_layout->addWidget(xy, 1);
	image_layout->addWidget(xz, 1);
	image_layout->addWidget(yz, 1);

	image_dlg->exec();
	glwindow->glvoxel->update();
	update();
}
void QtMainWindow::saveImageREN_event()
{
	image_mode = 1;
	image_dlg->close(1);
}
void QtMainWindow::saveImageXY_event()
{
	image_mode = 2;
	image_dlg->close(1);
}
void QtMainWindow::saveImageXZ_event()
{
	image_mode = 3;
	image_dlg->close(1);
}
void QtMainWindow::saveImageYZ_event()
{
	image_mode = 4;
	image_dlg->close(1);
}

void QtMainWindow::info_event()
{
	//if(info_dlg->isVisible()) info_dlg->hide();
	//else info_dlg->show();		
}

void QtMainWindow::about()
{
    QMessageBox::about( this, "Bioptonics Viewer v2.0",
			"A GPU-based volume rendering application.\n"
			"Developed for Optical Projection Tomography data.\n\n"
			"University of East Anglia / John Innes Centre.\n"
			"Jerome Avondo\n\n");

	glwindow->glvoxel->update();
	update();
}
void QtMainWindow::setBG_event(int r, int g, int b)
{
	QColor col;
	col.setRgb(r,g,b);

	glwindow->glvoxel->volrender.old_background_colour = glwindow->glvoxel->volrender.background_colour = col;

	Vector rgb, hsv;
	rgb.x = glwindow->glvoxel->volrender.background_colour.red()/255.0;
	rgb.y = glwindow->glvoxel->volrender.background_colour.green()/255.0;
	rgb.z = glwindow->glvoxel->volrender.background_colour.blue()/255.0;
	RGBtoHSV(rgb, &hsv);

	if(hsv.z>0.5)
	{
		glwindow->glvoxel->volrender.background_fontcolour = QColor(0.0,0.0,0.0);
	}
	else
	{
		glwindow->glvoxel->volrender.background_fontcolour = QColor(255.0,255.0,255.0);
	}

	glwindow->glvoxel->redraw_event();}
void QtMainWindow::custom_layout_event()
{
	QColorDialog* coldialogue=0;
	QColor col = coldialogue->getColor(glwindow->glvoxel->volrender.background_colour, this, "Choose a colour");
	setBG_event(col.red(), col.green(), col.blue());
	update();
}
void QtMainWindow::resetviewall_event(void)
{
	if(view_orientation==1) front_event();
	else if(view_orientation==2) back_event();
	else if(view_orientation==3) left_event();
	else if(view_orientation==4) right_event();
	else if(view_orientation==5) top_event();
	else if(view_orientation==6) bottom_event();

	glwindow->glvoxel->volrender.volrenderview.reset_view();

	glwindow->glvoxel->redraw_event();
}
void QtMainWindow::resetviewzoom_event(void)
{
	glwindow->glvoxel->volrender.volrenderview.reset_zoom();
	glwindow->glvoxel->redraw_event();
}
void QtMainWindow::resetviewtrans_event(void)
{
	glwindow->glvoxel->volrender.volrenderview.reset_translate();
	glwindow->glvoxel->redraw_event();
}

void QtMainWindow::back_event(void)
{
	glwindow->glvoxel->volrender.setViewfromEuler(90,0,0);
	glwindow->glvoxel->redraw_event();
}
void QtMainWindow::front_event(void)
{
	glwindow->glvoxel->volrender.setViewfromEuler(-90,180,0);
	glwindow->glvoxel->redraw_event();
}
void QtMainWindow::right_event(void)
{
	glwindow->glvoxel->volrender.setViewfromEuler(0,90,0);
	glwindow->glvoxel->volrender.setViewfromEuler2(0,0,-90);
	glwindow->glvoxel->redraw_event();
}
void QtMainWindow::left_event(void)
{
	glwindow->glvoxel->volrender.setViewfromEuler(0,-90,0);
	glwindow->glvoxel->volrender.setViewfromEuler2(0,0,90);
	glwindow->glvoxel->redraw_event();
}
void QtMainWindow::top_event(void)
{
	glwindow->glvoxel->volrender.setViewfromEuler(0,180,0);
	glwindow->glvoxel->redraw_event();
}
void QtMainWindow::bottom_event(void)
{
	glwindow->glvoxel->volrender.setViewfromEuler(0,0,0);
	glwindow->glvoxel->redraw_event();
}

void QtMainWindow::transfer_editor_event()
{

#ifdef LIBTIFF_SUPPORT
		if(glwindow->glvoxel->volobject.is16bit)
		{
			transferfunc16bit_dialogue->show();
			transferfunc16bit_dialogue->raise();
		}
		else
		{
			transferfunction_editor_tabdlg->show();
			transferfunction_editor_tabdlg->raise();
		}
#else
		transferfunction_editor_tabdlg->show();
		transferfunction_editor_tabdlg->raise();
#endif

	glwindow->glvoxel->update();
	update();
}
void QtMainWindow::viewpref_event()
{
	view_editor_tabdlg->show();
	view_editor_tabdlg->raise();
	glwindow->glvoxel->update();
	update();
}
void QtMainWindow::lightpref_event()
{
	light_editor_tabdlg->show();
	light_editor_tabdlg->raise();
	glwindow->glvoxel->update();
	update();
}
void QtMainWindow::moviepref_event()
{
	movie_editor_tabdlg->show();
	movie_editor_tabdlg->raise();
	glwindow->glvoxel->update();
	update();
}
void QtMainWindow::measurepref_event()
{
	measure_editor_tabdlg->show();
	measure_editor_tabdlg->raise();
	glwindow->glvoxel->update();
	update();
}
void QtMainWindow::tfunction_event()
{
/*        transferfunc_dialogue->show();
        transferfunc_dialogue->raise();
        glwindow->glvoxel->update();
        update();
*/
}
void QtMainWindow::cameraviewpref_event()
{
	camera_editor_tabdlg->show();
	camera_editor_tabdlg->raise();
	glwindow->glvoxel->update();
	update();
}
void QtMainWindow::renderviewpref_event()
{
	renderview_editor_tabdlg->show();
	renderview_editor_tabdlg->raise();
	glwindow->glvoxel->update();
	update();
}

void QtMainWindow::croppref_event()
{
	crop_editor_tabdlg->show();
	crop_editor_tabdlg->raise();
	glwindow->glvoxel->update();
	update();
}

void QtMainWindow::filterpref_event()
{
	filter_editor_tabdlg->show();
	filter_editor_tabdlg->raise();
	glwindow->glvoxel->update();
	update();
}

void QtMainWindow::volume_editor_event()
{
	volume_editor_tabdlg->show();
	volume_editor_tabdlg->raise();
	glwindow->glvoxel->update();
	update();
}

void QtMainWindow::transform_editor_event()
{
	clipping_editor_tabdlg->show();
	clipping_editor_tabdlg->raise();
	glwindow->glvoxel->update();
	update();
}
/*void QtMainWindow::measure_editor_event()
{
	measure_editor_tabdlg->show();
	measure_editor_tabdlg->raise();
	glwindow->glvoxel->update();
	update();
}
void QtMainWindow::domain_editor_event()
{
	domain_editor_tabdlg->show();
	domain_editor_tabdlg->raise();
	glwindow->glvoxel->update();
	*update();
}*/
void QtMainWindow::closeEvent( QCloseEvent* ce )
{
	volscript.execute_script_at_quit();
	
	if(!volscript.image_batch)
	{
		ce->accept();
		qApp->quit();
	}
	else
	{
		ce->ignore();
	}

	fclose (stdout);
	fclose (stderr);
}

void QtMainWindow::arc_mode_slot(void)
{
	//printf("%s\n", glwindow->glvoxel->volobject.file_name);

//	dragndrop_slot(glwindow->glvoxel->volobject.file_name);

	glwindow->glvoxel->volio.load_RAW(glwindow->glvoxel->volobject.file_name, 4);

	//update our histogram
	volume_editor_tabdlg->rebuild_tabs();
	//load our data to opengl
	glwindow->glvoxel->volrender.compute_boundingbox();
	glwindow->glvoxel->volrender.load_TextureMap();
	//setup our slice arrays and slice sliders
	glwindow->create();
}
void QtMainWindow::resizeEvent ( QResizeEvent * e)
{
/*	int w, h;

	w = width();
	h = height();

	w = w-main_width;
	h = h-main_height;

	if( (w>h && w>0) || (w<h && w<0))
	{
		main_width += w;
		main_height += w;
	}
	else
	{
		main_width += h;
		main_height += h;
	}
*/
//	resize(main_width, main_height);
}

void QtMainWindow::dragndrop_slot(QString sFileName)
{	
	if (!sFileName.isEmpty()) 
	{

		timelapsehelper=0;
		QFile* file = new QFile(sFileName);

		if(!file->exists()) return;

		if(sFileName.contains(".dat", true) || sFileName.contains(".DAT", true)) 
		{	
			openRawFile(sFileName);
		}
		else if(sFileName.contains(".raw", true) || sFileName.contains(".RAW", true)) 
		{		
			openRawFile(sFileName);
		}
		else if(sFileName.contains(".png", true) || sFileName.contains(".PNG", true)
			|| sFileName.contains(".bmp", true) || sFileName.contains(".BMP", true)
			|| sFileName.contains(".jpg", true) || sFileName.contains(".JPG", true) 
			|| sFileName.contains(".tif", true) || sFileName.contains(".TIF", true)) 
		{
			//remove the filename of first file to get only dirname
			//int pos = sFileName.findRev(QString("/").latin1(), -1, false);
			//sFileName.replace(pos+1, sFileName.length()-pos+1, "");

			openImageDir(sFileName);
		}
		else if(sFileName.contains(".obj", true) || sFileName.contains(".OBJ", true))
		{
			//printf("sFileName: %s\n", sFileName.latin1());
			glwindow->glvoxel->isosurface_load_slot(sFileName.latin1());
		}
		/*else if(sFileName.contains(".wlz", true) || sFileName.contains(".WLZ", true)) 
		{
//			glwindow->glvoxel->volio.load_WLZ(sFileName, 4);
			glwindow->glvoxel->volobject.file_name = sFileName;
			glwindow->glvoxel->volobject.file_type = 1;
		//	glwindow->glvoxel->volrender.slices = glwindow->glvoxel->volobject.texwidth;
			glwindow->glvoxel->volrender.load_TextureMap();
			glwindow->create();
			volume_editor_tabdlg->reset();
			glwindow->glvoxel->mcubes.clear();
			volume_editor_tabdlg->isosurface_tab->del_all();	
			volume_editor_tabdlg->rebuild_tabs();
			view_editor_tabdlg->rebuild_tabs();
			update_info_labels();
			glwindow->glvoxel->redraw_event();	
		}*/
		else if(sFileName.contains(".slc", true) || sFileName.contains(".SLC", true)) 
		{	
			clipping_editor_tabdlg->cliptab->dragndrop_slot(sFileName);
		}
		else if(sFileName.contains(".msr", true) || sFileName.contains(".MSR", true)) 
		{	
			measure_editor_tabdlg->measuring_tab->dragndrop_slot(sFileName);
		}
		else if(sFileName.contains(".tfn", true) || sFileName.contains(".TFN", true)) 
		{	
			transferfunction_editor_tabdlg->tranfer_function_tab->dragndrop_slot(sFileName);
		}
#ifdef LIBTIFF_SUPPORT
		else if(sFileName.contains(".tf2", true) || sFileName.contains(".TF2", true)) 
		{	
			transferfunc16bit_dialogue->tranfer_func_tab->dragndrop_slot(sFileName);
		}	
#endif
		else if(sFileName.contains(".vrv", true) || sFileName.contains(".VRV", true))
		{
			renderview_editor_tabdlg->renviewtab->dragndrop_slot(sFileName);
		}
		else 
		{
			return;
		}
  }
}
void QtMainWindow::updateStatusBar(void)
{
	QString x, y, z;
	x.setNum(glwindow->glvoxel->volobject.width, 10);
	y.setNum(glwindow->glvoxel->volobject.height, 10);
	z.setNum(glwindow->glvoxel->volobject.depth, 10);

	QString c;
	if(glwindow->glvoxel->volobject.is_greyscale) c = "1";
	else
	{
		int nchannels=0;
		if(glwindow->glvoxel->volobject.has_red) nchannels ++;
		if(glwindow->glvoxel->volobject.has_green) nchannels ++;
		if(glwindow->glvoxel->volobject.has_blue) nchannels ++;

		c.setNum(nchannels, 10);
	}
	QString statmsg = " <b>data:</b> [" + x + "x" + y + "x" + z + "x" + c + "]" + " | <b>file:</b> [" + glwindow->glvoxel->volobject.file_name + "]";
	glwindow->info_label->setText(statmsg);
	//BTAAAAAP	
	//statusBar->showMessage(statmsg, 0);
	//updateStatusBar();
}
void QtMainWindow::load_commandlineparams(int argc, char **argv)
{
	//printf("args: %d\n", argc);
	if(argc==1) return;

	QString commandparam_str = "";
	for(int i=1; i<argc; i++)
	{
		commandparam_str += QString(argv[i]) + "\n";
	}
	//printf("commandparam_str: %s\n", commandparam_str.latin1());
	
	volscript.parse_script(commandparam_str);
	volscript.execute_script_now();
//	commandline_parse(commandList);
}
void QtMainWindow::setWatchFile(QString fn)
{
	watchfile = fn.latin1();

	if(watchfile != "")
	{
		//check file exists.

		printf("watchfile: %s\n", watchfile.latin1());

		watchfile_qfilesystemwatcher = new QFileSystemWatcher(this);
		watchfile_qfilesystemwatcher->addPath(watchfile);
		connect(watchfile_qfilesystemwatcher, SIGNAL(fileChanged(QString)), this, SLOT(watchfileChanged(QString)));

		watchfileChanged(watchfile);
	}
}
void QtMainWindow::commandline_parse(QStringList commandList)
{
	QString str = "";
	QString voldir="";
	QString msrfile="";
	QString tfuncfile="";
	QString slcfile="";
	
	int load_mode = -1;
	int auto_close = 0;
	int auto_hide = 0;
	int render = 0;
	int rot_movie = -1;
	int set_view = -1;
	int rock_movie = -1;

	QStringList functionargus;

#ifdef OMERO_SUPPORT	
	omeroLoader.reset();
#endif

	for(int i=0; i<commandList.size(); i++)
	{
		printf("command[%i]: %s \n", i, commandList[i].latin1());

		if(commandList[i].contains("voldir"))
		{
			load_mode=1;
			QString temp = commandList[i].remove("voldir=");
			temp.remove('"');
			voldir = temp;	
		}
		else if(commandList[i].contains("msr"))
		{
			QString temp = commandList[i].remove("msr=");
			temp.remove('"');
			msrfile = temp;
		}
		else if(commandList[i].contains("tfunc"))
		{
			QString temp = commandList[i].remove("tfunc=");
			temp.remove('"');
			tfuncfile = temp;
		}
		else if(commandList[i].contains("slc"))
		{
			QString temp = commandList[i].remove("slc=");
			temp.remove('"');
			slcfile = temp;
		}
#ifdef OMERO_SUPPORT
		else if(commandList[i].contains("omero_server"))
		{
			load_mode=2;

			QString temp = commandList[i].remove("omero_server=");
			temp.remove('"');

			omeroLoader.svrname = temp.latin1();	
		}	
		else if(commandList[i].contains("omero_username"))
		{
			QString temp = commandList[i].remove("omero_username=");
			temp.remove('"');

			omeroLoader.username = temp.latin1();	
		}
		else if(commandList[i].contains("omero_sessionid"))
		{
			QString temp = commandList[i].remove("omero_sessionid=");
			temp.remove('"');

			omeroLoader.sessionID = temp.latin1();
		}
		else if(commandList[i].contains("omero_password"))
		{
			QString temp = commandList[i].remove("omero_password=");
			temp.remove('"');

			omeroLoader.psswd = temp.latin1();	
		}	
		else if(commandList[i].contains("omero_port"))
		{
			commandList[i].remove("omero_port=");
			omeroLoader.port = commandList[i].toInt();	
		}	
		else if(commandList[i].contains("omero_imageid"))
		{
			commandList[i].remove("omero_imageid=");
			omeroLoader.imageID = commandList[i].toInt();	
		}
#endif
		else if(commandList[i].contains("watchfile"))
		{
			QString temp = commandList[i].remove("watchfile=");
			//temp.remove('"');
			watchfile = temp.latin1();

			if(watchfile != "")
			{
				watchfile_qfilesystemwatcher = new QFileSystemWatcher(this);
				watchfile_qfilesystemwatcher->addPath(watchfile);
				connect(watchfile_qfilesystemwatcher, SIGNAL(fileChanged(QString)), this, SLOT(watchfileChanged(QString)));
			}
		}
		else if(commandList[i].contains("render"))
		{
			commandList[i].remove("render=");
			render = commandList[i].toInt();
		}		
		else if(commandList[i].contains("set_view("))
		{
			set_view = 1;

			QString temp = commandList[i].remove("set_view(");
			temp = temp.remove(")");
			temp = temp.remove('"');

			functionargus = temp.split(",");

			/*cout<<functionargus[0].latin1()<<endl;
			cout<<functionargus[1].latin1()<<endl;
			cout<<functionargus[2].latin1()<<endl;*/
		}		
		else if(commandList[i].contains("movie_rotate"))
		{
			rot_movie = 1;

			QString temp = commandList[i].remove("movie_rotate(");
			temp = temp.remove(")");
			temp = temp.remove('"');

			functionargus = temp.split(",");

			/*cout<<functionargus[0].latin1()<<endl;
			cout<<functionargus[1].latin1()<<endl;
			cout<<functionargus[2].latin1()<<endl;
			cout<<functionargus[3].latin1()<<endl;
			cout<<functionargus[4].latin1()<<endl;
			cout<<functionargus[5].latin1()<<endl;*/

		}	
		else if(commandList[i].contains("movie_rock"))
		{
			rock_movie = 1;

			QString temp = commandList[i].remove("movie_rock(");
			temp = temp.remove(")");
			temp = temp.remove('"');

			functionargus = temp.split(",");

			/*cout<<functionargus[0].latin1()<<endl;
			cout<<functionargus[1].latin1()<<endl;
			cout<<functionargus[2].latin1()<<endl;*/

		}
		else if(commandList[i].contains("auto_close"))
		{
			commandList[i].remove("auto_close=");
			auto_close = commandList[i].toInt();	
		}
		else if(commandList[i].contains("auto_hide"))
		{
			commandList[i].remove("auto_hide=");
			auto_hide = commandList[i].toInt();	
		}
	}

	if(auto_hide==1) setVisible(false);

	//clean up our string for local paths
	voldir = voldir.replace('\\', '/'); 
	if(!voldir.isEmpty() && !voldir.endsWith('/')) voldir += '/';
	msrfile = msrfile.replace('\\', '/');
	tfuncfile = tfuncfile.replace('\\', '/');
	slcfile = slcfile.replace('\\', '/');

	//debug output
	//printf("COMMANDLINE: voldir: %s\n", voldir.latin1());
	//printf("COMMANDLINE: msrfile: %s\n", msrfile.latin1());
	//printf("COMMANDLINE: tfuncfile: %s\n", tfuncfile.latin1());
	//printf("COMMANDLINE: slcfile: %s\n", slcfile.latin1());
	
	if(load_mode==1)
	{
		timelapsehelper=1;
		openImageDir(voldir);
	}
#ifdef OMERO_SUPPORT
	else if(load_mode==2)
	{
		printf("COMMANDLINE: Loading OMERO data...\n");
		//else
		{
			//omeroLoader.debug();
			openOmeroImage_LoadDataevent();
		}
	}
#endif

	if(!msrfile.isEmpty()) measure_editor_tabdlg->measuring_tab->open_file(msrfile);
	if(!slcfile.isEmpty()) clipping_editor_tabdlg->cliptab->dragndrop_slot(slcfile);
	if(!tfuncfile.isEmpty()) transferfunction_editor_tabdlg->tranfer_function_tab->dragndrop_slot(tfuncfile);

	if(functionargus.size()==3 && set_view==1)
	{
		printf("COMMANDLINE: Set View\n");
		glwindow->glvoxel->volrender.setViewfromEuler(functionargus[0].toFloat(), functionargus[1].toFloat(), functionargus[2].toFloat());
		glwindow->glvoxel->redraw_event();
	}
	if(functionargus.size()==3 && rock_movie==1)
	{
		printf("COMMANDLINE: Running Rock Movie\n");
		functionargus[1] = functionargus[1].replace('\\', '/');
		glwindow->glvoxel->movie_rockmovie_slot(this, functionargus[0].toInt(), 0, functionargus[1], functionargus[2].toFloat());
	}
	if(functionargus.size()==6 && rot_movie==1)
	{
		printf("COMMANDLINE: Running Rotate Movie\n");
		functionargus[1] = functionargus[1].replace('\\', '/');
		glwindow->glvoxel->movie_rotmovie_slot(this, functionargus[0].toInt(), 0, functionargus[1], functionargus[2].toInt(), functionargus[3].toInt(), functionargus[4].toInt(), functionargus[5].toFloat());
	}
	if(render)
	{
		printf("COMMANDLINE: Rendering...\n");
		
		//glwindow->glvoxel->volrender.blend_mode(2);

		glwindow->glvoxel->volrender.blend_mode(2);
		glwindow->glvoxel->redraw_event();

		glwindow->saveimageEvent(appPath+"/Render.png", 1);
	}

	if(auto_close==1)
	{
		printf("COMMANDLINE: Auto closing....\n");
		exit(1);
	}
}
void QtMainWindow::parse_inifile(QString fn)
{
	string label;
	ifstream input_file; 
	input_file.open(fn.latin1());

	if(!input_file.is_open())
	{
		printf("Error loading INI file!\n");
		system("pause");
		exit(1);
	}

	printf("LOADING: Tokenizing settings.ini file...\n");

	//tokenize our input file
	Tokenizer token(input_file);

	printf("LOADING: Finished tokenizing settings.ini file...\n");

	default_filename="";

	printf("LOADING: Parsing Settings.ini file...\n");

	//until oef is reached step through each token
	while(!input_file.eof()){
		if(token.ttype()==TT_WORD)
		{
			//get the first string lable
			label = token.sval();
			
			//printf("label: %s\n", label.data());

			if(label=="window_width")
			{		
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) main_width = token.nval();
			}
			else if(label=="window_height")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) main_height = token.nval();
			}
			else if(label=="window_maximised")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) maximised = token.nval();
			}
			else if(label=="window_consolle")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) consolle = token.nval();
			}							
			else if(label=="window_main_screen")
			{		
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) window_main_screen = token.nval();
			}					
			else if(label=="view_volume_toggle")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->volrender.volume_toggle = token.nval();
			}
			else if(label=="mouse_wheel_sensitivity")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->dw = token.nval();
			}
			else if(label=="view_stereo_toggle")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->volrender.stereo_toggle = token.nval();
			}
			else if(label=="view_stereo_mode")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->volrender.stereo_mode = token.nval();
			}
			else if(label=="view_stereo_seperation")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->volrender.stereo_eye_seperation = token.nval();
			//	cout<<glwindow->glvoxel->volrender.stereo_eye_seperation<<endl;
			}
			else if(label=="view_isosurface_toggle")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->volrender.isosurface_toggle = token.nval();
			}
			else if(label=="view_BB_toggle")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->volrender.BB_toggle = token.nval();

				if(glwindow->glvoxel->volrender.BB_toggle) boundingboxAct->setChecked(true);
				else  boundingboxAct->setChecked(false);
			}
			else if(label=="view_cursor_toggle")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->volrender.cursor_toggle = token.nval();
				
				//if(glwindow->glvoxel->volrender.cursor_toggle) cursorAct->setChecked(true);
				//else  cursorAct->setChecked(false);
			}
			else if(label=="view_fps_toggle")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->volrender.fps_toggle = token.nval();

				if(glwindow->glvoxel->volrender.fps_toggle) fpsAct->setChecked(true);
				else  fpsAct->setChecked(false);
			}
			else if(label=="view_axis_toggle")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->volrender.axis_toggle = token.nval();				

				if(glwindow->glvoxel->volrender.axis_toggle) axisAct->setChecked(true);
				else  axisAct->setChecked(false);

			}
			else if(label=="view_slice_draw_mode")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->volrender.slice_draw_mode = token.nval();
			}
			else if(label=="view_camera_settings")
			{
				printf("view_camera_settings\n");
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER)
				{
					int val = token.nval();
					printf("value: %d\n", val);

					if(val==0) cameraviewpref->setDisabled(true);
					else cameraviewpref->setDisabled(false);
				}
			}
			else if(label=="view_background_colour")
			{
				int r,g,b;

				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) r = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) g = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) b = token.nval();

				glwindow->glvoxel->volrender.background_colour.setRgb(r,g,b);
				glwindow->glvoxel->volrender.old_background_colour.setRgb(r,g,b);
			
				Vector rgb, hsv;
				rgb.x = glwindow->glvoxel->volrender.background_colour.red()/255.0;
				rgb.y = glwindow->glvoxel->volrender.background_colour.green()/255.0;
				rgb.z = glwindow->glvoxel->volrender.background_colour.blue()/255.0;
				RGBtoHSV(rgb, &hsv);

				if(hsv.z>0.5)
				{
					glwindow->glvoxel->volrender.background_fontcolour = QColor(0.0,0.0,0.0);
				}
				else
				{
					glwindow->glvoxel->volrender.background_fontcolour = QColor(255.0,255.0,255.0);
				}
			}	
			else if(label=="view_bb_colour")
			{
				int r,g,b;

				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) r = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) g = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) b = token.nval();

				glwindow->glvoxel->volrender.bb_colour.setRgb(r,g,b);
			}
			else if(label=="view_default_orientation")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				QString orient;
				if(token.ttype()==TT_STRING) orient = string(token.sval()).data();
				
				//printf("%s\n", orient.latin1());
				
				if(orient=="FRONT")
				{
					view_orientation = 1;
					front_event();
				}
				else if(orient=="BACK")
				{
					view_orientation = 2;
					back_event();
				}
				else if(orient=="LEFT")
				{
					view_orientation = 3;
					left_event();
				}
				else if(orient=="RIGHT")
				{
					view_orientation = 4;
					right_event();
				}
				else if(orient=="TOP")
				{
					view_orientation = 5;
					top_event();
				}
				else if(orient=="BOTTOM")
				{
					view_orientation = 6;
					bottom_event();
				}
			}
			else if(label=="view_gldrawbuffer")
			{
				token.nextToken(); 
				token.nextToken();
				QString orient;
				if(token.ttype()==TT_STRING) glwindow->glvoxel->volrender.gldrawbuffer = string(token.sval()).data();
			}
			else if(label=="view_light_toggle")
			{
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->volrender.lights_toggle = token.nval();
				
				if(glwindow->glvoxel->volrender.lights_toggle==1) lightAct->setChecked(true);				
				else  lightAct->setChecked(false);
			}
			else if(label=="view_scalebar_toggle")
			{
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->volrender.scalebar_toggle = token.nval();
				
				if(glwindow->glvoxel->volrender.scalebar_toggle==1) scalebarAct->setChecked(true);				
				else  scalebarAct->setChecked(false);
			}
			else if(label=="filter_easymode")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER)
				{
					int val = token.nval();
					filter_editor_tabdlg->filtertab->easymode(val);
				}
			}
			else if(label=="measure_modify_mode")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER)
				{
					int val = token.nval();
					measure_editor_tabdlg->measuring_tab->modifyshow_toggle = val;
				}
			}
			else if(label=="measure_selection_window")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER)
				{
					float val = token.nval();
					glwindow->glvoxel->volrender.measure_selection_window = val;
				}
			}
			else if(label=="measure_arrowhead_size")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER)
				{
					float val = token.nval();
					glwindow->glvoxel->volrender.measure_arrowhead_size = val;
				}
			}
			else if(label=="measure_active_line_size")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER)
				{
					float val = token.nval();
					glwindow->glvoxel->volrender.measure_active_line_size = val;
				}
			}
			else if(label=="measure_active_point_size")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER)
				{
					float val = token.nval();
					glwindow->glvoxel->volrender.measure_active_point_size = val;
				}
			}
			else if(label=="measure_line_size")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER)
				{
					float val = token.nval();
					glwindow->glvoxel->volrender.measure_line_size = val;
				}
			}
			else if(label=="measure_point_size")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER)
				{
					float val = token.nval();
					glwindow->glvoxel->volrender.measure_point_size = val;
				}
			}
			else if(label=="measure_active_face_colour")
			{
				float r,g,b;

				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) r = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) g = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) b = token.nval();

				glwindow->glvoxel->volrender.measure_active_face_col = Vector(r/255.0, g/255.0, b/255.0);
			}
			else if(label=="measure_active_edge_colour")
			{
				float r,g,b;

				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) r = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) g = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) b = token.nval();

				glwindow->glvoxel->volrender.measure_active_edge_col = Vector(r/255.0, g/255.0, b/255.0);
			}
			else if(label=="measure_active_vertex_colour")
			{
				float r,g,b;

				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) r = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) g = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) b = token.nval();

				glwindow->glvoxel->volrender.measure_active_vert_col = Vector(r/255.0, g/255.0, b/255.0);
			}
			else if(label=="measure_face_colour")
			{
				float r,g,b;

				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) r = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) g = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) b = token.nval();

				glwindow->glvoxel->volrender.measure_face_col = Vector(r/255.0, g/255.0, b/255.0);
			}
			else if(label=="measure_edge_colour")
			{
				float r,g,b;

				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) r = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) g = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) b = token.nval();

				glwindow->glvoxel->volrender.measure_edge_col = Vector(r/255.0, g/255.0, b/255.0);
			}
			else if(label=="measure_vertex_colour")
			{
				float r,g,b;

				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) r = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) g = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) b = token.nval();

				glwindow->glvoxel->volrender.measure_vert_col = Vector(r/255.0, g/255.0, b/255.0);
			}
			else if(label=="measure_selected_colour")
			{
				float r,g,b;

				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) r = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) g = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) b = token.nval();

				glwindow->glvoxel->volrender.measure_selected_col = Vector(r/255.0, g/255.0, b/255.0);
			}
			else if(label=="measure_magnet_colour")
			{
				float r,g,b;

				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) r = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) g = token.nval();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) b = token.nval();

				glwindow->glvoxel->volrender.measure_magnet_col = Vector(r/255.0, g/255.0, b/255.0);
			}
#ifdef OMERO_SUPPORT
			else if(label=="omero_server")
			{
				string str;

				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_STRING) str = token.sval();
				omeroLoader.svrname = str;
			}
			else if(label=="omero_port")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) serverport = token.nval();
				omeroLoader.port = serverport;
			}
			else if(label=="omero_username")
			{
				string str;

				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_STRING) str = token.sval();
				omeroLoader.username = str;
			}
#endif
			else if(label=="volume_maxquality")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER)
				{
					int val = token.nval();
					view_editor_tabdlg->rendersettings_tab->rebuild();
					view_editor_tabdlg->rendersettings_tab->renderquality_slider->setMaximum(val);
				}
			}
			else if(label=="volume_maxquality_onmove")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER)
				{
					int val = token.nval();
					view_editor_tabdlg->rendersettings_tab->rebuild();
					view_editor_tabdlg->rendersettings_tab->renderquality_lod_slider->setMaximum(val);
				}
			}
			else if(label=="volume_maxsize")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->volobject.maxsizeZ = glwindow->glvoxel->volobject.maxsizeY = glwindow->glvoxel->volobject.maxsizeX = token.nval();
			}
			else if(label=="volume_render_mode")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->volrender.volume_render_mode = token.nval();
			}
			else if(label=="volume_viewport_size")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->volrender.FBO_SIZE = token.nval();
			}
			else if(label=="volume_viewport_size_onmove")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->volrender.LOD_FBO_SIZE = token.nval();
			}
			else if(label=="volume_viewport_toggle")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->volrender.FBOmoveLOD_toggle = token.nval();

			}
			else if(label=="volume_slices")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->volrender.numbslices = glwindow->glvoxel->volrender.slices = token.nval();
		
			}
			else if(label=="volume_slices_onmove")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->volrender.sliceslod = token.nval();
		
			}
			else if(label=="volume_slices_toggle")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->volrender.SlicesmoveLOD_toggle = token.nval();
		
			}
			else if(label=="volume_postprocess")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->volrender.PostProcess_toggle = token.nval();
		
				if(glwindow->glvoxel->volrender.PostProcess_toggle==1) view_editor_tabdlg->rendersettings_tab->postprocess_pb->setOn(TRUE);
				else view_editor_tabdlg->rendersettings_tab->postprocess_pb->setOn(FALSE);
			}
			else if(label=="volume_lodonmove_toggle")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER)
				{
					int val = token.nval();
					glwindow->glvoxel->volrender.SlicesmoveLOD_toggle = val;
					glwindow->glvoxel->volrender.FBOmoveLOD_toggle = val;
					
					if(val==1) view_editor_tabdlg->rendersettings_tab->renderquality_lod_pb->setOn(TRUE);
					else view_editor_tabdlg->rendersettings_tab->renderquality_lod_pb->setOn(FALSE);
				}
			}
			else if(label=="volume_postprocess_amount")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->volrender.postprocess2d_amount = token.nval();
			}
			else if(label=="volume_quality")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) view_editor_tabdlg->rendersettings_tab->renderquality = token.nval();
			}
			else if(label=="volume_quality_onmove")
			{
				//skip the equal sign
				token.nextToken();
				token.nextToken();
				if(token.ttype()==TT_NUMBER) view_editor_tabdlg->rendersettings_tab->renderquality_lod = token.nval();
			}	
			else if(label=="file_default_filename")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_STRING) default_filename = string(token.sval()).data();
			}
			else if(label=="file_type")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) glwindow->glvoxel->volobject.file_type = token.nval();
			}
			else if(label=="file_last_working_dir")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_STRING) last_working_dir = string(token.sval()).data();
			}
			else if(label=="file_defaul_movie_dir")
			{
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_STRING)
				{
					movie_editor_tabdlg->moviesettings_tab->defaul_movie_dir = string(token.sval()).data();
					QDir dir(movie_editor_tabdlg->moviesettings_tab->defaul_movie_dir);
					if(!dir.exists())movie_editor_tabdlg->moviesettings_tab->defaul_movie_dir="";

					movie_editor_tabdlg->moviesettings_tab->choosedir_te->setText(movie_editor_tabdlg->moviesettings_tab->defaul_movie_dir);
				}
			}			
			//else we dont know what it is, go to next label
			else
			{
				//cout<<"WARNING: Unrecognized Label..."<<endl;
				token.nextToken();
			}
		}
		else token.nextToken();
	}
	input_file.close();

	printf("LOADING: Updating UI...\n");

	//UPDATA GUI STUFF NOW...
	if(glwindow->glvoxel->volrender.FBOmoveLOD_toggle && glwindow->glvoxel->volrender.SlicesmoveLOD_toggle)
	{
		view_editor_tabdlg->rendersettings_tab->renderquality_lod_pb->setOn(TRUE);
	}
	else
	{
		view_editor_tabdlg->rendersettings_tab->renderquality_lod_pb->setOn(FALSE);
	}

	if(glwindow->glvoxel->volobject.maxsizeZ)
	{
		view_editor_tabdlg->rendersettings_tab->maxtexturesize = glwindow->glvoxel->volobject.maxsizeZ;
	}


	if(glwindow->glvoxel->volrender.PostProcess_toggle)
	{
		view_editor_tabdlg->rendersettings_tab->postprocess_pb->setOn(TRUE);
	}
	else
	{
		view_editor_tabdlg->rendersettings_tab->postprocess_pb->setOn(FALSE);
	}

	if(glwindow->glvoxel->volrender.postprocess2d_amount)
	{
		view_editor_tabdlg->rendersettings_tab->postprocess = glwindow->glvoxel->volrender.postprocess2d_amount;
	}

	if(!default_filename.isEmpty())
	{
		dragndrop_slot(default_filename);
	}
	
	view_editor_tabdlg->rebuild_tabs();

	stereo_editor_tabdlg->stereosettings_tab->set_fromini(glwindow->glvoxel->volrender.stereo_toggle, glwindow->glvoxel->volrender.stereo_mode, glwindow->glvoxel->volrender.stereo_eye_seperation);

	int x_pos, y_pos;
	int screen0_w, screen0_h, screen1_w, screen1_h;
	
	resize(main_width, main_height);
	
	if((int)QApplication::desktop()->numScreens()>1)
	{
		//printf("mutli-screen \n");
		
		screen0_w = QApplication::desktop()->screenGeometry(0).width();
		screen0_h = QApplication::desktop()->screenGeometry(0).height();
		screen1_w = QApplication::desktop()->screenGeometry(1).width();
		screen1_h = QApplication::desktop()->screenGeometry(1).height();

		//printf("screen0: %d x %d\n", screen0_w, screen0_h);
		//printf("screen1: %d x %d\n", screen1_w, screen1_h);
		
		if(window_main_screen==0)
		{		
			printf("pos screen0: %d x %d\n", (int)((screen0_w/2.0) - (main_width/2.0)),  (int)((screen0_h/2.0) - (main_height/2.0)));
			move(screen0_w/2.0 - main_width/2.0,  screen0_h/2.0 - main_height/2.0);
		}
		else
		{
			//printf("pos screen1: %d x %d\n", (int) ((screen0_w + (screen1_w/2.0)) - (main_width/2.0)),  (int)((screen1_h/2.0) - (main_height/2.0)));
			move((screen0_w + (screen1_w/2.0)) - (main_width/2.0),  (screen1_h/2.0) - (main_height/2.0));		
		}
	}
	else
	{
		//printf("single-screen \n");

		screen0_w = QApplication::desktop()->width();
		screen0_h = QApplication::desktop()->height(); 
		move(screen0_w/2.0 - main_width/2.0,  screen0_h/2.0 - main_height/2.0);		
	}		
	
	if(maximised==1)
	{
		//printf("showMaximized\n");
		activateWindow ();
		raise();
		showMaximized();
	}	
	else
	{
		//printf("showNormal\n");
		activateWindow ();
		raise();
		showNormal();
	}

	printf("LOADING: Finished loading Settings.ini file...\n");

	glwindow->glvoxel->volrender.initialize_DrawBufferGL();
	glwindow->glvoxel->redraw_event();
}
void QtMainWindow::load_inifile(void)
{
	printf("LOADING: Settings.ini file...\n");

	QString fn = appPath+"/settings.ini";
	parse_inifile(fn);
}
void QtMainWindow::minibutton_savelayout_event(void)
{
	QByteArray state = saveState();

	QFile* windowStateFile = new QFile("states.cfg");
	if(windowStateFile->open(QIODevice::WriteOnly))
	{
	  windowStateFile->write(state);
	  windowStateFile->close();
	}
}

void QtMainWindow::minibutton_restorelayout_event(void)
{
	QFile* windowStateFile = new QFile("states.cfg");
    if(windowStateFile->open(QIODevice::ReadOnly))
    {
      QByteArray state = windowStateFile->readAll();
      restoreState(state);
      windowStateFile->close();
    }
}

void QtMainWindow::minibutton_I_event(void)
{
	togglevoxels_event();

	/*if(intentb->isOn())
	{
		redtb->setOn(true);
		greentb->setOn(true);
		bluetb->setOn(true);
	}
	else
	{
		redtb->setOn(false);
		greentb->setOn(false);
		bluetb->setOn(false);
	}*/
}
void QtMainWindow::minibutton_red_event(void)
{
	if(glwindow->glvoxel->volrender.r_channel) glwindow->glvoxel->volrender.r_channel =0;
	else glwindow->glvoxel->volrender.r_channel = 1;
	glwindow->glvoxel->redraw_event();
}
void QtMainWindow::minibutton_green_event(void)
{
	if(glwindow->glvoxel->volrender.g_channel) glwindow->glvoxel->volrender.g_channel =0;
	else glwindow->glvoxel->volrender.g_channel = 1;
	glwindow->glvoxel->redraw_event();
}
void QtMainWindow::minibutton_blue_event(void)
{
	if(glwindow->glvoxel->volrender.b_channel) glwindow->glvoxel->volrender.b_channel =0;
	else glwindow->glvoxel->volrender.b_channel = 1;
	glwindow->glvoxel->redraw_event();
}
void QtMainWindow::minibutton_bg_event(void)
{
	if(glwindow->glvoxel->volrender.bldmode==2) return;
	
	custom_layout_event();
}


void QtMainWindow::minibutton_vertfinder_event(void)
{
/*	glwindow->glvoxel->make_vertfinder();

	glwindow->glvoxel->volobject.has_red = 1;
	glwindow->glvoxel->volobject.has_green = 1;
	glwindow->glvoxel->volobject.has_blue = 1;

	glwindow->glvoxel->volrender.r_channel = 1;
	glwindow->glvoxel->volrender.g_channel = 1;
	glwindow->glvoxel->volrender.b_channel = 1;
	glwindow->glvoxel->volobject.is_greyscale=0;

	intentb->setOn(true);
	redtb->setOn(true);
	greentb->setOn(true);
	bluetb->setOn(true);
	transferfunction_editor_tabdlg->tranfer_function_tab->red_pb->setEnabled(true);
	transferfunction_editor_tabdlg->tranfer_function_tab->green_pb->setEnabled(true);
	transferfunction_editor_tabdlg->tranfer_function_tab->blue_pb->setEnabled(true);

	volume_editor_tabdlg->reset();
	volume_editor_tabdlg->rebuild_tabs();
	
	view_editor_tabdlg->rebuild_tabs();	
	view_editor_tabdlg->rendersettings_tab->rebuild();
	
	glwindow->create();
	updateStatusBar();
	
	*/

	timelapsehelper++;
	if(timelapsehelper>timelapsedirs.size()-1) timelapsehelper = timelapsedirs.size()-1;
	QString temp = last_working_dir;
	openImageDir(last_working_dir+"/"+timelapsedirs.at(timelapsehelper)+"/");
	last_working_dir = temp;
}
void QtMainWindow::timelapseEvent(void)
{
	timelapsehelper++;
	if(timelapsehelper>timelapsedirs.size()-1) timelapsehelper = timelapsedirs.size()-1;
	QString temp = last_working_dir;
	openImageDir(last_working_dir+"/"+timelapsedirs.at(timelapsehelper)+"/");
	last_working_dir = temp;
}
void QtMainWindow::minibutton_filter_event(void)
{
	/*glwindow->glvoxel->make_filter();

	glwindow->glvoxel->volobject.has_red = 1;
	glwindow->glvoxel->volobject.has_green = 1;
	glwindow->glvoxel->volobject.has_blue = 1;

	glwindow->glvoxel->volrender.r_channel = 1;
	glwindow->glvoxel->volrender.g_channel = 1;
	glwindow->glvoxel->volrender.b_channel = 1;
	glwindow->glvoxel->volobject.is_greyscale=0;

	intentb->setOn(true);
	redtb->setOn(true);
	greentb->setOn(true);
	bluetb->setOn(true);
	transferfunction_editor_tabdlg->tranfer_function_tab->red_pb->setEnabled(true);
	transferfunction_editor_tabdlg->tranfer_function_tab->green_pb->setEnabled(true);
	transferfunction_editor_tabdlg->tranfer_function_tab->blue_pb->setEnabled(true);

	volume_editor_tabdlg->reset();
	volume_editor_tabdlg->rebuild_tabs();
	
	view_editor_tabdlg->rebuild_tabs();	
	view_editor_tabdlg->rendersettings_tab->rebuild();
	
	glwindow->create();
	updateStatusBar();
	
*/
}
void QtMainWindow::minibutton_label_event(void)
{
/*	glwindow->glvoxel->make_binary();

	glwindow->glvoxel->volobject.has_red = 1;
	glwindow->glvoxel->volobject.has_green = 1;
	glwindow->glvoxel->volobject.has_blue = 1;

	glwindow->glvoxel->volrender.r_channel = 1;
	glwindow->glvoxel->volrender.g_channel = 1;
	glwindow->glvoxel->volrender.b_channel = 1;
	glwindow->glvoxel->volobject.is_greyscale=0;

	intentb->setOn(true);
	redtb->setOn(true);
	greentb->setOn(true);
	bluetb->setOn(true);
	transferfunction_editor_tabdlg->tranfer_function_tab->red_pb->setEnabled(true);
	transferfunction_editor_tabdlg->tranfer_function_tab->green_pb->setEnabled(true);
	transferfunction_editor_tabdlg->tranfer_function_tab->blue_pb->setEnabled(true);

	volume_editor_tabdlg->reset();
	volume_editor_tabdlg->rebuild_tabs();
	
	view_editor_tabdlg->rebuild_tabs();	
	view_editor_tabdlg->rendersettings_tab->rebuild();
	
	glwindow->create();
	updateStatusBar();
	
*/
	timelapsehelper--;
	if(timelapsehelper<0) timelapsehelper = 0;
	QString temp = last_working_dir;
	openImageDir(last_working_dir+"/"+timelapsedirs.at(timelapsehelper)+"/");
	last_working_dir = temp;
}

void QtMainWindow::minibutton_mip_event(void)
{
	if(glwindow->glvoxel->volrender.bldmode==1)
	{
		blend_mode2_event();
	}
	else
	{
		blend_mode1_event();
	}

}
void QtMainWindow::minibutton_atten_event(void)
{
	if(glwindow->glvoxel->volrender.bldmode==1) blend_mode4_event();
	else blend_mode1_event();
}
void QtMainWindow::minibutton_bb_event(void)
{
	toggleBB_event();
}
void QtMainWindow::minibutton_lights_event(void)
{
	togglelights_event();
}
void QtMainWindow::minibutton_axis_event(void)
{
	toggleaxis_event();
}
void QtMainWindow::minibutton_cursor_event(void)
{
	togglecursor_event();
}
void QtMainWindow::minibutton_surfaces_event(void)
{
	togglesurfaces_event();
}

void QtMainWindow::minibutton_fps_event(void)
{
	togglefps_event();
}
void QtMainWindow::minibutton_info_event(void)
{
	info_event();
}
void QtMainWindow::minibutton_stereo_event(void)
{
	stereo_editor_tabdlg->show();
}
void QtMainWindow::minibutton_fullscreen_event(void)
{

	glwindow->sections_toggle = !glwindow->sections_toggle;

	if(glwindow->sections_toggle)
	{
		glwindow->yzlb->hide();
		glwindow->slider_yz->setEnabled(false);
		glwindow->slider_yz->hide();
		glwindow->slider_yz_lb->hide();

		glwindow->xylb->hide();
		glwindow->slider_xy->setEnabled(false);
		glwindow->slider_xy->hide();
		glwindow->slider_xy_lb->hide();

		glwindow->xzlb->hide();
		glwindow->slider_xz->setEnabled(false);
		glwindow->slider_xz->hide();
		glwindow->slider_xz_lb->hide();
		
		glwindow->cursor_label->hide();
	}
	else
	{
		glwindow->yzlb->show();
		glwindow->slider_yz->setEnabled(true);
		glwindow->slider_yz->show();
		glwindow->slider_yz_lb->show();

		glwindow->xylb->show();
		glwindow->slider_xy->setEnabled(true);
		glwindow->slider_xy->show();
		glwindow->slider_xy_lb->show();

		glwindow->xzlb->show();
		glwindow->slider_xz->setEnabled(true);
		glwindow->slider_xz->show();
		glwindow->slider_xz_lb->show();

		glwindow->cursor_label->show();	
	}

}
void QtMainWindow::minibutton_hq_event(void)
{
	view_editor_tabdlg->rendersettings_tab->renderquality = 10.0;
	view_editor_tabdlg->rendersettings_tab->renderquality_lod = 10.0;
	view_editor_tabdlg->rebuild_tabs();
}
void QtMainWindow::minibutton_mq_event(void)
{
	view_editor_tabdlg->rendersettings_tab->renderquality = 5.0;
	view_editor_tabdlg->rendersettings_tab->renderquality_lod = 5.0;
	view_editor_tabdlg->rebuild_tabs();
}
void QtMainWindow::minibutton_lq_event(void)
{
	view_editor_tabdlg->rendersettings_tab->renderquality = 2.0;
	view_editor_tabdlg->rendersettings_tab->renderquality_lod = 2.0;
	view_editor_tabdlg->rebuild_tabs();
}
void QtMainWindow::single_channel_slot(void)
{
		intentb->setOn(true);
		redtb->setOn(false);
		greentb->setOn(false);
		bluetb->setOn(false);
}
void QtMainWindow::measure_setscale_slot(float x, float y, float z)
{
	printf("QtMainWindow::measure_setscale_slot\n");
	glwindow->glvoxel->volobject.xscale = x;
	glwindow->glvoxel->volobject.yscale = y;
	glwindow->glvoxel->volobject.zscale = z;
	glwindow->glvoxel->redraw_event();
}
void QtMainWindow::measureCursorOn_slot(void)
{
	setCursor(QCursor(Qt::CrossCursor));
}
void QtMainWindow::measureCursorOff_slot(void)
{
	setCursor(QCursor(Qt::ArrowCursor));
}
void QtMainWindow::ui_toggle_menubars(bool visible)
{
	if(visible==true)
	{
		buttonTools->show();
		fileTools->show();
		prefTools->show();
		settingsTools->show();
		aboutTool->show();
		glwindow->cursordock->show();
		glwindow->infodock->show();
	}
	else
	{
		buttonTools->hide();
		fileTools->hide();
		prefTools->hide();
		settingsTools->hide();
		aboutTool->hide();
		glwindow->cursordock->hide();
		glwindow->infodock->hide();
	}
}
void QtMainWindow::ui_toggle_orthosections(bool visible)
{
	if(visible==true)
	{
		glwindow->xydock->show();
		glwindow->xzdock->show();
		glwindow->yzdock->show();
	}
	else
	{
		glwindow->xydock->hide();
		glwindow->xzdock->hide();
		glwindow->yzdock->hide();
	}
}
void QtMainWindow::camerasettings_toggleUI_slot(bool mode)
{
	printf("camerasettings_toggleUI_slot\n");
	clipping->setEnabled(mode);
	measurepref->setEnabled(mode);
	croppref->setEnabled(mode);
	viewMenu->setEnabled(mode);
}
void QtMainWindow::about_event()
{
	QDialog* aboutdlg;
	aboutdlg = new QDialog(this, "About", FALSE);

	QPushButton* okbtn;
	okbtn = new QPushButton("OK",	0);
	connect(okbtn, SIGNAL( clicked () ), aboutdlg, SLOT( accept() ) );

	QLabel* tittle = new QLabel();
	tittle->setText("<b><big>VolViewer v3.0 rev" + svn.revision + "</b></big><br>"+"<small>by Jerome Avondo</small>");

#ifdef BIOPTONICS_SUPPORT
	tittle->setText("<b><big>BioptonicsViewer v3.0 rev" + svn.revision + "</b></big>");
#endif 

	/*QLabel* pixlbl = new QLabel();
	QPixmap* pixmap = new QPixmap(256, 128);
	pixmap->fill(Qt::black);
	pixlbl->setPixmap(*pixmap);*/

	QLabel* urls = new QLabel();
	urls->setText("Project page: <a href='http://cmpdartsvr1.cmp.uea.ac.uk/wiki/BanghamLab/index.php/VolViewer'>VolViewer</a><br>"
				  "Source code: <a href='"+svn.uRL+"'>SVN</a><br><br>"
				  "This work is licensed under the<br><a rel='license' href='http://creativecommons.org/licenses/by-nc-nd/3.0/'>Creative Commons License</a><br>"				  );

#ifdef BIOPTONICS_SUPPORT
	urls->setText("<a href='http://www.bioptonics.com/'>For support vist the Bioptonics website.</a>");
#endif 

	urls->setOpenExternalLinks(true);

	QBoxLayout* boxlayout = new QBoxLayout(QBoxLayout::TopToBottom, aboutdlg);
	boxlayout->add(tittle);
	//boxlayout->add(pixlbl);
	boxlayout->add(urls);
	boxlayout->add(okbtn);
	
	aboutdlg->show();
	
//	QMessageBox msgBox;
//	 msgBox.setText("VolViewer v2.0 rev" + svn.revision + "\n" + svn.date + "\n" + svn.uRL + "\n" + "<a href=\"http://www.such-and-such.com\">http://www.such-and-such.com</a>");
//	 msgBox.setInformativeText("by Jerome Avondo.");
//	 msgBox.exec();
}
