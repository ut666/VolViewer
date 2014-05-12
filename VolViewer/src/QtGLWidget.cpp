#include "QtGLWidget.h"
//Added by qt3to4:
#include <Q3StrList>
#include <QPixmap>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QKeyEvent>
#include <QDropEvent>
//#include <QTouchEvent>

#include "Constant.h"

QtGLWidget::QtGLWidget( QWidget* parent, const char* name, const QGLWidget* shareWidget )
    : QGLWidget( parent, name, shareWidget )
{
    startTimer( timer_interval );
	setMouseTracking (true );
	setAcceptDrops(true);
	setAutoFillBackground(false);
	grabGesture(Qt::PinchGesture);

	makeCurrent();
	
	argc_mode=0;
	depth_peal=false;
	showGUI=true;

	angleX = 0.0;

	/*QSizePolicy sp = QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding, false);
	sp.setHeightForWidth(true);
	setSizePolicy(sp);*/
}
QtGLWidget::QtGLWidget( const QGLFormat& format, QWidget* parent, const char* name, 
	      const QGLWidget* shareWidget )
    : QGLWidget( format, parent, name, shareWidget )
{
    startTimer( timer_interval );
	setMouseTracking (true );
	setAcceptDrops(true);
	setAutoFillBackground(false);

	//setAttribute(Qt::WA_AcceptTouchEvents);
	grabGesture(Qt::PinchGesture);
	makeCurrent();
	
	argc_mode=0;
	depth_peal=false;
	showGUI=true;

	angleX = 0.0;

	/*QSizePolicy sp = QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding, false);
	sp.setHeightForWidth(true);
	setSizePolicy(sp);*/
}
QtGLWidget::~QtGLWidget()
{
}

//===========================================================================================================

void QtGLWidget::initvariables()
{
	active = OBJ_ANGLE;
	planes_toggle = 0;

	maxflow_mode=-1;

	headingangle = pitchangle = 0.0;

/*	OPT_objective = 0.0;
	OPT_magnification = 0.0;
	OPT_voxelspermicron = 6.629355427;
*/
	volio.volobj = volrender.volobj = &volobject;
	volrender.volio = &volio;
	
	dw = 1.0;
	
	render = 1;
	
	currentworkingdir = "";

	serifFontBG = QFont("Arial", 24, QFont::Bold);
	serifFont = QFont("Arial", 22, QFont::Bold);
	serifFontLabel = QFont("Arial", 10, QFont::Bold);
}
#ifdef OPENCL_SUPPORT
void QtGLWidget::updateCL_UI(void)
{
	//printf("BOH\n");
	emit rendersettings_setopenclplat_signal(ocl_wrap.platform_index, ocl_wrap.platform_list);
}
void QtGLWidget::intializeCL(void)
{
	ocl_wrap.init_PlatformAndDevices();
	//ocl_wrap.create_NewMultiDeviceContext();
	ocl_wrap.create_NewContext();
	//ocl_wrap.create_SharedContext();
	
	QString kernel_fn;

	ocl_gaussian.ocl_wrapper = &ocl_wrap;
	ocl_gaussian.volobj = &volobject;
	kernel_fn = volrender.appPath+"/Kernels/gaussian.cl";
	ocl_gaussian.ocl_init_progam(kernel_fn.latin1());

	ocl_dilate.ocl_wrapper = &ocl_wrap;
	ocl_dilate.volobj = &volobject;
	kernel_fn = volrender.appPath+"/Kernels/dilate.cl";
	ocl_dilate.ocl_init_progam(kernel_fn.latin1());

	ocl_erode.ocl_wrapper = &ocl_wrap;
	ocl_erode.volobj = &volobject;
	kernel_fn = volrender.appPath+"/Kernels/erode.cl";
	ocl_erode.ocl_init_progam(kernel_fn.latin1());

	ocl_median.ocl_wrapper = &ocl_wrap;
	ocl_median.volobj = &volobject;
	kernel_fn = volrender.appPath+"/Kernels/median.cl";
	ocl_median.ocl_init_progam(kernel_fn.latin1());

	ocl_peronamalik.ocl_wrapper = &ocl_wrap;
	ocl_peronamalik.volobj = &volobject;
	kernel_fn = volrender.appPath+"/Kernels/anisotropic.cl";
	ocl_peronamalik.ocl_init_progam(kernel_fn.latin1());
	
	ocl_boundingbox.ocl_wrapper = &ocl_wrap;
	ocl_boundingbox.volobj = &volobject;
	kernel_fn = volrender.appPath+"/Kernels/boundingbox.cl";
	ocl_boundingbox.ocl_init_progam(kernel_fn.latin1());
	
	ocl_reduction.ocl_wrapper = &ocl_wrap;
	ocl_reduction.volobj = &volobject;
	kernel_fn = volrender.appPath+"/Kernels/reduction.cl";
	ocl_reduction.ocl_init_progam(kernel_fn.latin1());

	ocl_bilateral.ocl_wrapper = &ocl_wrap;
	ocl_bilateral.volobj = &volobject;
	kernel_fn = volrender.appPath+"/Kernels/bilateral.cl";
	ocl_bilateral.ocl_init_progam(kernel_fn.latin1());
	
	//ocl_exec.ocl_init_progam("Kernels\\kernel_anisotropic.cl");
	//ocl_exec.ocl_init_progam("Kernels\\kernel_dilate.cl");
	//	ocl_exec.ocl_init_progam("Kernels\\kernel_median.cl");
	//ocl_exec.ocl_init_progam("Kernels\\kernel_median_shared.cl");
}
#endif
void QtGLWidget::initializeGL()
{
	/*QGLFormat fmt;
	fmt.setDoubleBuffer(true);
	fmt.setStereo(false);
	fmt.setDirectRendering(true);
	fmt.setDepth(true);
	fmt.setAlpha(true);

	QGLFormat::setDefaultFormat(fmt);
*/
	//setAutoBufferSwap(false);
	//setDoubleBuffer(true);
	
	initvariables();
	
	//glClearColor( volrender.background_colour.red()/255.0, volrender.background_colour.green()/255.0, volrender.background_colour.blue()/255.0, 1.0f );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	volrender.initializeGL();

	//if(isValid()==true) cout<<"GLWIDGET is valid\n";
	//else cout<<"GLWIDGET creation failed!\n";

#ifdef OPENCL_SUPPORT
	intializeCL();
#endif

	if(argc_mode==1)
	{
		argc_mode=0;
		emit arc_mode_signal();
	}
}
void QtGLWidget::redraw_event(void)
{	
	//printf("Redraw Event\n");
	render=1;

	//makeCurrent();
	//makeOverlayCurrent();

	updateGL();
	update();
	swapBuffers();
	render=0;
}
int QtGLWidget::heightForWidth ( int w )
{
	return w;
}
void QtGLWidget::resizeGL( int w, int h )
{
/*	int maxsize=0;
	if(maxsize<w) maxsize = w;
	if(maxsize<h) maxsize = h;
*/
/*	glViewport(0,0,width(),height());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
	glMatrixMode(GL_MODELVIEW);
*/	
	volrender.resizeGL(w,h);

	//makeOverlayCurrent();
	//updateGL();
	
 	redraw_event();
}
void QtGLWidget::paintGL()
{
	if(depth_peal) 
	{
		//Turn OFF the peal
		volrender.detphpeal=false;
		volrender.vol_shaderobj.enable();
		volrender.vol_shaderobj.sendUniform1i("depthpeal", (int)volrender.detphpeal);
		volrender.vol_shaderobj.sendUniform1f("fbosize", volrender.FBO_SIZE);	
		volrender.vol_shaderobj.disable();

			//------------------------------------------------
			//Get the depth texture (z-buffer)
			//------------------------------------------------
			volrender.measure_toggle = 0;
			volrender.offscreen_toggle = 1;

			volrender.displayGL(render);

			glBindTexture(GL_TEXTURE_2D, volrender.TEXTURE2D_FBO_DP);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, volrender.pealdepthdata);
			
			glBindTexture(GL_TEXTURE_2D, volrender.TEXTURE2D_FBO_FB);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, volrender.coldata);
			/*
			unsigned char* tempimgdata = new unsigned char[4*volrender.FBO_SIZE*volrender.FBO_SIZE];
			for(int i=0; i<volrender.FBO_SIZE; i++)
			for(int j=0; j<volrender.FBO_SIZE; j++)
			{
				tempimgdata[4*(i+j*volrender.FBO_SIZE)+0] = 
				tempimgdata[4*(i+j*volrender.FBO_SIZE)+1] = 
				tempimgdata[4*(i+j*volrender.FBO_SIZE)+2] = 255.0*(volrender.pealdepthdata[i+j*volrender.FBO_SIZE]/4294967296.0);
				tempimgdata[4*(i+j*volrender.FBO_SIZE)+3] = 255;
			}
	
			QImage dstimg = QImage(tempimgdata, volrender.FBO_SIZE, volrender.FBO_SIZE, QImage::Format_ARGB32);
			dstimg.save("Depth.png", "PNG");
	
			delete[] tempimgdata;
			tempimgdata = 0;
		
			dstimg = QImage(volrender.coldata, volrender.FBO_SIZE, volrender.FBO_SIZE, QImage::Format_ARGB32);
			dstimg.save("Colour.png", "PNG");*/
		
			volrender.measure_toggle = 1;
			volrender.offscreen_toggle = 0;

			//printf("%f, %f\n", volrender.olddepthoffset, volrender.olddepththickness);
			//printf("%f, %f\n", volrender.depthoffset, volrender.depththickness);

		glBindTexture (GL_TEXTURE_2D, volrender.TEXTURE2D_FBO_DEPTH); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, volrender.FBO_SIZE, volrender.FBO_SIZE, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, volrender.pealdepthdata);
		
		glBindTexture (GL_TEXTURE_2D, volrender.TEXTURE2D_FBO_COL); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, volrender.FBO_SIZE, volrender.FBO_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, volrender.coldata);
/*
		//Turn OFF the peal
		volrender.vol_shaderobj.enable();
		volrender.vol_shaderobj.sendUniform1f("depthpeal", true);
		volrender.vol_shaderobj.sendUniform1f("fbosize", volrender.FBO_SIZE);	
		volrender.vol_shaderobj.sendUniform1f("depthoffset", volrender.olddepthoffset);
		volrender.vol_shaderobj.sendUniform1f("depththickness", volrender.olddepththickness);	
		volrender.vol_shaderobj.disable();

			//------------------------------------------------
			//Get the depth texture (z-buffer)
			//------------------------------------------------
			volrender.measure_toggle = 0;
			volrender.offscreen_toggle = 1;

			volrender.displayGL(render);

			glBindTexture(GL_TEXTURE_2D, volrender.TEXTURE2D_FBO_DP);
			glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, volrender.pealdepthdata);

			volrender.measure_toggle = 1;
			volrender.offscreen_toggle = 0;
			*/
			/*tempimgdata = new unsigned char[4*volrender.FBO_SIZE*volrender.FBO_SIZE];
			for(int i=0; i<volrender.FBO_SIZE; i++)
			for(int j=0; j<volrender.FBO_SIZE; j++)
			{
				tempimgdata[4*(i+j*volrender.FBO_SIZE)+0] = 
				tempimgdata[4*(i+j*volrender.FBO_SIZE)+1] = 
				tempimgdata[4*(i+j*volrender.FBO_SIZE)+2] = 255.0*(volrender.pealdepthdata[i+j*volrender.FBO_SIZE]/4294967296.0);
				tempimgdata[4*(i+j*volrender.FBO_SIZE)+3] = 255;
			}
	
			dstimg = QImage(tempimgdata, volrender.FBO_SIZE, volrender.FBO_SIZE, QImage::Format_ARGB32);
			dstimg.save("Depth2.png", "PNG");
	
			delete[] tempimgdata;
			tempimgdata = 0;*/

	//	printf("FBOSIZE: %d\n", volrender.FBO_SIZE);
	//	printf("viewport : %d %d\n", width(), height());
	
		//captureDepthMap();
/*
		glBindTexture (GL_TEXTURE_2D, volrender.TEXTURE2D_FBO_DEPTH); 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, volrender.FBO_SIZE, volrender.FBO_SIZE, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, volrender.pealdepthdata);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	*/
	/*	unsigned char* tempimgdata = new unsigned char[4*volrender.FBO_SIZE*volrender.FBO_SIZE];
		for(int i=0; i<volrender.FBO_SIZE; i++)
		for(int j=0; j<volrender.FBO_SIZE; j++)
		{
			tempimgdata[4*(i+j*volrender.FBO_SIZE)+0] = 
			tempimgdata[4*(i+j*volrender.FBO_SIZE)+1] = 
			tempimgdata[4*(i+j*volrender.FBO_SIZE)+2] = 255.0*(volrender.pealdepthdata[i+j*volrender.FBO_SIZE]/4294967296.0);
			tempimgdata[4*(i+j*volrender.FBO_SIZE)+3] = 255;
		}
	
		QImage dstimg = QImage(tempimgdata, volrender.FBO_SIZE, volrender.FBO_SIZE, QImage::Format_ARGB32);
		dstimg.save("Depth.png", "PNG");
	
		delete[] tempimgdata;
		tempimgdata = 0;
		*/

		//Turn ON the peal
		volrender.detphpeal=true;
		volrender.vol_shaderobj.enable();
		volrender.vol_shaderobj.sendUniform1i("depthpeal", (int)volrender.detphpeal);
		volrender.vol_shaderobj.sendUniform1f("depthoffset", volrender.depthoffset);
		volrender.vol_shaderobj.sendUniform1f("depththickness", volrender.depththickness);	
		//volrender.vol_shaderobj.sendUniform1i("normalTexture", 6);
		volrender.vol_shaderobj.disable();
	}
	
	//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	//if(render)
	{
		if(volrender.fps_toggle)volrender.timer.startFPS();
		volrender.displayGL(render);
		if(volrender.fps_toggle) fpsval = volrender.timer.endFPS();
		//glFinish();

		if(volrender.offscreen_toggle==0)
		{
		//now render some 2D UI non FBO stuff.
		glPushMatrix();

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			
			//glOrtho(-volobject.maxres-volrender.volrenderview.zoom, volobject.maxres+volrender.volrenderview.zoom, -volobject.maxres-volrender.volrenderview.zoom, volobject.maxres+volrender.volrenderview.zoom, -1024, 1024);
			glOrtho(-volobject.maxres, volobject.maxres, -volobject.maxres, volobject.maxres, -1024, 1024);

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			//glEnable(GL_BLEND);
			glDisable(GL_LIGHTING);

				//================================
				// SCALEBAR
				//================================
				if(volrender.scalebar_toggle)
				{
					glColor3f(volrender.background_fontcolour.red()/255.0, volrender.background_fontcolour.blue()/255.0, volrender.background_fontcolour.green()/255.0);
					renderText(volrender.scalebarfontpos.x, volrender.scalebarfontpos.y, volrender.scalebarfontpos.z, volrender.scalebartext.latin1(), serifFontBG);
					volrender.render_scalebar();
				}
	
				//================================
				// FPS
				//================================
				if(volrender.fps_toggle)
				{
					QString fps;
					fps.setNum(fpsval, 'g', 2);
					fps = "FPS: " + fps;
					qglColor(volrender.background_fontcolour);
					renderText(14,28, fps.latin1(), serifFontBG);
				}
				//================================
				// VERT INDICES
				//================================
				if(volobject.texture3d!=NULL && volrender.measure_vertlabel==1)
				{
					QString numb;
					for(int i=0; i<volrender.currentMeasureObject.vertices.size(); i++)
					{
						Vector v0 = volrender.currentMeasureObject.vertices[i];
						float vertZ; 
						Vector scene;
						unprojectDepth(v0, &vertZ, &scene);

						if(1.0-vertZ>=scene.z-0.0005)
						{
							numb.setNum(i);
							QString txt = "p"+numb;
							qglColor(volrender.background_fontcolour);
							renderTextBox(scene.x, scene.y, txt, serifFontLabel, 0);
							renderText(scene.x, scene.y, txt, serifFontLabel);
						}
					}
				}
				//================================
				// EDGE INDICES
				//================================
				if(volobject.texture3d!=NULL && volrender.measure_edgelabel==1)
				{
					QString numb;
					for(int i=0; i<volrender.currentMeasureObject.edges.size(); i++)
					{
						double dpX, dpY, dpZ, rX, rY, rZ, ddpx, ddpY, ddpZ;

						Vector v1 = volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.edges[i].v0];
						Vector v2 = volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.edges[i].v1];
						Vector v0 = (v1+v2)*0.5f;
						
						float vertZ; 
						Vector scene;
						unprojectDepth(v0, &vertZ, &scene);

						if(1.0-vertZ>=scene.z-0.0005)
						{
							numb.setNum(i);
							QString txt = "e"+numb;
							qglColor(volrender.background_fontcolour);
							renderTextBox(scene.x, scene.y, txt, serifFontLabel, 0);
							renderText(scene.x, scene.y, txt, serifFontLabel);
						}
					}
				}
				//================================
				// FACE INDICES
				//================================
				if(volobject.texture3d!=NULL && volrender.measure_facelabel==1)
				{
					QString numb;
					for(int i=0; i<volrender.currentMeasureObject.faces.size(); i++)
					{
						double dpX, dpY, dpZ, rX, rY, rZ, ddpx, ddpY, ddpZ;

						Vector v1;
						for(int ii=0; ii<volrender.currentMeasureObject.faces[i].size(); ii++)
						{
							v1 += volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[i][ii]];
						}

						Vector v0 = (v1)/(float)volrender.currentMeasureObject.faces[i].size();
						float vertZ; 
						Vector scene;
						unprojectDepth(v0, &vertZ, &scene);

						if(1.0-vertZ>=scene.z-0.0005)
						{
							numb.setNum(i);
							QString txt = "f"+numb;
							qglColor(volrender.background_fontcolour);
							renderTextBox(scene.x, scene.y, txt, serifFontLabel, 0);
							renderText(scene.x, scene.y, txt, serifFontLabel);
						}
					}
				}
				//================================
				// VERT LABELS
				//================================
				if(volobject.texture3d!=NULL && volrender.measure_labelslabel==1) //  				
				{
					if(!volrender.currentMeasureObject.vertices.empty() || volrender.measure_mode==1)
					{
						if(volrender.currentMeasureObject.vertices.size()==volrender.currentMeasureObject.vertex_labels.size())
						{
							QString numb;
							for(int i=0; i<volrender.currentMeasureObject.vertex_labels.size(); i++)
							{
								Vector v0 = volrender.currentMeasureObject.vertices[i];

								float vertZ; 
								Vector scene;
								unprojectDepth(v0, &vertZ, &scene);

								if(1.0-vertZ>=scene.z-0.0005)
								{
									for(int ii=0; ii<volrender.currentMeasureObject.vertex_labels[i].size(); ii++)
									{
										QString txt = QString::fromStdString(volrender.currentMeasureObject.vertex_labels[i][ii]);							
										renderTextBox(scene.x, scene.y, txt, serifFontLabel, 0);
										renderText(scene.x, scene.y, txt, serifFontLabel);
									}
								}
							}
						}
						//================================
						// EDGE LABELS
						//================================
						if(volrender.currentMeasureObject.edges.size()==volrender.currentMeasureObject.edge_labels.size())
						{
							for(int i=0; i<volrender.currentMeasureObject.edge_labels.size(); i++)
							{
								Vector v1 = volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.edges[i].v0];
								Vector v2 = volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.edges[i].v1];
								Vector v0 = (v1+v2)*0.5f;
								float vertZ; 
								Vector scene;
								unprojectDepth(v0, &vertZ, &scene);

								if(1.0-vertZ>=scene.z-0.0005)
								{
									for(int ii=0; ii<volrender.currentMeasureObject.edge_labels[i].size(); ii++)
									{
										QString txt = QString::fromStdString(volrender.currentMeasureObject.edge_labels[i][ii]);
										qglColor(volrender.background_fontcolour);
										renderTextBox(scene.x, scene.y, txt, serifFontLabel, 0);
										renderText(scene.x, scene.y, txt, serifFontLabel);
									}
								}
							}
						}
						//================================
						// FACE LABELS
						//================================
						if(volrender.currentMeasureObject.faces.size()==volrender.currentMeasureObject.face_labels.size())
						{
							for(int i=0; i<volrender.currentMeasureObject.faces.size(); i++)
							{
								Vector v1;
								for(int ii=0; ii<volrender.currentMeasureObject.faces[i].size(); ii++)
								{
									v1 += volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[i][ii]];
								}

								Vector v0 = (v1)/(float)volrender.currentMeasureObject.faces[i].size();
								float vertZ; 
								Vector scene;
								unprojectDepth(v0, &vertZ, &scene);

								if(1.0-vertZ>=scene.z-0.0005)
								{
									for(int ii=0; ii<volrender.currentMeasureObject.face_labels[i].size(); ii++)
									{
										QString txt = QString::fromStdString(volrender.currentMeasureObject.face_labels[i][ii]);
										qglColor(volrender.background_fontcolour);
										renderTextBox(scene.x, scene.y, txt, serifFontLabel, 0);
										renderText(scene.x, scene.y, txt, serifFontLabel);
									}
								}
							}
						}
					}
					else
					{
						QString numb;
						for(int i=0; i<volrender.measures.measure_objects.size(); i++)
						{
							if(volrender.measures.visible[i])
							{

								//================================================
								//VERTS
								//================================================
								for(int j=0; j<volrender.measures.measure_objects[i].vertex_labels.size(); j++)
								{
									if(j<volrender.measures.measure_objects[i].vertices.size())
									{
										Vector v0 = volrender.measures.measure_objects[i].vertices[j];

										float vertZ; 
										Vector scene;
										unprojectDepth(v0, &vertZ, &scene);

										if(1.0-vertZ>=scene.z-0.0005)
										{
											for(int ii=0; ii<volrender.measures.measure_objects[i].vertex_labels[j].size(); ii++)
											{
												QString txt = QString::fromStdString(volrender.measures.measure_objects[i].vertex_labels[j][ii]);							
												renderTextBox(scene.x, scene.y, txt, serifFontLabel, 0);
												renderText(scene.x, scene.y, txt, serifFontLabel);
											}
										}
									}
								}

								//================================================
								//EDGES
								//================================================
								for(int j=0; j<volrender.measures.measure_objects[i].edge_labels.size(); j++)
								{
									if(j<volrender.measures.measure_objects[i].edges.size())
									{
										Vector v1 = volrender.measures.measure_objects[i].vertices[volrender.measures.measure_objects[i].edges[j].v0];
										Vector v2 = volrender.measures.measure_objects[i].vertices[volrender.measures.measure_objects[i].edges[j].v1];
										Vector v0 = (v1+v2)*0.5f;
										float vertZ; 
										Vector scene;
										unprojectDepth(v0, &vertZ, &scene);

										if(1.0-vertZ>=scene.z-0.0005)
										{
											for(int ii=0; ii<volrender.measures.measure_objects[i].edge_labels[j].size(); ii++)
											{
												QString txt = QString::fromStdString(volrender.measures.measure_objects[i].edge_labels[j][ii]);
												qglColor(volrender.background_fontcolour);
												renderTextBox(scene.x, scene.y, txt, serifFontLabel, 0);
												renderText(scene.x, scene.y, txt, serifFontLabel);
											}
										}
									}
								}
								//================================================
								//FACES
								//================================================
								for(int j=0; j<volrender.measures.measure_objects[i].face_labels.size(); j++)
								{
									if(j<volrender.measures.measure_objects[i].faces.size())
									{
										Vector v1;
										for(int ii=0; ii<volrender.measures.measure_objects[i].faces[j].size(); ii++)
										{
											v1 += volrender.measures.measure_objects[i].vertices[volrender.measures.measure_objects[i].faces[j][ii]];
										}

										Vector v0 = (v1)/(float)volrender.measures.measure_objects[i].faces[j].size();
										float vertZ; 
										Vector scene;
										unprojectDepth(v0, &vertZ, &scene);

										if(1.0-vertZ>=scene.z-0.0005)
										{
											for(int ii=0; ii<volrender.measures.measure_objects[i].face_labels[j].size(); ii++)
											{
												QString txt = QString::fromStdString(volrender.measures.measure_objects[i].face_labels[j][ii]);
												qglColor(volrender.background_fontcolour);
												renderTextBox(scene.x, scene.y, txt, serifFontLabel, 0);
												renderText(scene.x, scene.y, txt, serifFontLabel);
											}
										}
									}
								}
							}
						}
					}
				}
				

/*
				QPainter painter(this);
				if(volobject.texture3d!=NULL && volrender.measure_lengthlabel==1)
				{
					Vector res = Vector((volobject.xsize/volobject.maxsize), (volobject.ysize/volobject.maxsize), (volobject.zsize/volobject.maxsize));
					QString numb;
					for(int i=0; i<volrender.currentMeasureObject.edges.size(); i++)
					{
						Vector v1 = volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.edges[i].v0];
						Vector v2 = volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.edges[i].v1];
						Vector v3 = v2;
						Vector v4 = v1-(v1+Vector(0,10,0));

						Vector v5 = v2-v1;
						
						v5.x *= volobject.xscale;
						v5.y *= volobject.yscale;
						v5.z *= volobject.zscale;

						v5.x /= res.x;
						v5.y /= res.y;
						v5.z /= res.z;
						
						v5.x *= volobject.x_resize;
						v5.y *= volobject.y_resize;
						v5.z *= volobject.z_resize;	

						float l = v5.length();

						Vector pos = (v1+v2)/2.0f;

						double dpX, dpY, dpZ, rZ;
						gluProject((double)pos.x,(double)pos.y, (double)pos.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dpX, &dpY, &dpZ);					
						double offset = volrender.volrenderview.winWidth-volrender.volrenderview.winHeight;
						offset /= 2.0;
						dpY = volrender.volrenderview.winWidth-(dpY+offset);

						double d0X, d0Y, d0Z;
						gluProject((double)v3.x,(double)v3.y, (double)v3.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &d0X, &d0Y, &d0Z);					
						d0Y = volrender.volrenderview.winWidth-(d0Y+offset);

						double d1X, d1Y, d1Z;
						gluProject((double)v4.x,(double)v4.y, (double)v4.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &d1X, &d1Y, &d1Z);					
						d1Y = volrender.volrenderview.winWidth-(d1Y+offset);

						v3 = Vector(d0X, d0Y, 0.0);
						v4 = Vector(d1X, d1Y, 0.0);

						//printf("d: %f, %f %f\n", dpX, dpY, dpZ);
						//printf("d0: %f, %f %f\n", d0X, d0Y, d0Z);
						//printf("d1: %f, %f %f\n", d1X, d1Y, d1Z);

						numb.setNum(l, 'f', 2);
									
						fmetric = QFontMetrics(serifFontLabel);
						QRect rect = fmetric.boundingRect(numb);
						painter.setFont(serifFontLabel);
						painter.setPen(Qt::red);

						Vector vi = Vector(dpX, dpY, 0)-Vector(d0X, d0Y, 0);
						Vector vj = Vector(dpX, dpY, 0)-Vector(dpX+64, dpY, 0);

						float perpdot = vi.x*vj.y-vi.y*vj.x;
						float dot = vi.x*vj.x-vi.y*vj.y;
						float angs = atan2(perpdot, dot)* (180.0/PI);


						painter.translate(dpX, dpY);
						painter.rotate(-angs);

						//rect.setWidth(rect.width());
						//rect.setHeight(rect.height());
						//rect.translate(-rect.width()*0.5, 0.0);
						//volrender.background_colour.setAlpha(32);
						//painter.fillRect(rect, volrender.background_colour);
						//volrender.background_colour.setAlpha(255);
						painter.drawText(QPoint(-rect.width()*0.5, -rect.height()*0.5), numb);
						painter.resetTransform();											
					}
				}
				if(volobject.texture3d!=NULL && volrender.measure_anglelabel==1)
				{
					QString numb;
					for(int i=0; i<volrender.currentMeasureObject.edges.size(); i++)
					{
						int vi_index1 = volrender.currentMeasureObject.edges[i].v0;
						int vi_index2 = volrender.currentMeasureObject.edges[i].v1;

						Vector vi1 = volrender.currentMeasureObject.vertices[vi_index1];
						Vector vi2 = volrender.currentMeasureObject.vertices[vi_index2];
						Vector vi3 = vi1-vi2;

						for(int j=0; j<volrender.currentMeasureObject.edges.size(); j++)
						{
							int vj_index1 = volrender.currentMeasureObject.edges[j].v0;
							int vj_index2 = volrender.currentMeasureObject.edges[j].v1;

							if(vi_index1==vj_index2)
							{
								Vector vj1 = volrender.currentMeasureObject.vertices[vj_index1];
								Vector vj2 = volrender.currentMeasureObject.vertices[vj_index2];
								Vector vj3 = vj1-vj2;

								Vector norm;
								norm.get_normal(vi1, vi2, vj1);
								float angle = vi3.signed_angle(vj3, norm) * (180.0/PI);

								Vector pos = (vi1+vi2+vj1+vj2)*0.25f;
								double dpX, dpY, dpZ, rZ;
								gluProject((double)pos.x,(double)pos.y, (double)pos.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dpX, &dpY, &dpZ);					
								double offset = volrender.volrenderview.winWidth-volrender.volrenderview.winHeight;
								offset /= 2.0;
								dpY = volrender.volrenderview.winWidth-(dpY+offset);

								if(angle<0) angle = -angle;

								if(angle!=0)
								{
									numb.setNum(angle, 'f', 2);
								//	renderText(dX, dY, numb, serifFontLabel);

									fmetric = QFontMetrics(serifFontLabel);

									painter.setPen(Qt::green);
									painter.setFont(serifFontLabel);

									QRect rect = fmetric.boundingRect(numb);
									painter.translate(dpX-((rect.width()*0.125)), dpY-((rect.height()*0.125)));
									rect.setWidth(rect.width()*1.25);
									rect.setHeight(rect.height()*1.25);
									//painter.drawRect(rect);
									volrender.background_colour.setAlpha(32);
									painter.fillRect(rect, volrender.background_colour);
									volrender.background_colour.setAlpha(255);
									painter.resetTransform();
									painter.drawText(QPoint(dpX, dpY), numb);
									
								}
							}
							else if(vi_index1==vj_index1)
							{
								Vector vj1 = volrender.currentMeasureObject.vertices[vj_index1];
								Vector vj2 = volrender.currentMeasureObject.vertices[vj_index2];
								Vector vj3 = vj1-vj2;

								Vector norm;
								norm.get_normal(vi1, vi2, vj2);
								float angle = vi3.signed_angle(vj3, norm) * (180.0/PI);

								Vector pos = (vi1+vi2+vj1+vj2)*0.25f;
								double dX, dY, dZ, rZ;
								gluProject((double)pos.x,(double)pos.y, (double)pos.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);					
								double offset = volrender.volrenderview.winWidth-volrender.volrenderview.winHeight;
								offset /= 2.0;
								dY = volrender.volrenderview.winWidth-(dY+offset);

								if(angle<0) angle = -angle;

								if(angle!=0)
								{
									numb.setNum(angle, 'f', 2);
								//	renderText(dX, dY, numb, serifFontLabel);

									fmetric = QFontMetrics(serifFontLabel);

									painter.setPen(Qt::green);
									painter.setFont(serifFontLabel);

									QRect rect = fmetric.boundingRect(numb);
									painter.translate(dX-((rect.width()*0.125)), dY-((rect.height()*0.125)));
									rect.setWidth(rect.width()*1.25);
									rect.setHeight(rect.height()*1.25);
									//painter.drawRect(rect);
									volrender.background_colour.setAlpha(32);
									painter.fillRect(rect, volrender.background_colour);
									volrender.background_colour.setAlpha(255);
									painter.resetTransform();
									painter.drawText(QPoint(dX, dY), numb);
								}
							}	
						}
					}
				}
				painter.end();
*/
			glColor3f(1,1,1);
			//glDisable(GL_BLEND);
			glDisable(GL_LIGHTING);

			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			


		glPopMatrix();
		}
		
	}
}

void QtGLWidget::renderTextBox(int x, int y, QString txt, QFont font, int offset)
{
	QFontMetrics fm(font);
	QRect rect = fm.boundingRect(txt);
	//printf("rect: %d %d\n", rect.size().width(), rect.size().height());
	
	int w = (float)rect.size().width()*1.2;
	int h = (float)rect.size().height()*1.2;

	x -= (float)rect.size().width()*0.1;
	y -= (float)rect.size().height()*0.1;

	glPushMatrix();
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, width(), 0, height(), -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glEnable(GL_BLEND);
	glColor4f(volrender.background_colour.red()/255.0,volrender.background_colour.green()/255.0, volrender.background_colour.blue()/255.0, 0.5);
	glBegin(GL_QUADS);
	glVertex2f(x, height()-y-(offset*20)-h*0.5);
	glVertex2f(x+w, height()-y-(offset*20)-h*0.5);
	glVertex2f(x+w, height()-y-(offset*20)+h*0.75);
	glVertex2f(x, height()-y-(offset*20)+h*0.75);
	glEnd();
	glDisable(GL_BLEND);

	qglColor(volrender.background_fontcolour);
	glBegin(GL_LINE_LOOP);
	glVertex2f(x, height()-y-(offset*20)-h*0.5);
	glVertex2f(x+w, height()-y-(offset*20)-h*0.5);
	glVertex2f(x+w, height()-y-(offset*20)+h*0.75);
	glVertex2f(x, height()-y-(offset*20)+h*0.75);
	glEnd();

	glPopMatrix();
}
void QtGLWidget::keyPressEvent ( QKeyEvent * e)
{
	Matrix4fT mat;
	mat = volrender.volrenderview.arcball.Transform;
	mat = Matrix4fInverse(&volrender.volrenderview.arcball.Transform);
	
	Vector v = Vector(0,0,-volrender.volrenderview.zoom);
	//v = v.mult_matrix(mat.M);
	//v = v.mult_matrix(volrender.volrenderview.arcball.Transform.M);

	Vector trans, dir;
	switch (e->key() ) 
	{

		case Qt::Key_Right:
			volrender.volrenderview.move_left(-volrender.cam_step_size);
			break;
		case Qt::Key_Left:
			volrender.volrenderview.move_left(volrender.cam_step_size);
			break;
		case Qt::Key_Up:
			volrender.volrenderview.move_forward(volrender.cam_step_size);
			break;
		case Qt::Key_Down:
			volrender.volrenderview.move_forward(-volrender.cam_step_size);
			break;
		case Qt::Key_PageUp:
			volrender.volrenderview.move_up(volrender.cam_step_size);
			break;
		case Qt::Key_PageDown:
			volrender.volrenderview.move_up(-volrender.cam_step_size);
			break;
		case Qt::Key_Delete:
			volrender.volrenderview.rotateZ(5.0);	
			break;
		case Qt::Key_End:
			volrender.volrenderview.rotateZ(-5.0);	
			break;
		case Qt::Key_G:
			volrender.brick ++;
			if(volrender.brick>pow((float)volrender.BRICKS_SUBDIV, 3)) volrender.brick = pow((float)volrender.BRICKS_SUBDIV, 3);
            break;
		case Qt::Key_B:
			volrender.brick --;
			if(volrender.brick<0) volrender.brick=0;
            break;

		default:
			break;
    }

//	printf("translate: %f, %f, %f\n", volrender.volrenderview.at.x, volrender.volrenderview.at.y, volrender.volrenderview.at.z);
	volrender.volshader_update_eye();
	redraw_event();
	updateGL();
}
void QtGLWidget::keyReleaseEvent ( QKeyEvent * e)
{

}
bool QtGLWidget::gestureEvent(QGestureEvent *event)
{
	if (QGesture *swipe = event->gesture(Qt::SwipeGesture))
	{
		swipeTriggered(static_cast<QSwipeGesture *>(swipe));
	}
	else if (QGesture *pan = event->gesture(Qt::PanGesture))
	{
		panTriggered(static_cast<QPanGesture *>(pan));
	}
	if (QGesture *pinch = event->gesture(Qt::PinchGesture))
	{
		pinchTriggered(static_cast<QPinchGesture *>(pinch));
	}
	
	return QGLWidget::event(event);
 }
bool QtGLWidget::event(QEvent* e)
{
	if (e->type() == QEvent::Gesture)
	{
		return gestureEvent(static_cast<QGestureEvent*>(e));
	}
	
	return QGLWidget::event(e);
	
	/*switch (e->type()) 
	{ 
		case QEvent::TouchBegin: 
		case QEvent::TouchUpdate: 
		case QEvent::TouchEnd: 
		{ 
			// creative stuff here
			QTouchEvent *te = static_cast<QTouchEvent *>(e); 
			QList<QTouchEvent::TouchPoint> touchPoints = te->touchPoints(); 
			
			if (touchPoints.count() == 2) 
			{ 
				// Do something interesting here 
				
				// determine scale factor
				QTouchEvent::TouchPoint p0 = touchPoints.first(); 
				QTouchEvent::TouchPoint p1 = touchPoints.last(); 
				
				QLineF line1(p0.startPos(), p1.startPos()); 
				QLineF line2(p0.pos(), p1.pos()); 
				
				float scaleFactor = line2.length() - line1.length();
				scaleFactor *= 0.25;
				
				//cout<<"scaleFactor: "<<scaleFactor<<endl;

				volrender.volrenderview.zoom -= scaleFactor;								
				redraw_event();
				return QGLWidget::event(e);			
			}
			else
			{
				return QGLWidget::event(e);			
			}
			
			break;
		}
		default:
			return QGLWidget::event(e);
	}*/
	
	//return true;
}
void QtGLWidget::panTriggered(QPanGesture*)
{
	redraw_event();
}
void QtGLWidget::pinchTriggered(QPinchGesture* gesture)
{
    QPinchGesture::ChangeFlags changeFlags = gesture->changeFlags();
	
	if (changeFlags & QPinchGesture::RotationAngleChanged) 
	{
		qreal value = gesture->property("rotationAngle").toReal();
		qreal lastValue = gesture->property("lastRotationAngle").toReal();
	//	rotationAngle += value - lastValue;
	}
	if (changeFlags & QPinchGesture::ScaleFactorChanged) 
	{
		qreal value = gesture->property("scaleFactor").toReal();
		
		volrender.volrenderview.zoom = zoomstart * ((value)*2.0f);
		cout<<"pinch zoom: "<< (float)volrender.volrenderview.zoom<<endl;		
	}
	
	if (gesture->state() == Qt::GestureStarted) 
	{
		cout<<"pinch start"<<endl;
		zoomstart = volrender.volrenderview.zoom;
	}
	else if (gesture->state() == Qt::GestureFinished) 
	{
		cout<<"pinch end"<<endl;
		zoomstart = (float) volrender.volrenderview.zoom;
		
		cout<<"zoomstart: "<<zoomstart<<endl;
		
		//		scaleFactor *= currentStepScaleFactor;
		//		currentStepScaleFactor = 1;
	}
		
	redraw_event();
}
void QtGLWidget::swipeTriggered(QSwipeGesture* gesture)
{
	if (gesture->state() == Qt::GestureFinished) 
	{
		if (gesture->horizontalDirection() == QSwipeGesture::Left
			|| gesture->verticalDirection() == QSwipeGesture::Up)
		{
		}
		else
		{
		}
		
		redraw_event();
	}
	
}

void QtGLWidget::wheelEvent(QWheelEvent *event)
{
	int numDegrees = event->delta() / (8);
	int numSteps = numDegrees / 15;

	if (event->orientation() == Qt::Horizontal) 
	{

	} 
	else if (event->orientation() == Qt::Vertical) 
	{
		dx = numSteps;
		dy = numSteps;

		int selected=-1;
		for(int i=0; i<6; i++)
		{
			if(volrender.volrenderview.cutting_planes.selected[i]==1) selected = i;
		}

		if(selected==-1)
		{

			if(volrender.measure_editmode==1 || volrender.measure_modify==1)
			{
				volrender.volrenderview.cutting_planes.selected[0]=1;
				render = 1;			
				active=CUTTING;
				cutplane_interact(-dy/dw);
				active=-1;
				volrender.volrenderview.cutting_planes.selected[0]=0;
			}
			else
			{
				volrender.volrenderview.zoom -= dy;
//				if(volrender.volrenderview.zoom<-volobject.maxres) volrender.volrenderview.zoom = -volobject.maxres; 
				if(volrender.volrenderview.projection_toggle==1) volrender.resizeGL(volrender.volrenderview.winWidth, volrender.volrenderview.winHeight);
				redraw_event();
			}
		}
		else
		{		
			render = 1;			
			active=CUTTING;
			cutplane_interact(-dy/dw);
			active=-1;
		}

	}
}
void QtGLWidget::mouseDoubleClickEvent(QMouseEvent * e)
{
	mouspos.x = e->x();
	mouspos.y = e->y();

	if(volrender.measure_dragmode==0 && active!=OBJ_ANGLE && active!=OBJ_ZOOM) processDepthClick(mouspos.x, mouspos.y, 3);
}
void QtGLWidget::mouseMoveEvent  ( QMouseEvent * e )
{
	mouspos.x = e->x();
	mouspos.y = e->y();
	dx = mouspos.x - global_mousepos.x;
	dy = mouspos.y - global_mousepos.y;
	//global_mousepos = mouspos;

	if(volrender.volrenderview.projection_toggle==PERSP_PROJECTION)
	{	
		volrender.volrenderview.mouse_move(e);
		volrender.volshader_update_eye();
		redraw_event();
		return;		
	}

	if(volrender.stereo_toggle)
	{
		double dClickX, dClickY;
		double offset = volrender.volrenderview.winWidth-volrender.volrenderview.winHeight;
		offset /= 2.0;
		
		dClickX = mouspos.x;
		dClickY = height()-mouspos.y;//double (volrender.volrenderview.winWidth-(mouspos.y+offset));

		double dX, dY, dZ;
		gluUnProject((double)dClickX, (double)dClickY, (double)0.0, volrender.nonfbo_mvmatrix, volrender.nonfbo_projmatrix, volrender.nonfbo_viewport, &dX, &dY, &dZ);
		volrender.stereo_cursor = Vector(dX, dY, dZ);
		setCursor(Qt::BlankCursor);
		redraw_event();
	}

	//------------------------------------------------
	//Get the depth texture (z-buffer)
	//------------------------------------------------
/*	if(volrender.volshader_id==SHADER_NOLIGHT)
	{
		volrender.vol_shaderobj.enable();
		volrender.vol_shaderobj.sendUniform1f("depthpeal", false);
		volrender.vol_shaderobj.sendUniform1f("fbosize", volrender.FBO_SIZE);
		volrender.vol_shaderobj.disable();
	}
*/

	if(volrender.measure_mode==0)
	{
		/*volrender.current_measure_vertindex = -1;
		volrender.current_measure_edgeindex = -1;
		volrender.current_measure_faceindex = -1;*/
		volrender.measure_magnetline_vert_old = -1;
		volrender.measure_magnetline_vert = -1;
	}

	if(volrender.measure_dragmode==0 && active!=OBJ_ANGLE && active!=OBJ_ZOOM) processDepthClick(mouspos.x, mouspos.y, 2);
	if(volrender.measure_dragmode==0 && active!=OBJ_ANGLE && active!=OBJ_ZOOM) processDepthClickCrop(mouspos.x, mouspos.y, 2);

	if(volrender.crop_shape_selected!=-1)
	{
		if ((e->buttons()&Qt::LeftButton))
		{
			processDepthClickCrop(mouspos.x, mouspos.y, 1);
			return;
		}
	}

	if(volrender.current_measure_vertindex!=-1)
	{
		if ((e->buttons()&Qt::LeftButton))
		{
			processDepthClick(mouspos.x, mouspos.y, 1);
			return;
		}
	}

	if(volrender.measure_modify==1 && volrender.current_measure_vertindex==-1)
	{
		switch(active)
		{
			case OBJ_ANGLE:
//				volrender.measure_arcball.mousemotion(&mouspos);
				measure_modify_rotatecurrent(0, (float)dy*0.01);
				measure_modify_rotatecurrent(1, (float)dx*0.01);
				redraw_event();
				break;
			case OBJ_TRANSLATE:			
				//volrender.measure_arcball.mousemotion(&mouspos);
				measure_modify_rotatecurrent(2, (float)dy*0.01);
				redraw_event();
				break;
			case OBJ_ZOOM:	
				measure_modify_zoomcurrent(1.0+(dy*0.001));
				redraw_event();
				break;
			default:
				break;
		}
		global_mousepos = mouspos;
		return;
	}

	if(volrender.measure_modify!=1)
	{
		switch(active)
		{
			case OBJ_ANGLE:
				volrender.volrenderview.arcball.mousemotion(&mouspos);

				redraw_event();
				break;
			case CUTTING:
				
				cutplane_interact(dy);
								
				redraw_event();
				break;
			case OBJ_ZOOM:	
				volrender.volrenderview.zoom -= dy;
				//if(volrender.volrenderview.zoom<-volobject.maxres) volrender.volrenderview.zoom = -volobject.maxres; 

				//printf("zoom: %d\n", (int)volrender.volrenderview.zoom);
				
				if(volrender.volrenderview.projection_toggle==1)
				{
					volrender.resizeGL(volrender.volrenderview.winWidth, volrender.volrenderview.winHeight);
				}

				redraw_event();
				break;

			case OBJ_TRANSLATE:
				volrender.volrenderview.eye.x -= dx;
				volrender.volrenderview.eye.y += dy;
				redraw_event();
				break;
			default:
				break;
			//global_mousepos = mouspos;
		}
	}
	global_mousepos = mouspos;

}
void QtGLWidget::mouseReleaseEvent(QMouseEvent * e )
{
	volrender.volrenderview.arcball.isDragging = false;
	mouspos.x = e->x();
	mouspos.y = e->y();
	global_mousepos = mouspos;

	if(volrender.volrenderview.projection_toggle==PERSP_PROJECTION)
	{
		volrender.volrenderview.mouse_release();
		redraw_event();
		return;
	}

	//volrender.measure_arcball.setIdentity();

	active = -1;
	render = 0;

	if(volrender.measure_dragmode==1) emit measure_updatecurrent_signal(volrender.currentMeasureObject);
	volrender.measure_dragmode = 0;

	emit measure_editpoint_select_signal(-1);
	emit measure_edge_select_signal(-1);

	if(volrender.FBOmoveLOD_toggle)
	{
		volrender.update_FBOsize(0);
		volrender.postprocess2d_shader();
	}
	if(volrender.SlicesmoveLOD_toggle)
	{
		volrender.numbslices = volrender.slices;
		volrender.load_1DLookupTables(true);
	}

	volrender.volrenderview.cutting_planes.mouseRelease();

	if(!volrender.currentMeasureObject.vertices.empty() || volrender.measure_mode==1) captureDepthMap();

//	printf("vertices empty: %d\n", volrender.currentMeasureObject.vertices.empty());
//	printf("measure_mode: %d\n", volrender.measure_mode);

	redraw_event();
}
void QtGLWidget::captureDepthMap(void)
{
	//return;
	//printf("Capturing DEPTHMAP\n");

	volrender.measure_toggle = 0;
	volrender.offscreen_toggle = 1;
	int shader = volrender.volshader_id;
	light_volshader_slot(SHADER_NOLIGHT);

	//paintGL();
	volrender.displayGL(render);

	glBindTexture(GL_TEXTURE_2D, volrender.TEXTURE2D_FBO_DP);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, volrender.depthdata);

	light_volshader_slot(shader);
	volrender.measure_toggle = 1;
	volrender.offscreen_toggle = 0;
}
void QtGLWidget::mousePressEvent ( QMouseEvent * e )
{
	mouspos.x = e->x();
	mouspos.y = e->y();
	global_mousepos = mouspos;

	if(volrender.volrenderview.projection_toggle==PERSP_PROJECTION)
	{	
		volrender.volrenderview.mouse_press(e);
		redraw_event();
		return;
	}
	
	int selected=-1;
	for(int i=0; i<6; i++)
	{
		if(volrender.volrenderview.cutting_planes.selected[i]==1) selected = i;
	}
	
	if ((e->button()&Qt::LeftButton)) 
	{			
		render = 1;

		if(volrender.cursor_toggle==1)
		{
			process3DCursor(mouspos.x, mouspos.y);
			mouseMoveEvent(e);
		}

		if(volrender.volrenderview.cutting_planes.renderall==1)
		{	
			if(processClickOnAnyPlane(mouspos.x, mouspos.y)) processClick(mouspos.x, mouspos.y, 0);
			return;
		}
		if(volrender.cropping_mode==1)
		{
			if(volrender.crop_shape_selected==-1)
			{
				processDepthClickCrop(mouspos.x, mouspos.y, 0);
			}
			else
			{
				processDepthClickCrop(mouspos.x, mouspos.y, 1);
			}
			return;
		}
		
		if(volrender.curve_translate==1)
		{
			processDepthClickTrans(mouspos.x, mouspos.y);
			return;
		}
		if(volrender.measure_mode==1|| volrender.measure_editmode==1) //|| volrender.volrenderview.cutting_planes.renderall==1 
		{
			if(volrender.current_measure_vertindex==-1)
			{
				processDepthClick(mouspos.x, mouspos.y, 0);
			}
			else
			{
				processDepthClick(mouspos.x, mouspos.y, 1);
			}
		}
	
		//ROTATE OBJECT
		active = OBJ_ANGLE;
		//volrender.measure_arcball.setIdentity();
		//volrender.measure_arcball.orien_type=0;
		volrender.volrenderview.arcball.clickDrag(&mouspos);	 

		if(volrender.FBOmoveLOD_toggle)
		{
			volrender.update_FBOsize(1);
			volrender.postprocess2d_shader();
		}
		if(volrender.SlicesmoveLOD_toggle)
		{
			volrender.numbslices = volrender.sliceslod;
			volrender.load_1DLookupTables(true);
		}
		
		if(selected!=-1)
		{
			if(processClickOnSelectedPlane(mouspos.x, mouspos.y)) active=CUTTING;
		}
		
		mouseMoveEvent(e);
    }
	if((e->button()&Qt::MidButton))
	{
		render = 1;
		active = OBJ_TRANSLATE;

		if(volrender.FBOmoveLOD_toggle)
		{
			volrender.update_FBOsize(1);
		}
		if(volrender.SlicesmoveLOD_toggle)
		{
			volrender.numbslices = volrender.sliceslod;
			volrender.load_1DLookupTables(true);
		}

		tempx =volrender.volrenderview.eye.x;
		tempy =volrender.volrenderview.eye.y;

		//volrender.currentMeasureObject.faces.clear();
		//processDepthClick(mouspos.x, mouspos.y);

		mouseMoveEvent(e);
	}
	if((e->button()&Qt::RightButton))
	{
		render = 1;
		active = OBJ_ZOOM;

		if(volrender.FBOmoveLOD_toggle)
		{
			volrender.update_FBOsize(1);
		}
		if(volrender.SlicesmoveLOD_toggle)
		{
			volrender.numbslices = volrender.sliceslod;
			volrender.load_1DLookupTables(true);
		}
		
		mouseMoveEvent(e);
	}
}

bool QtGLWidget::processClickOnSelectedPlane(int x, int y)
{
	//printf("processClickOnSelectedPlane\n");
	// This function will find 2 points in world space that are on the line into the screen defined by screenspace point (x,y)
	double dX, dY, dZ, dClickY; // glUnProject uses doubles, but I'm using floats for these 3D vectors
	//viewport[3]

	double offset = volrender.volrenderview.winWidth-volrender.volrenderview.winHeight;
	offset /= 2.0;

	dClickY = double (volrender.volrenderview.winWidth-(y+offset)); // OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top
	
	gluUnProject((double) x, dClickY, 0.0, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
	Vector ClickRayP1 = Vector( (float) dX, (float) dY, (float) dZ );

	gluUnProject((double) x, dClickY, 1.0, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
	Vector ClickRayP2 = Vector( (float) dX, (float) dY, (float) dZ );

	Vector vClickSlope;
	vClickSlope = ClickRayP2 - ClickRayP1;

	float fDistance = 9999999999999999999999999999.9;

	Vector v;
	Vector v1, v2, v3;
	Vector intersect;
	
	int selected=-1;
	for(int i=0; i<6; i++)
	{
		if(volrender.volrenderview.cutting_planes.selected[i]==1) selected = i;
	}

	if(selected!=-1)
	{
		//build up our plane faces
		vector<Face> faces;
		Face f;
		for(int j=1; j<volrender.volrenderview.cutting_planes.plane_clippoints[selected].size(); j++)
		{
			f.vert[0] = &volrender.volrenderview.cutting_planes.plane_clippoints[selected][j-1];
			f.vert[1] = &volrender.volrenderview.cutting_planes.plane_clippoints[selected][j];
			f.vert[2] = &volrender.volrenderview.cutting_planes.planes[selected].centre;	
			f.uv[0][0] = selected;

			faces.push_back(f);
		}
		f.vert[0] = &volrender.volrenderview.cutting_planes.plane_clippoints[selected][volrender.volrenderview.cutting_planes.plane_clippoints[selected].size()-1];
		f.vert[1] = &volrender.volrenderview.cutting_planes.plane_clippoints[selected][0];
		f.vert[2] = &volrender.volrenderview.cutting_planes.planes[selected].centre;
		f.uv[0][0] = selected;
		faces.push_back(f);
	
		Vector normal;
		for (int ii = 0; ii <faces.size(); ii++)
		{
			if(faces[ii].line_intersection(ClickRayP2, ClickRayP1, intersect))
			{
				Vector vSelSlope =  intersect - ClickRayP1;	
				float fTestDist = vSelSlope.length();

				if (fTestDist < fDistance)
				{
					fDistance = fTestDist;
					normal = faces[ii].facenormal;
					return true;
				}
			}	
		}
	}
	
	return false;
}
bool QtGLWidget::processClickOnAnyPlane(int x, int y)
{
	//printf("processClickOnAnyPlane\n");
	// This function will find 2 points in world space that are on the line into the screen defined by screenspace point (x,y)
	double dX, dY, dZ, dClickY; // glUnProject uses doubles, but I'm using floats for these 3D vectors
	//viewport[3]

	double offset = volrender.volrenderview.winWidth-volrender.volrenderview.winHeight;
	offset /= 2.0;

	dClickY = double (volrender.volrenderview.winWidth-(y+offset)); // OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top
	
	gluUnProject((double) x, dClickY, 0.0, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
	Vector ClickRayP1 = Vector( (float) dX, (float) dY, (float) dZ );

	gluUnProject((double) x, dClickY, 1.0, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
	Vector ClickRayP2 = Vector( (float) dX, (float) dY, (float) dZ );

	Vector vClickSlope;
	vClickSlope = ClickRayP2 - ClickRayP1;

	float fDistance = 9999999999999999999999999999.9;

	Vector v;
	Vector v1, v2, v3;
	Vector intersect;
	
	//build up our plane faces
	vector<Face> faces;
	Face f;
	for(int i=0; i<6; i++)
	{
		for(int j=1; j<volrender.volrenderview.cutting_planes.plane_clippoints[i].size(); j++)
		{
			f.vert[0] = &volrender.volrenderview.cutting_planes.plane_clippoints[i][j-1];
			f.vert[1] = &volrender.volrenderview.cutting_planes.plane_clippoints[i][j];
			f.vert[2] = &volrender.volrenderview.cutting_planes.planes[i].centre;	
			f.uv[0][0] = i;

			faces.push_back(f);
		}
		f.vert[0] = &volrender.volrenderview.cutting_planes.plane_clippoints[i][volrender.volrenderview.cutting_planes.plane_clippoints[i].size()-1];
		f.vert[1] = &volrender.volrenderview.cutting_planes.plane_clippoints[i][0];
		f.vert[2] = &volrender.volrenderview.cutting_planes.planes[i].centre;
		f.uv[0][0] = i;
		faces.push_back(f);
	}

	Vector normal;
	for (int ii = 0; ii <faces.size(); ii++)
	{
		if(faces[ii].line_intersection(ClickRayP2, ClickRayP1, intersect))
		{
			Vector vSelSlope =  intersect - ClickRayP1;	
			float fTestDist = vSelSlope.length();

			if (fTestDist < fDistance)
			{
				fDistance = fTestDist;
				normal = faces[ii].facenormal;
				return true;
			}
		}	
	}
	
	return false;
}
void QtGLWidget::processDepthClickCrop(int x, int y, int mode)
{
	//printf("processDepthClickCrop\n");
	Vector ss_point;

	// This function will find 2 points in world space that are on the line into the screen defined by screenspace point (x,y)
	double dX, dY, dZ, dClickY; // glUnProject uses doubles, but I'm using floats for these 3D vectors

	double offset = volrender.volrenderview.winWidth-volrender.volrenderview.winHeight;
	offset /= 2.0;

	//dClickY =double(volrender.volrenderview.viewport[3] - (y-offset));
	dClickY = double (volrender.volrenderview.winWidth-(y+offset)); // OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top

	ss_point.x = x;
	ss_point.y = dClickY;
	ss_point.z = 0.0;

	gluUnProject((double)ss_point.x, (double)ss_point.y, (double)ss_point.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
	
	Vector position3D = Vector(dX, dY, dZ);

	//printf("ss_point: %f, %f, %f\n", ss_point.x, ss_point.y, ss_point.z);
	gluUnProject((double)ss_point.x, (double)ss_point.y, (double)ss_point.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);

	Vector v;
	v.x = dX;
	v.y = dY;
	v.z = dZ;

	float screenzoom = (float)(volobject.maxres+volrender.volrenderview.zoom)/(float)volobject.maxres;
	float linesize = ((float)volobject.maxres/100.0)/screenzoom;
	float pointsize = ((float)volobject.maxres/100.0)/screenzoom;

	Vector bb_size = Vector(volobject.texwidth*volobject.xsize/volobject.maxsize, volobject.texheight*volobject.ysize/volobject.maxsize, volobject.texdepth*volobject.zsize/volobject.maxsize);
	bb_size /= 2.0;
	float maxl = bb_size.x;
	maxl = max (maxl, bb_size.y);
	maxl = max (maxl, bb_size.z);
	pointsize = (float)((2*maxl)/20.0)*screenzoom;
	linesize = (float)((2*maxl)/20.0)*screenzoom;
	if(pointsize>1.0) pointsize = 1.0;
	if(pointsize<0.25) pointsize = 0.25;
	if(linesize>2.0) linesize = 2.0;
	if(linesize<1.0) linesize = 1.0;
	float sizebb = pointsize*4;

	if(mode==0)			//CLICK
	{
		volrender.crop_shape.push_back(v);

		ss_point.w = volrender.crop_shape.size()-1;
		volrender.crop_shape2d.push_back(ss_point);

		printf("computing normals\n");

		double dX, dY, dZ;
		volrender.crop_normals.clear();
		volrender.crop_centroids.clear();

		Vector planecentre;
		for(int iii=1; iii<volrender.crop_shape.size(); iii++)
		{
			planecentre += volrender.crop_shape[iii];
		}
		planecentre/= volrender.crop_shape.size();

		volrender.crop_triangles.clear();
		Triangulate::Process(volrender.crop_shape2d, volrender.crop_triangles);

		//NORMALS
		if(volrender.crop_shape.size()>2)
		{
			for(int iii=0; iii<volrender.crop_triangles.size()/3; iii++)
			{
				int index0 = volrender.crop_triangles[3*iii].w;
				int index1 = volrender.crop_triangles[3*iii+1].w;
				int index2 = volrender.crop_triangles[3*iii+2].w;

				Vector v0 = volrender.crop_shape[index0];
				Vector v1 = volrender.crop_shape[index1];
						
				gluProject((double)v0.x,(double)v0.y, (double)v0.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
				Vector ssv2 = Vector(dX, dY, 1.0);

				gluProject((double)v1.x,(double)v1.y, (double)v1.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
				Vector ssv3 = Vector(dX, dY, 1.0);

				gluUnProject((double)ssv2.x, (double)ssv2.y, (double)ssv2.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
				Vector v2 = Vector((float)dX, (float)dY, (float)dZ);
				gluUnProject((double)ssv3.x, (double)ssv3.y, (double)ssv3.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
				Vector v3 = Vector((float)dX, (float)dY, (float)dZ);

				Vector centroid = (v0+v1+v2+v3)/4.0f;
				volrender.crop_centroids.push_back(centroid);

				Vector normal0;
				normal0.get_normal(v2,v1,v0);
				volrender.crop_normals.push_back(normal0);

				//--------------------------------------------------------------------

				v0 = volrender.crop_shape[index1];
				v1 = volrender.crop_shape[index2];
						
				gluProject((double)v0.x,(double)v0.y, (double)v0.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
				ssv2 = Vector(dX, dY, 1.0);

				gluProject((double)v1.x,(double)v1.y, (double)v1.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
				ssv3 = Vector(dX, dY, 1.0);

				gluUnProject((double)ssv2.x, (double)ssv2.y, (double)ssv2.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
				v2 = Vector((float)dX, (float)dY, (float)dZ);
				gluUnProject((double)ssv3.x, (double)ssv3.y, (double)ssv3.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
				v3 = Vector((float)dX, (float)dY, (float)dZ);

				centroid = (v0+v1+v2+v3)/4.0f;
				volrender.crop_centroids.push_back(centroid);

				normal0;
				normal0.get_normal(v2,v1,v0);
				volrender.crop_normals.push_back(normal0);

				//--------------------------------------------------------------------

				v0 = volrender.crop_shape[index2];
				v1 = volrender.crop_shape[index0];
						
				gluProject((double)v0.x,(double)v0.y, (double)v0.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
				ssv2 = Vector(dX, dY, 1.0);

				gluProject((double)v1.x,(double)v1.y, (double)v1.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
				ssv3 = Vector(dX, dY, 1.0);

				gluUnProject((double)ssv2.x, (double)ssv2.y, (double)ssv2.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
				v2 = Vector((float)dX, (float)dY, (float)dZ);
				gluUnProject((double)ssv3.x, (double)ssv3.y, (double)ssv3.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
				v3 = Vector((float)dX, (float)dY, (float)dZ);

				centroid = (v0+v1+v2+v3)/4.0f;
				volrender.crop_centroids.push_back(centroid);

				normal0;
				normal0.get_normal(v2,v1,v0);
				volrender.crop_normals.push_back(normal0);
			}
		}
	}
	else if(mode==1)	//DRAG
	{
		volrender.measure_dragmode=1;
		if(volrender.crop_shape_selected!=-1)
		{
			ss_point.w = volrender.crop_shape_selected;
			volrender.crop_shape[volrender.crop_shape_selected] = v;
			volrender.crop_shape2d[volrender.crop_shape_selected] = ss_point;

			volrender.crop_triangles.clear();
			Triangulate::Process(volrender.crop_shape2d, volrender.crop_triangles);

			redraw_event();
		}
	}
	else if(mode==2)	//HOVER
	{
		volrender.crop_shape_selected=-1;
		for(int i=0; i<volrender.crop_shape.size(); i++)
		{
			Vector v0 = volrender.crop_shape[i];						
			gluProject((double)v0.x,(double)v0.y, (double)v0.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
			Vector screenspace = Vector(dX, dY, dZ);

			//printf("sspace: %f, %f, %f\n", screenspace.x, screenspace.y, screenspace.z);
			if(ss_point.x+sizebb*2.0 > screenspace.x
			&& ss_point.x-sizebb*2.0 < screenspace.x
			&& ss_point.y+sizebb*2.0 > screenspace.y
			&& ss_point.y-sizebb*2.0 < screenspace.y)
			{
				//printf("SELECT POINT: 2D POINT FOUND.\n");
				volrender.crop_shape_selected = i;
				redraw_event();				
			}
		}
	}
}
void QtGLWidget::unprojectDepth(Vector v0, float* vZ, Vector* scene)
{
	double dpX, dpY, dpZ, rX, rY, rZ, ddpx, ddpY, ddpZ;

	//project our 3d coord to 2d screen space
	gluProject((double)v0.x,(double)v0.y, (double)v0.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dpX, &dpY, &dpZ);

	double offset = volrender.volrenderview.winWidth-volrender.volrenderview.winHeight;
	offset /= 2.0;
	dpY = (dpY+offset);

	//convert screenspace to our depthmap texture
	Vector texcoord;
	texcoord.x = (((double)(dpX)/(double)volrender.volrenderview.winWidth))*(double)volrender.FBO_SIZE;
	texcoord.y = (((double)(dpY)/(double)volrender.volrenderview.winWidth))*(double)volrender.FBO_SIZE;
	if(texcoord.x>volrender.FBO_SIZE-1) texcoord.x=volrender.FBO_SIZE-1;
	if(texcoord.y>volrender.FBO_SIZE-1) texcoord.y=volrender.FBO_SIZE-1;
	if(texcoord.x<0.0) texcoord.x=0.0;
	if(texcoord.y<0.0) texcoord.y=0.0;
	rZ = volrender.depthdata[(int)(texcoord.x+0.5)+(volrender.FBO_SIZE*(int)(texcoord.y+0.5))];
	rZ /= 4294967296.0;
		
	Vector ss_point;
	ss_point.x = v0.x;
	ss_point.y = v0.y;
	ss_point.z = rZ;
	gluProject((double)ss_point.x, (double)ss_point.y, (double)ss_point.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &rX, &rY, &rZ);
					
	dpY = volrender.volrenderview.winWidth-dpY;

	*vZ = rZ;
	*scene = Vector(dpX, dpY, dpZ);
}
void QtGLWidget::process3DCursor(int x, int y)
{
	captureDepthMap();

	Vector ss_point;

	// This function will find 2 points in world space that are on the line into the screen defined by screenspace point (x,y)
	double dX, dY, dZ, dClickY; // glUnProject uses doubles, but I'm using floats for these 3D vectors

	double offset = volrender.volrenderview.winWidth-volrender.volrenderview.winHeight;
	offset /= 2.0;

	dClickY = double (volrender.volrenderview.winWidth-(y+offset)); // OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top

	ss_point.x = x;
	ss_point.y = dClickY;
	ss_point.z = 0.0;

	unsigned int* imgdata = new unsigned int[volrender.FBO_SIZE*volrender.FBO_SIZE];
	glBindTexture(GL_TEXTURE_2D, volrender.TEXTURE2D_FBO_DP);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, imgdata);

	Vector texcoord;	
	texcoord.x = ((float)(ss_point.x)/(float)volrender.volrenderview.viewport[2])*(float)volrender.FBO_SIZE;
	texcoord.y = ((float)(ss_point.y)/(float)volrender.volrenderview.viewport[3])*(float)volrender.FBO_SIZE;
	
	if(texcoord.x>volrender.FBO_SIZE-1) texcoord.x=volrender.FBO_SIZE-1;
	if(texcoord.y>volrender.FBO_SIZE-1) texcoord.y=volrender.FBO_SIZE-1;
	if(texcoord.x<0.0) texcoord.x=0.0;
	if(texcoord.y<0.0) texcoord.y=0.0;

	float val = volrender.depthdata[(int)(texcoord.x)+volrender.FBO_SIZE*(int)(texcoord.y)];
	val /= 4294967296.0;

	float zoom = (float)(volobject.maxres-volrender.volrenderview.zoom)/(float)volobject.maxres;

	delete[] imgdata;
	imgdata = 0;
	
	if(val==0 || val==1.0) val=0.5;
	ss_point.z = (val);

	gluUnProject((double)ss_point.x, (double)ss_point.y, (double)ss_point.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
	
	volrender.cursor3D_screenspace = Vector(dX, dY, dZ);

	sliceframe_updatepixel();
}
void QtGLWidget::processDepthClick(int x, int y, int mode)
{
	//printf("processDepthClick\n");

	Vector ss_point;
	double dX, dY, dZ, dClickY;
	double offset = volrender.volrenderview.winWidth-volrender.volrenderview.winHeight;
	offset /= 2.0;
	dClickY = double (volrender.volrenderview.winWidth-(y+offset));
	
	ss_point.x = x;
	ss_point.y = dClickY;
	ss_point.z = 0.0;

	unsigned int* imgdata = new unsigned int[volrender.FBO_SIZE*volrender.FBO_SIZE];
	glBindTexture(GL_TEXTURE_2D, volrender.TEXTURE2D_FBO_DP);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, imgdata);

	Vector texcoord;	
	texcoord.x = ((float)(ss_point.x)/(float)volrender.volrenderview.viewport[2])*(float)volrender.FBO_SIZE;
	texcoord.y = ((float)(ss_point.y)/(float)volrender.volrenderview.viewport[3])*(float)volrender.FBO_SIZE;
	
	if(texcoord.x>volrender.FBO_SIZE-1) texcoord.x=volrender.FBO_SIZE-1;
	if(texcoord.y>volrender.FBO_SIZE-1) texcoord.y=volrender.FBO_SIZE-1;
	if(texcoord.x<0.0) texcoord.x=0.0;
	if(texcoord.y<0.0) texcoord.y=0.0;

	float val = volrender.depthdata[(int)(texcoord.x)+volrender.FBO_SIZE*(int)(texcoord.y)];
	val /= 4294967296.0;
	delete[] imgdata;
	imgdata = 0;
	
	//if(val==0 || val==1.0) val=0.5;

	ss_point.z = (val);
	gluUnProject((double)ss_point.x, (double)ss_point.y, (double)ss_point.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
	
	Vector position3D = Vector(dX, dY, dZ);
	int selected = -1;
	
	Vector sizebb = Vector(volobject.texwidth*volobject.xsize/volobject.maxsize, volobject.texheight*volobject.ysize/volobject.maxsize, volobject.texdepth*volobject.zsize/volobject.maxsize);
	sizebb *= volrender.measure_selection_window;

	gluUnProject((double)ss_point.x, (double)ss_point.y, (double)-1.0, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
	Vector ClickRayP1 = Vector( (float) dX, (float) dY, (float) dZ );

	gluUnProject((double)ss_point.x, (double)ss_point.y, (double)1.0, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
	Vector ClickRayP2 = Vector( (float) dX, (float) dY, (float) dZ );

	if(mode==0)	//LEFT CLICK
	{
//			printf("ADD POINT\n");
			if(volrender.current_measure_vertindex==-1)
			{
				//printf("ADD POINT: ADDING A POINT\n");
				if(volrender.current_measure_editmode==MEASURE_EDIT_VERTS)
				{
					volrender.currentMeasureObject.addPoint(position3D);
				}

				if(volrender.current_measure_drawmode==MEASURE_DRAWMODE_MAGNETLINE)
				{
					volrender.measure_magnetline_vert = volrender.currentMeasureObject.vertices.size()-1;
				}

				if(volrender.current_measure_editmode==MEASURE_EDIT_VERTS)
				{
					measure_rebuild_current_edges();
					emit measure_point_signal(position3D);
					emit measure_updatecurrent_signal(volrender.currentMeasureObject);
				}
			}
	}
	else if(mode==1) //DRAG
	{
		//printf("EDIT POINT\n");
		if(volrender.current_measure_vertindex!=-1)
		{
			if(volrender.current_measure_editmode==MEASURE_EDIT_FACES && volrender.measure_editmode==1 && volrender.measure_modify==0)
			{
				//printf("EDIT FACE: DRAG\n");
				if(volrender.measure_magnetline_vert!=-1)
				{
					bool found = false;
					for(int xx=0; xx<volrender.current_face.size(); xx++)
					{
						if(volrender.current_face[xx]==volrender.measure_magnetline_vert) found = true;
					}

					if(found==false)
					{
						//printf("FACES: index: %d\n", volrender.measure_magnetline_vert);
						//printf("ADD FACE VERT\n");
						volrender.current_face.push_back(volrender.measure_magnetline_vert);
						emit measure_addvertface_signal(volrender.measure_magnetline_vert);
					}
					else
					{
						//printf("REMOVE FACE VERT\n");
						int todelete=-1;
						for(int i=0; i<volrender.current_face.size(); i++)
						{
							if(volrender.current_face[i]==volrender.measure_magnetline_vert) todelete = i;
						}
						volrender.current_face.erase(volrender.current_face.begin()+todelete);
						emit measure_rmvvertface_signal(volrender.measure_magnetline_vert);
					}
				}
			}
			else
			{
				if(volrender.measure_editmode==0 && volrender.measure_modify==0)
				{
					//printf("EDIT POINT: POINT MOVE!\n");
					volrender.measure_dragmode = 1;
					volrender.currentMeasureObject.vertices[volrender.current_measure_vertindex] = position3D;
				}
				else if(volrender.measure_editmode==0 && volrender.measure_modify==1)
				{
					//printf("EDIT POINT: GROUP MOVE!\n");
					volrender.measure_dragmode = 1;

					Vector v = volrender.currentMeasureObject.vertices[volrender.current_measure_vertindex]-position3D;
					volrender.currentMeasureObject.vertices[volrender.current_measure_vertindex] = position3D;

					for(int i=0; i<volrender.currentMeasureObject.vertices.size(); i++)
					{
						if(i!=volrender.current_measure_vertindex)
						{
							volrender.currentMeasureObject.vertices[i] -= v;
						}
					}
					measure_modify_start_slot();
				}
				if(volrender.current_measure_drawmode==MEASURE_DRAWMODE_MAGNETLINE && volrender.measure_mode==1)
				{
					if(volrender.measure_magnetline_vert_old!=-1 && volrender.measure_magnetline_vert!=-1 && volrender.measure_magnetline_vert_old!=volrender.measure_magnetline_vert)
					{
						measure_rebuild_current_edges();
						emit measure_updatecurrent_signal(volrender.currentMeasureObject);
					}
					volrender.measure_magnetline_vert_old = volrender.measure_magnetline_vert;
				}
			}

			redraw_event();
		}
	}
	else if(mode==2) //HOVER
	{
		//printf("SELECT POINT\n");
		selected = -1;
		volrender.current_measure_vertindex = selected;
		volrender.current_measure_edgeindex = selected;
		volrender.current_measure_faceindex = selected;

		if(volrender.current_measure_viewmode==MEASURE_VIEW_VERTS || volrender.current_measure_editmode==MEASURE_EDIT_EDGES  || volrender.current_measure_editmode==MEASURE_EDIT_FACES)
		{
			//printf("HOVER VERTS\n");
			//printf("LOOK FOR A VERTEX IN 3D\n");
			//LOOK FOR A VERTEX IN 3D
			for(int i=0; i<volrender.currentMeasureObject.vertices.size(); i++)
			{
				//int i = volrender.measure_magnetline_destvert;

				if(volrender.currentMeasureObject.vertices[i].x+sizebb.x > position3D.x
					&& volrender.currentMeasureObject.vertices[i].x-sizebb.x < position3D.x
					&& volrender.currentMeasureObject.vertices[i].y+sizebb.y > position3D.y
					&& volrender.currentMeasureObject.vertices[i].y-sizebb.y < position3D.y
					&& volrender.currentMeasureObject.vertices[i].z+sizebb.z > position3D.z
					&& volrender.currentMeasureObject.vertices[i].z-sizebb.z < position3D.z)
				{
					//printf("SELECT POINT: 3D POINT FOUND.\n");
					selected = i;
					
					if(volrender.current_measure_drawmode!=MEASURE_DRAWMODE_POINTS)
					{
						//printf("volrender.measure_editmode: %d\n", volrender.measure_editmode);
						volrender.measure_magnetline_vert = i;
						if(volrender.measure_editmode==0)
						{
							volrender.measure_magnetline_vert_old = i;
						}
					}

					emit measure_editpoint_select_signal(selected);
					redraw_event();
				}
			}

			//LOOK FOR A VERTEX IN 2D
			if(selected==-1) //&& volrender.measure_mode==1
			{
				//printf("LOOK FOR A VERTEX IN 2D\n");
				for(int i=0; i<volrender.currentMeasureObject.vertices.size(); i++)
				{
					//int i = volrender.measure_magnetline_destvert;

					Vector v0 = volrender.currentMeasureObject.vertices[i];						
					gluProject((double)v0.x,(double)v0.y, (double)v0.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
					Vector screenspace = Vector(dX, dY, dZ);

					//printf("sspace: %f, %f, %f\n", screenspace.x, screenspace.y, screenspace.z);
					if(ss_point.x+sizebb.x*2.0 > screenspace.x
					&& ss_point.x-sizebb.x*2.0 < screenspace.x
					&& ss_point.y+sizebb.y*2.0 > screenspace.y
					&& ss_point.y-sizebb.y*2.0 < screenspace.y)
					{
						//printf("SELECT POINT: 2D POINT FOUND.\n");
						selected = i;
						
						if(volrender.current_measure_drawmode!=MEASURE_DRAWMODE_POINTS)
						{
							//printf("volrender.measure_editmode: %d\n", volrender.measure_editmode);

							volrender.measure_magnetline_vert = i;
							if(volrender.measure_editmode==0)
							{
								volrender.measure_magnetline_vert_old = i;
							}
						}

						emit measure_editpoint_select_signal(selected);
						redraw_event();				
					}
				}
			}

			if(volrender.current_measure_vertindex!=-1)
			{
				if(volrender.current_measure_editmode==MEASURE_EDIT_FACES && volrender.measure_editmode==1 && volrender.measure_modify==0 && volrender.measure_facehover==false)
				{
					//printf("EDIT FACE: DRAG\n");
					if(volrender.measure_magnetline_vert!=-1)
					{

						bool found = false;
						for(int xx=0; xx<volrender.current_face.size(); xx++)
						{
							if(volrender.current_face[xx]==volrender.measure_magnetline_vert) found = true;
						}

						if(found==false)
						{
						//	printf("			FACES: index: %d\n", volrender.measure_magnetline_vert);
						//	printf("ADD FACE VERT\n");
							volrender.current_face.push_back(volrender.measure_magnetline_vert);
							emit measure_addvertface_signal(volrender.measure_magnetline_vert);
						}
						else
						{
							//printf("REMOVE FACE VERT\n");
							int todelete=-1;
							for(int i=0; i<volrender.current_face.size(); i++)
							{
								if(volrender.current_face[i]==volrender.measure_magnetline_vert) todelete = i;
							}
							volrender.current_face.erase(volrender.current_face.begin()+todelete);
							emit measure_rmvvertface_signal(volrender.measure_magnetline_vert);
							volrender.current_measure_vertindex = -1;
							volrender.measure_magnetline_vert = -1;
							volrender.current_measure_faceindex=-1;
						}

						volrender.measure_facehover = true;
					}
				}	
			}
			else volrender.measure_facehover = false;

		}
		else if(volrender.current_measure_viewmode==MEASURE_VIEW_EDGES)
		{
			//Look in 3D
			if(selected==-1 && volrender.measure_mode==0)
			{
				volrender.current_measure_edgeindex=-1;
				for(int i=0; i<volrender.currentMeasureObject.edges.size(); i++)
				{
					Vector edge_v0 = volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.edges[i].v0];
					Vector edge_v1 = volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.edges[i].v1];
					Vector edgevec = edge_v0 - edge_v1;
					float elength = edgevec.length();
					edgevec.normalize();
					elength *= 0.2;

					edge_v0 = edge_v0 - (elength*edgevec);
					edge_v1 = edge_v1 + (elength*edgevec);

					float dist = position3D.distance_to_line(edge_v0, edge_v1);

					//printf("dist: %f\n", dist);
					if(dist<sizebb.x)
					{
						//printf("SELECT EDGE: 3D EDGE FOUND.\n");

						volrender.current_measure_edgeindex=i;
						volrender.current_measure_vertindex=-1;
						volrender.current_measure_faceindex=-1;
						emit measure_edge_select_signal(volrender.current_measure_edgeindex);
					}
				}
			}
			//Look in 2D
			if(volrender.current_measure_edgeindex==-1 && volrender.measure_mode==0)
			{
				//printf("EDGE HOVER 2D\n");

				volrender.current_measure_edgeindex=-1;
				for(int i=0; i<volrender.currentMeasureObject.edges.size(); i++)
				{
					Vector edge_v0 = volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.edges[i].v0];
					Vector edge_v1 = volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.edges[i].v1];
					Vector edgevec = edge_v0 - edge_v1;
					float elength = edgevec.length();
					edgevec.normalize();
					elength *= 0.2;

					edge_v0 = edge_v0 - (elength*edgevec);
					edge_v1 = edge_v1 + (elength*edgevec);

					gluProject((double)edge_v0.x,(double)edge_v0.y, (double)edge_v0.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
					edge_v0 = Vector(dX, dY, dZ);

					gluProject((double)edge_v1.x,(double)edge_v1.y, (double)edge_v1.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
					edge_v1 = Vector(dX, dY, dZ);

					Vector position2D;
					gluProject((double)position3D.x,(double)position3D.y, (double)position3D.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
					position2D = Vector(dX, dY, dZ);

					float dist = position2D.distance_to_line(edge_v0, edge_v1);

					//printf("dist: %f\n", dist);
					if(dist<sizebb.x)
					{
						//printf("SELECT EDGE: 3D EDGE FOUND.\n");

						volrender.current_measure_edgeindex=i;
						volrender.current_measure_vertindex=-1;
						volrender.current_measure_faceindex=-1;
						emit measure_edge_select_signal(volrender.current_measure_edgeindex);
					}
				}
			}
		}
		else if(volrender.current_measure_viewmode==MEASURE_VIEW_FACES)
		{
			//printf("HOVER FACES\n");

			//LOOK FOR A FACE IN 3D
			if(selected==-1 && volrender.measure_mode==0)
			{
				Face f;
				Vector centroid;
				vector<Face> faces;

				bool b_intersection = false;
				float fDistance = 9999999999999999999999999999.9;
				for(int i=0; i<volrender.currentMeasureObject.faces.size(); i++)
				{
					for(int j=0; j<volrender.currentMeasureObject.faces[i].size(); j++)
						centroid += volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[i][j]];

					centroid /= (float)volrender.currentMeasureObject.faces[i].size();

					faces.clear();

					//if(volrender.currentMeasureObject.faces[i].size()!=3)
					{
						for(int j=1; j<volrender.currentMeasureObject.faces[i].size(); j++)
						{
							f.vert[0] = &volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[i][j-1]];
							f.vert[1] = &volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[i][j]];
							f.vert[2] = &centroid;
							faces.push_back(f);
						}
						f.vert[0] = &volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[i][volrender.currentMeasureObject.faces[i].size()-1]];
						f.vert[1] = &volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[i][0]];
						f.vert[2] = &centroid;
						faces.push_back(f);
					}
					/*else
					{
						f.vert[0] = &volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[i][0]];
						f.vert[1] = &volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[i][1]];
						f.vert[2] = &volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[i][2]];
						faces.push_back(f);
					}*/

					Vector normal, intersect;
					for (int ii = 0; ii <faces.size(); ii++)
					{
						if(faces[ii].line_intersection(ClickRayP2, ClickRayP1, intersect))
						{
							Vector vSelSlope =  intersect - ClickRayP1;	
							float fTestDist = vSelSlope.length();

							if (fTestDist < fDistance)
							{
								fDistance = fTestDist;
								b_intersection=true;
								volrender.current_measure_faceindex=i;
							}
						}	
					}
				}

				if(b_intersection==true)
				{
					volrender.current_measure_edgeindex=-1;
					volrender.current_measure_vertindex=-1;					
					emit measure_face_select_signal(volrender.current_measure_faceindex);
				}	

			}
		}
	}
	else if(mode==3) //DOUBLE CLICK
	{
		if(volrender.current_measure_viewmode==MEASURE_VIEW_EDGES)
		{
			volrender.current_measure_edgeindex=-1;
		
			//Look in 3D
			if(selected==-1 && volrender.measure_mode==0)
			{
				volrender.current_measure_edgeindex=-1;
				for(int i=0; i<volrender.currentMeasureObject.edges.size(); i++)
				{
					Vector edge_v0 = volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.edges[i].v0];
					Vector edge_v1 = volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.edges[i].v1];
					Vector edgevec = edge_v0 - edge_v1;
					float elength = edgevec.length();
					edgevec.normalize();
					elength *= 0.2;

					edge_v0 = edge_v0 - (elength*edgevec);
					edge_v1 = edge_v1 + (elength*edgevec);

					float dist = position3D.distance_to_line(edge_v0, edge_v1);

					//printf("dist: %f\n", dist);
					if(dist<sizebb.x)
					{
						//printf("SELECT EDGE: 3D EDGE FOUND.\n");

						volrender.current_measure_edgeindex=i;
						volrender.current_measure_vertindex=-1;
						volrender.current_measure_faceindex=-1;
						emit measure_edge_select_signal(volrender.current_measure_edgeindex);
					}
				}
			}
			//Look in 2D
			if(volrender.current_measure_edgeindex==-1 && volrender.measure_mode==0)
			{
				//printf("EDGE HOVER 2D\n");
				for(int i=0; i<volrender.currentMeasureObject.edges.size(); i++)
				{
					Vector edge_v0 = volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.edges[i].v0];
					Vector edge_v1 = volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.edges[i].v1];
					Vector edgevec = edge_v0 - edge_v1;
					float elength = edgevec.length();
					edgevec.normalize();
					elength *= 0.2;

					edge_v0 = edge_v0 - (elength*edgevec);
					edge_v1 = edge_v1 + (elength*edgevec);

					gluProject((double)edge_v0.x,(double)edge_v0.y, (double)edge_v0.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
					edge_v0 = Vector(dX, dY, dZ);

					gluProject((double)edge_v1.x,(double)edge_v1.y, (double)edge_v1.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
					edge_v1 = Vector(dX, dY, dZ);

					Vector position2D;
					gluProject((double)position3D.x,(double)position3D.y, (double)position3D.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
					position2D = Vector(dX, dY, dZ);

					float dist = position2D.distance_to_line(edge_v0, edge_v1);

					//printf("dist: %f\n", dist);
					if(dist<sizebb.x)
					{
						//printf("SELECT EDGE: 3D EDGE FOUND.\n");

						volrender.current_measure_edgeindex=i;
						volrender.current_measure_vertindex=-1;
						volrender.current_measure_faceindex=-1;
						emit measure_edge_select_signal(volrender.current_measure_edgeindex);
					}
				}
			}		
			if(volrender.current_measure_edgeindex!=-1)
			{
				Vector v0 = volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.edges[volrender.current_measure_edgeindex].v0];
				Vector v1 = volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.edges[volrender.current_measure_edgeindex].v1];
					
				gluProject((double)v0.x,(double)v0.y, (double)v0.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
				Vector screenspace = Vector(dX, dY, dZ);

				//printf("sspace: %f, %f, %f\n", screenspace.x, screenspace.y, screenspace.z);

				Vector ssv2, ssv3;
				ssv2.x = ssv3.x = screenspace.x;
				ssv2.y = ssv3.y = screenspace.y;
				ssv2.z = 1.0;
				ssv3.z = 0.0;

				gluUnProject((double)ssv2.x, (double)ssv2.y, (double)ssv2.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
				Vector v2 = Vector((float)dX, (float)dY, (float)dZ);

				gluUnProject((double)ssv3.x, (double)ssv3.y, (double)ssv3.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
				Vector v3 = Vector((float)dX, (float)dY, (float)dZ);
	
				volrender.volrenderview.cutting_planes.planes[0]._v1 = (v0);
				volrender.volrenderview.cutting_planes.planes[0]._v2 = (v1);
				volrender.volrenderview.cutting_planes.planes[0]._v3 = (v2);
				volrender.volrenderview.cutting_planes.planes[0]._v4 = (v3);
				volrender.volrenderview.cutting_planes.planes[0].normal.get_normal(v0,v1,v2);

				volrender.volrenderview.cutting_planes.planes[0].eval_planeequation();
				volrender.volrenderview.cutting_planes.eval_planeintersections();
			}
		}
		else if(volrender.current_measure_viewmode==MEASURE_VIEW_FACES)
		{
			printf("DOUBLE CLICK FACE\n");
			//LOOK FOR A FACE IN 3D
			if(selected==-1 && volrender.measure_mode==0)
			{
				Face f;
				Vector centroid;
				vector<Face> faces;
				volrender.measure_section_face = -1;

				bool b_intersection = false;
				float fDistance = 9999999999999999999999999999.9;
				for(int i=0; i<volrender.currentMeasureObject.faces.size(); i++)
				{
					for(int j=0; j<volrender.currentMeasureObject.faces[i].size(); j++)
						centroid += volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[i][j]];

					centroid /= (float)volrender.currentMeasureObject.faces[i].size();

					faces.clear();

					//if(volrender.currentMeasureObject.faces[i].size()!=3)
					{
						for(int j=1; j<volrender.currentMeasureObject.faces[i].size(); j++)
						{
							f.vert[0] = &volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[i][j-1]];
							f.vert[1] = &volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[i][j]];
							f.vert[2] = &centroid;
							faces.push_back(f);
						}

						f.vert[0] = &volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[i][volrender.currentMeasureObject.faces[i].size()-1]];
						f.vert[1] = &volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[i][0]];
						f.vert[2] = &centroid;
						faces.push_back(f);
					}

					Vector normal, intersect;
					for (int ii = 0; ii <faces.size(); ii++)
					{
						if(faces[ii].line_intersection(ClickRayP2, ClickRayP1, intersect))
						{
							Vector vSelSlope =  intersect - ClickRayP1;	
							float fTestDist = vSelSlope.length();

							if (fTestDist < fDistance)
							{
								fDistance = fTestDist;
								b_intersection=true;
								volrender.current_measure_faceindex=i;
							}
						}	
					}
				}

				if(b_intersection==true)
				{
					volrender.current_measure_edgeindex=-1;
					volrender.current_measure_vertindex=-1;					
					volrender.measure_section_face = volrender.current_measure_faceindex;

					emit measure_face_select_signal(volrender.current_measure_faceindex);
				}	


				if(volrender.current_measure_faceindex!=-1)
				{
					if(volrender.currentMeasureObject.faces[volrender.current_measure_faceindex].size()<3) return;

					Vector v1 =volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[volrender.current_measure_faceindex][0]];
					Vector v2 =volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[volrender.current_measure_faceindex][1]];
					Vector v3 =volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[volrender.current_measure_faceindex][2]];

					//v1 = v1.mult_matrix(volrender.texrot_mat.m);
					//v2 = v2.mult_matrix(volrender.texrot_mat.m);
					//v3 = v3.mult_matrix(volrender.texrot_mat.m);

					//v1 = v1.mult_matrix(volrender.volrenderview.arcball.Transform.M);
					//v2 = v2.mult_matrix(volrender.volrenderview.arcball.Transform.M);
					//v3 = v3.mult_matrix(volrender.volrenderview.arcball.Transform.M);

					//compute rotation matrix to move current view
					Face f;
					f.vert[0] = &v1;
					f.vert[1] = &v2;
					f.vert[2] = &v3;

					Vector centroid = *f.vert[0]+*f.vert[1]+*f.vert[2];
					centroid /= 3.0;
				
					vector < Vector > face_axis;

					//Face normal
					f.evaluate_face_normal();
					face_axis.push_back(f.facenormal);

					//Face edge
					Vector v = *f.vert[1]-*f.vert[0];
					v.normalize();
					face_axis.push_back(v);

					//cross product edge & normal
					Vector p;
					p.cross(v, f.facenormal);
					p.normalize();
					face_axis.push_back(p);

					Vector worlaxis1 = Vector(1,0,0); 
					Vector worlaxis2 = Vector(0,1,0);
					Vector worlaxis3 = Vector(0,0,1);	

					//face_axis[0] = face_axis[0].mult_matrix(volrender.volrenderview.arcball.Transform.M);
					
					face_axis[0].normalize();
					worlaxis3.normalize();

					Vector rot_axis;
					rot_axis.cross(face_axis[0], worlaxis3);
					rot_axis.normalize();

					float angle = face_axis[0].signed_angle(worlaxis3, rot_axis);
					//printf("angle: %f\n", angle);
					
					if(volrender.measure_face_invert) angle = angle+PI;

					//volrender.volrenderview.arcball.setIdentity();
					//v.build_rotatemat(rot_axis, angle, volrender.volrenderview.arcball.Transform.M);				
									
					Matrix4x4 rot;
					v.build_rotatemat(rot_axis, angle, rot.m);
					//rot.find_rotationfromaxis(face_axis[0], face_axis[1], face_axis[2], worlaxis1, worlaxis2, worlaxis3);		
					//rot.find_rotation(face_axis[0], worlaxis3);

					if(volrender.measure_updateview_toggle==true)
					{
						printf("UPDATE VIEW\n");
						//volrender.volrenderview.arcball.LastRot = volrender.volrenderview.arcball.ThisRot;
						volrender.volrenderview.arcball.ThisRot.M[0] = rot.m[0];
						volrender.volrenderview.arcball.ThisRot.M[1] = rot.m[1];
						volrender.volrenderview.arcball.ThisRot.M[2] = rot.m[2];
						volrender.volrenderview.arcball.ThisRot.M[3] = rot.m[4];
						volrender.volrenderview.arcball.ThisRot.M[4] = rot.m[5];
						volrender.volrenderview.arcball.ThisRot.M[5] = rot.m[6];
						volrender.volrenderview.arcball.ThisRot.M[6] = rot.m[8];
						volrender.volrenderview.arcball.ThisRot.M[7] = rot.m[9];
						volrender.volrenderview.arcball.ThisRot.M[8] = rot.m[10];
					
						//Matrix3fMulMatrix3f(&volrender.volrenderview.arcball.ThisRot, &volrender.volrenderview.arcball.LastRot);				// Accumulate Last Rotation Into This One
						Matrix4fSetRotationFromMatrix3f(&volrender.volrenderview.arcball.Transform, &volrender.volrenderview.arcball.ThisRot);	// Set Our Final Transform's Rotation From This One
					}

					if(volrender.measure_section_toggle)
					{
						Vector translate;
					
						volrender.volrenderview.cutting_planes.reset_all();
					
						f.evaluate_face_normal();
						face_axis[0] = f.facenormal;
						face_axis[0].normalize();

						volrender.volrenderview.cutting_planes.planes[0].normal.get_normal(volrender.volrenderview.cutting_planes.planes[0]._v1, volrender.volrenderview.cutting_planes.planes[0]._v2, volrender.volrenderview.cutting_planes.planes[0]._v3);
						worlaxis3 = volrender.volrenderview.cutting_planes.planes[0].normal;
						worlaxis3.normalize();

						rot_axis.cross(face_axis[0], worlaxis3);
						rot_axis.normalize();
						angle = worlaxis3.signed_angle(face_axis[0], rot_axis);
						v.build_rotatemat(rot_axis, angle, rot.m);

						printf("angle: %f\n", angle);

						volrender.volrenderview.cutting_planes.planes[0]._v1 = volrender.volrenderview.cutting_planes.planes[0]._v1.mult_matrix(rot.m);
						volrender.volrenderview.cutting_planes.planes[0]._v2 = volrender.volrenderview.cutting_planes.planes[0]._v2.mult_matrix(rot.m);
						volrender.volrenderview.cutting_planes.planes[0]._v3 = volrender.volrenderview.cutting_planes.planes[0]._v3.mult_matrix(rot.m);
						volrender.volrenderview.cutting_planes.planes[0]._v4 = volrender.volrenderview.cutting_planes.planes[0]._v4.mult_matrix(rot.m);
						volrender.volrenderview.cutting_planes.planes[0].normal.get_normal(volrender.volrenderview.cutting_planes.planes[0]._v1, volrender.volrenderview.cutting_planes.planes[0]._v2, volrender.volrenderview.cutting_planes.planes[0]._v3);
					
						volrender.volrenderview.cutting_planes.planes[0].centre = volrender.volrenderview.cutting_planes.planes[0]._v1+volrender.volrenderview.cutting_planes.planes[0]._v2+volrender.volrenderview.cutting_planes.planes[0]._v3;
						volrender.volrenderview.cutting_planes.planes[0].centre /= 3.0;
					
						translate = volrender.volrenderview.cutting_planes.planes[0].centre-centroid;
						volrender.volrenderview.cutting_planes.planes[0]._v1 -= translate;
						volrender.volrenderview.cutting_planes.planes[0]._v2 -= translate;
						volrender.volrenderview.cutting_planes.planes[0]._v3 -= translate;
						volrender.volrenderview.cutting_planes.planes[0]._v4 -= translate;
					
						volrender.volrenderview.cutting_planes.planes[0]._v1 -= (float)volrender.measure_section_thickn * volrender.volrenderview.cutting_planes.planes[0].normal;
						volrender.volrenderview.cutting_planes.planes[0]._v2 -= (float)volrender.measure_section_thickn * volrender.volrenderview.cutting_planes.planes[0].normal;
						volrender.volrenderview.cutting_planes.planes[0]._v3 -= (float)volrender.measure_section_thickn * volrender.volrenderview.cutting_planes.planes[0].normal;
						volrender.volrenderview.cutting_planes.planes[0]._v4 -= (float)volrender.measure_section_thickn * volrender.volrenderview.cutting_planes.planes[0].normal;

						volrender.volrenderview.cutting_planes.planes[0].eval_planeequation();

						volrender.volrenderview.cutting_planes.planes[1]._v1 = volrender.volrenderview.cutting_planes.planes[1]._v1.mult_matrix(rot.m);
						volrender.volrenderview.cutting_planes.planes[1]._v2 = volrender.volrenderview.cutting_planes.planes[1]._v2.mult_matrix(rot.m);
						volrender.volrenderview.cutting_planes.planes[1]._v3 = volrender.volrenderview.cutting_planes.planes[1]._v3.mult_matrix(rot.m);
						volrender.volrenderview.cutting_planes.planes[1]._v4 = volrender.volrenderview.cutting_planes.planes[1]._v4.mult_matrix(rot.m);
						volrender.volrenderview.cutting_planes.planes[1].normal.get_normal(volrender.volrenderview.cutting_planes.planes[1]._v1, volrender.volrenderview.cutting_planes.planes[1]._v2, volrender.volrenderview.cutting_planes.planes[1]._v3);
					
						volrender.volrenderview.cutting_planes.planes[1].centre = volrender.volrenderview.cutting_planes.planes[1]._v1+volrender.volrenderview.cutting_planes.planes[1]._v2+volrender.volrenderview.cutting_planes.planes[1]._v3;
						volrender.volrenderview.cutting_planes.planes[1].centre /= 3.0;
					
						translate = volrender.volrenderview.cutting_planes.planes[1].centre-centroid;
						volrender.volrenderview.cutting_planes.planes[1]._v1 -= translate;
						volrender.volrenderview.cutting_planes.planes[1]._v2 -= translate;
						volrender.volrenderview.cutting_planes.planes[1]._v3 -= translate;
						volrender.volrenderview.cutting_planes.planes[1]._v4 -= translate;
					
						volrender.volrenderview.cutting_planes.planes[1]._v1 -= (float)volrender.measure_section_thickn * volrender.volrenderview.cutting_planes.planes[1].normal;
						volrender.volrenderview.cutting_planes.planes[1]._v2 -= (float)volrender.measure_section_thickn * volrender.volrenderview.cutting_planes.planes[1].normal;
						volrender.volrenderview.cutting_planes.planes[1]._v3 -= (float)volrender.measure_section_thickn * volrender.volrenderview.cutting_planes.planes[1].normal;
						volrender.volrenderview.cutting_planes.planes[1]._v4 -= (float)volrender.measure_section_thickn * volrender.volrenderview.cutting_planes.planes[1].normal;

						volrender.volrenderview.cutting_planes.planes[1].eval_planeequation();

						volrender.volrenderview.cutting_planes.eval_planeintersections();
					}

					//volrender.current_measure_faceindex = -1;
				}
			}
		}

		redraw_event();
	}

	if(volrender.cursor_toggle==1) sliceframe_updatepixel();

	//printf("imgspace: %d, %d\n", x,y);
	glDisable(GL_CULL_FACE);
}
void QtGLWidget::processDepthClickTrans(int x, int y)
{
	printf("processDepthClickTrans\n");
	Vector ss_point;

	// This function will find 2 points in world space that are on the line into the screen defined by screenspace point (x,y)
	double dX, dY, dZ, dClickY; // glUnProject uses doubles, but I'm using floats for these 3D vectors
	//viewport[3]

	double offset = volrender.volrenderview.winWidth-volrender.volrenderview.winHeight;
	offset /= 2.0;

	//dClickY =double(volrender.volrenderview.viewport[3] - (y-offset));
	dClickY = double (volrender.volrenderview.winWidth-(y+offset)); // OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top

	ss_point.x = x;
	ss_point.y = dClickY;
	ss_point.z = 0.0;

	unsigned int* imgdata = new unsigned int[volrender.FBO_SIZE*volrender.FBO_SIZE];
	glBindTexture(GL_TEXTURE_2D, volrender.TEXTURE2D_FBO_DP);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, imgdata);

	Vector texcoord;	
	texcoord.x = ((float)(ss_point.x)/(float)volrender.volrenderview.viewport[2])*(float)volrender.FBO_SIZE;
	texcoord.y = ((float)(ss_point.y)/(float)volrender.volrenderview.viewport[3])*(float)volrender.FBO_SIZE;
	
	if(texcoord.x>volrender.FBO_SIZE-1) texcoord.x=volrender.FBO_SIZE-1;
	if(texcoord.y>volrender.FBO_SIZE-1) texcoord.y=volrender.FBO_SIZE-1;
	if(texcoord.x<0.0) texcoord.x=0.0;
	if(texcoord.y<0.0) texcoord.y=0.0;

	float val = volrender.depthdata[(int)(texcoord.x)+volrender.FBO_SIZE*(int)(texcoord.y)];
	val /= 4294967296.0;

	float zoom = (float)(volobject.maxres-volrender.volrenderview.zoom)/(float)volobject.maxres;

	delete[] imgdata;
	imgdata = 0;
	
	if(val==0 || val==1.0)
	{
		val=0.5;
	}

	ss_point.z = (val);

	gluUnProject((double)ss_point.x, (double)ss_point.y, (double)ss_point.z, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
	
	Vector v = Vector(dX, dY, dZ);	
	volrender.debugv = v;

	int selected=-1;
	for(int i=0; i<6; i++)
	{
		if(volrender.volrenderview.cutting_planes.selected[i]==1) selected = i;
	}

	if(selected==-1) return;

	volrender.curve_vectranslate = (volrender.volrenderview.cutting_planes.planes[selected].centre-v);

/*	volrender.volrenderview.cutting_planes.planes[selected]._v1 -= vtrans;
	volrender.volrenderview.cutting_planes.planes[selected]._v2 -= vtrans;
	volrender.volrenderview.cutting_planes.planes[selected]._v3 -= vtrans;
	volrender.volrenderview.cutting_planes.planes[selected]._v4 -= vtrans;
	volrender.volrenderview.cutting_planes.planes[selected].v1 -= vtrans;
	volrender.volrenderview.cutting_planes.planes[selected].v2 -= vtrans;
	volrender.volrenderview.cutting_planes.planes[selected].v3 -= vtrans;
	volrender.volrenderview.cutting_planes.planes[selected].v4 -= vtrans;
	
	volrender.volrenderview.cutting_planes.planes[selected].eval_planeequation();
	volrender.volrenderview.cutting_planes.eval_planeintersections();
*/

	redraw_event();
}
void QtGLWidget::processClick(int x, int y, int mode)
{
	printf("processClick\n");

	// This function will find 2 points in world space that are on the line into the screen defined by screenspace point (x,y)
	double dX, dY, dZ, dClickY; // glUnProject uses doubles, but I'm using floats for these 3D vectors
	//viewport[3]

	double offset = volrender.volrenderview.winWidth-volrender.volrenderview.winHeight;
	offset /= 2.0;

	dClickY = double (volrender.volrenderview.winWidth-(y+offset)); // OpenGL renders with (0,0) on bottom, mouse reports with (0,0) on top
	
	gluUnProject((double) x, dClickY, 0.0, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
	Vector ClickRayP1 = Vector( (float) dX, (float) dY, (float) dZ );

	gluUnProject((double) x, dClickY, 1.0, volrender.volrenderview.mvmatrix, volrender.volrenderview.projmatrix, volrender.volrenderview.viewport, &dX, &dY, &dZ);
	Vector ClickRayP2 = Vector( (float) dX, (float) dY, (float) dZ );

	Vector vClickSlope;
	vClickSlope = ClickRayP2 - ClickRayP1;

	float fDistance = 9999999999999999999999999999.9;

	Vector v;
	Vector v1, v2, v3;
	Vector intersect;

	vector<Face> faces;

	if(volrender.measure_mode==0 && volrender.measure_editmode==0)
	{	
		//printf("Resetting selected...\n");
		for(int i=0; i<6; i++) volrender.volrenderview.cutting_planes.selected[i] = -1;
	}

	int selected=-1;
	for(int i=0; i<6; i++)
	{
		//printf("i: %d,  selected: %d \n", i, (int)volrender.volrenderview.cutting_planes.selected[i]);

		if(volrender.volrenderview.cutting_planes.selected[i]==1) selected = i;
	}
	//printf("selected: %d\n", (int) selected);
	
	if(selected!=-1)
	{
		//printf("Process Click, plane already selected\n");
		emit cutplane_enableplanepanel_signal();

		Face f;
		for(int j=1; j<volrender.volrenderview.cutting_planes.plane_clippoints[selected].size(); j++)
		{
			f.vert[0] = &volrender.volrenderview.cutting_planes.plane_clippoints[selected][j-1];
			f.vert[1] = &volrender.volrenderview.cutting_planes.plane_clippoints[selected][j];
			f.vert[2] = &volrender.volrenderview.cutting_planes.planes[selected].centre;	
			f.uv[0][0] = selected;

			faces.push_back(f);
		}

		f.vert[0] = &volrender.volrenderview.cutting_planes.plane_clippoints[selected][volrender.volrenderview.cutting_planes.plane_clippoints[selected].size()-1];
		f.vert[1] = &volrender.volrenderview.cutting_planes.plane_clippoints[selected][0];
		f.vert[2] = &volrender.volrenderview.cutting_planes.planes[selected].centre;
		f.uv[0][0] = selected;

		faces.push_back(f);
	
		bool b_intersection = false;
		Vector normal;
		for (int ii = 0; ii <faces.size(); ii++)
		{
			if(faces[ii].line_intersection(ClickRayP2, ClickRayP1, intersect))
			{
				Vector vSelSlope =  intersect - ClickRayP1;	
				float fTestDist = vSelSlope.length();

				if (fTestDist < fDistance)
				{
					fDistance = fTestDist;
					normal = faces[ii].facenormal;
					b_intersection=true;
				}
			}	
		}

		if(b_intersection==true)
		{
			if(volrender.measure_mode==1)
			{
				//printf("ADD POINT\n");

				if(volrender.current_measure_drawmode==MEASURE_DRAWMODE_LINES && volrender.currentMeasureObject.vertices.size()>=2) return;

				//volrender.currentMeasureObject.face_normals.push_back(normal);
				volrender.currentMeasureObject.addPoint(intersect);
				emit measure_point_signal(intersect);
				emit measure_updatecurrent_signal(volrender.currentMeasureObject);
			}
			
			if(volrender.measure_editmode==1)
			{
				//printf("EDIT POINT\n");
				
				//if mode == clicked nb mode == 1 == drag...
				if(mode==0)
				{
					int selected = -1;
					for(int i=0; i<volrender.currentMeasureObject.vertices.size(); i++)
					{
						float sizebb = volrender.volobj->maxres/64.0f;
						if(sizebb<1.0) sizebb=1.0;
						sizebb=5.0;

						if(volrender.currentMeasureObject.vertices[i].x+sizebb > intersect.x)
						if(volrender.currentMeasureObject.vertices[i].x-sizebb < intersect.x)
						if(volrender.currentMeasureObject.vertices[i].y+sizebb > intersect.y)
						if(volrender.currentMeasureObject.vertices[i].y-sizebb < intersect.y)
						if(volrender.currentMeasureObject.vertices[i].z+sizebb > intersect.z)
						if(volrender.currentMeasureObject.vertices[i].z-sizebb < intersect.z)
						{
							selected = i;
						}
					}

					if(selected!=-1)
					{
						emit measure_editpoint_select_signal(selected);
						return;
					}
				}

				if(volrender.current_measure_vertindex>=0 || volrender.current_measure_vertindex<volrender.currentMeasureObject.vertices.size()-1)
				{
					//printf("volrender.current_measure_vertindex: %d\n", volrender.current_measure_vertindex);
					volrender.currentMeasureObject.vertices[volrender.current_measure_vertindex] = intersect;
					//volrender.currentMeasureObject.face_normals[volrender.current_measure_vertindex] = normal;

					emit measure_editpoint_signal(intersect);
				}
			}
		}	
	
	}
	else
	{
		//printf("Process Click, plane not selected\n");

		Face f;
		for(int sel=0; sel<6; sel++)
		{
			for(int j=1; j<volrender.volrenderview.cutting_planes.plane_clippoints[sel].size(); j++)
			{
				f.vert[0] = &volrender.volrenderview.cutting_planes.plane_clippoints[sel][j];
				f.vert[1] = &volrender.volrenderview.cutting_planes.plane_clippoints[sel][j-1];
				f.vert[2] = &volrender.volrenderview.cutting_planes.planes[sel].centre;
				f.uv[0][0] = sel;

				faces.push_back(f);
			}

			f.vert[0] = &volrender.volrenderview.cutting_planes.plane_clippoints[sel][volrender.volrenderview.cutting_planes.plane_clippoints[sel].size()-1];
			f.vert[1] = &volrender.volrenderview.cutting_planes.plane_clippoints[sel][0];
			f.vert[2] = &volrender.volrenderview.cutting_planes.planes[sel].centre;
			f.uv[0][0] = sel;

			faces.push_back(f);
		}
		//printf("selected: %d\n", selected);

		selected = -1;
		Vector normal;
		for (int ii = 0; ii <faces.size(); ii++)
		{
			if(faces[ii].line_intersection(ClickRayP2, ClickRayP1, intersect))
			{
				Vector vSelSlope =  intersect - ClickRayP1;
				float fTestDist = vSelSlope.length();

				if (fTestDist < fDistance)
				{
					fDistance = fTestDist;
					normal = faces[ii].facenormal;
					//volrender.intersect = intersect;
					selected = (int)faces[ii].uv[0][0];
				}
			}
		}

		//printf("selected: %d\n", selected);

		if(selected==-1) emit cutplane_selected_signal(0, -1);
		else
		{
			for(int iiii=0; iiii<6; iiii++) volrender.volrenderview.cutting_planes.selected[iiii] = 0;
			volrender.volrenderview.cutting_planes.selected[selected] = 1;
			emit cutplane_selected_signal(selected, volrender.volrenderview.plane_rendermode[selected]);
			emit cutplane_enableplanepanel_signal();
		}
	}

	redraw_event();
}
void QtGLWidget::dragEnterEvent(QDragEnterEvent* event)
{
	event->accept(Q3TextDrag::canDecode(event));
}
void QtGLWidget::dropEvent(QDropEvent* event)
{
	Q3StrList URIs;

	if ( Q3UriDrag::decode(event, URIs) ) 
	{
		QString text = Q3UriDrag::uriToLocalFile(URIs.getFirst());
		emit dragndrop_signal(text);
	}
}
//===========================================================================================================
void QtGLWidget::realign(void)
{
	PCA pca;
	int i;
	Vector v;
	int x0,y0,z0;
	int x,y,z;
	int r,g,b;
	int index;
	int index0;
	
	Q3ProgressDialog* progressbar = new Q3ProgressDialog("Recomputing Centre of the Volume Data...", "Cancel", volobject.texdepth*2, 0, "progress", TRUE);		
	progressbar->setCaption("Please Wait");
	progressbar->setMinimumDuration(10);

	for(z = 0; z <volobject.texdepth; z++)
	{	
		progressbar->setProgress(z);
		if (progressbar->wasCancelled()) break;

		for(x = 0; x <volobject.texwidth; x++)
		{
			for(y = 0; y <volobject.texheight; y++)
			{
				index = ((((z*volobject.texheight)+y)*volobject.texwidth)+x);

				r = volobject.get_value(x,y,z,0);
				g = volobject.get_value(x,y,z,1);
				b = volobject.get_value(x,y,z,2);

				if(r+g+b>0)
				{
					v = Vector(x,y,z,index);
					pca.data.push_back(v);
				}
			}
		}
	}

	pca.eval_PCA();

	int volsize = 3*volobject.texdepth*volobject.texwidth*volobject.texheight;
	unsigned char* temptex = new unsigned char[volsize];
	
	for (i=0; i<volsize; i++)
	{
		temptex[i] = volobject.get_value(i,0);
	}
	
	for (i=0; i<pca.aligned_data.size(); i++)
	{		
		x = pca.aligned_data[i].x+pca.originalmeanx;
		y = pca.aligned_data[i].y+pca.originalmeany;
		z = pca.aligned_data[i].z+pca.originalmeanz;
		index0 = pca.aligned_data[i].w;
		
		if(x>volobject.texwidth-1) x = volobject.texwidth-1;
		if(y>volobject.texheight-1) y = volobject.texheight-1;
		if(z>volobject.texdepth-1) z = volobject.texdepth-1;
		if(x<0) x = 0;
		if(y<0) y = 0;
		if(z<0) z = 0;
			
		volobject.set_value(x,y,z,0, temptex[3*index0+0]);
		volobject.set_value(x,y,z,1, temptex[3*index0+1]);
		volobject.set_value(x,y,z,2, temptex[3*index0+2]);
	}

	delete temptex;
	
	volio.pad(1.0);

	progressbar->setProgress(volobject.texdepth*2);
	delete progressbar;	
}
void QtGLWidget::recentre(void)
{
	//printf("RECENTRE\n");
	int x,y,z;
	int x1,y1,z1;
	int r,g,b;
	int index=0;
	int index2=0;
	Vector v;
	Vector v1;
	Vector centre=Vector(0,0,0);
	int numbvox=1;

	Q3ProgressDialog* progressbar = new Q3ProgressDialog("Recomputing Centre of the Volume Data...", "Cancel", volobject.texdepth*2, 0, "progress", TRUE);		
	progressbar->setCaption("Please Wait");
	progressbar->setMinimumDuration(10);

	for(z = 0; z <volobject.texdepth; z++)
	{	
		progressbar->setProgress(z);
		if (progressbar->wasCancelled()) break;

		for(x = 0; x <volobject.texwidth; x++)
		{
			for(y = 0; y <volobject.texheight; y++)
			{
				r=g=b=0;

				r = volobject.get_value(x,y,z,0);
				g = volobject.get_value(x,y,z,1);
				b = volobject.get_value(x,y,z,2);

				if(r+g+b>0)
				{
					centre += Vector(x,y,z,0);
					numbvox++;
				}
			}
		}
	}

	centre /= (float)numbvox;

	//printf("centre0 %f, %f, %f\n", centre.x, centre.y, centre.z);

	Vector offset = Vector(volobject.texwidth/2.0, volobject.texheight/2.0, volobject.texdepth/2.0);
	centre = offset-centre;

	centre.x = int(centre.x+0.5);
	centre.y = int(centre.y+0.5);
	centre.z = int(centre.z+0.5);
	
	//printf("centre1 %f, %f, %f\n", centre.x, centre.y, centre.z);

	int xend, zend, yend;
	int xbegin, zbegin, ybegin;
	int xincr, zincr, yincr;

	if(centre.z<=0)
	{
		zbegin = z = 0;
		zbegin=0;
		zend=volobject.texdepth-1;
		zincr=1;
	}
	else
	{
		z = volobject.texdepth-1;
		zbegin=z;
		zend=0;
		zincr=-1;
	}

	if(centre.y<=0)
	{
		y = 0;
		ybegin=0;
		yend=volobject.texheight-1;
		yincr=1;
	}
	else
	{
		y = volobject.texheight-1;
		ybegin=y;
		yend=0;
		yincr=-1;
	}

	if(centre.x<=0)
	{
		x = 0;
		xbegin=0;
		xend=volobject.texwidth-1;
		xincr=1;
	}
	else
	{
		x = volobject.texwidth-1;
		xbegin=x;
		xend=0;
		xincr=-1;
	}

	//now recentre the data.

	int zprog=0;
	while(z!=zend)
	{	
		progressbar->setProgress(volobject.texdepth+zprog);
		if (progressbar->wasCancelled()) break;

		while(x!=xend)
		{
			while(y!=yend)
			{
				x1 = x-(centre.x);
				y1 = y-(centre.y);
				z1 = z-(centre.z);

				index = volobject.get_index1D(x,y,z);
				index2 = volobject.get_index1D(x1,y1,z1);

				volobject.set_value(index, 0, volobject.get_value(index2, 0));
				volobject.set_value(index, 1, volobject.get_value(index2, 1));
				volobject.set_value(index, 2, volobject.get_value(index2, 2));
				y += yincr;
			}
			y=ybegin;
			x += xincr;
		}
		x=xbegin;
		z += zincr;
		zprog++;
	}
	
	//also pad the data to avoid texture boundary issues.
	volio.pad(1.0);

	progressbar->setProgress(volobject.texdepth*2);
	delete progressbar;
}
void QtGLWidget::invcrop(void)
{
	//printf("Applying CLIPPING planes... \n");
	int index=0;
	Vector v = Vector(0,0,0,0);
	
	Q3ProgressDialog* progressbar = new Q3ProgressDialog("Cropping the Volume Data...", "Cancel", volobject.texdepth, 0, "progress", TRUE);		
	progressbar->setCaption("Please Wait");
	progressbar->setMinimumDuration(10);

	int r,g,b;
	Vector c;

	for(int z = 0; z <volobject.texdepth; z++)
	{	
		//progress(z, volobject.texdepth);
		progressbar->setProgress(z);
		if (progressbar->wasCancelled()) break;

		for(int x = 0; x <volobject.texwidth; x++)
		{
			for(int y = 0; y <volobject.texheight; y++)
			{
				index = volobject.get_index1D(x,y,z);

				r = volobject.get_value(index, 0);
				g = volobject.get_value(index, 1);
				b = volobject.get_value(index, 2);

				if(r+g+b>0)
				{
					v = volobject.imagespacetoworldspace(Vector(x,y,z));

					int helper=0;
					for(int i=0; i<volrender.volrenderview.cutting_planes.planes.size(); i++)
					{
                        c = volrender.volrenderview.cutting_planes.planes[i].centre;

						if(volrender.volrenderview.cutting_planes.planes[i].normal.dot(v-c)>0.0)
						{
							helper++;
						}
					}
					if(helper==6)
					{
						if(volrender.volrenderview.redclip_toggle==1) volobject.set_value(index, 0, 0);
						if(volrender.volrenderview.greenclip_toggle==1) volobject.set_value(index, 1, 0);
						if(volrender.volrenderview.blueclip_toggle==1) volobject.set_value(index, 2, 0);						
					}
				}
			}
		}
	}

	progressbar->setProgress(volobject.texdepth);
	delete progressbar;	
}
void QtGLWidget::crop(void)
{
	//printf("Applying CLIPPING planes... \n");
	int index=0;
	Vector v = Vector(0,0,0,0);
	
	Q3ProgressDialog* progressbar = new Q3ProgressDialog("Cropping the Volume Data...", "Cancel", volobject.texdepth, 0, "progress", TRUE);		
	progressbar->setCaption("Please Wait");
	progressbar->setMinimumDuration(10);

	int r,g,b;


	Vector c;
	for(int z = 0; z <volobject.texdepth; z++)
	{	
		//progress(z, volobject.texdepth);
		progressbar->setProgress(z);
		if (progressbar->wasCancelled()) break;

		for(int x = 0; x <volobject.texwidth; x++)
		{
			for(int y = 0; y <volobject.texheight; y++)
			{
				index = volobject.get_index1D(x,y,z);

				r = volobject.get_value(index, 0);
				g = volobject.get_value(index, 1);
				b = volobject.get_value(index, 2);

				if(r+g+b>0)
				{
					v = volobject.imagespacetoworldspace(Vector(x,y,z));

					for(int i=0; i<volrender.volrenderview.cutting_planes.planes.size(); i++)
					{
						c = volrender.volrenderview.cutting_planes.planes[i].centre;

						if(volrender.volrenderview.cutting_planes.planes[i].normal.dot(v-c)<0.0 && v.w!=-1.0)
						{
							v.w = -1.0;

							if(volrender.volrenderview.redclip_toggle==1) volobject.set_value(index, 0, 0);
							if(volrender.volrenderview.greenclip_toggle==1) volobject.set_value(index, 1, 0);
							if(volrender.volrenderview.blueclip_toggle==1) volobject.set_value(index, 2, 0); 
						}
					}
				}
			}
		}
	}

	progressbar->setProgress(volobject.texdepth);
	delete progressbar;
}
void QtGLWidget::transfer_function( vector<Vector> cp, float graphsize, float border, int mode)
{
	if(volobject.texture3d==NULL) return;
	int size = volobject.texwidth * volobject.texheight * volobject.texdepth * 3;

	double rvalue=0;
	double gvalue=0;
	double bvalue=0;
	double avalue=0;
	double new_value=0;

	float max_func = (256+cp[0].z);

	printf("Applying TRANSFER FUNCTION to volume... \n");

	border -=0.5;

//	undo_volobject = volobject;
	for(int i=0; i<size; i+=3)
	{
		rvalue = (int)volobject.get_value(i, 0);
		gvalue = (int)volobject.get_value(i, 1);
		bvalue = (int)volobject.get_value(i, 2);

		progress(i, size);

		if(mode==1)
		{
			new_value = 255-(cp[rvalue].y-border);				
			volobject.set_value(i, 0, int(new_value+0.5));

			new_value = 255-(cp[gvalue].y-border);				
			volobject.set_value(i, 1, int(new_value+0.5));

			new_value = 255-(cp[bvalue].y-border);				
			volobject.set_value(i, 2, int(new_value+0.5));
		}
	}
	 
	printf("\n");

	volrender.load_TextureMap();
	redraw_event();
}
void QtGLWidget::gaussian(int size, float sigma)
{
	if(volobject.texwidth==1 || volobject.texheight==1 || volobject.texdepth==1) return;

	Filter filter;
	filter.volobj = &volobject;
	filter.apply_gaussian3D(size, sigma, 1.0);	
	filter.volobj = 0;

	volrender.load_TextureMap();
	redraw_event();
}
void QtGLWidget::sobel(void)
{
	if(volobject.texwidth==1 || volobject.texheight==1 || volobject.texdepth==1) return;

	Filter filter;
	filter.volobj = &volobject;
	filter.apply_sobel3D(SOBEL_HVD);	
	filter.volobj = 0;

	volrender.load_TextureMap();
	redraw_event();
}
void QtGLWidget::unsharp_slot(int size, float sigma, int bright, int cont)
{
	if(volobject.texture3d==NULL) return;

	Filter filter;
	filter.volobj = &volobject;
	filter.apply_unsharpmask3D(-bright, -cont, sigma, size);	
	filter.volobj = 0;

	volrender.load_TextureMap();
	emit frames_redraw_signal();
	redraw_event();
}
void QtGLWidget::thresholdmin_slot(int value)
{
/*	if(volobject.texture3d==NULL) return;
    glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, (float)(value)/255.0);

	redraw_event();*/
}
void QtGLWidget::thresholdmax_slot(int value)
{
/*	if(volobject.texture3d==NULL) return;
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_LESS, (float)(value)/255.0);
	redraw_event();*/
}
void QtGLWidget::threshold_slot(int min, int max)
{
	if(volobject.texture3d==NULL) return;

//	threshold(min, max);
//	emit frames_redraw_signal();	
	redraw_event();
}
void QtGLWidget::brightcontrast_slot(int b, int c)
{
	if(volobject.texture3d==NULL) return;

//	undo_volobject = volobject;

	int size = 3*volobject.texwidth * volobject.texheight * volobject.texdepth;

	printf("Applying Brightness & Contrast to volume... \n");

//	undo_volobject = volobject;

	float K_os =   (float)b/100.0;
	float K_oe = (float)c/100.0;

//	printf("%f, %f\n", K_os, K_oe);

	float value;
	for(int i=0; i<size; i++)
	{
		value = (float)volobject.get_value(i, 0);

		if(value)
		{
			value = pow((K_os*value),K_oe);

			if(value<0) value = 0;
			if(value>255) value = 255;

			volobject.set_value(i, 0, value);
		}
	}

	volrender.load_TextureMap();
//	emit frames_redraw_signal();
	redraw_event();
}
void QtGLWidget::setcurrentworkingdir_slot(QString dn)
{
	currentworkingdir = dn;
}

//===========================================================================================================
void QtGLWidget::tfunc_histoupdate_slot(int toggle)
{
	volrender.updatehisto = toggle;
	volrender.build_histogram();
}

void QtGLWidget::tfunc_rebuild_histogram_slot(void)
{
	if(volobject.texture3d==NULL) return;
	volrender.build_histogram();
}
//===========================================================================================================

#ifdef LIBTIFF_SUPPORT
void QtGLWidget::tfunc_remap16bit_slot(int min_l, int max_l, const vector<Vector>& tfunc_curvepts_luminance, int min_o, int max_o, const vector<Vector>& tfunc_curvepts_opacity)
{
	if(volobject.texture3d==NULL) return;

	int index=0;
	int index2d=0;	
	uint16 indexlookup16bit=0;
	uint16 value=0;

	uint16 minthresh_l = min_l;
	uint16 maxthresh_l = max_l;
	uint16 range_l = maxthresh_l - minthresh_l;
	
	uint16 minthresh_o = min_o;
	uint16 maxthresh_o = max_o;
	uint16 range_o = maxthresh_o - minthresh_o;

	printf("min: %d\n", min_l);
	printf("max: %d\n", max_l);
	printf("range 16bit: %d\n", range_l);

	int value8bit_l=0;
	int value8bit_o=0;
	
	Q3ProgressDialog* progressbar = new Q3ProgressDialog("Remapping 16bit data to 8bit...", "Cancel", volobject.texdepth, 0, "progress", TRUE);		
	progressbar->setCaption("Please Wait");
	progressbar->setMinimumDuration(10);

	for(int i=0; i<256*3; i++) volobject.histogram[i]=0.0;
	
	for(int z=0; z<volobject.texdepth; z++)
	{
		//update our progress bar
		progressbar->setProgress((int)z);
		if (progressbar->wasCancelled())
			break;

		for(int y=0; y<volobject.texheight; y++)
		{
			for(int x=0; x<volobject.texwidth; x++)
			{
				index = (((z*volobject.texheight)+y)*volobject.texwidth)+x;
				index2d = (y*volobject.texwidth)+x;

				indexlookup16bit = volobject.voldata[z][index2d];
				
				if(indexlookup16bit<minthresh_l) indexlookup16bit = 0.0;
				if(indexlookup16bit>maxthresh_l) indexlookup16bit = 0.0;
				value = 65535 - tfunc_curvepts_luminance[(int)indexlookup16bit].y;			 
				if(value>minthresh_l) value = value - minthresh_l;					
				value8bit_l = ((float)value/(float)range_l)*255.0;		
				if(value8bit_l>255) value8bit_l = 255;
				if(value8bit_l<0) value8bit_l = 0;

				indexlookup16bit = volobject.voldata[z][index2d];

				if(indexlookup16bit<minthresh_o) indexlookup16bit = 0.0;
				if(indexlookup16bit>maxthresh_o) indexlookup16bit = 0.0;
				value = 65536 - tfunc_curvepts_opacity[(int)indexlookup16bit].y;			 
				if(value>minthresh_o) value = value - minthresh_o;					
				value8bit_o = ((float)value/(float)range_o)*255.0;
				if(value8bit_o>255) value8bit_o = 255;
				if(value8bit_o<0) value8bit_o = 0;
				
				value = (float)value8bit_l*(float)value8bit_o;
				if(value>255) value = 255;
				if(value<0) value = 0;

				//printf("index3d: %d of %d\n", index, 512*512*512*3);
				//printf("value8bit: %d\n", value8bit);

				volobject.set_value(index, 0, value8bit_o);
				volobject.set_value(index, 1, value8bit_o);
				volobject.set_value(index, 2, value8bit_o);
			
				volobject.histogram[3*value8bit_o+0]++;
				volobject.histogram[3*value8bit_o+1]++;
				volobject.histogram[3*value8bit_o+2]++;
			}
		}	
	}
	
	delete progressbar;
	progressbar=NULL;
	
	volio.pad(1.0);
	volrender.load_3DTextureMap();

//	tfunc_rebuild_histogram_slot();
//	emit tfunc_histogram_signal(&volobject.histogram[0]);
	emit tfunc_16bit_histogram_signal(&volobject.histogram16bit[0], &volobject.histogram[0]);
	
	redraw_event();
//	emit frames_redraw_signal();
}
#endif
//===========================================================================================================
void QtGLWidget::hist_tfunc_slot(int mode, TfuncState tfunc)
{
	volrender.apply_transfer_function(mode, tfunc);
	redraw_event();
}
//===========================================================================================================
void QtGLWidget::isosurface_new_slot(QString name, int quality, int value, int smooth, Vector col, bool* channels, int clipstate)
{
	//printf("isosurface_new_slot\n");
	
//	VolumeObject	filteredvolobject;
//	filteredvolobject = volobject;

//	filteredvolobject.resize((float)quality/100.0f);
//	quality=100;

	float size = 1.0;
	float sigma = 1.0;

	if(smooth==25)
	{
		size = 3.0;
		sigma = 0.5;
	}
	else if(smooth==50)
	{
		size = 3.0;
		sigma = 1.5;
	}
	else if(smooth==75)
	{
		size = 3.0;
		sigma = 2.5;
	}
	else if(smooth==100)
	{
		size = 3.0;
		sigma = 3.5;
	}
	else
	{
		smooth=0.0;
		size = 1.0;
		sigma = 1.0;
	}
	
	printf("smooth: %f\n", smooth);
	printf("size: %f\n", size);
	printf("sigma: %f\n", sigma);
	
	/*if(smooth!=0)
	{
		//printf("smooth %d\n", smooth);
		Filter filt;
		filt.volobj = &volobject;
		filt.apply_gaussian3D(size, sigma, quality);
	}*/

	//printf("isosurface_new_slot\n");

	mcubes.mesh = volrender.meshlist.new_mesh();
	mcubes.volobj = &volobject;
	
	if(smooth==0) mcubes.vMarchingCubes(channels, quality, value, volrender.ragaba_tfunc);
	else mcubes.vMarchingCubes(channels, quality, value, volrender.ragaba_tfunc, size, sigma);

	float maxsize =  max((float) mcubes.volobj->texwidth, (float)mcubes.volobj->texheight);
	maxsize =  max((int)maxsize, (int)mcubes.volobj->texdepth);
	Vector vscale = Vector(1.0/(maxsize/(float)mcubes.volobj->texwidth),1.0/(maxsize/(float)mcubes.volobj->texdepth),  1.0/(maxsize/(float)mcubes.volobj->texheight));
	
	maxsize =  max(mcubes.volobj->xsize, mcubes.volobj->ysize);
	maxsize =  max(maxsize, mcubes.volobj->zsize);
	Vector voxrescale = Vector(mcubes.volobj->xsize/maxsize, mcubes.volobj->zsize/maxsize, mcubes.volobj->ysize/maxsize);

	vscale = vscale*voxrescale;
	mcubes.mesh->scalev(vscale);

	volrender.meshlist.add(name, col, quality, value, size, sigma, clipstate);

	//safe to clear our mcubes data now
	mcubes.clear();
	
	mcubes.mesh = NULL;
	mcubes.volobj = NULL;

	//volrender.load_TextureMap();

	//emit frames_redraw_signal();
	redraw_event();
}
void QtGLWidget::isosurface_get_slot(QString name, int* quality, int* value, int* smooth, Vector* col, int* nv, int* nf, int* clipstate)
{

	float size = 0.0;
	float sigma = 0.0;
	
	volrender.meshlist.get(name, quality, value, &size, &sigma, col, nv, nf, clipstate);

	if(size==3.0&&sigma==0.5)
	{
		*smooth = 25;
	}
	else if(size==3.0&&sigma==1.5)
	{
		*smooth = 50;
	}
	else if(size==3.0&&sigma==2.5)
	{
		*smooth = 75;
	}
	else if(size==3.0&&sigma==3.5)
	{
		*smooth = 100;
	}
	

//	emit isosurface_get_signal();
	redraw_event();
}
void QtGLWidget::isosurface_del_slot(QString name)
{
	volrender.meshlist.del(name);

	//emit redraw event
	redraw_event();
}
void QtGLWidget::isosurface_update_slot(QString oldname, QString name, int quality, int value, int smooth, Vector col, bool* channels, int clpst)
{
	float size = 1.0;
	float sigma = 1.0;

	if(smooth==25)
	{
		size = 3.0;
		sigma = 0.5;
	}
	else if(smooth==50)
	{
		size = 3.0;
		sigma = 1.5;
	}
	else if(smooth==75)
	{
		size = 3.0;
		sigma = 2.5;
	}
	else if(smooth==100)
	{
		size = 3.0;
		sigma = 3.5;
	}

	int index = volrender.meshlist.set(oldname, name, quality, value, size, sigma, col, clpst);

	//if val or qual has been changed recompute iso-surface for index
	if(index!=-1)
	{
	//	volrender.meshlist.del(name);
	//	isosurface_new_slot(name, quality, value, smooth, col, channels, clpst);
		
		/*if(smooth!=0)
		{
			Filter filt;
			filt.volobj = &volobject;
			filt.apply_gaussian3D(3.0, 2.0, quality);
		}*/
		
		printf("update2!!\n");
		
		mcubes.mesh = NULL;
		volrender.meshlist.meshes[index].reset();
		mcubes.mesh = &volrender.meshlist.meshes[index];
		mcubes.volobj = &volobject;
		//mcubes.vMarchingCubes(channels, quality, value, volrender.rgb_tfunc, size, sigma);

		if(smooth==0) mcubes.vMarchingCubes(channels, quality, value, volrender.ragaba_tfunc);
		else mcubes.vMarchingCubes(channels, quality, value, volrender.ragaba_tfunc, size, sigma);

		float maxsize =  max(mcubes.volobj->texwidth, mcubes.volobj->texheight);
		maxsize =  max((float)maxsize, (float)mcubes.volobj->texdepth);
		Vector vscale = Vector(1.0/(maxsize/(float)mcubes.volobj->texwidth),1.0/(maxsize/(float)mcubes.volobj->texdepth),  1.0/(maxsize/(float)mcubes.volobj->texheight));
		
		maxsize =  max(mcubes.volobj->xsize, mcubes.volobj->ysize);
		maxsize =  max(maxsize, mcubes.volobj->zsize);
		Vector voxrescale = Vector(mcubes.volobj->xsize/maxsize, mcubes.volobj->zsize/maxsize, mcubes.volobj->ysize/maxsize);

		vscale = vscale*voxrescale;
		mcubes.mesh->scalev(vscale);

		//update mesh
		volrender.meshlist.meshes[index].glextensions = volrender.meshlist.glextensions;
		volrender.meshlist.meshes[index].init_vbo();

		//reset
		mcubes.clear();
		mcubes.volobj = NULL;
		mcubes.mesh = NULL;
	}

	//emit redraw event
	redraw_event();
}
void QtGLWidget::isosurface_load_slot(QString fn)
{
	printf("isosurface_load_slot\n");

	Q3ProgressDialog* progressbar = new Q3ProgressDialog("Loading OBJ file...", "Cancel", 2, 0, "progress", TRUE);		
	progressbar->setCaption("Please Wait");
    progressbar->setProgress(1);

	volrender.meshlist.load(fn);

	isosurface_addnew_signal(volrender.meshlist.names[volrender.meshlist.names.size()-1], volrender.meshlist.colours[volrender.meshlist.names.size()-1]);
   
	progressbar->setProgress(2);
	delete progressbar;
	progressbar=NULL;
}
void QtGLWidget::isosurface_save_slot(void)
{
	printf("isosurface_save_slot\n");

	QString fn = QFileDialog::getExistingDirectory ( this, "Please pick a save directory.", currentworkingdir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if(fn.isEmpty()) return;
	
	fn+="/";
	
	volrender.meshlist.save(fn);
}
void QtGLWidget::isosurface_crop_slot(int selected)
{
	printf("selected: %d\n", selected);
	if(volobject.texture3d==NULL) return;

	int index=0;
	Vector v = Vector(0,0,0,0);
	
	Q3ProgressDialog* progressbar = new Q3ProgressDialog("Cropping the Volume Data...", "Cancel", volobject.texdepth, 0, "progress", TRUE);		
	progressbar->setCaption("Please Wait");
	progressbar->setMinimumDuration(10);

	float halfw = volobject.texwidth/2.0;
	float halfh = volobject.texheight/2.0;
	float halfd = volobject.texdepth/2.0;
	int r,g,b;

	int v0, v1, v2;
	float maxsize =  max(volobject.xsize, volobject.ysize);
	maxsize =  max(maxsize, volobject.zsize);

	Vector c;
	Vector res = Vector((volobject.xsize/maxsize), (volobject.ysize/maxsize), (volobject.zsize/maxsize));

	for(int i=0; i<volrender.meshlist.meshes[selected].nf; i++)
	{
		v0 = volrender.meshlist.meshes[selected].localfaces[i].vertindex[0];
		v1 = volrender.meshlist.meshes[selected].localfaces[i].vertindex[1];
		v2 = volrender.meshlist.meshes[selected].localfaces[i].vertindex[2];
		
		volrender.meshlist.meshes[selected].localfaces[i].facenormal.get_normal(volrender.meshlist.meshes[selected].vert[v0], volrender.meshlist.meshes[selected].vert[v1], volrender.meshlist.meshes[selected].vert[v2]);
	}
	
	for(int z = 0; z <volobject.texdepth; z++)
	{	
		//progress(z, volobject.texdepth);
		progressbar->setProgress(z);
		if (progressbar->wasCancelled()) break;

		for(int x = 0; x <volobject.texwidth; x++)
		{
			for(int y = 0; y <volobject.texheight; y++)
			{
				index = ((((z*volobject.texheight)+y)*volobject.texwidth)+x);
				
				r = volobject.get_value(index, 0);
				g = volobject.get_value(index, 1);
				b = volobject.get_value(index, 2);

				if(r+g+b>0)
				{
					v.x = x-halfw;
					v.y = y-halfh;
					v.z = z-halfd;
					v.w = 0.0;
					
					for(int i=0; i<volrender.meshlist.meshes[selected].nf; i++)
					{
						v0 = volrender.meshlist.meshes[selected].localfaces[i].vertindex[0];
						v1 = volrender.meshlist.meshes[selected].localfaces[i].vertindex[1];
						v2 = volrender.meshlist.meshes[selected].localfaces[i].vertindex[2];
						
						c = volrender.meshlist.meshes[selected].vert[v0]+volrender.meshlist.meshes[selected].vert[v1]+volrender.meshlist.meshes[selected].vert[v2];
						c /= 3.0;
												
						if(volrender.meshlist.meshes[selected].localfaces[i].facenormal.dot(v-c)<0.0)
						{
							//printf("BOH\n");
							if(volrender.volrenderview.redclip_toggle==1) volobject.texture3d[3*index+0] = GLubyte(0);
							if(volrender.volrenderview.greenclip_toggle==1) volobject.texture3d[3*index+1] = GLubyte(0);
							if(volrender.volrenderview.blueclip_toggle==1) volobject.texture3d[3*index+2] = GLubyte(0); 							
						}
					}
					
					/*for(int i=0; i<volrender.volrenderview.cutting_planes.planes.size(); i++)
					{
						c = volrender.volrenderview.cutting_planes.planes[i].centre/res;

						if(volrender.volrenderview.cutting_planes.planes[i].normal.dot(v-c)<0.0 && v.w!=-1.0)
						{
							v.w = -1.0;

							if(volrender.volrenderview.redclip_toggle==1) volobject.texture3d[3*index+0] = GLubyte(0);
							if(volrender.volrenderview.greenclip_toggle==1) volobject.texture3d[3*index+1] = GLubyte(0);
							if(volrender.volrenderview.blueclip_toggle==1) volobject.texture3d[3*index+2] = GLubyte(0); 
						}
					}*/
				}
			}
		}
	}

	progressbar->setProgress(volobject.texdepth);
	delete progressbar;

	volrender.load_TextureMap();

	//emit frames_redraw_signal();
	redraw_event();

}

//===========================================================================================================
void QtGLWidget::datasetview_loadFolder_slot(QString dn)
{
	emit mw_openImageDir_sinal(dn);
	redraw_event();
}
void QtGLWidget::datasetview_updatethumbnail_slot(int i)
{
	redraw_event();

	QPixmap pixmap;
	pixmap.convertFromImage(volrender.render_framebuffer(false));

	emit datasetview_setThumbnail_signal(i, pixmap);
}
//===========================================================================================================
void QtGLWidget::renderview_renderview_slot(VolumeRenderView view)
{
	if(volobject.texture3d==NULL) return;

	//volrender.offscreen_toggle = 1;

	VolumeRenderView tempview = volrender.volrenderview;
	volrender.volrenderview = view;
	volrender.update_transfer_functions(view);

	if(volrender.volrenderview.projection_toggle==PERSP_PROJECTION) volrender.volrenderview.lookat();

	//camera mode update signal
	if(volrender.volrenderview.projection_toggle==PERSP_PROJECTION)
	{
		emit camerasettings_setmode_signal(1, volrender.volrenderview.camera.fov);
		emit camerasettings_toggleUI_signal(false);
	}
	else
	{
		emit camerasettings_setmode_signal(0, volrender.volrenderview.camera.fov);
		emit camerasettings_toggleUI_signal(true);
	}

	//emit tfunc_updateAll_signal(view.luminance, view.alpha, view.redluminance, view.redalpha, view.greenluminance, view.greenalpha, view.blueluminance, view.bluealpha);
		
	makeCurrent();
	redraw_event();

	QPixmap* pixmap = new QPixmap();
//	pixmap->resize(512, 512);
//	pixmap->fill(Qt::black);
	pixmap->convertFromImage(volrender.render_framebuffer(false));

	emit renderview_addsavedview_signal(pixmap);

	volrender.volrenderview = tempview;
	redraw_event();

	delete pixmap;
	//resizeGL(width(), height());
	//volrender.offscreen_toggle = 0;
}
void QtGLWidget::renderview_saveview_slot(void)
{
	if(volobject.texture3d==NULL) return;
	//printf("SAVE RENDER VIEW\n");
	
	QPixmap* pixmap = new QPixmap();
	//pixmap->resize(512, 512);
	//pixmap->fill(Qt::black);
	pixmap->convertFromImage(volrender.render_framebuffer(false));

	emit renderview_addview_signal(pixmap, volrender.volrenderview);
	delete pixmap;
}
void QtGLWidget::renderview_updateview_slot(int sel)
{
	if(volobject.texture3d==NULL) return;
	//printf("UPDATE RENDER VIEW\n");
	
	QPixmap* pixmap = new QPixmap();
	//pixmap->resize(512, 512);
	//pixmap->fill(Qt::black);
	pixmap->convertFromImage(volrender.render_framebuffer(false));

	emit renderview_updateview_signal(sel, pixmap, volrender.volrenderview);
	delete pixmap;
}

void QtGLWidget::renderview_setview_slot(VolumeRenderView view)
{
	//printf("SETTING VIEW\n");
	volrender.volrenderview = view;
	volrender.update_transfer_functions(view);
	
	/*printf("zoom: %f\n", view.zoom);
	printf("tx: %f\n", view.translate.x);
	printf("ty: %f\n", view.translate.y);
	printf("zoom: %f\n", volrender.volrenderview.zoom);
	printf("tx: %f\n", volrender.volrenderview.translate.x);
	printf("ty: %f\n", volrender.volrenderview.translate.y);*/

	//camera mode update signal
	if(volrender.volrenderview.projection_toggle==PERSP_PROJECTION)
	{
		emit camerasettings_setmode_signal(1, volrender.volrenderview.camera.fov);
		emit camerasettings_toggleUI_signal(false);
	}
	else
	{
		emit camerasettings_setmode_signal(0, volrender.volrenderview.camera.fov);
		emit camerasettings_toggleUI_signal(true);
	}

	if(volrender.volrenderview.projection_toggle==PERSP_PROJECTION) volrender.volrenderview.lookat();

	emit tfunc_updateAll_signal(view.luminance, view.alpha, view.redluminance, view.redalpha, view.greenluminance, view.greenalpha, view.blueluminance, view.bluealpha);

	redraw_event();
}
void QtGLWidget::renderview_interpolate_slot(vector< VolumeRenderView > vv)
{
	if(vv.size()<2) return;

	volumeviews.clear();

	for(int i=0; i<vv.size(); i++)
	volumeviews.push_back(vv[i]);
}

//===========================================================================================================
void QtGLWidget::sliceframe_pixel_slot(int x, int y, int z, int mode)
{
	if(volobject.texture3d==NULL) return;

	volrender.cursor3D_texturespace.x = x;
	volrender.cursor3D_texturespace.y = y;
	volrender.cursor3D_texturespace.z = z;

	int index = volobject.get_index1D(volrender.cursor3D_texturespace.x, volrender.cursor3D_texturespace.y, volrender.cursor3D_texturespace.z);
	float voxval = volobject.texture3d[3*index];

	volrender.cursor3D_screenspace = volobject.imagespacetoworldspace(volrender.cursor3D_texturespace);

	if(volrender.cursor_toggle==1) sliceframe_updatepixel();
}
void QtGLWidget::sliceframe_updatepixel(void)
{
	if(volobject.texture3d==NULL) return;

	volrender.cursor3D_texturespace = volobject.worldspacetoimagespace(volrender.cursor3D_screenspace);

	volrender.x_slice = volrender.cursor3D_texturespace.x;
	volrender.y_slice = volrender.cursor3D_texturespace.y;
	volrender.z_slice = volrender.cursor3D_texturespace.z;

	QString s;
	volrender.cursor3D = "(";
	s.setNum(volrender.cursor3D_texturespace.x);
	volrender.cursor3D +=s;
	volrender.cursor3D +=",";
	s.setNum(volrender.cursor3D_texturespace.y);
	volrender.cursor3D +=s;
	volrender.cursor3D +=",";
	s.setNum(volrender.cursor3D_texturespace.z);
	volrender.cursor3D +=s;
	volrender.cursor3D +=")";

	redraw_event();
	emit frames_redraw_signal();
	emit updatestatus_cursor_signal();
}
//===========================================================================================================
void QtGLWidget::camerasettings_step_slot(int step)
{
	printf("camerasettings_step_signal!!!\n");

	volrender.cam_step_size = step;
}
void QtGLWidget::camerasettings_mode_slot(int id)
{
	printf("camerasettings_mode_slot!!!\n");

	if(id==0)
	{
		volrender.volrenderview.at = Vector(0,0,0);
		volrender.volrenderview.eye = Vector(0,0,1);
		volrender.volrenderview.arcball.setIdentity();

		//ortho
		volrender.volrenderview.projection_toggle = ORTHO_PROJECTION;

		emit camerasettings_toggleUI_signal(true);
		releaseKeyboard();
	}
	else
	{
		//perspective
		volrender.volrenderview.projection_toggle = PERSP_PROJECTION;
		
		volrender.volrenderview.at = Vector(0,0,0);
		volrender.volrenderview.eye = Vector(0,0,1);
		volrender.volrenderview.eye.z += (volrender.volrenderview.zoom+volobject.texwidth*1.2);
		volrender.volrenderview.lookat();

		emit camerasettings_toggleUI_signal(false);
		grabKeyboard();

		/*Vector view_dir = volrender.volrenderview.eye-volrender.volrenderview.at;
		view_dir.normalize();


		volrender.volrenderview.eye = volrender.volrenderview.zoom*(view_dir+240);
		volrender.volrenderview.at = volrender.volrenderview.zoom*(view_dir+240);
		volrender.volrenderview.lookat();*/
	}
	redraw_event();
}
void QtGLWidget::camerasettings_fov_slot(int fov)
{
	printf("camerasettings_fov_slot\n");

	if(volrender.volrenderview.projection_toggle!=PERSP_PROJECTION) return;
	volrender.volrenderview.camera.fov = fov;
	redraw_event();
}
//===========================================================================================================
void QtGLWidget::meausre_setuprendering_slot(void)
{
	if(!volobject.texture3d)
	{
		//printf("SETUP RENDERING\n");

		Vector aabb[8];

		volrender.measures.evaluate_aabb(aabb);

		Vector l = aabb[4]-aabb[1];
		
		volrender.volrenderview.zoom = l.length();
		volrender.update_FBOsize(1024);
	}
	else
	{
		//printf("DO NOTTHING\n");
	}
	redraw_event();
}
void QtGLWidget::measure_modify_rotatecurrent(int mode, float angle)
{
	//printf("rotate\n");

	int helper=0;
	volrender.current_measure_centre = Vector(0,0,0);
	for(int i=0; i<volrender.currentMeasureObject.vertices.size(); i++)
	{
		Vector v = volrender.currentMeasureObject.vertices[i];
		volrender.current_measure_centre += v;
		helper++;
	}
	volrender.current_measure_centre /= (float)helper;

	Vector axis, axis2;
	if(mode==0) axis=Vector(1,0,0);
	else if(mode==1) axis=Vector(0,1,0);
	else if(mode==2) axis=Vector(0,0,1);

	Matrix4fT mat = Matrix4fInverse(&volrender.volrenderview.arcball.Transform);
	//Matrix4fMulVector4f(Matrix4fInverse(&volrender.volrenderview.arcball.Transform), &axis);
	RotateVector(mat.M,axis,axis2);
	axis = axis2;

	for(int i=0; i<volrender.currentMeasureObject.vertices.size(); i++)
	{
		Vector v = volrender.currentMeasureObject.vertices[i];
		v -= volrender.current_measure_centre;
		//Matrix4fMulVector4f(Matrix4fInverse(&volrender.volrenderview.arcball.Transform), &v);
		v.rotate3D(axis, angle);
		//Matrix4fMulVector4f(volrender.volrenderview.arcball.Transform, &v);
		v += volrender.current_measure_centre;
		volrender.currentMeasureObject.vertices[i] = v;
	}

	updateGL();
}
void QtGLWidget::measure_modify_zoomcurrent(float measure_zoom)
{
	int helper=0;
	volrender.current_measure_centre = Vector(0,0,0);
	for(int i=0; i<volrender.currentMeasureObject.vertices.size(); i++)
	{
		Vector v = volrender.currentMeasureObject.vertices[i];
		volrender.current_measure_centre += v;
		helper++;
	}
	volrender.current_measure_centre /= (float)helper;

	for(int i=0; i<volrender.currentMeasureObject.vertices.size(); i++)
	{
		Vector v = volrender.currentMeasureObject.vertices[i];
		v -= volrender.current_measure_centre;
		v *= measure_zoom;
		v += volrender.current_measure_centre;
		volrender.currentMeasureObject.vertices[i] = v;
	}
	updateGL();
}
void QtGLWidget::measure_modify_start_slot(void)
{
	volrender.measure_modify=1;
	updateGL();
}
void QtGLWidget::measure_modify_stop_slot(void)
{
	volrender.current_measure_centre = Vector(0.0, 0.0, 0.0);
	volrender.measure_modify=0;
	emit measure_updatecurrent_signal(volrender.currentMeasureObject);
}
void QtGLWidget::measure_clearcurrent_slot(void)
{
	volrender.currentMeasureObject.clear();
}	
void QtGLWidget::measure_clearall_slot(void)
{
	//printf("CLEARING\n");
	
	volrender.current_measure_drawmode=MEASURE_DRAWMODE_NONE;
	volrender.currentMeasureObject.clear();
	volrender.measures.clear();
}
void QtGLWidget::measure_start_slot(void)
{
	int selected = -1;
	for(int i=0; i<6; i++)
	{
		if(volrender.volrenderview.cutting_planes.selected[i]==1) selected=1;
	}
	
	if(volrender.current_measure_editmode==MEASURE_EDIT_FACES) volrender.measure_current_facemode=1;

	volrender.measure_mode = 1;
	volrender.measure_editmode = 1;
	redraw_event();
}
void QtGLWidget::measure_pause_slot(void)
{
	if(volrender.current_measure_editmode==MEASURE_EDIT_FACES && volrender.measure_mode==1 && volrender.current_face.size()>2)
	{
		volrender.measure_current_facemode=0;
		volrender.currentMeasureObject.addFace(volrender.current_face);
		volrender.current_face.clear();
		emit measure_endvertface_signal();
		emit measure_updatecurrent_signal(volrender.currentMeasureObject);
	}

	volrender.measure_mode = 0;
	volrender.measure_editmode = 0;
	volrender.measure_magnetline_vert_old = -1;
	volrender.measure_magnetline_vert = -1;
	redraw_event();
}
void QtGLWidget::measure_edit_slot(void)
{
	volrender.measure_editmode = 1;
	volrender.measure_mode = 0;
	redraw_event();
}
void QtGLWidget::measure_stop_slot(void)
{
	volrender.measure_mode = -1;
	volrender.measure_editmode = -1;

	volrender.current_measure_index = -1;
	volrender.current_measure_vertindex = -1;

	volrender.measure_magnetline_vert_old = -1;
	volrender.measure_magnetline_vert = -1;
	//cutplane_unselect_slot();
	//emit cutplane_unselect_signal();

	redraw_event();
}
void QtGLWidget::measure_selected_vertindex_slot(int index)
{
	//printf("Current vert: %d\n", index);
	volrender.current_measure_vertindex = index;
	volrender.current_measure_edgeindex = -1;
	volrender.current_measure_faceindex = -1;
	//volrender.measure_magnetline_vert_old = -1;
	//volrender.measure_magnetline_vert = -1;
	redraw_event();
	
	//printf("measure_selected_vertindex_slot current_measure_vertindex: %d\n",volrender.current_measure_vertindex);
}
void QtGLWidget::measure_selected_faceindex_slot(int index)
{
	//printf("Current face: %d\n", index);
	volrender.current_measure_faceindex = index;
	volrender.current_measure_edgeindex = -1;
	volrender.current_measure_vertindex = -1;
	//volrender.measure_magnetline_vert_old = -1;
	//volrender.measure_magnetline_vert = -1;
	redraw_event();
}
void QtGLWidget::measure_selected_edgeindex_slot(int index)
{
	//printf("Current edge: %d\n", index);
	volrender.current_measure_edgeindex = index;
	volrender.current_measure_faceindex = -1;
	volrender.current_measure_vertindex = -1;
	//volrender.measure_magnetline_vert_old = -1;
	//volrender.measure_magnetline_vert = -1;

	redraw_event();
}
void QtGLWidget::measure_rmv_currentvert_slot(MeasureObject obj, int index)
{
	volrender.currentMeasureObject = obj;

	if(volrender.currentMeasureObject.vertices.empty()) return;
	if(index==-1) return;

	vector<int> todelete;
	int delindex;

//	printf("volrender.currentMeasureObject.vertices: %d\n", volrender.currentMeasureObject.vertices.size());
//	printf("volrender.currentMeasureObject.vertex_labels: %d\n", volrender.currentMeasureObject.vertex_labels.size());

	//===============================
	//VERTEX
	//===============================
	//delete our vertex
	if(index==0) volrender.currentMeasureObject.vertices.erase(volrender.currentMeasureObject.vertices.begin());
	else volrender.currentMeasureObject.vertices.erase(volrender.currentMeasureObject.vertices.begin()+index);
	
	//VERTEX LABELS
	if(!volrender.currentMeasureObject.vertex_labels.empty() && index<volrender.currentMeasureObject.vertex_labels.size())
	{
		//printf("VERTEX LABELS delete\n");
		volrender.currentMeasureObject.vertex_labels[index].clear();
		if(index==0) volrender.currentMeasureObject.vertex_labels.erase(volrender.currentMeasureObject.vertex_labels.begin());
		else volrender.currentMeasureObject.vertex_labels.erase(volrender.currentMeasureObject.vertex_labels.begin()+index);
	}

	//===============================
	//EDGE
	//===============================
	//and delete any edges which had the vertex
	for(int i=0; i<volrender.currentMeasureObject.edges.size(); i++)
	{
		int v0 = volrender.currentMeasureObject.edges[i].v0;
		int v1 = volrender.currentMeasureObject.edges[i].v1;

		if(v0==index || v1==index)	todelete.push_back(i);
	}
	//printf("edges to delete: %d\n", todelete.size());
	for(int i=todelete.size()-1; i>=0; i--)
	{
		delindex = todelete[i];

		if(delindex==0) volrender.currentMeasureObject.edges.erase(volrender.currentMeasureObject.edges.begin());
		else volrender.currentMeasureObject.edges.erase(volrender.currentMeasureObject.edges.begin()+(delindex));
	
		//EDGE LABELS
		if(!volrender.currentMeasureObject.edge_labels.empty() && delindex<volrender.currentMeasureObject.edge_labels.size())
		{
			volrender.currentMeasureObject.edge_labels[delindex].clear();
			if(delindex==0) volrender.currentMeasureObject.edge_labels.erase(volrender.currentMeasureObject.edge_labels.begin());
			else volrender.currentMeasureObject.edge_labels.erase(volrender.currentMeasureObject.edge_labels.begin()+delindex);
		}
	}
	todelete.clear();
	//and make sure to update indices for remaining edge vertices
	for(int i=0; i<volrender.currentMeasureObject.edges.size(); i++)
	{
		int v0 = volrender.currentMeasureObject.edges[i].v0;
		int v1 = volrender.currentMeasureObject.edges[i].v1;

		if(v0>index) volrender.currentMeasureObject.edges[i].v0--;
		if(v1>index) volrender.currentMeasureObject.edges[i].v1--;
	}

	//===============================
	//FACE
	//===============================
	//delete any faces that have the vertex
	for(int i=0; i<volrender.currentMeasureObject.faces.size(); i++)
	{
		bool found=false;
		for(int ii=0; ii<volrender.currentMeasureObject.faces[i].size(); ii++)
		{
			if(volrender.currentMeasureObject.faces[i][ii]==index) found=true;
		}

		if(found)	todelete.push_back(i);
	}
//	printf("faces to delete: %d\n", todelete.size());
	for(int i=todelete.size()-1; i>=0; i--)
	{
		delindex = todelete[i];

		volrender.currentMeasureObject.faces[delindex].clear();

		if(delindex==0) volrender.currentMeasureObject.faces.erase(volrender.currentMeasureObject.faces.begin());
		else volrender.currentMeasureObject.faces.erase(volrender.currentMeasureObject.faces.begin()+(delindex));

		//FACE LABELS
		if(!volrender.currentMeasureObject.face_labels.empty() && delindex<volrender.currentMeasureObject.face_labels.size())
		{
			volrender.currentMeasureObject.face_labels[delindex].clear();
			if(delindex==0) volrender.currentMeasureObject.face_labels.erase(volrender.currentMeasureObject.face_labels.begin());
			else volrender.currentMeasureObject.face_labels.erase(volrender.currentMeasureObject.face_labels.begin()+delindex);
		}
	}
	todelete.clear();
	//and make sure to update indices for remaining face vertices
	for(int i=0; i<volrender.currentMeasureObject.faces.size(); i++)
	{
		for(int ii=0; ii<volrender.currentMeasureObject.faces[i].size(); ii++)
		{
			if(volrender.currentMeasureObject.faces[i][ii]>index) volrender.currentMeasureObject.faces[i][ii]--;
		}
	}

	volrender.measure_magnetline_vert_old = volrender.currentMeasureObject.vertices.size()-1;
	emit measure_updatecurrent_signal(volrender.currentMeasureObject);
	redraw_event();
}
void QtGLWidget::measure_rmv_currentface_slot(int f)
{
	if(volrender.currentMeasureObject.faces.empty()) return;
	if(f==-1) return;

	//printf("ERASING FACE: %d\n", f);
	//printf("NUMB FACES: %d\n", volrender.currentMeasureObject.faces.size());
	if(f==0) volrender.currentMeasureObject.faces.erase(volrender.currentMeasureObject.faces.begin());
	else volrender.currentMeasureObject.faces.erase(volrender.currentMeasureObject.faces.begin()+f);

	//FACE LABELS
	if(!volrender.currentMeasureObject.face_labels.empty())
	{
	//	printf("DEL FACELABEL\n");
		volrender.currentMeasureObject.face_labels[f].clear();
		if(f==0) volrender.currentMeasureObject.face_labels.erase(volrender.currentMeasureObject.face_labels.begin());
		else volrender.currentMeasureObject.face_labels.erase(volrender.currentMeasureObject.face_labels.begin()+f);
	}

	emit measure_updatecurrent_signal(volrender.currentMeasureObject);
	redraw_event();
}
void QtGLWidget::measure_rmv_currentedge_slot(int e)
{
	if(volrender.currentMeasureObject.edges.empty()) return;
	if(e==-1) return;

	vector<int> todelete;
	int delindex;

	//printf("ERASING EDGE: %d\n", e);
	//printf("NUMB EDGES: %d\n", volrender.currentMeasureObject.edges.size());
	
	//===============================
	//EDGE
	//===============================
	//delete our edge
	int v0 = volrender.currentMeasureObject.edges[e].v0;
	int v1 = volrender.currentMeasureObject.edges[e].v1;
	volrender.currentMeasureObject.edges.erase(volrender.currentMeasureObject.edges.begin()+e);
	
	//EDGE LABELS
	if(!volrender.currentMeasureObject.edge_labels.empty())
	{
		volrender.currentMeasureObject.edge_labels[e].clear();
		if(e==0) volrender.currentMeasureObject.edge_labels.erase(volrender.currentMeasureObject.edge_labels.begin());
		else volrender.currentMeasureObject.edge_labels.erase(volrender.currentMeasureObject.edge_labels.begin()+e);
	}

	//===============================
	//FACE
	//===============================
	//delete any faces that have the edge vertices
	for(int i=0; i<volrender.currentMeasureObject.faces.size(); i++)
	{
		int found=0;
		for(int ii=0; ii<volrender.currentMeasureObject.faces[i].size(); ii++)
		{
			if(volrender.currentMeasureObject.faces[i][ii]==v0) found++;
			if(volrender.currentMeasureObject.faces[i][ii]==v1) found++;
		}

		if(found==2)	todelete.push_back(i);
	}
	//printf("faces to delete: %d\n", todelete.size());
	for(int i=todelete.size()-1; i>=0; i--)
	{
		delindex = todelete[i];

		volrender.currentMeasureObject.faces[delindex].clear();

		if(delindex==0) volrender.currentMeasureObject.faces.erase(volrender.currentMeasureObject.faces.begin());
		else volrender.currentMeasureObject.faces.erase(volrender.currentMeasureObject.faces.begin()+(delindex));

		//FACE LABELS
		if(!volrender.currentMeasureObject.face_labels.empty())
		{
			volrender.currentMeasureObject.face_labels[delindex].clear();
			if(delindex==0) volrender.currentMeasureObject.face_labels.erase(volrender.currentMeasureObject.face_labels.begin());
			else volrender.currentMeasureObject.face_labels.erase(volrender.currentMeasureObject.face_labels.begin()+delindex);
		}
	}
	todelete.clear();

	volrender.measure_magnetline_vert_old = volrender.currentMeasureObject.vertices.size()-1;
	emit measure_updatecurrent_signal(volrender.currentMeasureObject);
	redraw_event();
}
void QtGLWidget::measure_rmv_currentlabel_slot(int index, bool delete_geometry)
{
	vector < int > geometry_to_delete;

//	printf("LABEL INDEX: %d\n", index);

	list<string>	vertlist;
	list<string>	edgelist;
	list<string>	facelist;

	//===============================
	// VERTEX LABELS
	//===============================
	for(int i=0; i<volrender.currentMeasureObject.vertex_labels.size(); i++)
		for(int j=0; j<volrender.currentMeasureObject.vertex_labels[i].size(); j++)
			vertlist.push_back(volrender.currentMeasureObject.vertex_labels[i][j]);

	vertlist.sort();
	vertlist.unique();

	//===============================
	// EDGE LABELS
	//===============================
	for(int i=0; i<volrender.currentMeasureObject.edge_labels.size(); i++)
		for(int j=0; j<volrender.currentMeasureObject.edge_labels[i].size(); j++)
			edgelist.push_back(volrender.currentMeasureObject.edge_labels[i][j]);

	edgelist.sort();
	edgelist.unique();

	//===============================
	// FACE LABELS
	//===============================
	for(int i=0; i<volrender.currentMeasureObject.face_labels.size(); i++)
		for(int j=0; j<volrender.currentMeasureObject.face_labels[i].size(); j++)
			facelist.push_back(volrender.currentMeasureObject.face_labels[i][j]);

	facelist.sort();
	facelist.unique();

//	printf("VERTLABEL: %d\n", vertlist.size());
//	printf("EDGELABEL: %d\n", edgelist.size());
//	printf("FACELABEL: %d\n", facelist.size());

	int label_type = -1;
	if(index<vertlist.size())
	{
		label_type = 1;
	//	printf("VERTEXLABEL\n");
		list<string>::iterator it=vertlist.begin();
		for(int i=0; i<index; i++) ++it;

		QString label_string = QString::fromStdString(*it);
	//	printf("vert label: %s\n", label_string.toAscii().constData());

		for(int i=0; i<volrender.currentMeasureObject.vertex_labels.size(); i++)
		{
			int todelete = -1;
			for(int j=0; j<volrender.currentMeasureObject.vertex_labels[i].size(); j++)
			{
				if(volrender.currentMeasureObject.vertex_labels[i][j]==label_string.toAscii().constData())
				{
	//				printf("index found: %d\n", j);
					todelete = j;
					j = volrender.currentMeasureObject.vertex_labels[i].size();
				}
			}
			
			if(todelete!=-1)
			{
				volrender.currentMeasureObject.vertex_labels[i].erase(volrender.currentMeasureObject.vertex_labels[i].begin()+todelete);
				if(delete_geometry) geometry_to_delete.push_back(i);
			}
		}
	}
	else if(index<vertlist.size()+edgelist.size())
	{
		label_type = 2;
	//	printf("EDGELABEL\n");
		list<string>::iterator it=edgelist.begin();
		for(int i=0; i<index-vertlist.size(); i++) ++it;

		QString label_string = QString::fromStdString(*it);
	//	printf("vert label: %s\n", label_string.toAscii().constData());
		
		for(int i=0; i<volrender.currentMeasureObject.edge_labels.size(); i++)
		{
			int todelete = -1;
			for(int j=0; j<volrender.currentMeasureObject.edge_labels[i].size(); j++)
			{
				if(volrender.currentMeasureObject.edge_labels[i][j]==label_string.toAscii().constData())
				{
		//			printf("index found: %d\n", j);
					todelete = j;
					j = volrender.currentMeasureObject.edge_labels[i].size();
				}
			}
			
			if(todelete!=-1)
			{
				volrender.currentMeasureObject.edge_labels[i].erase(volrender.currentMeasureObject.edge_labels[i].begin()+todelete);
				if(delete_geometry) geometry_to_delete.push_back(i);
			}
		}	
	}
	else
	{
		label_type = 3;

	//	printf("FACELABEL\n");
		list<string>::iterator it=facelist.begin();
		for(int i=0; i<index-vertlist.size()-edgelist.size(); i++) ++it;


		QString label_string = QString::fromStdString(*it);
		//printf("face label: %s\n", label_string.toAscii().constData());
		
		for(int i=0; i<volrender.currentMeasureObject.face_labels.size(); i++)
		{
			int todelete = -1;
			for(int j=0; j<volrender.currentMeasureObject.face_labels[i].size(); j++)
			{
				if(volrender.currentMeasureObject.face_labels[i][j]==label_string.toAscii().constData())
				{
			//		printf("index found: %d\n", j);
					todelete = j;
					j = volrender.currentMeasureObject.face_labels[i].size();
				}
			}
			
			if(todelete!=-1)
			{
				volrender.currentMeasureObject.face_labels[i].erase(volrender.currentMeasureObject.face_labels[i].begin()+todelete);
				if(delete_geometry) geometry_to_delete.push_back(i);
			}
		}	
	}

	if(label_type!=-1)
	{
		for(int i=geometry_to_delete.size()-1; i>=0; i--)
		{
			int delindex = geometry_to_delete[i];

			printf("label_type: %d\n", label_type);
			printf("Deleting: %d\n", delindex);

			if(label_type==1) measure_rmv_currentvert_slot(volrender.currentMeasureObject, delindex);
			else if(label_type==2) measure_rmv_currentedge_slot(delindex);
			else if(label_type==3) measure_rmv_currentface_slot(delindex);
		}
	}

	emit measure_updatecurrent_signal(volrender.currentMeasureObject);
	redraw_event();
}
void QtGLWidget::measure_eval_triangles_slot(int index)
{
	/*if(volrender.current_measure_drawmode==4 && !volrender.measure_objects[index].empty())
	{		
		PCA pca;
		int i;
		
		for(i=0; i<volrender.measure_objects[index].size(); i++)
		{
			volrender.measure_objects[index][i].w=i;
			pca.data.push_back(volrender.measure_objects[index][i]);
		}

		pca.eval_PCA();
		pca.eval_aligned_data();

		float x, y, z;
		x=y=z=0;
		for (i=0; i<pca.aligned_data.size(); i++)
		{
			pca.aligned_data[i].round(4);
			x =+ pca.aligned_data[i].x;
			y =+ pca.aligned_data[i].y;
			z =+ pca.aligned_data[i].z;
		}

		int axis = -1;
		if(x==0) axis=0;
		else if(y==0) axis=1;
		else axis=2;

		vector < Vector > points2d;
		vector < Vector > result;

		for (i=0; i<pca.aligned_data.size(); i++)
		{
			if(axis==0)
			{
				points2d.push_back(Vector(pca.aligned_data[i].y, pca.aligned_data[i].z,0,i));
			}
			else if(axis==1)
			{
				points2d.push_back(Vector(pca.aligned_data[i].x, pca.aligned_data[i].z,0,i));
			}
			else if(axis==2)
			{
				points2d.push_back(Vector(pca.aligned_data[i].x, pca.aligned_data[i].y,0,i));
			}
		}

		Triangulate::Process(points2d,result);

		int numbtris = result.size()/3;
		for (i=0; i<numbtris; i++)
		{
			volrender.current_triangles.push_back((int)result[3*i+0].w);
			volrender.current_triangles.push_back((int)result[3*i+1].w);
			volrender.current_triangles.push_back((int)result[3*i+2].w);
		}
	
	}*/
}
void QtGLWidget::measure_evalcurrent_triangles_slot	(void)
{
	/*if(volrender.current_measure_drawmode==4 && !volrender.currentMeasureObject.vertices.empty())
	{
		volrender.current_triangles.clear();

		PCA pca;
		int i;
		
		for(i=0; i<volrender.currentMeasureObject.vertices.size(); i++)
		{
			volrender.currentMeasureObject.vertices[i].w=i;

			pca.data.push_back(volrender.currentMeasureObject.vertices[i]);
		}

		pca.eval_PCA();
		pca.eval_aligned_data();

		float x, y, z;
		x=y=z=0;
		for (i=0; i<pca.aligned_data.size(); i++)
		{
			pca.aligned_data[i].round(4);
			x =+ pca.aligned_data[i].x;
			y =+ pca.aligned_data[i].y;
			z =+ pca.aligned_data[i].z;
		}

		int axis = -1;
		if(x==0) axis=0;
		else if(y==0) axis=1;
		else axis=2;

		printf("(x:%f, y:%f, z:%f)\n", x, y, z);

		vector < Vector > points2d;
		vector < Vector > result;

		for (i=0; i<pca.aligned_data.size(); i++)
		{
			if(axis==0)
			{
				points2d.push_back(Vector(pca.aligned_data[i].y, pca.aligned_data[i].z,0,i));
			}
			else if(axis==1)
			{
				points2d.push_back(Vector(pca.aligned_data[i].x, pca.aligned_data[i].z,0,i));
			}
			else if(axis==2)
			{
				points2d.push_back(Vector(pca.aligned_data[i].x, pca.aligned_data[i].y,0,i));
			}
		}

		Triangulate::Process(points2d,result);

		int numbtris = result.size()/3;
		for (i=0; i<numbtris; i++)
		{
			printf("indices: %d %d %d \n", result[3*i+0].w, result[3*i+1].w, result[3*i+2].w);

			volrender.current_triangles.push_back((int)result[3*i+0].w);
			volrender.current_triangles.push_back((int)result[3*i+1].w);
			volrender.current_triangles.push_back((int)result[3*i+2].w);
		}
	}*/
}
void QtGLWidget::measure_section_toggle_slot(bool on)
{
	volrender.measure_section_toggle = on;

	if(on==false)
	{
		volrender.measure_section_face=-1;
		volrender.volrenderview.cutting_planes.reset_all();
	}
	else
	{
		measure_section_thickn_slot(volrender.measure_section_thickn);
	}

	redraw_event();
}
void QtGLWidget::measure_section_trans_slot(int trans)
{
	if(volrender.measure_section_toggle==false) return;

	float dTrans = trans - volrender.measure_section_trans;
	volrender.measure_section_trans = trans;
	//dTrans *= volrender.measure_section_thickn;

	volrender.volrenderview.cutting_planes.planes[0]._v1 += dTrans * volrender.volrenderview.cutting_planes.planes[0].normal;
	volrender.volrenderview.cutting_planes.planes[0]._v2 += dTrans * volrender.volrenderview.cutting_planes.planes[0].normal;
	volrender.volrenderview.cutting_planes.planes[0]._v3 += dTrans * volrender.volrenderview.cutting_planes.planes[0].normal;
	volrender.volrenderview.cutting_planes.planes[0]._v4 += dTrans * volrender.volrenderview.cutting_planes.planes[0].normal;
	volrender.volrenderview.cutting_planes.planes[0].eval_planeequation();
					
	volrender.volrenderview.cutting_planes.planes[1]._v1 += -dTrans * volrender.volrenderview.cutting_planes.planes[1].normal;
	volrender.volrenderview.cutting_planes.planes[1]._v2 += -dTrans * volrender.volrenderview.cutting_planes.planes[1].normal;
	volrender.volrenderview.cutting_planes.planes[1]._v3 += -dTrans * volrender.volrenderview.cutting_planes.planes[1].normal;
	volrender.volrenderview.cutting_planes.planes[1]._v4 += -dTrans * volrender.volrenderview.cutting_planes.planes[1].normal;
	volrender.volrenderview.cutting_planes.planes[1].eval_planeequation();

	volrender.volrenderview.cutting_planes.eval_planeintersections();
	
	redraw_event();
}
void QtGLWidget::measure_section_thickn_slot(int thickn)
{
	if(volrender.measure_section_toggle==false) return;
	if(volrender.measure_section_face==-1) return;
	if(volrender.measure_section_face>volrender.currentMeasureObject.faces.size()-1) return;

	volrender.measure_section_thickn = thickn;

	Matrix4x4 rot;
	Vector translate, centroid, rot_axis, v, face_axis;
					
	volrender.volrenderview.cutting_planes.reset_all();

	if(volrender.currentMeasureObject.faces[volrender.measure_section_face].size()<3) return;

	Face f;
	f.vert[0] = &volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[volrender.measure_section_face][0]];
	f.vert[1] = &volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[volrender.measure_section_face][1]];
	f.vert[2] = &volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[volrender.measure_section_face][2]];

	centroid = volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[volrender.measure_section_face][0]]+volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[volrender.measure_section_face][1]]+volrender.currentMeasureObject.vertices[volrender.currentMeasureObject.faces[volrender.measure_section_face][2]];
	centroid /= 3.0;

	f.evaluate_face_normal();
	face_axis = f.facenormal;
	face_axis.normalize();

	volrender.volrenderview.cutting_planes.planes[0].normal.get_normal(volrender.volrenderview.cutting_planes.planes[0]._v1, volrender.volrenderview.cutting_planes.planes[0]._v2, volrender.volrenderview.cutting_planes.planes[0]._v3);
	Vector worlaxis = volrender.volrenderview.cutting_planes.planes[0].normal;
	worlaxis.normalize();

	rot_axis.cross(face_axis, worlaxis);
	rot_axis.normalize();
	float angle = worlaxis.signed_angle(face_axis, rot_axis);
	v.build_rotatemat(rot_axis, angle, rot.m);

	volrender.volrenderview.cutting_planes.planes[0]._v1 = volrender.volrenderview.cutting_planes.planes[0]._v1.mult_matrix(rot.m);
	volrender.volrenderview.cutting_planes.planes[0]._v2 = volrender.volrenderview.cutting_planes.planes[0]._v2.mult_matrix(rot.m);
	volrender.volrenderview.cutting_planes.planes[0]._v3 = volrender.volrenderview.cutting_planes.planes[0]._v3.mult_matrix(rot.m);
	volrender.volrenderview.cutting_planes.planes[0]._v4 = volrender.volrenderview.cutting_planes.planes[0]._v4.mult_matrix(rot.m);
	volrender.volrenderview.cutting_planes.planes[0].normal.get_normal(volrender.volrenderview.cutting_planes.planes[0]._v1, volrender.volrenderview.cutting_planes.planes[0]._v2, volrender.volrenderview.cutting_planes.planes[0]._v3);
					
	volrender.volrenderview.cutting_planes.planes[0].centre = volrender.volrenderview.cutting_planes.planes[0]._v1+volrender.volrenderview.cutting_planes.planes[0]._v2+volrender.volrenderview.cutting_planes.planes[0]._v3;
	volrender.volrenderview.cutting_planes.planes[0].centre /= 3.0;
					
	translate = volrender.volrenderview.cutting_planes.planes[0].centre-centroid;
	volrender.volrenderview.cutting_planes.planes[0]._v1 -= translate;
	volrender.volrenderview.cutting_planes.planes[0]._v2 -= translate;
	volrender.volrenderview.cutting_planes.planes[0]._v3 -= translate;
	volrender.volrenderview.cutting_planes.planes[0]._v4 -= translate;
					
	volrender.volrenderview.cutting_planes.planes[0]._v1 -= (float)volrender.measure_section_thickn * volrender.volrenderview.cutting_planes.planes[0].normal;
	volrender.volrenderview.cutting_planes.planes[0]._v2 -= (float)volrender.measure_section_thickn * volrender.volrenderview.cutting_planes.planes[0].normal;
	volrender.volrenderview.cutting_planes.planes[0]._v3 -= (float)volrender.measure_section_thickn * volrender.volrenderview.cutting_planes.planes[0].normal;
	volrender.volrenderview.cutting_planes.planes[0]._v4 -= (float)volrender.measure_section_thickn * volrender.volrenderview.cutting_planes.planes[0].normal;

	volrender.volrenderview.cutting_planes.planes[0].eval_planeequation();

	volrender.volrenderview.cutting_planes.planes[1]._v1 = volrender.volrenderview.cutting_planes.planes[1]._v1.mult_matrix(rot.m);
	volrender.volrenderview.cutting_planes.planes[1]._v2 = volrender.volrenderview.cutting_planes.planes[1]._v2.mult_matrix(rot.m);
	volrender.volrenderview.cutting_planes.planes[1]._v3 = volrender.volrenderview.cutting_planes.planes[1]._v3.mult_matrix(rot.m);
	volrender.volrenderview.cutting_planes.planes[1]._v4 = volrender.volrenderview.cutting_planes.planes[1]._v4.mult_matrix(rot.m);
	volrender.volrenderview.cutting_planes.planes[1].normal.get_normal(volrender.volrenderview.cutting_planes.planes[1]._v1, volrender.volrenderview.cutting_planes.planes[1]._v2, volrender.volrenderview.cutting_planes.planes[1]._v3);
					
	volrender.volrenderview.cutting_planes.planes[1].centre = volrender.volrenderview.cutting_planes.planes[1]._v1+volrender.volrenderview.cutting_planes.planes[1]._v2+volrender.volrenderview.cutting_planes.planes[1]._v3;
	volrender.volrenderview.cutting_planes.planes[1].centre /= 3.0;
					
	translate = volrender.volrenderview.cutting_planes.planes[1].centre-centroid;
	volrender.volrenderview.cutting_planes.planes[1]._v1 -= translate;
	volrender.volrenderview.cutting_planes.planes[1]._v2 -= translate;
	volrender.volrenderview.cutting_planes.planes[1]._v3 -= translate;
	volrender.volrenderview.cutting_planes.planes[1]._v4 -= translate;
					
	volrender.volrenderview.cutting_planes.planes[1]._v1 -= (float)volrender.measure_section_thickn * volrender.volrenderview.cutting_planes.planes[1].normal;
	volrender.volrenderview.cutting_planes.planes[1]._v2 -= (float)volrender.measure_section_thickn * volrender.volrenderview.cutting_planes.planes[1].normal;
	volrender.volrenderview.cutting_planes.planes[1]._v3 -= (float)volrender.measure_section_thickn * volrender.volrenderview.cutting_planes.planes[1].normal;
	volrender.volrenderview.cutting_planes.planes[1]._v4 -= (float)volrender.measure_section_thickn * volrender.volrenderview.cutting_planes.planes[1].normal;

	volrender.volrenderview.cutting_planes.planes[1].eval_planeequation();

	volrender.volrenderview.cutting_planes.eval_planeintersections();

	redraw_event();
}
void QtGLWidget::measure_facealign_updateview_slot(bool state)
{
	volrender.measure_updateview_toggle=state;
}
void QtGLWidget::measure_facealign_orient_slot(bool state)
{
	volrender.measure_face_invert=state;
}
void QtGLWidget::measure_add_item_slot(MeasureObject obj, int draw_mode)
{
	volrender.current_measure_index = -1;
	
	volrender.measures.add(obj);

	volrender.currentMeasureObject.clear();
	redraw_event();
}
void QtGLWidget::measure_update_item_slot(int index, MeasureObject obj, int draw_mode)
{
	int i=0;
	volrender.current_measure_index = -1;

	volrender.measures.replace(index, obj);

	measure_cancel_slot();
	redraw_event();
}
void QtGLWidget::measure_cancel_slot(void)
{
	volrender.current_measure_index = -1;
	volrender.current_measure_vertindex = -1;

	volrender.currentMeasureObject.clear();

	volrender.current_face.clear();

	redraw_event();
}


void QtGLWidget::measure_update_visible_slot(int index, bool state)
{
	//printf("state: %d\n", (int) state);

	if(volrender.measures.visible.size()<=index) return;

	volrender.measures.visible[index] = state;
	redraw_event();
}
void QtGLWidget::measure_delete_item_slot(int index)
{
	volrender.current_measure_index = -1;
	
	volrender.measures.erase(index);

	redraw_event();
}
void QtGLWidget::measure_update_current_slot(MeasureObject obj)
{
	volrender.currentMeasureObject = obj;
	emit measure_updatecurrent_signal(volrender.currentMeasureObject);
	redraw_event();
}
void QtGLWidget::measure_update_current_slot(int index)
{
	volrender.current_measure_index = index;

	volrender.currentMeasureObject.clear();
	volrender.current_face.clear();

	volrender.currentMeasureObject = volrender.measures.measure_objects[index];

	emit measure_updatecurrent_signal(volrender.currentMeasureObject);

	redraw_event();
}
void QtGLWidget::measure_rebuild_current_edges(void)
{
	if(volrender.current_measure_drawmode!=MEASURE_DRAWMODE_MAGNETLINE) volrender.currentMeasureObject.edges.clear();

	Edge e;
	if(volrender.current_measure_drawmode==MEASURE_DRAWMODE_POINTS)
	{
	}
	else if(volrender.current_measure_drawmode==MEASURE_DRAWMODE_LINES)
	{
		for(int i=0; i<volrender.currentMeasureObject.vertices.size(); i++)
		{
			if(i%2==0)
			{
				if(i<volrender.currentMeasureObject.vertices.size()-1)
				{
					e.v1 = i;
					e.v0 = i+1;
					volrender.currentMeasureObject.addEdge(e);
				}
			}
		}
	}
	else if(volrender.current_measure_drawmode==MEASURE_DRAWMODE_CROP)
	{
		Edge e;
		for(int i=0; i<volrender.currentMeasureObject.vertices.size(); i++)
		{
			if(i<volrender.currentMeasureObject.vertices.size()-1)
			{
				e.v1 = i;
				e.v0 = i+1;
				volrender.currentMeasureObject.addEdge(e);
			}			
		}
		if(volrender.currentMeasureObject.vertices.size()>1)
		{
			e.v0 = volrender.currentMeasureObject.vertices.size()-1;
			e.v1 = 0;
			volrender.currentMeasureObject.addEdge(e);
		}
	}
	else if(volrender.current_measure_drawmode==MEASURE_DRAWMODE_MAGNETLINE || volrender.current_measure_editmode==MEASURE_EDIT_EDGES)
	{

		if(volrender.currentMeasureObject.vertices.size()>1)
		{
			if(volrender.currentMeasureObject.vertices.size()==2) volrender.measure_magnetline_vert_old = 0;

			Edge e;
			if(volrender.measure_magnetline_vert!=-1 && volrender.measure_magnetline_vert_old!=-1 && volrender.measure_magnetline_vert!=volrender.measure_magnetline_vert_old)
			{
			//	printf("building edges1\n");
				e.v0 = volrender.measure_magnetline_vert_old;
				e.v1 = volrender.measure_magnetline_vert;

				for(int i=0; i<volrender.currentMeasureObject.edges.size(); i++)
				{
					int v0 = volrender.currentMeasureObject.edges[i].v0;
					int v1 = volrender.currentMeasureObject.edges[i].v1;

					if(e.v1==v0 && e.v0==v1) return;
					if(e.v1==v1 && e.v0==v0) return;
				}

				volrender.currentMeasureObject.addEdge(e);
				volrender.measure_magnetline_vert_old = volrender.measure_magnetline_vert;

				//printf("edge: %d, %d\n", e.v0,e.v1);
			}
			else if(volrender.measure_magnetline_vert_old!=-1)
			{
				//printf("building edges2\n");
				e.v0 = volrender.currentMeasureObject.vertices.size()-1;
				e.v1 = volrender.measure_magnetline_vert_old;

				for(int i=0; i<volrender.currentMeasureObject.edges.size(); i++)
				{
					int v0 = volrender.currentMeasureObject.edges[i].v0;
					int v1 = volrender.currentMeasureObject.edges[i].v1;

					if(e.v1==v0 && e.v0==v1) return;
					if(e.v1==v1 && e.v0==v0) return;
				}

				volrender.currentMeasureObject.addEdge(e);
				volrender.measure_magnetline_vert_old=volrender.currentMeasureObject.vertices.size()-1;;

			//	printf("edge: %d, %d\n", e.v0,e.v1);
			}
		}
	}
}
void QtGLWidget::measure_update_current_editmode_slot(int editmode)
{
	volrender.current_measure_editmode  = editmode;

	//printf("measure_update_current_editmode_slot: %d\n", volrender.current_measure_editmode);
}
void QtGLWidget::measure_update_current_drawmode_slot(int drawmode)
{
	volrender.current_measure_drawmode  = drawmode;

	//printf("measure_update_current_drawmode_slot: %d\n", volrender.current_measure_drawmode);
	
	if(volrender.current_measure_drawmode!=MEASURE_DRAWMODE_MAGNETLINE)
	{
		measure_rebuild_current_edges();

		emit measure_updatecurrent_signal(volrender.currentMeasureObject);
	}

	//printf("measure_update_current_drawmode_slot: %d\n", volrender.current_measure_drawmode);

	captureDepthMap();

	redraw_event();
}
void QtGLWidget::measure_update_current_viewmode_slot(int viewmode)
{
	volrender.current_measure_viewmode  = viewmode;

	if(MEASURE_VIEW_VERTS)
	{
		volrender.current_measure_faceindex = -1;
		volrender.current_measure_edgeindex = -1;
	}
	else if(MEASURE_VIEW_EDGES)
	{
		volrender.current_measure_vertindex = -1;
		//volrender.measure_magnetline_vert_old = -1;
		//volrender.measure_magnetline_vert = -1;
		volrender.current_measure_faceindex = -1;
	}
	else
	{
		volrender.current_measure_vertindex = -1;
		//volrender.measure_magnetline_vert_old = -1;
		//volrender.measure_magnetline_vert = -1;
		volrender.current_measure_edgeindex = -1;
	}
}

void QtGLWidget::measure_setcutplane_slot(int selected, float translate, float rotang1, float rotang2)
{
//	printf("SET ITEM: %d %f %f %f \n", selected, translate, rotang1, rotang2);
	
	if(selected==-1) return;
	
	cutplane_resetall_slot();
	cutplane_select_slot(selected);
	
	volrender.volrenderview.cutting_planes.planes[selected].translate = translate;
	volrender.volrenderview.cutting_planes.planes[selected].angleX = rotang1;
	volrender.volrenderview.cutting_planes.planes[selected].angleY = rotang2;

	Vector v1cen, v2cen, v3cen, v4cen, n, cen, vcen;
	Matrix4x4 rot_matX, rot_matY, axis_rotmat;
	
	//============================================================================
	//TRANSLATE
	//============================================================================		
	Vector _v1 = volrender.volrenderview.cutting_planes.planes[selected]._v1;
	Vector _v2 = volrender.volrenderview.cutting_planes.planes[selected]._v2;
	Vector _v3 = volrender.volrenderview.cutting_planes.planes[selected]._v3;
	Vector _v4 = volrender.volrenderview.cutting_planes.planes[selected]._v4;
	Vector _normal = volrender.volrenderview.cutting_planes.planes[selected].normal;
	
	Vector vv_0 = (_v1+(volrender.volrenderview.cutting_planes.planes[selected].translate*_normal));		
	Vector vv_1 = (_v2+(volrender.volrenderview.cutting_planes.planes[selected].translate*_normal));		
	Vector vv_2 = (_v3+(volrender.volrenderview.cutting_planes.planes[selected].translate*_normal));		
	Vector vv_3 = (_v4+(volrender.volrenderview.cutting_planes.planes[selected].translate*_normal));		

	volrender.volrenderview.cutting_planes.planes[selected]._v1 = (vv_0);
	volrender.volrenderview.cutting_planes.planes[selected]._v2 = (vv_1);
	volrender.volrenderview.cutting_planes.planes[selected]._v3 = (vv_2);
	volrender.volrenderview.cutting_planes.planes[selected]._v4 = (vv_3);
	volrender.volrenderview.cutting_planes.planes[selected].eval_planeequation();

	//============================================================================
	//ANGLEX
	//============================================================================

	//translate our clipping plane to the origin
	cen = volrender.volrenderview.cutting_planes.planes[selected].v1+volrender.volrenderview.cutting_planes.planes[selected].v2+volrender.volrenderview.cutting_planes.planes[selected].v3+volrender.volrenderview.cutting_planes.planes[selected].v4;
	cen *= 0.25;
	v1cen = volrender.volrenderview.cutting_planes.planes[selected].v1-cen;
	v2cen = volrender.volrenderview.cutting_planes.planes[selected].v2-cen;
	v3cen = volrender.volrenderview.cutting_planes.planes[selected].v3-cen;
	v4cen = volrender.volrenderview.cutting_planes.planes[selected].v4-cen;
	
	//rotate our x axis by our new angle
	v1cen.build_rotatemat(volrender.volrenderview.cutting_planes.planes[selected].perpX, volrender.volrenderview.cutting_planes.planes[selected].angleX, &rot_matX.m[0]);
	volrender.volrenderview.cutting_planes.planes[selected].perpY = volrender.volrenderview.cutting_planes.planes[selected].perpY.mult_matrix(rot_matX.m);
	
	//and evaluate our new normal for the new plane rotation
	n.cross(volrender.volrenderview.cutting_planes.planes[selected].perpY,volrender.volrenderview.cutting_planes.planes[selected].perpX);
	
	//now we can evaluate the rotation matrix to go from our original set of axis to the other
	axis_rotmat.find_rotationfromaxis(volrender.volrenderview.cutting_planes.planes[selected].perpX, volrender.volrenderview.cutting_planes.planes[selected].perpY, n, volrender.volrenderview.cutting_planes.planes[selected].axisX, volrender.volrenderview.cutting_planes.planes[selected].axisY, volrender.volrenderview.cutting_planes.planes[selected].axisZ);
	volrender.volrenderview.cutting_planes.planes[selected].rot_mat = axis_rotmat;
	
	//rotate our plane vertices now
	volrender.volrenderview.cutting_planes.planes[selected].rot_mat = axis_rotmat;	
	v1cen = v1cen.mult_matrix(volrender.volrenderview.cutting_planes.planes[selected].rot_mat.m);
	v2cen = v2cen.mult_matrix(volrender.volrenderview.cutting_planes.planes[selected].rot_mat.m);
	v3cen = v3cen.mult_matrix(volrender.volrenderview.cutting_planes.planes[selected].rot_mat.m);
	v4cen = v4cen.mult_matrix(volrender.volrenderview.cutting_planes.planes[selected].rot_mat.m);

	//and now retranslate our plane to its original position
	volrender.volrenderview.cutting_planes.planes[selected]._v1 = v1cen+cen;
	volrender.volrenderview.cutting_planes.planes[selected]._v2 = v2cen+cen;
	volrender.volrenderview.cutting_planes.planes[selected]._v3 = v3cen+cen;
	volrender.volrenderview.cutting_planes.planes[selected]._v4 = v4cen+cen;
	vcen = volrender.volrenderview.cutting_planes.planes[selected].centre-cen;
	volrender.volrenderview.cutting_planes.planes[selected]._v1 += vcen;
	volrender.volrenderview.cutting_planes.planes[selected]._v2 += vcen;
	volrender.volrenderview.cutting_planes.planes[selected]._v3 += vcen;
	volrender.volrenderview.cutting_planes.planes[selected]._v4 += vcen;
	volrender.volrenderview.cutting_planes.planes[selected].eval_planeequation();

	//============================================================================
	//ANGLEY
	//============================================================================

	//translate our clipping plane to the origin
	cen = volrender.volrenderview.cutting_planes.planes[selected].v1+volrender.volrenderview.cutting_planes.planes[selected].v2+volrender.volrenderview.cutting_planes.planes[selected].v3+volrender.volrenderview.cutting_planes.planes[selected].v4;
	cen *= 0.25;
	v1cen = volrender.volrenderview.cutting_planes.planes[selected].v1-cen;
	v2cen = volrender.volrenderview.cutting_planes.planes[selected].v2-cen;
	v3cen = volrender.volrenderview.cutting_planes.planes[selected].v3-cen;
	v4cen = volrender.volrenderview.cutting_planes.planes[selected].v4-cen;
	
	//rotate our y axis by our new angle
	v1cen.build_rotatemat(volrender.volrenderview.cutting_planes.planes[selected].perpY, volrender.volrenderview.cutting_planes.planes[selected].angleY, &rot_matY.m[0]);
	volrender.volrenderview.cutting_planes.planes[selected].perpX = volrender.volrenderview.cutting_planes.planes[selected].perpX.mult_matrix(rot_matY.m);

	//and evaluate our new normal for the new plane rotation
	n.cross(volrender.volrenderview.cutting_planes.planes[selected].perpX,volrender.volrenderview.cutting_planes.planes[selected].perpY);

	//now we can evaluate the rotation matrix to go from our original set of axis to the other
	axis_rotmat.find_rotationfromaxis(volrender.volrenderview.cutting_planes.planes[selected].perpX, volrender.volrenderview.cutting_planes.planes[selected].perpY, n, volrender.volrenderview.cutting_planes.planes[selected].axisX, volrender.volrenderview.cutting_planes.planes[selected].axisY, volrender.volrenderview.cutting_planes.planes[selected].axisZ);
	volrender.volrenderview.cutting_planes.planes[selected].rot_mat = axis_rotmat;
	
	//rotate our plane vertices now
	v1cen = v1cen.mult_matrix(volrender.volrenderview.cutting_planes.planes[selected].rot_mat.m);
	v2cen = v2cen.mult_matrix(volrender.volrenderview.cutting_planes.planes[selected].rot_mat.m);
	v3cen = v3cen.mult_matrix(volrender.volrenderview.cutting_planes.planes[selected].rot_mat.m);
	v4cen = v4cen.mult_matrix(volrender.volrenderview.cutting_planes.planes[selected].rot_mat.m);
	
	//and now retranslate our plane to its original position	
	volrender.volrenderview.cutting_planes.planes[selected]._v1 = v1cen+cen;
	volrender.volrenderview.cutting_planes.planes[selected]._v2 = v2cen+cen;
	volrender.volrenderview.cutting_planes.planes[selected]._v3 = v3cen+cen;
	volrender.volrenderview.cutting_planes.planes[selected]._v4 = v4cen+cen;
	vcen = volrender.volrenderview.cutting_planes.planes[selected].centre-cen;
	volrender.volrenderview.cutting_planes.planes[selected]._v1 += vcen;
	volrender.volrenderview.cutting_planes.planes[selected]._v2 += vcen;
	volrender.volrenderview.cutting_planes.planes[selected]._v3 += vcen;
	volrender.volrenderview.cutting_planes.planes[selected]._v4 += vcen;

	//============================================================================
	//evaluate the equation of the plane, the intersection points with the bounding box, and upload our planes to the GPU
	//============================================================================
	volrender.volrenderview.cutting_planes.planes[selected].eval_planeequation();
	volrender.volrenderview.cutting_planes.eval_planeintersections();
}
void QtGLWidget::measure_labels_slot(int vert_label, int edge_label, int face_label, int arrow_label, int labels_label, int lists_label, int tris_label)
{
//	printf("BOH\n");

	//printf("labels: %d %d %d %d\n", vert_label, length_label, angle_label, arrow_label);

	volrender.measure_vertlabel = vert_label;
	volrender.measure_edgelabel = edge_label;
	volrender.measure_facelabel = face_label;
	volrender.measure_arrowlabel = arrow_label;
	volrender.measure_labelslabel = labels_label;
	volrender.measure_listslabel = lists_label;
	volrender.measure_tris = tris_label;

	redraw_event();
}
//===========================================================================================================

void QtGLWidget::light_volshader_slot(int id)
{
	depth_peal = false;
	if(id==SHADER_DEPTH) depth_peal = true;

	volrender.volshader_event(id);
	redraw_event();
}
void QtGLWidget::light_gooch_lighting_slot(Vector warm, Vector cool, int contrib, int expon, int alpha, int degen)
{
	//printf("contrib: %d\n", contrib);
	//printf("expon: %d\n", expon);
	volrender.shaderGoochEvent(warm, cool, contrib, expon, alpha, degen);
	redraw_event();
}
void QtGLWidget::light_depthcue_slot(Vector col, int contrib)
{
	float val = ((float)contrib/100.0)*8.0;

	volrender.shaderDepthCueEvent(col, val);
	redraw_event();
}
void QtGLWidget::light_depthpeal_slot(int offset, int thickness)
{
	//printf("boh\n");
	volrender.olddepthoffset = volrender.depthoffset;
	volrender.olddepththickness = volrender.depththickness;

	volrender.depthoffset = ((float)offset/100.0);
	volrender.depththickness = ((float)thickness/100.0);

	printf("thick: %d\n", thickness);
	printf("offset: %d\n", offset);

	if(volrender.volshader_id==SHADER_DEPTH)
	{
		volrender.vol_shaderobj.enable();
		volrender.vol_shaderobj.sendUniform1f("depthoffset", volrender.depthoffset);
		volrender.vol_shaderobj.sendUniform1f("depththickness", volrender.depththickness);	
		volrender.vol_shaderobj.disable();
	}

	redraw_event();
}
void QtGLWidget::light_altitude_slot(float alt)
{
	volrender.light_altitude = alt;
//	volrender.aim_light();
	redraw_event();
}
void QtGLWidget::light_azimuth_slot(float azi)
{
	volrender.light_azimuth = azi;
//	volrender.aim_light();
	redraw_event();
}
void QtGLWidget::light_colour_slot(Vector amb, Vector diff, Vector spec)
{
	volrender.light_colours(amb, diff, spec);
	redraw_event();
}

//===========================================================================================================
void QtGLWidget::filter_maxflow_interact_slot(int mode)
{
	maxflow_mode = mode;
	emit sliceframe_drawmode_signal(mode);
}
void QtGLWidget::filter_slot(int filtertype, vector<bool> channels, vector<int> intparams, vector<double> doubparams)
{
	printf("filter_slot\n");
	if(volobject.texwidth==1 || volobject.texheight==1 || volobject.texdepth==1) return;

	Filter filter;
	filter.volobj = &volobject;
	
#ifdef OPENCL_SUPPORT
	filter.ocl_gaussian_ptr = &ocl_gaussian;
	filter.ocl_dilate_ptr = &ocl_dilate;
	filter.ocl_erode_ptr = &ocl_erode;
	filter.ocl_median_ptr = &ocl_median;
	filter.ocl_peronamalik_ptr = &ocl_peronamalik;
	filter.ocl_bilateral_ptr = &ocl_bilateral;
#endif

	printf("channels: %d, %d, %d\n", (int)channels[0], (int)channels[1], (int)channels[2]);

	bool uploadTexture = true;

	if(filtertype==FILTER_GAUSSIAN)
	{
		if(intparams.size()!=2 && doubparams.size()!=1) 
		{
			printf("ERROR: Filter parameters\n");
			return;
		}

		int dim = intparams[0];
		int size = intparams[1];
		double sigma = doubparams[0];
		
		volobject.compute_boundingbox();

		timer.startTimer();
//		filter.apply_gaussian_filter(size, sigma, dim, channels);
		filter.gaussian_filter_decomp_init(size, sigma);
		filter.apply_gaussian_filter_decomp(size, sigma, dim, channels);
		filter.gaussian_filter_decomp_dealloc();
		timer.endTimer("CPU Filter:");
	}
	else if(filtertype==FILTER_BILATERAL)
	{
		if(intparams.size()!=1 && doubparams.size()!=1) 
		{
			printf("ERROR: Filter parameters\n");
			return;
		}

		int spatial = intparams[0];
		double range = doubparams[0];
		
		volobject.compute_boundingbox();
		timer.startTimer();
		filter.apply_bilateral_filter(spatial, range, channels);
		timer.endTimer("CPU Filter:");
	}
	else if (filtertype==FILTER_SWAP)
	{
		if(intparams.size()!=2)
		{
			printf("ERROR: Filter parameters\n");
			return;
		}

		int srcA = intparams[0];
		int srcB = intparams[1];

		volobject.compute_boundingbox();

		filter.apply_swap(srcA, srcB);
	}
	else if (filtertype==FILTER_COUNT)
	{
		int offset=0;
		if(channels[1]==true) offset=1;
		else if(channels[2]==true) offset=2;

		int numbvoxels = filter.apply_count(offset);
		printf("numbvoxels: %d\n", numbvoxels);
		QString numvox_str;
		numvox_str.setNum(numbvoxels);

		printf("scale: %f, %f, %f\n", volobject.xscale, volobject.yscale, volobject.zscale);
		printf("resize: %f, %f, %f\n", volobject.x_resize, volobject.y_resize, volobject.z_resize);
		printf("size: %f, %f, %f\n", volobject.xsize, volobject.ysize, volobject.zsize);

		float maxsize = volobject.xsize;
		maxsize = max(maxsize, volobject.ysize);
		maxsize = max(maxsize, volobject.zsize);
	
		//float unitvol = (volobject.xscale*volobject.x_resize) * (volobject.yscale*volobject.y_resize) * (volobject.zscale*volobject.z_resize);
		float resize = volobject.x_resize;
		float unitvol = ((volobject.xscale)*resize)*maxsize;
		unitvol = unitvol*unitvol*unitvol;

		float volume = (float)numbvoxels*unitvol;
		QString volume_str = setUnit(volume);
		volume_str += QString::fromStdWString (L"\u00B3");

		QDialog countDialog(this, Qt::Dialog);
		
		QVBoxLayout *layout = new QVBoxLayout;

			QLabel* voxels_lb = new QLabel("Voxels: ", this);
			QLineEdit* voxels_le = new QLineEdit("", this);
			voxels_le->setText(numvox_str);

			QLabel* volume_lb = new QLabel("Volume: ", this);
			QLineEdit* volume_le = new QLineEdit("", this);
			volume_le->setText(volume_str);

			QPushButton* ok_pb = new QPushButton("OK", this);
			connect( ok_pb, SIGNAL( clicked () ), &countDialog, SLOT( accept() ) );

		layout->addWidget(voxels_lb);
		layout->addWidget(voxels_le);
		layout->addWidget(volume_lb);
		layout->addWidget(volume_le);
		layout->addWidget(ok_pb);

		countDialog.setLayout(layout);
		countDialog.exec();
	}
	else if(filtertype==FILTER_MEADIAN)
	{
		if(intparams.size()!=2)
		{
			printf("ERROR: Filter parameters\n");
			return;
		}

		int dim = intparams[0];
		int size = intparams[1];

		volobject.compute_boundingbox();

		timer.startTimer();
		filter.apply_median_filter(size, dim, channels);
		timer.endTimer("CPU Filter:");
	}
	else if(filtertype==FILTER_UNSHARP)
	{
		if(intparams.size()!=2 && doubparams.size()!=1) 
		{
			printf("ERROR: Filter parameters\n");
			return;
		}

		int dim = intparams[0];
		int size = intparams[1];
		double sigma = doubparams[0];

		volobject.compute_boundingbox();
		filter.apply_unsharpmask_filter(size, sigma, dim, channels);	
	}
	else if(filtertype==FILTER_MEAN)
	{
		if(intparams.size()!=2)
		{
			printf("ERROR: Filter parameters\n");
			return;
		}

		int dim = intparams[0];
		int size = intparams[1];
		volobject.compute_boundingbox();
		filter.apply_mean_filter(size, dim, channels);	
	}
	else if(filtertype==FILTER_THRESHOLD)
	{
		int offset=0;
		if(channels[1]==true) offset=1;
		else if(channels[2]==true) offset=2;
		
		int threshval = volrender.volrenderview.redalpha.threshold_min;
		if(channels[1]==true) threshval = volrender.volrenderview.greenalpha.threshold_min;
		else if(channels[2]==true) threshval = volrender.volrenderview.bluealpha.threshold_min;

		filter.apply_threshold(offset, threshval);
	}	
	else if(filtertype==FILTER_CLEAR)
	{
		int offset=0;
		if(channels[1]==true) offset=1;
		else if(channels[2]==true) offset=2;

		filter.apply_clear(offset);
	}
	else if(filtertype==FILTER_BINARIZE)
	{

		int offset=0;
		if(channels[1]==true) offset=1;
		else if(channels[2]==true) offset=2;

		filter.apply_binarize(offset);
	}
	else if(filtertype==FILTER_SOBEL)
	{
		volobject.compute_boundingbox();
		filter.apply_sobel3D_filter(channels);	
	}
	else if(filtertype==FILTER_EQUALIZE)
	{
		//autolevels_event
		volobject.compute_boundingbox();
		filter.apply_auto_levels_filter(channels);	
	}
	else if(filtertype==FILTER_STRETCH)
	{
		//contraststretch_event	
		volobject.compute_boundingbox();
		filter.apply_contrast_stretching_filter(channels);	
	}
	else if(filtertype==FILTER_INVERT)
	{
		//invert_event	
		int offset=0;
		if(channels[1]==true) offset=1;
		else if(channels[2]==true) offset=2;

		filter.apply_invert(offset);
	}
	else if(filtertype==FILTER_COLLAPSE)
	{
		//collapse_event
		filter.apply_collapse();
	}
	else if(filtertype==FILTER_MAXFLOW)
	{
		if(intparams.size()!=2 && doubparams.size()!=3) 
		{
			printf("ERROR: Filter parameters\n");
			return;
		}

		int srcchan = intparams[0];
		int dstchan = intparams[1];
		
		double wval = doubparams[0];
		double vval = doubparams[1];
		double kval = doubparams[2];

		printf("src: %d, dst: %d\n", srcchan, dstchan);
		printf("weights: %f, %f, %f\n", wval, vval, kval);

		volobject.compute_boundingbox();
		filter.apply_maxflow(srcchan, dstchan, wval, vval, kval, maxflowbg_indices, maxflowfg_indices);
		//maxflowbg_indices.clear();
		//maxflowfg_indices.clear();
	}
	else if(filtertype==FILTER_PERONAMALIK)
	{
		if(intparams.size()!=2 && doubparams.size()!=2) 
		{
			printf("ERROR: Filter parameters\n");
			return;
		}

		int dim = intparams[0];
		double l = doubparams[0];
		double dt = doubparams[1];
		int iters = intparams[1];

		timer.startTimer();
		filter.apply_peronamalik(dim, l, dt, iters, channels);
		timer.endTimer("CPU Filter:");
	}
	else if(filtertype==FILTER_SIEVE)
	{
		if(intparams.size()!=3) 
		{
			printf("ERROR: Filter parameters\n");
			return;
		}
				
		int srcchan = intparams[0];
		int dstchan = intparams[1];
		int size = intparams[2];

		volobject.compute_boundingbox();
		filter.apply_sieve(srcchan, dstchan, size);
	}
	else if(filtertype==FILTER_DILATE)
	{
		volobject.compute_boundingbox();
		timer.startTimer();
		filter.apply_dilate_filter(1,0,channels);
		timer.endTimer("CPU Filter:");
	}
	else if(filtertype==FILTER_ERODE)
	{
		volobject.compute_boundingbox();
		timer.startTimer();
		filter.apply_erode_filter(1,0,channels);
		timer.endTimer("CPU Filter:");
	}
	else if(filtertype==FILTER_OPEN)
	{
		volobject.compute_boundingbox();
		filter.apply_erode_filter(1,0,channels);
		filter.apply_dilate_filter(1,0,channels);
	}
	else if(filtertype==FILTER_CLOSE)
	{
		volobject.compute_boundingbox();
		filter.apply_dilate_filter(1,0,channels);
		filter.apply_erode_filter(1,0,channels);
	}
	else if(filtertype==FILTER_REPLACE)
	{
		if(intparams.size()!=4)
		{
			printf("ERROR: Filter parameters\n");
			return;
		}

		int ifchan = intparams[0];
		int inchan = intparams[1];
		int withchan = intparams[2];
		int repop = intparams[3];

		printf("if %d is %d replace in %d with %d\n", ifchan, repop, inchan, withchan);

		filter.apply_replace(ifchan, repop, inchan, withchan);
	}
	else if(filtertype==FILTER_COPY)
	{
		if(intparams.size()!=2) 
		{
			printf("ERROR: Filter parameters\n");
			return;
		}

		int srcchan = intparams[0];
		int dstchan = intparams[1];

		filter.apply_copyto(srcchan, dstchan);
	}
	else if(filtertype==FILTER_ADD)
	{
		if(intparams.size()!=2) 
		{
			printf("ERROR: Filter parameters\n");
			return;
		}

		int srcchan = intparams[0];
		int dstchan = intparams[1];

		filter.apply_addto(srcchan, dstchan);
	}
	else if(filtertype==FILTER_SETOPERATOR)
	{
		if(intparams.size()!=3) 
		{
			printf("ERROR: Filter parameters\n");
			return;
		}

		int a_chan = intparams[0];
		int b_chan = intparams[1];
		int operatormode = intparams[2];

		//printf("Operator: %d\n", dim);
		filter.apply_setoperator(a_chan, b_chan, operatormode);

	}
	else if(filtertype==FILTER_ARITHM)
	{
		if(intparams.size()!=3) 
		{
			printf("ERROR: Filter parameters\n");
			return;
		}

		int a_chan = intparams[0];
		int b_chan = intparams[1];
		int operatormode = intparams[2];

		//printf("Operator: %d\n", dim);
		filter.apply_arithmoperator(a_chan, b_chan, operatormode);
	}
	else if(filtertype==FILTER_FLOODFILL)
	{
		if(intparams.size()!=3) 
		{
			printf("ERROR: Filter parameters\n");
			return;
		}

		int srcchan = intparams[0];
		int dstchan = intparams[1];
		int window = intparams[2];
		filter.apply_floodfill(srcchan, dstchan, window, volrender.cursor3D_texturespace.x, volrender.cursor3D_texturespace.y, volrender.cursor3D_texturespace.z);
	}
#ifdef OPENCL_SUPPORT
	else if(filtertype==FILTER_GPUPERONAMALIK)
	{
		if(intparams.size()!=2 && doubparams.size()!=2) 
		{
			printf("ERROR: Filter parameters\n");
			return;
		}

		int dim = intparams[0];
		double l = doubparams[0];
		double dt = doubparams[1];
		int iters = intparams[1];

		timer.startTimer();
		filter.apply_peronamalik_gpu(dim, l, dt, iters, channels);
		timer.endTimer("GPU Filter:");
	}
	else if(filtertype==FILTER_GPUGAUSSIAN)
	{
		if(intparams.size()!=2 && doubparams.size()!=1) 
		{
			printf("ERROR: Filter parameters\n");
			return;
		}

		int dim = intparams[0];
		int size = intparams[1];
		double sigma = doubparams[0];
		
		timer.startTimer();	
		filter.apply_gaussian_filter_gpu(size, sigma, dim, channels);
		timer.endTimer("GPU Filter:");
	}
	else if(filtertype==FILTER_GPUMEDIAN)
	{
		printf("FILTER: MEDIAN\n");
		timer.startTimer();
		filter.apply_median_filter_gpu(1, 1, channels);
		timer.endTimer("GPU Filter:");
	}
	else if(filtertype==FILTER_GPUDILATE)
	{
		timer.startTimer();
		filter.apply_dilate_filter_gpu(1, 1, channels);
		timer.endTimer("GPU Filter:");
	}
	else if(filtertype==FILTER_GPUERODE)
	{
		timer.startTimer();
		filter.apply_erode_filter_gpu(1, 1, channels);
		timer.endTimer("GPU Filter:");
	}
	else if(filtertype==FILTER_GPUBILATERAL)
	{
		if(intparams.size()!=1 && doubparams.size()!=1) 
		{
			printf("ERROR: Filter parameters\n");
			return;
		}

		int spatial = intparams[0];
		double range = doubparams[0];
		
		printf("spatial: %d range: %f\n", spatial, range);

		timer.startTimer();
		filter.apply_bilateral_filter_gpu(spatial, range, channels);
		timer.endTimer("GPU Filter:");
	}
#endif

	filter.volobj = 0;
	volio.pad(1);
	if(uploadTexture) volrender.load_TextureMap();
	emit frames_redraw_signal();
	emit update_channels_signal();
	redraw_event();
}
#ifdef OPENCL_SUPPORT

void QtGLWidget::filter_gpu_smthdilate_slot(int srcchan, int dstchan, int seed_window, int minsize, int gaussian_width, float gaussian_strength, int gaussian_window, int show_sections, int show_volume)
{
	if(volobject.texwidth==1 || volobject.texheight==1 || volobject.texdepth==1) return;

	Filter filter;
	filter.volobj = &volobject;
	filter.ocl_gaussian_ptr = &ocl_gaussian;
	filter.ocl_dilate_ptr = &ocl_dilate;

	if(srcchan==0 || srcchan==0) volrender.r_channel = volobject.has_red = true;
	if(srcchan==1 || dstchan==1) volrender.g_channel = volobject.has_green = true;
	if(srcchan==2 || dstchan==2) volrender.b_channel = volobject.has_blue = true;

	printf("Source: %d\n", srcchan);
	printf("Destination: %d\n", dstchan);
	printf("Seed Window: %d\n", seed_window);
	printf("Kernel Width: %d\n", gaussian_width);
	printf("Kernel Strength: %f\n", gaussian_strength);
	printf("Gaussian Window: %d\n", gaussian_window);

	int size = volobject.texwidth*volobject.texheight*volobject.texdepth;
	for(int i=0; i<size; i++) volobject.texture3d[3*i+dstchan] = 0;
	
	volobject.resetBoundingBox();
	filter.ocl_gaussian_ptr->ocl_filterGaussian_init(gaussian_strength, gaussian_width);

	int oldsize=-1;
	int currentsize=0;
	filter_smthdilate_cont=true;

	float totalDt=0.0;
	float filt_time, text_time, frames_time, redraw_time;

	while(filter_smthdilate_cont)
	{
		timer.startTimer();
		currentsize=filter.apply_stepperdilate_gpu(volrender.cursor3D_texturespace, srcchan, dstchan, seed_window, minsize, gaussian_width, gaussian_strength, gaussian_window);
		filt_time = timer.endTimer("Filter Step: ");
		totalDt += filt_time;

		if(currentsize==oldsize) filter_smthdilate_cont = false;

		if(show_volume)
		{
			volrender.load_TextureMap();
			redraw_event();
		}
		if(show_sections) emit frames_redraw_signal();

		oldsize = currentsize;
		QApplication::processEvents();
	}
	printf("Total time: %fmsec\n", totalDt);

	filter.volobj = 0;

	volrender.load_TextureMap();
	emit frames_redraw_signal();
	redraw_event();
}
#endif

void QtGLWidget::filter_smthdilate_slot(int srcchan, int dstchan, int seed_window, int minsize, int gaussian_width, float gaussian_strength, int gaussian_window, int show_sections, int show_volume)
{
	if(volobject.texwidth==1 || volobject.texheight==1 || volobject.texdepth==1) return;

	Filter filter;
	filter.volobj = &volobject;

	if(srcchan==0 || srcchan==0) volrender.r_channel = volobject.has_red = true;
	if(srcchan==1 || dstchan==1) volrender.g_channel = volobject.has_green = true;
	if(srcchan==2 || dstchan==2) volrender.b_channel = volobject.has_blue = true;

	printf("filt slot\n");

	printf("Source: %d\n", srcchan);
	printf("Destination: %d\n", dstchan);
	printf("Seed Window: %d\n", seed_window);
	printf("Kernel Width: %d\n", gaussian_width);
	printf("Kernel Strength: %f\n", gaussian_strength);
	printf("Gaussian Window: %d\n", gaussian_window);

	int size = volobject.texwidth*volobject.texheight*volobject.texdepth;

	for(int i=0; i<size; i++) volobject.texture3d[3*i+dstchan] = 0;
	
	filter.tempsize = 1;
	volobject.resetBoundingBox();
	filter.gaussian_filter_decomp_init(gaussian_width, gaussian_strength);

	int oldsize=-1;
	int currentsize=0;
	filter_smthdilate_cont=true;

	float totalDt=0.0;
	float filt_time, text_time, frames_time, redraw_time;

	while(filter_smthdilate_cont)
	{
		timer.startTimer();
		currentsize=filter.apply_stepperdilate(volrender.cursor3D_texturespace, srcchan, dstchan, seed_window, minsize, gaussian_width, gaussian_strength, gaussian_window);
		filt_time = timer.endTimer("Filter Step: ");
		totalDt += filt_time;

		if(currentsize==oldsize) filter_smthdilate_cont = false;
 
		if(show_volume)
		{
			volrender.load_TextureMap();
			redraw_event();
		}
		if(show_sections) emit frames_redraw_signal();

		oldsize = currentsize;
		QApplication::processEvents();
	}
	printf("Total time: %fmsec\n", totalDt);

	filter.gaussian_filter_decomp_dealloc();
	filter.volobj = 0;

	volrender.load_TextureMap();
	emit frames_redraw_signal();
	redraw_event();
}
void QtGLWidget::filter_smthdilateabort_slot(void)
{
//	printf("filt abort\n");
	filter_smthdilate_cont=false;
	QApplication::processEvents();

	volrender.load_TextureMap();
	emit frames_redraw_signal();
	redraw_event();
}
//===========================================================================================================
void QtGLWidget::rendersettings_framebuffer_lod_slot(int value)
{
	volrender.LOD_FBO_SIZE = value;
	if(value==0) return;
	volrender.init_FBO();
	redraw_event();
}
void QtGLWidget::rendersettings_framebuffer_slot(int value)
{
	volrender.FBO_SIZE = value;
	if(value==0) return;
	volrender.init_FBO();
	redraw_event();
}
void QtGLWidget::rendersettings_postprocess_slot(int value)
{
	volrender.postprocess2d_amount = value/100.0;
	volrender.postprocess2d_shader();
//	updateGL();

	redraw_event();
}
void QtGLWidget::rendersettings_silhouettes_slot(int value)
{
	volrender.silhouette_amount = value/100.0;
	volrender.silhouette_shader();
	updateGL();

//	redraw_event();
}
void QtGLWidget::rendersettings_numbslices_slot(int s)
{
	printf("numbslices: %d\n", s);

	volrender.slices = s;
	volrender.numbslices = s;

	volrender.load_1DLookupTables(true);

	redraw_event();
}
void QtGLWidget::rendersettings_numbsliceslod_slot(int s)
{
//	printf("%d\n", s);

	volrender.sliceslod = s;
	volrender.load_1DLookupTables(true);

	redraw_event();
}
void QtGLWidget::rendersettings_numbsliceslodtoggle_slot(int t)
{
	volrender.SlicesmoveLOD_toggle = t;
	volrender.load_1DLookupTables(true);
//	printf("%d\n", t);

	redraw_event();
}
void QtGLWidget::rendersettings_framebuffertoggle_slot(int t)
{
	volrender.FBOmoveLOD_toggle = t;

	redraw_event();
}
void QtGLWidget::rendersettings_postprocesstoggle_slot(int t)
{
	volrender.PostProcess_toggle = t;
//	printf("%d\n", t);

	redraw_event();
}
void QtGLWidget::rendersettings_maxtexturesize_slot(int t)
{
	volobject.maxsizeZ = volobject.maxsizeY = volobject.maxsizeX = t;
}
void QtGLWidget::rendersettings_renderquality_slot(float w)
{
	volrender.numbslices = volrender.slices = w*volobject.maxres*0.5;
	volrender.FBO_SIZE = w*volobject.maxres*0.85;

	if(volrender.FBO_SIZE>2.0*volobject.maxres) volrender.FBO_SIZE = 2.0*volobject.maxres;
	//if(volrender.numbslices>volobject.maxres) volrender.numbslices = volobject.maxres;

	/*printf("\n interp: %f\n\n", w);*/
//	printf("slices: %d\n", (int)volrender.numbslices);
//	printf("fbo: %d\n", volrender.FBO_SIZE);

	//volrender.init_VBO();
	volrender.init_FBO();
	volrender.load_1DLookupTables(true);
	volrender.postprocess2d_shader();
	redraw_event();
}
void QtGLWidget::rendersettings_renderqualitylod_slot(float w)
{
	volrender.sliceslod = w*volobject.maxres*0.25;
	volrender.LOD_FBO_SIZE = w*volobject.maxres*0.5;
	//volrender.LOD_FBO_SIZE = volrender.FBO_SIZE;

	//if(volrender.FBO_SIZE>2.0*volobject.maxres) volrender.FBO_SIZE = 2.0*volobject.maxres;
	//if(volrender.numbslices>volobject.maxres) volrender.numbslices = volobject.maxres;

	/*printf("\n lod_interp: %f\n\n", w);*/
//	printf("lod_slices: %d\n", (int)volrender.sliceslod);
//	printf("lod_fbo: %d\n", volrender.LOD_FBO_SIZE);

	volrender.init_FBO();
	//volrender.init_VBO();
	volrender.load_1DLookupTables(true);
	volrender.postprocess2d_shader();
	redraw_event();
}
#ifdef OPENCL_SUPPORT
void QtGLWidget::rendersettings_openclplat_slot(int plat)
{
	printf("Selected: %d\n", plat);
	ocl_wrap.switch_Platform(plat);

	ocl_gaussian.ocl_init_progam("Kernels\\gaussian.cl");
	ocl_dilate.ocl_init_progam("Kernels\\dilate.cl");
	ocl_erode.ocl_init_progam("Kernels\\erode.cl");
	ocl_median.ocl_init_progam("Kernels\\median.cl");
	ocl_peronamalik.ocl_init_progam("Kernels\\anisotropic.cl");
	ocl_boundingbox.ocl_init_progam("Kernels\\boundingbox.cl");
	ocl_reduction.ocl_init_progam("Kernels\\reduction.cl");
}
#endif

void QtGLWidget::rendersettings_channelmode_slot(int r,int g,int b,int i)
{
//	printf("%d, %d, %d, %d\n", r,g,b,i);

	volrender.r_channel = r;
	volrender.g_channel = g;
	volrender.b_channel = b;

	redraw_event();
}
//===========================================================================================================
void QtGLWidget::crop_invsel_slot(int mode)
{
	printf("mode: %d\n", mode);

	volrender.crop_inv=!volrender.crop_inv;
	//else volrender.crop_inv=false;
}
void QtGLWidget::crop_reset_slot(void)
{
	printf("reset\n");
	volrender.cropping_mode=0;

	volrender.crop_shape.clear();
	volrender.crop_shape2d.clear();
	volrender.crop_triangles.clear();
	volrender.crop_normals.clear();
	redraw_event();
}
void QtGLWidget::crop_start_slot(void)
{
	//printf("crop start\n");
	
	volrender.current_measure_drawmode = MEASURE_DRAWMODE_CROP;
	volrender.cropping_mode=1;
	volrender.crop_shape.clear();
	volrender.crop_shape2d.clear();
	volrender.crop_triangles.clear();
	volrender.crop_normals.clear();
	//measure_start_slot();
}
void QtGLWidget::crop_end_slot(vector<bool> channels)
{
	volrender.cropping_mode=0;

	//printf("crop end\n");
	if(volrender.crop_normals.size()<3)
	{
		QMessageBox* mb = new QMessageBox( "WARNING",
				"Please specify at least 3 points for the crop shape.\n"
				"The crop shape is specified by clicking around your object.\n",QMessageBox::Warning,
				QMessageBox::Ok | QMessageBox::Default,0,0 );
		mb->exec();

		crop_reset_slot();

		return;
	}

	
	printf("crop mode: %d\n", (int)volrender.crop_inv);
	printf("chans: %d, %d, %d\n", (int)channels[0], (int)channels[1], (int)channels[2]);

	//printf("Applying CLIPPING planes... \n");
	int index=0;
	Vector v = Vector(0,0,0,0);
	
	Q3ProgressDialog* progressbar = new Q3ProgressDialog("Cropping the Volume Data...", "Cancel", volobject.texdepth, 0, "progress", TRUE);		
	progressbar->setCaption("Please Wait");
	progressbar->setMinimumDuration(10);

	float halfw = volobject.texwidth/2.0;
	float halfh = volobject.texheight/2.0;
	float halfd = volobject.texdepth/2.0;
	int r,g,b;

	float maxsize =  max(volobject.xsize, volobject.ysize);
	maxsize =  max(maxsize, volobject.zsize);

	Vector c;
	
	Vector res = Vector((volobject.xsize/maxsize), (volobject.ysize/maxsize), (volobject.zsize/maxsize));
	
//	Vector res = Vector((volobject.xsize), (volobject.ysize), (volobject.zsize));
//	res = Vector(volobject.x_resize, volobject.y_resize, volobject.z_resize)/res;

	printf("size: %f, %f, %f\n", volobject.xsize, volobject.ysize, volobject.zsize);
	printf("resize: %f, %f, %f\n", volobject.x_resize, volobject.y_resize, volobject.z_resize);


	int helper,helper2;

	for(int z = 0; z <volobject.texdepth; z++)
	{	
		//progress(z, volobject.texdepth);
		progressbar->setProgress(z);
		if (progressbar->wasCancelled()) break;

		for(int x = 0; x <volobject.texwidth; x++)
		{
			for(int y = 0; y <volobject.texheight; y++)
			{
				index = ((((z*volobject.texheight)+y)*volobject.texwidth)+x);
				
				r=g=b=0;
				if(channels[0]==true) r = volobject.texture3d[3*index+0];
				if(channels[1]==true) g = volobject.texture3d[3*index+1];
				if(channels[2]==true) b = volobject.texture3d[3*index+2];

				if(r+g+b>0)
				{
					v.x = x-halfw;
					v.y = y-halfh;
					v.z = z-halfd;
					v.w = 0.0;

					v.x *= volobject.xsize/maxsize;
					v.y *= volobject.ysize/maxsize;
					v.z *= volobject.zsize/maxsize;

					helper2=0;
					for(int i=0; i<volrender.crop_normals.size()/3; i++)
					{
						helper=0;
						c = volrender.crop_centroids[3*i];
						if(volrender.crop_normals[3*i].dot(v-c)<0.0) helper++;
						c = volrender.crop_centroids[3*i+1];
						if(volrender.crop_normals[3*i+1].dot(v-c)<0.0) helper++;
						c = volrender.crop_centroids[3*i+2];
						if(volrender.crop_normals[3*i+2].dot(v-c)<0.0) helper++;

						if(helper==3)
						{
							//INSIDE
							if(volrender.crop_inv==true)
							{
								if(channels[0]==true) volobject.texture3d[3*index+0] = GLubyte(0);
								if(channels[1]==true) volobject.texture3d[3*index+1] = GLubyte(0);
								if(channels[2]==true) volobject.texture3d[3*index+2] = GLubyte(0);
								
								/*if(volobject.is_greyscale==1)
								{
									volobject.texture3d[3*index+0] = GLubyte(0);
									volobject.texture3d[3*index+1] = GLubyte(0);
									volobject.texture3d[3*index+2] = GLubyte(0);
								}
								if(volobject.has_red==1) volobject.texture3d[3*index+0] = GLubyte(0);
								if(volobject.has_green==1) volobject.texture3d[3*index+1] = GLubyte(0);
								if(volobject.has_blue==1) volobject.texture3d[3*index+2] = GLubyte(0); */
							}
						}
						else
						{
							//OUTSIDE
							if(volrender.crop_inv==false)
							{
								helper2++;
							}
						}
					}
					if(volrender.crop_inv==false)
					{
						if(helper2==volrender.crop_normals.size()/3)
						{
							//OUTSIDE ALL TRIANGLES
							if(channels[0]==true) volobject.texture3d[3*index+0] = GLubyte(0);
							if(channels[1]==true) volobject.texture3d[3*index+1] = GLubyte(0);
							if(channels[2]==true) volobject.texture3d[3*index+2] = GLubyte(0);

							/*if(volobject.is_greyscale==1)
							{
								volobject.texture3d[3*index+0] = GLubyte(0);
								volobject.texture3d[3*index+1] = GLubyte(0);
								volobject.texture3d[3*index+2] = GLubyte(0);
							}
							if(volobject.has_red==1) volobject.texture3d[3*index+0] = GLubyte(0);
							if(volobject.has_green==1) volobject.texture3d[3*index+1] = GLubyte(0);
							if(volobject.has_blue==1) volobject.texture3d[3*index+2] = GLubyte(0); */
						}
					}
				}
			}
		}
	}

	progressbar->setProgress(volobject.texdepth);
	delete progressbar;

	volobject.compute_boundingbox();
	//volrender.compute_boundingbox();

	volrender.load_TextureMap();

	volrender.crop_shape.clear();
	volrender.crop_shape2d.clear();
	volrender.crop_triangles.clear();

	redraw_event();
	emit frames_redraw_signal();
}

//===========================================================================================================

void QtGLWidget::stereo_onoff_slot(int toggle)
{
	volrender.stereo_toggle = toggle;
	redraw_event();

/*
	if(volrender.stereo_toggle==1)
	{
		if(volrender.stereo_mode!=0) volrender.background_colour=Qt::black;
	}
	else
	{
		volrender.background_colour=volrender.old_background_colour;
	}*/
}
void QtGLWidget::stereo_rendermode_slot(int mode)
{
	volrender.stereo_mode = mode;
/*
	if(volrender.stereo_toggle==1)
	{
		if(volrender.stereo_mode!=0) volrender.background_colour=Qt::black;
	}
	*/
	redraw_event();
}
void QtGLWidget::stereo_seperation_slot(float sep)
{
	volrender.stereo_eye_seperation = sep;
	redraw_event();
}
void QtGLWidget::stereo_zscreen_slot(int zscreen)
{
	volrender.volrenderview.camera.zscreen = zscreen;
	redraw_event();
}
//===========================================================================================================

void QtGLWidget::cutplane_invert_slot(int selected)
{
	volrender.volrenderview.cutting_planes.invert_cut=selected;

	if(selected==1) volrender.volrenderview.cutting_planes.renderall=selected;
	if(selected==0)
	{
		volrender.volrenderview.cutting_planes.renderall=selected;
		planes_toggle = 0;
	}
	redraw_event();
}
void QtGLWidget::cutplane_showall_slot(int selected)
{
	volrender.volrenderview.cutting_planes.renderall=selected;
	volrender.volrenderview.cutting_planes.eval_planeintersections();
	redraw_event();
}
void QtGLWidget::cutplane_select_slot(int selected)
{
	volrender.volrenderview.cutting_planes.select(selected);

	planes_toggle = 0;
	for(int i=0; i<6; i++)
	{
		if(volrender.volrenderview.cutting_planes.selected[i]==1) planes_toggle=1;
	}
	redraw_event();
}
void QtGLWidget::cutplane_load_slot(QString fn)
{
	//volrender.volrenderview.cutting_planes.load(fn);
        cutplane_resetall_slot();
        volrender.volrenderview.cutting_planes.load(fn);      

		volrender.curve_vectranslate = volrender.volrenderview.cutting_planes.curve_translate;
		volrender.curveXang = volrender.volrenderview.cutting_planes.curve_angleX;
		volrender.curveYang = volrender.volrenderview.cutting_planes.curve_angleY;

        for(int i=0; i<6; i++)
        {
            volrender.volrenderview.cutting_planes.planes[i].perpX = volrender.volrenderview.cutting_planes.planes[i]._v2 - volrender.volrenderview.cutting_planes.planes[i]._v1;
            volrender.volrenderview.cutting_planes.planes[i].perpY = volrender.volrenderview.cutting_planes.planes[i]._v4 - volrender.volrenderview.cutting_planes.planes[i]._v1;

            volrender.volrenderview.cutting_planes.planes[i].perpX.normalize();
            volrender.volrenderview.cutting_planes.planes[i].perpY.normalize();

            cutplane_selected_slot(i);
            cutplane_planeslider_slot(0.0);
            cutplane_unselect_slot();

            /* Matrix4x4 axis_rotmat;

            axis_rotmat.find_rotationfromaxis(volrender.volrenderview.cutting_planes.planes[i].v1, volrender.volrenderview.cutting_planes.planes[i].v2, volrender.volrenderview.cutting_planes.planes[i].v3,
            volrender.volrenderview.cutting_planes.planes[i]._v1, volrender.volrenderview.cutting_planes.planes[i]._v2, volrender.volrenderview.cutting_planes.planes[i]._v3);

            volrender.volrenderview.cutting_planes.planes[i].perpX = volrender.volrenderview.cutting_planes.planes[i].axisX.mult_matrix(axis_rotmat.m);
            volrender.volrenderview.cutting_planes.planes[i].perpY = volrender.volrenderview.cutting_planes.planes[i].axisY.mult_matrix(axis_rotmat.m);
*/
            /*volrender.volrenderview.cutting_planes.planes[i].angleY = volrender.volrenderview.cutting_planes.planes[i].perpX.dot(volrender.volrenderview.cutting_planes.planes[i].axisX);
            volrender.volrenderview.cutting_planes.planes[i].angleX = volrender.volrenderview.cutting_planes.planes[i].perpY.dot(volrender.volrenderview.cutting_planes.planes[i].axisY);

            printf("angleX: %f\n", volrender.volrenderview.cutting_planes.planes[i].angleX);
            printf("angleY: %f\n", volrender.volrenderview.cutting_planes.planes[i].angleY);*/

            volrender.volrenderview.cutting_planes.planes[i].angleX = 0.0;
            volrender.volrenderview.cutting_planes.planes[i].angleY = 0.0;
            volrender.volrenderview.cutting_planes.planes[i].translate = 0.0;
        }

		//volrender.volrenderview.cutting_planes.eval_planeintersections();
        redraw_event();
}
void QtGLWidget::cutplane_resetall_slot(void)
{
	volrender.volrenderview.cutting_planes.reset_all();
	planes_toggle = 0;
	redraw_event();
}
void QtGLWidget::cutplane_resetcurrent_slot(void)
{
	volrender.volrenderview.cutting_planes.reset_selected();
	redraw_event();
}
void QtGLWidget::cutplane_save_slot(QString fn)
{
	volrender.volrenderview.cutting_planes.curve_translate = volrender.curve_vectranslate;
	volrender.volrenderview.cutting_planes.curve_angleX = volrender.curveXang;
	volrender.volrenderview.cutting_planes.curve_angleY = volrender.curveYang;

	volrender.volrenderview.cutting_planes.save(fn);
	redraw_event();
}
void QtGLWidget::cutplane_redclip_slot(void)
{
	volrender.volrenderview.redclip_toggle = !volrender.volrenderview.redclip_toggle;
	redraw_event();
}
void QtGLWidget::cutplane_greenclip_slot(void)
{
	volrender.volrenderview.greenclip_toggle = !volrender.volrenderview.greenclip_toggle;
	redraw_event();
}
void QtGLWidget::cutplane_blueclip_slot(void)
{
	volrender.volrenderview.blueclip_toggle = !volrender.volrenderview.blueclip_toggle;
	redraw_event();
}
void QtGLWidget::cutplane_isoclip_slot(void)
{
	volrender.isoclip_toggle = !volrender.isoclip_toggle;
	redraw_event();
}
void QtGLWidget::cutplane_interact_clicked_slot(int mode)
{
	//emit cutplane_selected_signal(-1);
	volrender.volrenderview.cutting_planes.rotmode = mode;
}
void QtGLWidget::cutplane_applyall_drawmode_slot(void)
{
	int selected=-1;
	int i;

	for(i=0; i<6; i++)
	{
		if(volrender.volrenderview.cutting_planes.selected[i]==1) selected = i;
	}

	if(selected==-1) return;

	int drawmode = volrender.volrenderview.plane_rendermode[selected];

//	printf("drawmode: %d\n", drawmode);

	for(i=0; i<6; i++)
	{
		volrender.volrenderview.plane_rendermode[i]=drawmode;
	}

	redraw_event();	
}
void QtGLWidget::cutplane_solid_slot(bool on)
{
	//printf("boh\n");
	if(on)
	{
		//volrender.volume_toggle=0;
		volrender.volrenderview.plane_rendermode[0]=2;
	}
	else
	{
		//volrender.volume_toggle=1;
		volrender.volrenderview.plane_rendermode[0]=0;
	}
	redraw_event();	
}
void QtGLWidget::cutplane_flip_slot(bool on)
{
	Vector v1 = volrender.volrenderview.cutting_planes.planes[0]._v1;
	Vector v2 = volrender.volrenderview.cutting_planes.planes[0]._v2;
	Vector v3 = volrender.volrenderview.cutting_planes.planes[0]._v3;
	Vector v4 = volrender.volrenderview.cutting_planes.planes[0]._v4;

	volrender.volrenderview.cutting_planes.planes[0]._v1 = v4; 
	volrender.volrenderview.cutting_planes.planes[0]._v2 = v3;
	volrender.volrenderview.cutting_planes.planes[0]._v3 = v2;
	volrender.volrenderview.cutting_planes.planes[0]._v4 = v1;
	
	volrender.volrenderview.cutting_planes.planes[0].eval_planeequation();

	redraw_event();	
}
void QtGLWidget::cutplane_unselect_slot(void)
{
	for(int i=0; i<6; i++) volrender.volrenderview.cutting_planes.selected[i] = -1;

	redraw_event();
}
void QtGLWidget::cutplane_selected_slot(int index)
{
	for(int i=0; i<6; i++)
	{
		volrender.volrenderview.cutting_planes.selected[i]=-1;
	}

	volrender.volrenderview.cutting_planes.select(index);
	emit cutplane_selectedupdate_signal(index, volrender.volrenderview.plane_rendermode[index]);	

	redraw_event();	
}
void QtGLWidget::cutplane_invcrop_slot(void)
{
	if(volobject.texture3d==NULL) return;

	invcrop();

	volrender.compute_boundingbox();
	volrender.load_TextureMap();

	emit frames_redraw_signal();
	redraw_event();	
}
void QtGLWidget::cutplane_crop_slot(void)
{
	if(volobject.texture3d==NULL) return;

	crop();

	volrender.load_TextureMap();

	emit frames_redraw_signal();
	redraw_event();
}
void QtGLWidget::cutplane_realign_slot(void)
{
	if(volobject.texture3d==NULL) return;

	realign();

	volrender.compute_boundingbox();
	volrender.load_TextureMap();

	emit frames_redraw_signal();
	redraw_event();	
}
void QtGLWidget::cutplane_recentre_slot(void)
{
	if(volobject.texture3d==NULL) return;

	recentre();

	volrender.compute_boundingbox();
	volrender.load_TextureMap();

	float maxsize =  max((float)volobject.xsize, (float)volobject.ysize);
	maxsize =  max((float)maxsize, (float)volobject.zsize);
	
	//volrender.volrenderview.cutting_planes.init(volobject.boundingboxX*volobject.xsize/maxsize*1.25, volobject.boundingboxY*volobject.ysize/maxsize*1.25, volobject.boundingboxZ*volobject.zsize/maxsize*1.25);

	//TODO recompute the bounding box dimensions.!!!

	/*	Vector bb_size = Vector(volobject.boundingboxX*volobject.xsize/maxsize, volobject.boundingboxY*volobject.ysize/maxsize, volobject.boundingboxZ*volobject.zsize/maxsize);
	bb_size /= 2.0;
	volrender.bb_v1=volrender.bb_v1*bb_size;
	volrender.bb_v2=volrender.bb_v2*bb_size;
	volrender.bb_v3=volrender.bb_v3*bb_size;
	volrender.bb_v4=volrender.bb_v4*bb_size;
	volrender.bb_v5=volrender.bb_v5*bb_size;
	volrender.bb_v6=volrender.bb_v6*bb_size;
	volrender.bb_v7=volrender.bb_v7*bb_size;
	volrender.bb_v8=volrender.bb_v8*bb_size;
*/	
	
	emit frames_redraw_signal();
	redraw_event();
}
void QtGLWidget::cutplane_rendermode_slot(int drawmode)
{
//	printf("drawmode: %d\n", drawmode);
	
	int selected=-1;
	for(int i=0; i<6; i++)
	{
		if(volrender.volrenderview.cutting_planes.selected[i]==1) selected = i;
	}

	if(selected==-1) return;

	volrender.volrenderview.plane_rendermode[selected]=drawmode;
	redraw_event();	
}
void QtGLWidget::cutplane_planeslider_slot(float dist)
{
	int selected=-1;
	for(int i=0; i<6; i++)
	{
		if(volrender.volrenderview.cutting_planes.selected[i]==1) selected = i;
	}

	if(selected==-1) return;

//	printf("selected: %d\n", selected);
	
	Vector _v1 = volrender.volrenderview.cutting_planes.planes[selected]._v1;
	Vector _v2 = volrender.volrenderview.cutting_planes.planes[selected]._v2;
	Vector _v3 = volrender.volrenderview.cutting_planes.planes[selected]._v3;
	Vector _v4 = volrender.volrenderview.cutting_planes.planes[selected]._v4;
	Vector _normal = volrender.volrenderview.cutting_planes.planes[selected].normal;
	
	Vector vv_0 = (_v1+(dist*_normal));		
	Vector vv_1 = (_v2+(dist*_normal));		
	Vector vv_2 = (_v3+(dist*_normal));		
	Vector vv_3 = (_v4+(dist*_normal));		

	volrender.volrenderview.cutting_planes.planes[selected]._v1 = (vv_0);
	volrender.volrenderview.cutting_planes.planes[selected]._v2 = (vv_1);
	volrender.volrenderview.cutting_planes.planes[selected]._v3 = (vv_2);
	volrender.volrenderview.cutting_planes.planes[selected]._v4 = (vv_3);

	volrender.volrenderview.cutting_planes.planes[selected].eval_planeequation();
	volrender.volrenderview.cutting_planes.eval_planeintersections();

	Vector c1 = volrender.volrenderview.cutting_planes.planes[selected]._v1 + volrender.volrenderview.cutting_planes.planes[selected]._v2 + volrender.volrenderview.cutting_planes.planes[selected]._v3 + volrender.volrenderview.cutting_planes.planes[selected]._v4;
	c1 /= 4.0;
	Vector c2 = volrender.volrenderview.cutting_planes.planes[selected].v1 + volrender.volrenderview.cutting_planes.planes[selected].v2 + volrender.volrenderview.cutting_planes.planes[selected].v3 + volrender.volrenderview.cutting_planes.planes[selected].v4;
	c2 /= 4.0;
	Vector l = c1-c2;
	
	volrender.volrenderview.cutting_planes.planes[selected].translate = l.length();
//	printf("dist: %f\n", volrender.volrenderview.cutting_planes.planes[selected].translate);

	redraw_event();
}
void QtGLWidget::cutplane_planerot1_slot(float angle)
{
	//find which plane we have currently active
	int selected=-1;
	for(int i=0; i<6; i++)
	{
		if(volrender.volrenderview.cutting_planes.selected[i]==1) selected = i;
	}
	//if none then stop
	if(selected==-1) return;

	//increment our angle of rotation.
	volrender.volrenderview.cutting_planes.planes[selected].angleX += angle;

	//translate our clipping plane to the origin
	Vector cen = volrender.volrenderview.cutting_planes.planes[selected].v1+volrender.volrenderview.cutting_planes.planes[selected].v2+volrender.volrenderview.cutting_planes.planes[selected].v3+volrender.volrenderview.cutting_planes.planes[selected].v4;
	cen *= 0.25;
	Vector v1cen = volrender.volrenderview.cutting_planes.planes[selected].v1-cen;
	Vector v2cen = volrender.volrenderview.cutting_planes.planes[selected].v2-cen;
	Vector v3cen = volrender.volrenderview.cutting_planes.planes[selected].v3-cen;
	Vector v4cen = volrender.volrenderview.cutting_planes.planes[selected].v4-cen;

	Matrix4x4 rot_matX, axis_rotmat;
	Vector n;
	
	//rotate our x axis by our new angle
	v1cen.build_rotatemat(volrender.volrenderview.cutting_planes.planes[selected].perpX, angle, &rot_matX.m[0]);
	volrender.volrenderview.cutting_planes.planes[selected].perpY = volrender.volrenderview.cutting_planes.planes[selected].perpY.mult_matrix(rot_matX.m);
	
	//and evaluate our new normal for the new plane rotation
	n.cross(volrender.volrenderview.cutting_planes.planes[selected].perpY,volrender.volrenderview.cutting_planes.planes[selected].perpX);
	
	//now we can evaluate the rotation matrix to go from our original set of axis to the other
	axis_rotmat.find_rotationfromaxis(volrender.volrenderview.cutting_planes.planes[selected].perpX, volrender.volrenderview.cutting_planes.planes[selected].perpY, n, volrender.volrenderview.cutting_planes.planes[selected].axisX, volrender.volrenderview.cutting_planes.planes[selected].axisY, volrender.volrenderview.cutting_planes.planes[selected].axisZ);
	volrender.volrenderview.cutting_planes.planes[selected].rot_mat = axis_rotmat;
	
	//rotate our plane vertices now
	volrender.volrenderview.cutting_planes.planes[selected].rot_mat = axis_rotmat;	
	v1cen = v1cen.mult_matrix(volrender.volrenderview.cutting_planes.planes[selected].rot_mat.m);
	v2cen = v2cen.mult_matrix(volrender.volrenderview.cutting_planes.planes[selected].rot_mat.m);
	v3cen = v3cen.mult_matrix(volrender.volrenderview.cutting_planes.planes[selected].rot_mat.m);
	v4cen = v4cen.mult_matrix(volrender.volrenderview.cutting_planes.planes[selected].rot_mat.m);

	//and now retranslate our plane to its original position
	volrender.volrenderview.cutting_planes.planes[selected]._v1 = v1cen+cen;
	volrender.volrenderview.cutting_planes.planes[selected]._v2 = v2cen+cen;
	volrender.volrenderview.cutting_planes.planes[selected]._v3 = v3cen+cen;
	volrender.volrenderview.cutting_planes.planes[selected]._v4 = v4cen+cen;
	Vector vcen = volrender.volrenderview.cutting_planes.planes[selected].centre-cen;
	volrender.volrenderview.cutting_planes.planes[selected]._v1 += vcen;
	volrender.volrenderview.cutting_planes.planes[selected]._v2 += vcen;
	volrender.volrenderview.cutting_planes.planes[selected]._v3 += vcen;
	volrender.volrenderview.cutting_planes.planes[selected]._v4 += vcen;

	//evaluate the equation of the plane, the intersection points with the bounding box, and upload our planes to the GPU
	volrender.volrenderview.cutting_planes.planes[selected].eval_planeequation();
	volrender.volrenderview.cutting_planes.eval_planeintersections();
}
void QtGLWidget::cutplane_planerot2_slot(float angle)
{
	//find which plane we have currently active
	int selected=-1;
	for(int i=0; i<6; i++)
	{
		if(volrender.volrenderview.cutting_planes.selected[i]==1) selected = i;
	}
	//if none then stop
	if(selected==-1) return;

	//increment our angle of rotation.
	volrender.volrenderview.cutting_planes.planes[selected].angleY += angle;

	//translate our clipping plane to the origin
	Vector cen = volrender.volrenderview.cutting_planes.planes[selected].v1+volrender.volrenderview.cutting_planes.planes[selected].v2+volrender.volrenderview.cutting_planes.planes[selected].v3+volrender.volrenderview.cutting_planes.planes[selected].v4;
	cen *= 0.25;
	Vector v1cen = volrender.volrenderview.cutting_planes.planes[selected].v1-cen;
	Vector v2cen = volrender.volrenderview.cutting_planes.planes[selected].v2-cen;
	Vector v3cen = volrender.volrenderview.cutting_planes.planes[selected].v3-cen;
	Vector v4cen = volrender.volrenderview.cutting_planes.planes[selected].v4-cen;

	Matrix4x4 rot_matY, axis_rotmat;
	Vector n;
	
	//rotate our y axis by our new angle
	v1cen.build_rotatemat(volrender.volrenderview.cutting_planes.planes[selected].perpY, angle, &rot_matY.m[0]);
	volrender.volrenderview.cutting_planes.planes[selected].perpX = volrender.volrenderview.cutting_planes.planes[selected].perpX.mult_matrix(rot_matY.m);

	//and evaluate our new normal for the new plane rotation
	n.cross(volrender.volrenderview.cutting_planes.planes[selected].perpX,volrender.volrenderview.cutting_planes.planes[selected].perpY);

	//now we can evaluate the rotation matrix to go from our original set of axis to the other
	axis_rotmat.find_rotationfromaxis(volrender.volrenderview.cutting_planes.planes[selected].perpX, volrender.volrenderview.cutting_planes.planes[selected].perpY, n, volrender.volrenderview.cutting_planes.planes[selected].axisX, volrender.volrenderview.cutting_planes.planes[selected].axisY, volrender.volrenderview.cutting_planes.planes[selected].axisZ);
	volrender.volrenderview.cutting_planes.planes[selected].rot_mat = axis_rotmat;
	
	//rotate our plane vertices now
	v1cen = v1cen.mult_matrix(volrender.volrenderview.cutting_planes.planes[selected].rot_mat.m);
	v2cen = v2cen.mult_matrix(volrender.volrenderview.cutting_planes.planes[selected].rot_mat.m);
	v3cen = v3cen.mult_matrix(volrender.volrenderview.cutting_planes.planes[selected].rot_mat.m);
	v4cen = v4cen.mult_matrix(volrender.volrenderview.cutting_planes.planes[selected].rot_mat.m);
	
	//and now retranslate our plane to its original position	
	volrender.volrenderview.cutting_planes.planes[selected]._v1 = v1cen+cen;
	volrender.volrenderview.cutting_planes.planes[selected]._v2 = v2cen+cen;
	volrender.volrenderview.cutting_planes.planes[selected]._v3 = v3cen+cen;
	volrender.volrenderview.cutting_planes.planes[selected]._v4 = v4cen+cen;
	Vector vcen = volrender.volrenderview.cutting_planes.planes[selected].centre-cen;
	volrender.volrenderview.cutting_planes.planes[selected]._v1 += vcen;
	volrender.volrenderview.cutting_planes.planes[selected]._v2 += vcen;
	volrender.volrenderview.cutting_planes.planes[selected]._v3 += vcen;
	volrender.volrenderview.cutting_planes.planes[selected]._v4 += vcen;

	//evaluate the equation of the plane, the intersection points with the bounding box, and upload our planes to the GPU
	volrender.volrenderview.cutting_planes.planes[selected].eval_planeequation();
	volrender.volrenderview.cutting_planes.eval_planeintersections();
}
void QtGLWidget::cutplane_interact(float dy)
{
	int selected=-1;
	for(int i=0; i<6; i++)
	{
		if(volrender.volrenderview.cutting_planes.selected[i]==1) selected = i;
	}
	
	if(selected==-1) return;
	
	float translate, angle1, angle2;

	if(volrender.volrenderview.cutting_planes.rotmode==0)
	{
		dy *= (float)volobject.maxres/200.0;
		translate = dy;
		cutplane_planeslider_slot(dy);
		redraw_event();
	}
	if(volrender.volrenderview.cutting_planes.rotmode==1)
	{
		dy *= 360.0/200.0;
		Vector n0;
		n0.get_normal(volrender.volrenderview.cutting_planes.planes[selected].v1, volrender.volrenderview.cutting_planes.planes[selected].v2, volrender.volrenderview.cutting_planes.planes[selected].v3);
		Vector vv5;
		vv5.cross(n0, volrender.volrenderview.cutting_planes.planes[selected].normal);
		float angle = n0.signed_angle(volrender.volrenderview.cutting_planes.planes[selected].normal, vv5);
		angle=dy*(PI/180.0);
		angle1 = angle;
		cutplane_planerot1_slot(angle);
		redraw_event();
	}
	if(volrender.volrenderview.cutting_planes.rotmode==2)
	{
		dy *= 360.0/200.0;
		Vector n0;
		n0.get_normal(volrender.volrenderview.cutting_planes.planes[selected].v1, volrender.volrenderview.cutting_planes.planes[selected].v2, volrender.volrenderview.cutting_planes.planes[selected].v3);				
		Vector vv5;
		vv5.cross(n0, volrender.volrenderview.cutting_planes.planes[selected].normal);
		float angle = n0.signed_angle(volrender.volrenderview.cutting_planes.planes[selected].normal, vv5);
		angle=dy*(PI/180.0);
		angle2 = angle;
		cutplane_planerot2_slot(angle);
		redraw_event();
	}
}

//===========================================================================================================

void QtGLWidget::movie_orthosectmovie_slot(QWidget* parent, int sampling, int preview, QString dn)
{
	QColor oldcol = volrender.background_colour;
	QColor oldbackupcol = volrender.old_background_colour;
	
	volrender.old_background_colour = Qt::black;
	volrender.background_colour = Qt::black;

	int steps = sampling;
	int framecount_helper=0;
	int numbframes = ((volobject.texdepth)/steps)+((volobject.texheight)/steps)+((volobject.texwidth)/steps);

	int oldzoom = volrender.volrenderview.zoom;
	volrender.volrenderview.zoom = -(volobject.maxres/2.0)*0.85;
	
	Q3ProgressDialog* progressbar = NULL;

	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Saving Movie Frames...", "Cancel", numbframes, parent, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
		progressbar->setMinimumDuration(10);
	}

	volrender.volrenderview.winWidth = width();
	volrender.volrenderview.winHeight = height();
	volrender.volrenderview.arcball.setBounds((GLfloat)width(), (GLfloat)height());

	QString filename = dn;
	QString extension = ".png";
	QString framenumber;

	if(!preview)
	{
		QDir dir = QDir(dn);
		dir.mkdir("XSections");
		dir.mkdir("YSections");
		dir.mkdir("ZSections");
	}
	
	QImage im;
	QPixmap pm;

	int x=0;
	int y=0;
	int z=0;
	int oldx=0;
	int oldy=0;
	int oldz=0;
	int i=0;
	int framecount=0;

	Vector mousepos;

	volrender.setViewfromEuler(x,y,z);

	volrender.x_slice=0;
	volrender.y_slice=0;
	volrender.z_slice=0;
	volrender.volume_toggle = 0;
	volrender.slice_draw_mode = 2;

	redraw_event();	

	for(i=0; i<volobject.texdepth; i+= steps)
	{
		emit frames_redraw_signal();
		redraw_event();
	
		//====================================================
		//save the current frame
		//====================================================
		framenumber.setNum(framecount+300000);
		filename = dn+"\\ZSections\\";
		filename += framenumber;
		filename += extension;
		makeCurrent();
		
		if(!preview) emit save_sectionimage_signal(filename, 0, i);
		
		/*if(!preview)
		{
			im = volrender.render_framebuffer(false);
			pm.convertFromImage(im);
			im.reset();
			if(!pm.isNull()) pm.save(filename, "PNG");
		}*/

		framecount++;
		//if(i<volobject.texdepth-1) 
		volrender.z_slice+=steps;
		//else volrender.z_slice-=steps;
		
		//if(volrender.z_slice<0) volrender.z_slice=0;
		framecount_helper++;

		QApplication::processEvents();

		if(showGUI)
		{
			progressbar->setProgress(framecount_helper);
			if (progressbar->wasCancelled())
			{
				volrender.volume_toggle = 1;
				volrender.slice_draw_mode = 0;
				volrender.background_colour = oldcol;
				volrender.old_background_colour = oldbackupcol;
				volrender.volrenderview.zoom = oldzoom;

				redraw_event();
				delete progressbar;
				return;
			}
		}
		else progress(framecount_helper, numbframes);
	}
	volrender.x_slice=0;
	volrender.y_slice=0;
	volrender.z_slice=0;

	x += 90;
	volrender.setViewfromEuler2(x,y,z);
	redraw_event();

	framecount=0;
	for(i=0; i<volobject.texheight; i+= steps)
	{
		emit frames_redraw_signal();
	
		redraw_event();	

		//====================================================
		//save the current frame
		//====================================================
		framenumber.setNum(framecount+200000);
		filename = dn+"\\YSections\\";
		filename += framenumber;
		filename += extension;

		if(!preview) emit save_sectionimage_signal(filename, 1, i);
		
		/*if(!preview)
		{
			im = volrender.render_framebuffer(false);
			pm.convertFromImage(im);			
			im.reset();
			if(!pm.isNull()) pm.save(filename, "PNG");
		}*/

		framecount++;
		//if(i<volobject.texheight-1) 
		volrender.y_slice+=steps;
		//else volrender.y_slice-=steps;
		//if(volrender.y_slice<0) volrender.y_slice=0;
		framecount_helper++;

		QApplication::processEvents();

		if(showGUI)
		{
			progressbar->setProgress(framecount_helper);
			if (progressbar->wasCancelled())
			{
				volrender.volume_toggle = 1;
				volrender.slice_draw_mode = 0;
				volrender.background_colour = oldcol;
				volrender.old_background_colour = oldbackupcol;
				volrender.volrenderview.zoom = oldzoom;
				redraw_event();
				delete progressbar;
				return;
			}
		}
		else progress(framecount_helper, numbframes);

	}
	volrender.x_slice=0;
	volrender.y_slice=0;
	volrender.z_slice=0;

	z += 90;
	volrender.setViewfromEuler2(x,y,z);
	redraw_event();
	updateOverlayGL();

	framecount=0;
	for(i=0; i<volobject.texwidth; i+= steps)
	{
		emit frames_redraw_signal();
		redraw_event();
	
		//====================================================
		//save the current frame
		//====================================================
		framenumber.setNum(framecount+100000);
		filename = dn+"\\XSections\\";
		filename += framenumber;
		filename += extension;
		makeCurrent();
		
		/*if(!preview)
		{
			im = volrender.render_framebuffer(false);
			pm.convertFromImage(im);
			im.reset();			
			if(!pm.isNull()) pm.save(filename, "PNG");
		}*/
		if(!preview) emit save_sectionimage_signal(filename, 2, i);
	

		framecount++;
		//if(i<volobject.texwidth-1)
		volrender.x_slice+=steps;
		//else volrender.x_slice-=steps;
		//if(volrender.x_slice<0) volrender.x_slice=0;
		framecount_helper++;

		QApplication::processEvents();

		if(showGUI)
		{
			progressbar->setProgress(framecount_helper);
			if (progressbar->wasCancelled())
			{
				volrender.volume_toggle = 1;
				volrender.slice_draw_mode = 0;
				volrender.background_colour = oldcol;
				volrender.old_background_colour = oldbackupcol;
				volrender.volrenderview.zoom = oldzoom;
				redraw_event();
				delete progressbar;
				return;
			}
		}
		else progress(framecount_helper, numbframes);

	}

	volrender.volume_toggle = 1;
	volrender.slice_draw_mode = 0;
	volrender.background_colour = oldcol;
	volrender.old_background_colour = oldbackupcol;
	volrender.volrenderview.zoom = oldzoom;

	redraw_event();
	
	if(showGUI)
	{
		delete progressbar;
		progressbar = NULL;
	}
	else progress(numbframes, numbframes);
}
void QtGLWidget::movie_rockmovie_slot(QWidget* parent, int sampling, int preview, QString dn, float ang)
{
	int framecount=0;
	int steps = sampling;

	/*Q3ProgressDialog* progressbar = new Q3ProgressDialog("Saving Movie Frames...", "Cancel", ang, parent, "progress", TRUE);		
	progressbar->setCaption("Please Wait");
	progressbar->setMinimumDuration(10);*/

	//for(int ii=0; ii<volrender.timelapsefoldersize; ii+=2)
	{
		for(int i=0; i>=-ang/2.0; i-=steps)
		{
			volrender.setViewfromEuler2(0,(float)i,0);

			/*progressbar->setProgress(i);
			qApp->processEvents();
			if (progressbar->wasCancelled())
			{
				delete progressbar;
				progressbar = NULL;
				volrender.setViewfromEuler2(0,0,0);
				makeCurrent();
				redraw_event();
				updateOverlayGL();
				return;
			}*/

			QString filename = dn + "//";
			QString extension = ".png";
			QString framenumber;		
			framenumber.setNum(framecount+100000);
			filename += framenumber;
			filename += extension;
			
			redraw_event();
			
			if(!preview)
			{
				QImage im = volrender.render_framebuffer(false);
				QPixmap pm;
				pm.convertFromImage(im);
				im.reset();
				if(!pm.isNull()) pm.save(filename, "PNG");	
			}
			
			framecount = framecount + 1;
		}
		//emit timelapseEvent();
		for(int i=-ang/2.0; i<=ang/2.0; i+=steps)
		{
			volrender.setViewfromEuler2(0,(float)i,0);

			/*progressbar->setProgress(i);
			qApp->processEvents();
			if (progressbar->wasCancelled())
			{
				delete progressbar;
				progressbar = NULL;
				volrender.setViewfromEuler2(0,0,0);
				makeCurrent();
				redraw_event();
				updateOverlayGL();
				return;
			}*/

			QString filename = dn + "//";
			QString extension = ".png";
			QString framenumber;		
			framenumber.setNum(framecount+100000);
			filename += framenumber;
			filename += extension;
			
			redraw_event();
			
			if(!preview)
			{
				QImage im = volrender.render_framebuffer(false);
				QPixmap pm;
				pm.convertFromImage(im);
				im.reset();
				if(!pm.isNull()) pm.save(filename, "PNG");	
			}
			
			framecount = framecount + 1;
		}
		//emit timelapseEvent();
		for(int i=ang/2.0; i>=0; i-=steps)
		{
			volrender.setViewfromEuler2(0,(float)i,0);

			/*progressbar->setProgress(i);
			qApp->processEvents();
			if (progressbar->wasCancelled())
			{
				delete progressbar;
				progressbar = NULL;
				volrender.setViewfromEuler2(0,0,0);
				makeCurrent();
				redraw_event();
				updateOverlayGL();
				return;
			}*/

			QString filename = dn + "//";
			QString extension = ".png";
			QString framenumber;		
			framenumber.setNum(framecount+100000);
			filename += framenumber;
			filename += extension;
			
			redraw_event();
			
			if(!preview)
			{
				QImage im = volrender.render_framebuffer(false);
				QPixmap pm;
				pm.convertFromImage(im);
				im.reset();
				if(!pm.isNull()) pm.save(filename, "PNG");	
			}
			
			framecount = framecount + 1;
		}
	}

	redraw_event();
}
void QtGLWidget::movie_rotmovie_slot(QWidget* parent, int sampling, int preview, QString dn, int x, int y, int z, float ang)
{
	int framecount=0;
	int steps = sampling;

	Q3ProgressDialog* progressbar = NULL;

	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Saving Movie Frames...", "Cancel", ang, parent, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
		progressbar->setMinimumDuration(10);
	}

	for(int i=0; i<=ang; i+=steps)
	{
		if(x==1 && y==0 && z==0) volrender.setViewfromEuler2((float)i,0,0);
		if(x==0 && y==1 && z==0) volrender.setViewfromEuler2(0,(float)i,0);
		if(x==0 && y==0 && z==1) volrender.setViewfromEuler2(0,0,(float)i);
		if(x==1 && y==1 && z==0) volrender.setViewfromEuler2((float)i,(float)i,0);
		if(x==1 && y==0 && z==1) volrender.setViewfromEuler2((float)i,0,(float)i);
		if(x==0 && y==1 && z==1) volrender.setViewfromEuler2(0,(float)i,(float)i);
		if(x==1 && y==1 && z==1) volrender.setViewfromEuler2((float)i,(float)i,(float)i);

		QApplication::processEvents();

		if(showGUI)
		{
			progressbar->setProgress(i);
			if (progressbar->wasCancelled())
			{
				delete progressbar;
				progressbar = NULL;
				volrender.setViewfromEuler2(0,0,0);
				makeCurrent();
				redraw_event();
				updateOverlayGL();
				return;
			}
		}
		else progress(i, ang);

		QString filename = dn + "//";
		QString extension = ".png";
		QString framenumber;		
		framenumber.setNum(framecount+100000);
		filename += framenumber;
		filename += extension;
		
		redraw_event();
		
		if(!preview)
		{
			QImage im = volrender.render_framebuffer(false);
			QPixmap pm;
			pm.convertFromImage(im);
			im.reset();
			if(!pm.isNull()) pm.save(filename, "PNG");	
		}
		
		framecount = framecount + 1;
	}
	
	if(showGUI)
	{
		delete progressbar;
		progressbar = NULL;
	}
	else progress(ang, ang);

	redraw_event();
}
void QtGLWidget::movie_renderviewmovie_slot(QWidget* parent, int sampling, int preview, QString dn)
{
	//printf("BOH\n");
	emit renderview_getallviews_signal();
	
	//printf("size: %d\n", volumeviews.size());

	VolumeRenderView src, dst;
	Quaternion src_quat, dst_quat, result_quat;
	PlaneWidgetInCube src_cplanes, dst_cplanes, result_cplanes;
	Matrix3f_t mat;

	Vector src_at;
	Vector dst_at;
	Vector result_at;

	Vector src_eye;
	Vector dst_eye;
	Vector result_eye;

	Vector src_up;
	Vector dst_up;
	Vector result_up;

	float src_zoom;
	float dst_zoom;
	float result_zoom;

	float src_fov;
	float dst_fov;
	float result_fov;

	int framecount=0;
	float timepointframes = 64;

	int src_rgb_array[256*3];
	int src_ragaba_array[256*3];
	int dst_rgb_array[256*3];
	int dst_ragaba_array[256*3];
	int result_rgb_array[256*3];
	int result_ragaba_array[256*3];

	int maxframes = (volumeviews.size())*(timepointframes/(float)sampling);

	Q3ProgressDialog* progressbar = NULL;

	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Saving Movie Frames...", "Cancel", maxframes, parent, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
		progressbar->setMinimumDuration(10);
	}

	volrender.volrenderview.arcball.setIdentity();
	for(int j=0; j<volumeviews.size()-1; j++)
	{
		src = volumeviews[j];
		src_fov = volumeviews[j].camera.fov;
		src_at = volumeviews[j].at;
		src_eye = volumeviews[j].eye;
		src_up = volumeviews[j].up;
		src_zoom = volumeviews[j].zoom;
		src_cplanes = volumeviews[j].cutting_planes;
		volrender.compute_transfer_function(3, src.redluminance, src_rgb_array);
		volrender.compute_transfer_function(4, src.greenluminance, src_rgb_array);
		volrender.compute_transfer_function(5, src.blueluminance, src_rgb_array);
		volrender.compute_transfer_function(6, src.redalpha, src_ragaba_array);
		volrender.compute_transfer_function(7, src.greenalpha, src_ragaba_array);
		volrender.compute_transfer_function(8, src.bluealpha, src_ragaba_array);

		dst = volumeviews[j+1];
		dst_fov = volumeviews[j+1].camera.fov-src_fov;
		dst_at = volumeviews[j+1].at-src_at;
		dst_eye = volumeviews[j+1].eye-src_eye;
		dst_up = volumeviews[j+1].up-src_up;
		dst_zoom = volumeviews[j+1].zoom-src_zoom;
		dst_cplanes = volumeviews[j+1].cutting_planes;
		volrender.compute_transfer_function(3, dst.redluminance, dst_rgb_array);
		volrender.compute_transfer_function(4, dst.greenluminance, dst_rgb_array);
		volrender.compute_transfer_function(5, dst.blueluminance, dst_rgb_array);
		volrender.compute_transfer_function(6, dst.redalpha, dst_ragaba_array);
		volrender.compute_transfer_function(7, dst.greenalpha, dst_ragaba_array);
		volrender.compute_transfer_function(8, dst.bluealpha, dst_ragaba_array);

		src_quat.from_matrix(&src.arcball.ThisRot);
		dst_quat.from_matrix(&dst.arcball.ThisRot);

		src_quat.quat.normalize();
		dst_quat.quat.normalize();

		float interp=0;
		for(int i=0; i<=(int)timepointframes; i+=sampling)
		{
			if(showGUI)
			{
				progressbar->setProgress(framecount);

				//printf("interp: %f\n", interp);
				if (progressbar->wasCancelled())
				{
					volrender.volrenderview = volumeviews[0];
					makeCurrent();
					redraw_event();
					updateOverlayGL();
					return;
				}
			}
			else progress(framecount, maxframes);

			//our interpolator
			interp = (float)i/timepointframes;

			//tfuncs
			for(int j=0; j<256*3; j++)
			{
				result_rgb_array[j] = src_rgb_array[j] + interp * (dst_rgb_array[j]-src_rgb_array[j]);
				result_ragaba_array[j] = src_ragaba_array[j]+ interp * (dst_ragaba_array[j]-src_ragaba_array[j]);
			}

			//fov
			result_fov = src_fov + interp*dst_fov;
			
			//translation
			result_at = src_at + interp*dst_at;
			result_eye = src_eye + interp*dst_eye;
			result_up = src_up + interp*dst_up;
			
			//zoom
			result_zoom = src_zoom + interp*dst_zoom;

			//rotation
			slerp(&src_quat, &dst_quat, interp, &result_quat);

			//cutting planes
			result_cplanes.interpolate(src_cplanes, dst_cplanes, interp);

			//apply interpolation results
			Matrix3fSetRotationFromQuat4f(&volrender.volrenderview.arcball.ThisRot, &result_quat.quat);		// Convert Quaternion Into Matrix3fT
			Matrix3fMulMatrix3f(&volrender.volrenderview.arcball.ThisRot, &volrender.volrenderview.arcball.LastRot);				// Accumulate Last Rotation Into This One
			Matrix4fSetRotationFromMatrix3f(&volrender.volrenderview.arcball.Transform, &volrender.volrenderview.arcball.ThisRot);	// Set Our Final Transform's Rotation From This One
			
			volrender.volrenderview.camera.fov = result_fov;
			volrender.volrenderview.at = result_at;
			volrender.volrenderview.eye = result_eye;
			volrender.volrenderview.up = result_up;
			volrender.volrenderview.zoom = result_zoom;

			if(volrender.volrenderview.projection_toggle==PERSP_PROJECTION) volrender.volrenderview.lookat();

			volrender.volrenderview.cutting_planes = result_cplanes;
			volrender.apply_transfer_functions(&result_rgb_array[0], &result_ragaba_array[0]);

			QApplication::processEvents();
			redraw_event();

			QString filename = dn + "//";
			QString extension = ".png";
			QString framenumber;		
			framenumber.setNum(framecount+100000);
			filename += framenumber;
			filename += extension;
			
			makeCurrent();
			redraw_event();
			updateOverlayGL();
			
			if(!preview)
			{
				QImage im = volrender.render_framebuffer(false);
				QPixmap pm;
				pm.convertFromImage(im);
				im.reset();
				if(!pm.isNull()) pm.save(filename, "PNG");	
			}
			
			framecount = framecount + 1;
		}
	}

	volumeviews.clear();
	
	if(showGUI)
	{
		progressbar->setProgress(maxframes);
		delete progressbar;
		progressbar = NULL;
	}
	else progress(maxframes, maxframes);
}

void QtGLWidget::movie_clipmovie_slot(QWidget* parent, int sampling, int preview, QString dn, int xaxis, int yaxis, int zaxis, float ang, int clipmode, int clipafterrot)
{
	int x=0;
	int y=0;
	int z=0;
	int oldx=0;
	int oldy=0;
	int oldz=0;
	int i=0;
	int steps = sampling;
	int framecount = 0;
	
	Vector mousepos = 0.0;
	volrender.volrenderview.cutting_planes.invert_cut=0;
	planes_toggle = 1;

	//select the right clipping plane and size
	int clipplane_selected = 0;
	int clipdimensions = 0;

	volrender.volrenderview.cutting_planes.moviemode = 1;

	if(clipmode==3)
	{
		clipplane_selected = 1;
		clipdimensions = volobject.texdepth;
	}
	else if(clipmode==2)
	{
		clipplane_selected = 5;
		clipdimensions = volobject.texheight;
	}
	else if(clipmode==1)
	{
		clipplane_selected = 3;
		clipdimensions = volobject.texwidth;
	}
	else if(clipmode==0)
	{
		clipplane_selected = 1;
	
		clipdimensions=0.0;

		if(clipdimensions<volobject.texdepth*1.5)
			clipdimensions = volobject.texdepth*1.5;
		
		if(clipdimensions<volobject.texheight*1.5)
			clipdimensions = volobject.texheight*1.5;

		if(clipdimensions<volobject.texwidth*1.5)
			clipdimensions = volobject.texwidth*1.5;

		Vector v1, v2, v3, v4;
		v1 = Vector(-1.0,  1.0,  1.0);
		v2 = Vector( 1.0,  1.0,  1.0);
		v3 = Vector(-1.0, -1.0,  1.0);
		v4 = Vector( 1.0, -1.0,  1.0);
		v1 *= clipdimensions/2.0;
		v2 *= clipdimensions/2.0;
		v3 *= clipdimensions/2.0;
		v4 *= clipdimensions/2.0;

		//Matrix4f_t mat = Matrix4fInverse(&volrender.volrenderview.arcball.Transform);
		Matrix4f_t mat = volrender.volrenderview.arcball.Transform;
		
	/*	v1 = v1.mult_matrix(mat.M);
		v2 = v2.mult_matrix(mat.M);
		v3 = v3.mult_matrix(mat.M);
		v4 = v4.mult_matrix(mat.M);
*/
		Matrix4fMulVector4f(mat, &v1);
		Matrix4fMulVector4f(mat, &v2);
		Matrix4fMulVector4f(mat, &v3);
		Matrix4fMulVector4f(mat, &v4);

		volrender.volrenderview.cutting_planes.planes[clipplane_selected] = PlaneWidget(v1,v2,v4,v3,1);
		volrender.volrenderview.cutting_planes.planes[clipplane_selected]._v1 = v1;
		volrender.volrenderview.cutting_planes.planes[clipplane_selected]._v2 = v2;
		volrender.volrenderview.cutting_planes.planes[clipplane_selected]._v3 = v4;
		volrender.volrenderview.cutting_planes.planes[clipplane_selected]._v4 = v3;
		volrender.volrenderview.cutting_planes.planes[clipplane_selected].v1 = v1;
		volrender.volrenderview.cutting_planes.planes[clipplane_selected].v2 = v2;
		volrender.volrenderview.cutting_planes.planes[clipplane_selected].v3 = v4;
		volrender.volrenderview.cutting_planes.planes[clipplane_selected].v4 = v3;
		volrender.volrenderview.cutting_planes.planes[clipplane_selected].normal.get_normal(v1,v2,v3);
		volrender.volrenderview.cutting_planes.planes[clipplane_selected].eval_planeequation();
		volrender.volrenderview.cutting_planes.planes[clipplane_selected].mouseMove(Vector(0.0, 0.0, 0.0));

		//volrender.volrenderview.cutting_planes.planes[clipplane_selected].eval_planeequation();
	}

	float anglestepper = ang / clipdimensions;
	anglestepper /= 2.0;
	anglestepper *= steps;

	if(ang==0) anglestepper = 0.0;
	float angiterp = 0;

	if(clipafterrot==0) ang = 0;

	Q3ProgressDialog* progressbar = NULL;

	if(showGUI)
	{
		progressbar = new Q3ProgressDialog("Saving Movie Frames...", "Cancel", ang+2+(clipdimensions*2.0)/steps, parent, "progress", TRUE);		
		progressbar->setCaption("Please Wait");
		progressbar->setMinimumDuration(10);
	}

	if(clipafterrot==1)
	{
		for(int i=0; i<=ang; i+=steps)
		{
			if(xaxis==1 && yaxis==0 && zaxis==0) volrender.setViewfromEuler2((float)i,0,0);
			if(xaxis==0 && yaxis==1 && zaxis==0) volrender.setViewfromEuler2(0,(float)i,0);
			if(xaxis==0 && yaxis==0 && zaxis==1) volrender.setViewfromEuler2(0,0,(float)i);
			if(xaxis==1 && yaxis==1 && zaxis==0) volrender.setViewfromEuler2((float)i,(float)i,0);
			if(xaxis==1 && yaxis==0 && zaxis==1) volrender.setViewfromEuler2((float)i,0,(float)i);
			if(xaxis==0 && yaxis==1 && zaxis==1) volrender.setViewfromEuler2(0,(float)i,(float)i);
			if(xaxis==1 && yaxis==1 && zaxis==1) volrender.setViewfromEuler2((float)i,(float)i,(float)i);

			QApplication::processEvents();

			if(showGUI)
			{
				progressbar->setProgress(i);

			
				if (progressbar->wasCancelled())
				{
					delete progressbar;
					progressbar = NULL;
					volrender.setViewfromEuler2(0,0,0);
					volrender.volrenderview.cutting_planes.reset_all();
					makeCurrent();
					redraw_event();
					updateOverlayGL();
					return;
				}
			}
			else progress(i, ang+2+(clipdimensions*2.0)/steps);

			QString filename = dn + "//";
			QString extension = ".png";
			QString framenumber;		
			framenumber.setNum(framecount+100000);
			filename += framenumber;
			filename += extension;
			
			makeCurrent();
			redraw_event();
			updateOverlayGL();
			
			if(!preview)
			{
				QImage im = volrender.render_framebuffer(false);
				QPixmap pm;
				pm.convertFromImage(im);
				im.reset();
				if(!pm.isNull()) pm.save(filename, "PNG");
			}
			
			framecount = framecount + 1;
		}
	}

	volrender.volrenderview.cutting_planes.select(clipplane_selected);
	volrender.volrenderview.cutting_planes.mousePress(mousepos, 3);
	for(i=0; i>=-clipdimensions; i-=steps)
	{
		if(clipafterrot==0)
		{
			if(xaxis==1 && yaxis==0 && zaxis==0) volrender.setViewfromEuler2((float)angiterp,0,0);
			if(xaxis==0 && yaxis==1 && zaxis==0) volrender.setViewfromEuler2(0,(float)angiterp,0);
			if(xaxis==0 && yaxis==0 && zaxis==1) volrender.setViewfromEuler2(0,0,(float)angiterp);
			if(xaxis==1 && yaxis==1 && zaxis==0) volrender.setViewfromEuler2((float)angiterp,(float)angiterp,0);
			if(xaxis==1 && yaxis==0 && zaxis==1) volrender.setViewfromEuler2((float)angiterp,0,(float)angiterp);
			if(xaxis==0 && yaxis==1 && zaxis==1) volrender.setViewfromEuler2(0,(float)angiterp,(float)angiterp);
			if(xaxis==1 && yaxis==1 && zaxis==1) volrender.setViewfromEuler2((float)angiterp,(float)angiterp,(float)angiterp);
		}

		//move our clipping plane
		mousepos.y = i;
		volrender.volrenderview.cutting_planes.mouseMove(mousepos);

		//save the current frame
		QString filename = dn + "//";
		QString extension = ".png";
		QString framenumber;		
		framenumber.setNum(framecount+100000);
		filename += framenumber;
		filename += extension;
		
		makeCurrent();
		redraw_event();
		updateOverlayGL();
		if(!preview)
		{
			QImage im = volrender.render_framebuffer(false);
			QPixmap pm;
			pm.convertFromImage(im);
			im.reset();
			if(!pm.isNull()) pm.save(filename, "PNG");	
		}

		//update our frame counter
		framecount = framecount + 1;

		QApplication::processEvents();

		if(showGUI)
		{
			//update our progress bar
			progressbar->setProgress(framecount);
			if (progressbar->wasCancelled()) 
			{
				Vector v1, v2, v3, v4;
				v1 = Vector(-1.0,  1.0,  1.0);
				v2 = Vector( 1.0,  1.0,  1.0);
				v3 = Vector(-1.0, -1.0,  1.0);
				v4 = Vector( 1.0, -1.0,  1.0);
				v1 *= clipdimensions/2.0;
				v2 *= clipdimensions/2.0;
				v3 *= clipdimensions/2.0;
				v4 *= clipdimensions/2.0;
				volrender.volrenderview.cutting_planes.planes[clipplane_selected] = PlaneWidget(v1,v2,v4,v3,1);

				volrender.setViewfromEuler2(0,0,0);
				//volrender.volrenderview.cutting_planes.select(-1);
				planes_toggle = 0;
				volrender.volrenderview.cutting_planes.reset_all();
				redraw_event();
				updateOverlayGL();
				volrender.volrenderview.cutting_planes.moviemode = 0;
				return;
			}
		}
		else progress(framecount, ang+2+(clipdimensions*2.0)/steps);

		angiterp += anglestepper;
	}
	for(i=-clipdimensions; i<=0; i+=steps)
	{
		if(clipafterrot==0)
		{
			if(xaxis==1 && yaxis==0 && zaxis==0) volrender.setViewfromEuler2((float)angiterp,0,0);
			if(xaxis==0 && yaxis==1 && zaxis==0) volrender.setViewfromEuler2(0,(float)angiterp,0);
			if(xaxis==0 && yaxis==0 && zaxis==1) volrender.setViewfromEuler2(0,0,(float)angiterp);
			if(xaxis==1 && yaxis==1 && zaxis==0) volrender.setViewfromEuler2((float)angiterp,(float)angiterp,0);
			if(xaxis==1 && yaxis==0 && zaxis==1) volrender.setViewfromEuler2((float)angiterp,0,(float)angiterp);
			if(xaxis==0 && yaxis==1 && zaxis==1) volrender.setViewfromEuler2(0,(float)angiterp,(float)angiterp);
			if(xaxis==1 && yaxis==1 && zaxis==1) volrender.setViewfromEuler2((float)angiterp,(float)angiterp,(float)angiterp);
		}		
			//move our clipping plane
			mousepos.y = i;
			volrender.volrenderview.cutting_planes.mouseMove(mousepos);

			//save the current frame
			QString filename = dn + "//";
			QString extension = ".png";
			QString framenumber;		
			framenumber.setNum(framecount+100000);
			filename += framenumber;
			filename += extension;
			
			makeCurrent();
			redraw_event();
			updateOverlayGL();
			if(!preview)
			{
				QImage im = volrender.render_framebuffer(false);
				QPixmap pm;
				pm.convertFromImage(im);
				im.reset();
				if(!pm.isNull()) pm.save(filename, "PNG");	
			}

			//update our frame counter
			framecount = framecount + 1;
			
			QApplication::processEvents();
			
			if(showGUI)
			{
				progressbar->setProgress(framecount);
				if (progressbar->wasCancelled())
				{
					Vector v1, v2, v3, v4;
					v1 = Vector(-1.0,  1.0,  1.0);
					v2 = Vector( 1.0,  1.0,  1.0);
					v3 = Vector(-1.0, -1.0,  1.0);
					v4 = Vector( 1.0, -1.0,  1.0);
					v1 *= clipdimensions/2.0;
					v2 *= clipdimensions/2.0;
					v3 *= clipdimensions/2.0;
					v4 *= clipdimensions/2.0;
					volrender.volrenderview.cutting_planes.planes[clipplane_selected] = PlaneWidget(v1,v2,v4,v3,1);

					volrender.setViewfromEuler2(0,0,0);
					//volrender.volrenderview.cutting_planes.select(-1);
					volrender.volrenderview.cutting_planes.reset_all();
					planes_toggle = 0;
					redraw_event();
					updateOverlayGL();
					volrender.volrenderview.cutting_planes.moviemode = 0;
					return;
				}
			}
			else progress(framecount, ang+2+(clipdimensions*2.0)/steps);

			angiterp += anglestepper;
	}

	if(showGUI) delete progressbar;
	else progress(ang+2+(clipdimensions*2.0)/steps, ang+2+(clipdimensions*2.0)/steps);

	volrender.volrenderview.cutting_planes.moviemode = 0;

	//volrender.volrenderview.cutting_planes.select(-1);
	volrender.volrenderview.cutting_planes.reset_all();
	planes_toggle = 0;
	redraw_event();
	updateOverlayGL();	
}
void QtGLWidget::cutplane_curvecoords_slot(bool state)
{
	volrender.curve_coords = state;
	updateGL();	
}
void QtGLWidget::cutplane_curveangle_slot(int x, int y)
{
	volrender.curveXang = x;
	volrender.curveYang = y;
	updateGL();	
}
void QtGLWidget::cutplane_curvetrans_slot(bool b)
{
	if(b==true)
		volrender.curve_translate = 1;
	else
		volrender.curve_translate = 0;
}
