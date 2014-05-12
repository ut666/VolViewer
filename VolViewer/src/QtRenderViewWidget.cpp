/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include "QtRenderViewWidget.h"
//Added by qt3to4:
#include <QPixmap>

	
QtRenderViewWidget::QtRenderViewWidget(QWidget* parent, const char* name)
: QWidget( parent, name )
{
	currentworkingdir="";
	setAcceptDrops(true);

	//mylistwidget = NULL;
	currentworkingdir = "";
}
QtRenderViewWidget::~QtRenderViewWidget()
{
}

void QtRenderViewWidget::rebuild(void)
{
}
void QtRenderViewWidget::reset(void)
{

}
void QtRenderViewWidget::create(void)
{
	mylistwidget = new QListWidget();
	mylistwidget->setViewMode(QListView::IconMode);
	mylistwidget->setIconSize(QSize(128, 128));
	mylistwidget->setWrapping(true);
	mylistwidget->setMovement(QListView::Static);
	mylistwidget->setFlow(QListView::LeftToRight);
	mylistwidget->setMinimumHeight(500);
//	mylistwidget->setResizeMode(QListView::);

	connect(mylistwidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(itemDoublClicked_event(QListWidgetItem*)));
	
	Q3ButtonGroup* opensave_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "File:", 0, "opensave_grp");
    opensave_grp->setExclusive( FALSE );
	opensave_grp->setAlignment(Qt::AlignLeft);
	//opensave_grp->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));

		QPushButton* open = new QPushButton(QIcon(QPixmap("icons/fileopen.xpm")), "Open", opensave_grp, "open");
		connect( open, SIGNAL( clicked () ), this, SLOT( open_event() ) );
		//open->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));
		
		QPushButton* save = new QPushButton(QIcon(QPixmap("icons/filesave.xpm")), "Save", opensave_grp, "save");
		connect( save, SIGNAL( clicked () ), this, SLOT( save_event() ) );
		//save->setSizePolicy(QSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum, false));

	Q3ButtonGroup *act_grp = new Q3ButtonGroup( 1, Qt::Horizontal, "Actions:", 0, "act_grp");

		QPushButton* saveview = new QPushButton("Add", act_grp);
		connect(saveview, SIGNAL( clicked () ), this, SLOT( saveview_event() ) );
		
		QPushButton* deleteview = new QPushButton("Delete", act_grp);
		connect(deleteview, SIGNAL( clicked () ), this, SLOT( deleteview_event() ) );

		QPushButton* updateview = new QPushButton("Update", act_grp);
		connect(updateview, SIGNAL( clicked () ), this, SLOT( updateview_event() ) );

		QPushButton* interpolview = new QPushButton("Interpolate Views", act_grp);
		connect(interpolview, SIGNAL( clicked () ), this, SLOT( interpolate_event() ) );
		interpolview->hide();

	Q3BoxLayout * myright_blayout = new Q3BoxLayout ( 0, Q3BoxLayout::TopToBottom, 0, 5, "right_blayout");
	myright_blayout->setAlignment(Qt::AlignTop);
	myright_blayout->addWidget(mylistwidget, 10, Qt::AlignTop);

	Q3BoxLayout * left_blayout = new Q3BoxLayout( 0, Q3BoxLayout::TopToBottom, 0, 5, "left_blayout");
	left_blayout->setAlignment(Qt::AlignTop);
    left_blayout->addWidget( opensave_grp, 0, Qt::AlignTop);
    left_blayout->addWidget( act_grp, 0, Qt::AlignTop);

	//our transfertab top level layout
	Q3BoxLayout * topmost = new Q3BoxLayout ( this, Q3BoxLayout::LeftToRight, 10, 5, "topmost_blayout");
	topmost->addLayout( left_blayout, 1);
	topmost->addLayout( myright_blayout, 100);
}
void QtRenderViewWidget::save_file(QString fn)
{	
	if(!fn.isEmpty())
	{
		ofstream fout;
		fout.open(fn.latin1());
		fout.precision(10);

		fout<<"VRV_VERSION = 1.0"<<endl;
		fout<<"NUMBER_OF_VOLUMERENDERVIEW = "<<volumeviews.size()<<endl;

		for(int i=0; i<volumeviews.size(); i++)
		{
			fout<<"VOLUMERENDERVIEW"<<endl;
			volumeviews[i].save(fout);
		}
	
		fout.close();
	}
}
void QtRenderViewWidget::save_event(void)
{
	QString fn = QFileDialog::getSaveFileName(currentworkingdir, "*.vrv", this );
	save_file(fn);
}
void QtRenderViewWidget::load_file(QString fn)
{
	printf("QtRenderViewWidget::load_file\n");
	//if(!fn.isEmpty())
	//{
		volumeviews.clear();
		mylistwidget->clear();

		ifstream input_file; 
		input_file.open(fn.latin1());

		if(!input_file.is_open()) return;

		Tokenizer token(input_file);
		token.eolIsSignificant(true);
		string label;

		int current_index = -1;
		int current_plane_index = -1;

		//until oef is reached step through each token
		while(!input_file.eof())
		{
			if(token.ttype()==TT_WORD)
			{
				//get the first string lable
				label = token.sval();
				
				if(label=="NUMBER_OF_VOLUMERENDERVIEW")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float size = token.nval();
						//printf("size: %f\n", size);

						volumeviews.resize(size);
					}
				}
				else if(label=="VOLUMERENDERVIEW")
				{
					current_index++;
					//printf("current_index: %d\n", current_index);
					token.nextToken();
				}
				else if(label=="WINWIDTH")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("WINWIDTH: %f\n", var);

						volumeviews[current_index].winWidth = var;
					}
				}
				else if(label=="WINHEIGT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("WINHEIGT: %f\n", var);

						volumeviews[current_index].winHeight = var;
					}
				}
				else if(label=="WINMAXDIM")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("WINMAXDIM: %f\n", var);

						volumeviews[current_index].winMaxDim = var;
					}
				}
				else if(label=="ZOOM")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						//printf("ZOOM: %f\n", var);

						volumeviews[current_index].zoom = var;
					}
				}
				else if(label=="TX")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						//printf("TX: %f\n", var);

						volumeviews[current_index].at.x = var;
					}
				}
				else if(label=="TY")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						//printf("TY: %f\n", var);

						volumeviews[current_index].at.y = var;
					}
				}
				else if(label=="TZ")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						//printf("TZ: %f\n", var);

						volumeviews[current_index].at.z = var;
					}
				}
				else if(label=="BB_VONE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						//printf("BB: %f\n", var);
						volumeviews[current_index].bb_v1.x = var;
						token.nextToken();
						var = token.nval();
						//printf("BB: %f\n", var);
						volumeviews[current_index].bb_v1.y = var;
						token.nextToken();
						var = token.nval();
						//printf("BB: %f\n", var);
						volumeviews[current_index].bb_v1.z = var;
					}
				}
				else if(label=="BB_VTWO")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v2.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v2.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v2.z = var;
					}
				}
				else if(label=="BB_VTHREE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v3.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v3.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v3.z = var;
					}
				}
				else if(label=="BB_VFOUR")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v4.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v4.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v4.z = var;
					}
				}
				else if(label=="BB_VFIVE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v5.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v5.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v5.z = var;
					}
				}
				else if(label=="BB_VSIX")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v6.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v6.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v6.z = var;
					}
				}
				else if(label=="BB_VSEVEN")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v7.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v7.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v7.z = var;
					}
				}
				else if(label=="BB_VEIGHT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v8.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v8.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v8.z = var;
					}
				}
				else if(label=="TEX_TRANSLATE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("TEX_TRANSLATE: %f\n", var);
						volumeviews[current_index].tex_translate.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("TEX_TRANSLATE: %f\n", var);
						volumeviews[current_index].tex_translate.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("TEX_TRANSLATE: %f\n", var);
						volumeviews[current_index].tex_translate.z = var;
					}
				}
				else if(label=="CAMERA_EYE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_EYE: %f\n", var);
						volumeviews[current_index].camera.eye.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("CAMERA_EYE: %f\n", var);
						volumeviews[current_index].camera.eye.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("CAMERA_EYE: %f\n", var);
						volumeviews[current_index].camera.eye.z = var;
					}
				}
				else if(label=="CAMERA_EYE_OLD")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_EYE_OLD: %f\n", var);
						volumeviews[current_index].camera.eyeOld.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("CAMERA_EYE_OLD: %f\n", var);
						volumeviews[current_index].camera.eyeOld.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("CAMERA_EYE_OLD: %f\n", var);
						volumeviews[current_index].camera.eyeOld.z = var;
					}
				}
				else if(label=="CAMERA_EYE_NEW")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_EYE_NEW: %f\n", var);
						volumeviews[current_index].camera.eyeNew.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_EYE_NEW: %f\n", var);
						volumeviews[current_index].camera.eyeNew.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("CAMERA_EYE_NEW: %f\n", var);
						volumeviews[current_index].camera.eyeNew.z = var;
					}
				}
				else if(label=="CAMERA_AT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_AT: %f\n", var);
						volumeviews[current_index].camera.at.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_AT: %f\n", var);
						volumeviews[current_index].camera.at.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_AT: %f\n", var);
						volumeviews[current_index].camera.at.z = var;
					}
				}
				else if(label=="CAMERA_AT_OLD")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_AT_OLD: %f\n", var);
						volumeviews[current_index].camera.atOld.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_AT_OLD: %f\n", var);
						volumeviews[current_index].camera.atOld.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_AT_OLD: %f\n", var);
						volumeviews[current_index].camera.atOld.z = var;
					}
				}
				else if(label=="CAMERA_AT_NEW")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_AT_NEW: %f\n", var);
						volumeviews[current_index].camera.atNew.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_AT_NEW: %f\n", var);
						volumeviews[current_index].camera.atNew.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_AT_NEW: %f\n", var);
						volumeviews[current_index].camera.atNew.z = var;
					}
				}
				else if(label=="CAMERA_UP")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_UP: %f\n", var);
						volumeviews[current_index].camera.up.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_UP: %f\n", var);
						volumeviews[current_index].camera.up.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_UP: %f\n", var);
						volumeviews[current_index].camera.up.z = var;
					}
				}
				else if(label=="CAMERA_STEPS")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_STEPS: %f\n", var);

						volumeviews[current_index].camera.steps = var;
					}
				}
				else if(label=="CAMERA_MAXSTEPS")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_MAXSTEPS: %f\n", var);

						volumeviews[current_index].camera.maxSteps = var;
					}
				}
				else if(label=="CAMERA_SMOOTH")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_SMOOTH: %f\n", var);

						volumeviews[current_index].camera.smooth = var;
					}
				}
				else if(label=="CAMERA_FOV")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_FOV: %f\n", var);

						volumeviews[current_index].camera.fov = var;
					}
				}
				else if(label=="CAMERA_ZNEAR")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_ZNEAR: %f\n", var);

						volumeviews[current_index].camera.znear = var;
					}
				}
				else if(label=="CAMERA_ZFAR")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_ZFAR: %f\n", var);

						volumeviews[current_index].camera.zfar = var;
					}
				}
				else if(label=="CAMERA_ZSCREEN")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_ZSCREEN: %f\n", var);

						volumeviews[current_index].camera.zscreen = var;
					}
				}
				else if(label=="CAMERA_RATIO")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_RATIO: %f\n", var);

						volumeviews[current_index].camera.ratio = var;
					}
				}
				else if(label=="CAMERA_WIDTH")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_WIDTH: %f\n", var);

						volumeviews[current_index].camera.width = var;
					}
				}
				else if(label=="CAMERA_HEIGHT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						//printf("CAMERA_HEIGHT: %f\n", var);

						volumeviews[current_index].camera.height = var;
					}
				}
				else if(label=="CAMERA_MOUSE_ACTIVE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_MOUSE_ACTIVE: %f\n", var);

						volumeviews[current_index].camera.mouse_active = var;
					}
				}
				else if(label=="CAMERA_CURSORID")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						//printf("CAMERA_CURSORID: %f\n", var);

						volumeviews[current_index].camera.cursor_id = var;
					}
				}
				else if(label=="CAMERA_MODE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						//printf("CAMERA_MODE: %f\n", var);

						volumeviews[current_index].camera.mode = var;
					}
				}
				else if(label=="CAMERA_MOUSECENTRE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						//printf("CAMERA_MOUSECENTRE: %f\n", var);
						volumeviews[current_index].camera.mouse_centre.x = var;
						token.nextToken();
						var = token.nval();
						//printf("CAMERA_MOUSECENTRE: %f\n", var);
						volumeviews[current_index].camera.mouse_centre.y = var;
						token.nextToken();
						var = token.nval();
						//printf("CAMERA_MOUSECENTRE: %f\n", var);
						volumeviews[current_index].camera.mouse_centre.z = var;
					}
				}
				else if(label=="CAMERA_MOUSECURRENT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						//printf("CAMERA_MOUSECURRENT: %f\n", var);
						volumeviews[current_index].camera.mouse_current.x = var;
						token.nextToken();
						var = token.nval();
						//printf("CAMERA_MOUSECURRENT: %f\n", var);
						volumeviews[current_index].camera.mouse_current.y = var;
						token.nextToken();
						var = token.nval();
						//printf("CAMERA_MOUSECURRENT: %f\n", var);
						volumeviews[current_index].camera.mouse_current.z = var;
					}
				}
				else if(label=="ARCBALL_ROTTYPE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_ROTTYPE: %f\n", var);

						volumeviews[current_index].arcball.rot_type = var;
					}
				}
				else if(label=="ARCBALL_ORIENTTYPE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_ORIENTTYPE: %f\n", var);

						volumeviews[current_index].arcball.orien_type = var;
					}
				}
				else if(label=="ARCBALL_DRAGGING")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_DRAGGING: %f\n", var);

						volumeviews[current_index].arcball.isDragging = var;
					}
				}
				else if(label=="ARCBALL_ZOOMING")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_ZOOMING: %f\n", var);

						volumeviews[current_index].arcball.isZooming = var;
					}
				}
				else if(label=="ARCBALL_TRANSLATING")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_TRANSLATING: %f\n", var);

						volumeviews[current_index].arcball.isTranslate = var;
					}
				}
				else if(label=="ARCBALL_OLDPOS")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_OLDPOS: %f\n", var);
						volumeviews[current_index].arcball.oldpos.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_OLDPOS: %f\n", var);
						volumeviews[current_index].arcball.oldpos.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_OLDPOS: %f\n", var);
						volumeviews[current_index].arcball.oldpos.z = var;
					}
				}
				else if(label=="ARCBALL_TRANSFORM_MATRIX")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<16; i++)
						{
							var = token.nval();
						//	printf("ARCBALL_TRANSFORM_MATRIX: %f\n", var);
							volumeviews[current_index].arcball.Transform.M[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="ARCBALL_LASTROT_MATRIX")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<9; i++)
						{
							var = token.nval();
					//		printf("ARCBALL_LASTROT_MATRIX: %f\n", var);
							volumeviews[current_index].arcball.LastRot.M[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="ARCBALL_THISROT_MATRIX")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<9; i++)
						{
							var = token.nval();
					//		printf("ARCBALL_THISROT_MATRIX: %f\n", var);
							volumeviews[current_index].arcball.ThisRot.M[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="ARCBALL_STVEC")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_STVEC: %f\n", var);
						volumeviews[current_index].arcball.StVec.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_STVEC: %f\n", var);
						volumeviews[current_index].arcball.StVec.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_STVEC: %f\n", var);
						volumeviews[current_index].arcball.StVec.z = var;
					}
				}
				else if(label=="ARCBALL_ENVEC")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						//printf("ARCBALL_ENVEC: %f\n", var);
						volumeviews[current_index].arcball.EnVec.x = var;
						token.nextToken();
						var = token.nval();
						//printf("ARCBALL_ENVEC: %f\n", var);
						volumeviews[current_index].arcball.EnVec.y = var;
						token.nextToken();
						var = token.nval();
						//printf("ARCBALL_ENVEC: %f\n", var);
						volumeviews[current_index].arcball.EnVec.z = var;
					}
				}
				else if(label=="ARCBALL_ADJWIDTH")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						//printf("ARCBALL_ADJWIDTH: %f\n", var);

						volumeviews[current_index].arcball.AdjustWidth = var;
					}
				}
				else if(label=="ARCBALL_ADJHEIGHT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						//printf("ARCBALL_ADJHEIGHT: %f\n", var);

						volumeviews[current_index].arcball.AdjustHeight = var;
					}
				}
				else if(label=="MODELVIEW")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<16; i++)
						{
							var = token.nval();
							//printf("MODELVIEW: %f\n", var);
							volumeviews[current_index].mvmatrix[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="PROJECTION")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<16; i++)
						{
							var = token.nval();
							//printf("PROJECTION: %f\n", var);
							volumeviews[current_index].projmatrix[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="VIEWPORT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<4; i++)
						{
							var = token.nval();
							//printf("VIEWPORT: %f\n", var);
							volumeviews[current_index].viewport[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="CUTTING_PLANES_SIZE")
				{
					current_plane_index = -1;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						var = token.nval();
						//printf("CUTTING_PLANES_SIZE: %f\n", var);
						volumeviews[current_index].cutting_planes.planes.resize((int) var);
					}
				}
				else if(label=="P")
				{
					current_plane_index++;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;

						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].centre.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].centre.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].centre.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].normal.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].normal.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].normal.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v1.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v1.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v1.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v2.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v2.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v2.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v3.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v3.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v3.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v4.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v4.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v4.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v1.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v1.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v1.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v2.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v2.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v2.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v3.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v3.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v3.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v4.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v4.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v4.z = var;
						token.nextToken();
						
						for(int i=0; i<4; i++)
						{
							var = token.nval();
							volumeviews[current_index].cutting_planes.planes[current_plane_index].plane_equation[i] = var;
							token.nextToken();
						}

						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].arcball.orien_type = var;
						token.nextToken();

						for(int i=0; i<16; i++)
						{
							var = token.nval();
							volumeviews[current_index].cutting_planes.planes[current_plane_index].arcball.Transform.M[i] = var;
							token.nextToken();
						}

						for(int i=0; i<9; i++)
						{
							var = token.nval();
							volumeviews[current_index].cutting_planes.planes[current_plane_index].arcball.LastRot.M[i] = var;
							token.nextToken();
						}

						for(int i=0; i<9; i++)
						{
							var = token.nval();
							volumeviews[current_index].cutting_planes.planes[current_plane_index].arcball.ThisRot.M[i] = var;
							token.nextToken();
						}

						volumeviews[current_index].cutting_planes.planes[current_plane_index].translate = 0.0;
					}
				}
				else if(label=="PLANE_RENDERMODE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						volumeviews[current_index].cutting_planes.eval_planeintersections();

						float var;
						for(int i=0; i<6; i++)
						{
							var = token.nval();
							//printf("PLANE_RENDERMODE: %f\n", var);
							volumeviews[current_index].plane_rendermode[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="TF_LUMINANCE")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].luminance.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].luminance.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].luminance.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].luminance.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].luminance.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].luminance.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].luminance.functioneditor.bs.control_points[i] = v;
					}
					volumeviews[current_index].luminance.functioneditor.bs.compute();
				}
				else if(label=="TF_ALPHA")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].alpha.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].alpha.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].alpha.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].alpha.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].alpha.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].alpha.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].alpha.functioneditor.bs.control_points[i] = v;
					}
					volumeviews[current_index].alpha.functioneditor.bs.compute();
				}
				else if(label=="TF_REDLUM")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redluminance.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redluminance.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redluminance.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redluminance.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redluminance.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].redluminance.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].redluminance.functioneditor.bs.control_points[i] = v;
					}
					volumeviews[current_index].redluminance.functioneditor.bs.compute();
				}
				else if(label=="TF_REDALPHA")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redalpha.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redalpha.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redalpha.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redalpha.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redalpha.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].redalpha.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].redalpha.functioneditor.bs.control_points[i] = v;
					}
					volumeviews[current_index].redalpha.functioneditor.bs.compute();
				}
				else if(label=="TF_GREENLUM")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenluminance.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenluminance.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenluminance.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenluminance.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenluminance.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].greenluminance.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].greenluminance.functioneditor.bs.control_points[i] = v;
					}
					volumeviews[current_index].greenluminance.functioneditor.bs.compute();
				}
				else if(label=="TF_GREENALPHA")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenalpha.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenalpha.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenalpha.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenalpha.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenalpha.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].greenalpha.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].greenalpha.functioneditor.bs.control_points[i] = v;
					}
					volumeviews[current_index].greenalpha.functioneditor.bs.compute();
				}				
				else if(label=="TF_BLUELUM")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].blueluminance.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].blueluminance.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].blueluminance.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].blueluminance.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].blueluminance.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].blueluminance.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].blueluminance.functioneditor.bs.control_points[i] = v;
					}
					volumeviews[current_index].blueluminance.functioneditor.bs.compute();
				}
				else if(label=="TF_BLUEALPHA")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].bluealpha.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].bluealpha.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].bluealpha.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].bluealpha.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].bluealpha.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].bluealpha.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].bluealpha.functioneditor.bs.control_points[i] = v;
					}

					volumeviews[current_index].bluealpha.functioneditor.bs.compute();
				}
				else
				{
					token.nextToken();
				}
			}
			else token.nextToken();
		}

		input_file.close();
	//}

	for(int i=0; i<volumeviews.size(); i++)
	{
		emit renderview_renderview_signal(volumeviews[i]);
	}
}
void QtRenderViewWidget::open_event(void)
{
	QString fn = QFileDialog::getOpenFileName(currentworkingdir, "*.vrv", this );
	
	float versumb = check_file_version(fn);
	versumb += 0;

	if(versumb==-1.0f)
	{
		printf("VRV_VERSION=0\n");
		load_version0(fn);
	}
	else if(versumb==1.0f)
	{
		printf("VRV_VERSION=1.0\n");
		load_version1(fn);
	}

//	load_file(fn);
}
float QtRenderViewWidget::check_file_version(QString fn)
{
	float versionnumb=-1;

	if(!fn.isEmpty())
	{
		string label;
		ifstream input_file; 
		input_file.open(fn.latin1());

		if(!input_file.is_open()) return -3;

		Tokenizer token(input_file);
		token.eolIsSignificant(true);

		while(!input_file.eof())
		{
			if(token.ttype()==TT_WORD)
			{
				//get the first string lable
				label = token.sval();
				
				if(label=="VRV_VERSION")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						versionnumb = token.nval();
						input_file.close();
						return versionnumb;
					}
				}
				else
				{
					token.nextToken();
				}
			}
			else token.nextToken();
		}

		input_file.close();
	}
	else return -3;

	return versionnumb;}
void QtRenderViewWidget::load_version0(QString fn)
{
	int current_index = -1;
	int current_plane_index = -1;

	volumeviews.clear();
	mylistwidget->clear();

	if(!fn.isEmpty())
	{
		ifstream input_file; 
		input_file.open(fn.latin1());

		if(!input_file.is_open()) return;

		Tokenizer token(input_file);
		token.eolIsSignificant(true);
		string label;

		int current_index = -1;
		int current_plane_index = -1;

		//until oef is reached step through each token
		while(!input_file.eof())
		{
			if(token.ttype()==TT_WORD)
			{
				//get the first string lable
				label = token.sval();
				
				if(label=="NUMBER_OF_VOLUMERENDERVIEW")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float size = token.nval();
						//printf("size: %f\n", size);

						volumeviews.resize(size);
					}
				}
				else if(label=="VOLUMERENDERVIEW")
				{
					current_index++;
					volumeviews[current_index].projection_toggle = ORTHO_PROJECTION;

					//printf("current_index: %d\n", current_index);
					token.nextToken();
				}
				else if(label=="WINWIDTH")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("WINWIDTH: %f\n", var);

						volumeviews[current_index].winWidth = var;
					}
				}
				else if(label=="WINHEIGT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("WINHEIGT: %f\n", var);

						volumeviews[current_index].winHeight = var;
					}
				}
				else if(label=="WINMAXDIM")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("WINMAXDIM: %f\n", var);

						volumeviews[current_index].winMaxDim = var;
					}
				}
				else if(label=="ZOOM")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("ZOOM: %f\n", var);

						volumeviews[current_index].zoom = var;
					}
				}
				else if(label=="TX")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						printf("TX: %f\n", var);

						volumeviews[current_index].eye.x = var;
					}
				}
				else if(label=="TY")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						printf("TY: %f\n", var);

						volumeviews[current_index].eye.y = var;
					}
				}
				else if(label=="TZ")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						printf("TZ: %f\n", var);

						volumeviews[current_index].eye.z = var;
					}
				}
				else if(label=="BB_VONE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						//printf("BB: %f\n", var);
						volumeviews[current_index].bb_v1.x = var;
						token.nextToken();
						var = token.nval();
						printf("BB: %f\n", var);
						volumeviews[current_index].bb_v1.y = var;
						token.nextToken();
						var = token.nval();
						printf("BB: %f\n", var);
						volumeviews[current_index].bb_v1.z = var;
					}
				}
				else if(label=="BB_VTWO")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v2.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v2.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v2.z = var;
					}
				}
				else if(label=="BB_VTHREE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v3.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v3.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v3.z = var;
					}
				}
				else if(label=="BB_VFOUR")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v4.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v4.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v4.z = var;
					}
				}
				else if(label=="BB_VFIVE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v5.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v5.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v5.z = var;
					}
				}
				else if(label=="BB_VSIX")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v6.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v6.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v6.z = var;
					}
				}
				else if(label=="BB_VSEVEN")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v7.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v7.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v7.z = var;
					}
				}
				else if(label=="BB_VEIGHT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v8.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v8.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v8.z = var;
					}
				}
				else if(label=="TEX_TRANSLATE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("TEX_TRANSLATE: %f\n", var);
						volumeviews[current_index].tex_translate.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("TEX_TRANSLATE: %f\n", var);
						volumeviews[current_index].tex_translate.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("TEX_TRANSLATE: %f\n", var);
						volumeviews[current_index].tex_translate.z = var;
					}
				}
				else if(label=="CAMERA_EYE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_EYE: %f\n", var);
						volumeviews[current_index].camera.eye.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("CAMERA_EYE: %f\n", var);
						volumeviews[current_index].camera.eye.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("CAMERA_EYE: %f\n", var);
						volumeviews[current_index].camera.eye.z = var;
					}
				}
				else if(label=="CAMERA_EYE_OLD")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_EYE_OLD: %f\n", var);
						volumeviews[current_index].camera.eyeOld.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("CAMERA_EYE_OLD: %f\n", var);
						volumeviews[current_index].camera.eyeOld.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("CAMERA_EYE_OLD: %f\n", var);
						volumeviews[current_index].camera.eyeOld.z = var;
					}
				}
				else if(label=="CAMERA_EYE_NEW")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_EYE_NEW: %f\n", var);
						volumeviews[current_index].camera.eyeNew.x = var;
						token.nextToken();
						var = token.nval();
			//			printf("CAMERA_EYE_NEW: %f\n", var);
						volumeviews[current_index].camera.eyeNew.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("CAMERA_EYE_NEW: %f\n", var);
						volumeviews[current_index].camera.eyeNew.z = var;
					}
				}
				else if(label=="CAMERA_AT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_AT: %f\n", var);
						volumeviews[current_index].camera.at.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_AT: %f\n", var);
						volumeviews[current_index].camera.at.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_AT: %f\n", var);
						volumeviews[current_index].camera.at.z = var;
					}
				}
				else if(label=="CAMERA_AT_OLD")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_AT_OLD: %f\n", var);
						volumeviews[current_index].camera.atOld.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_AT_OLD: %f\n", var);
						volumeviews[current_index].camera.atOld.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_AT_OLD: %f\n", var);
						volumeviews[current_index].camera.atOld.z = var;
					}
				}
				else if(label=="CAMERA_AT_NEW")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_AT_NEW: %f\n", var);
						volumeviews[current_index].camera.atNew.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_AT_NEW: %f\n", var);
						volumeviews[current_index].camera.atNew.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_AT_NEW: %f\n", var);
						volumeviews[current_index].camera.atNew.z = var;
					}
				}
				else if(label=="CAMERA_UP")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_UP: %f\n", var);
						volumeviews[current_index].camera.up.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("CAMERA_UP: %f\n", var);
						volumeviews[current_index].camera.up.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("CAMERA_UP: %f\n", var);
						volumeviews[current_index].camera.up.z = var;
					}
				}
				else if(label=="CAMERA_STEPS")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
			//			printf("CAMERA_STEPS: %f\n", var);

						volumeviews[current_index].camera.steps = var;
					}
				}
				else if(label=="CAMERA_MAXSTEPS")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
			//			printf("CAMERA_MAXSTEPS: %f\n", var);

						volumeviews[current_index].camera.maxSteps = var;
					}
				}
				else if(label=="CAMERA_SMOOTH")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_SMOOTH: %f\n", var);

						volumeviews[current_index].camera.smooth = var;
					}
				}
				else if(label=="CAMERA_FOV")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_FOV: %f\n", var);

						volumeviews[current_index].camera.fov = var;
					}
				}
				else if(label=="CAMERA_ZNEAR")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_ZNEAR: %f\n", var);

						volumeviews[current_index].camera.znear = var;
					}
				}
				else if(label=="CAMERA_ZFAR")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_ZFAR: %f\n", var);

						volumeviews[current_index].camera.zfar = var;
					}
				}
				else if(label=="CAMERA_ZSCREEN")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_ZSCREEN: %f\n", var);

						volumeviews[current_index].camera.zscreen = var;
					}
				}
				else if(label=="CAMERA_RATIO")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_RATIO: %f\n", var);

						volumeviews[current_index].camera.ratio = var;
					}
				}
				else if(label=="CAMERA_WIDTH")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_WIDTH: %f\n", var);

						volumeviews[current_index].camera.width = var;
					}
				}
				else if(label=="CAMERA_HEIGHT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_HEIGHT: %f\n", var);

						volumeviews[current_index].camera.height = var;
					}
				}
				else if(label=="CAMERA_MOUSE_ACTIVE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_MOUSE_ACTIVE: %f\n", var);

						volumeviews[current_index].camera.mouse_active = var;
					}
				}
				else if(label=="CAMERA_CURSORID")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_CURSORID: %f\n", var);

						volumeviews[current_index].camera.cursor_id = var;
					}
				}
				else if(label=="CAMERA_MODE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_MODE: %f\n", var);

						volumeviews[current_index].camera.mode = var;
					}
				}
				else if(label=="CAMERA_MOUSECENTRE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_MOUSECENTRE: %f\n", var);
						volumeviews[current_index].camera.mouse_centre.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_MOUSECENTRE: %f\n", var);
						volumeviews[current_index].camera.mouse_centre.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_MOUSECENTRE: %f\n", var);
						volumeviews[current_index].camera.mouse_centre.z = var;
					}
				}
				else if(label=="CAMERA_MOUSECURRENT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_MOUSECURRENT: %f\n", var);
						volumeviews[current_index].camera.mouse_current.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_MOUSECURRENT: %f\n", var);
						volumeviews[current_index].camera.mouse_current.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("CAMERA_MOUSECURRENT: %f\n", var);
						volumeviews[current_index].camera.mouse_current.z = var;
					}
				}
				else if(label=="ARCBALL_ROTTYPE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_ROTTYPE: %f\n", var);

						volumeviews[current_index].arcball.rot_type = var;
					}
				}
				else if(label=="ARCBALL_ORIENTTYPE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_ORIENTTYPE: %f\n", var);

						volumeviews[current_index].arcball.orien_type = var;
					}
				}
				else if(label=="ARCBALL_DRAGGING")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_DRAGGING: %f\n", var);

						volumeviews[current_index].arcball.isDragging = var;
					}
				}
				else if(label=="ARCBALL_ZOOMING")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_ZOOMING: %f\n", var);

						volumeviews[current_index].arcball.isZooming = var;
					}
				}
				else if(label=="ARCBALL_TRANSLATING")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_TRANSLATING: %f\n", var);

						volumeviews[current_index].arcball.isTranslate = var;
					}
				}
				else if(label=="ARCBALL_OLDPOS")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_OLDPOS: %f\n", var);
						volumeviews[current_index].arcball.oldpos.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_OLDPOS: %f\n", var);
						volumeviews[current_index].arcball.oldpos.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_OLDPOS: %f\n", var);
						volumeviews[current_index].arcball.oldpos.z = var;
					}
				}
				else if(label=="ARCBALL_TRANSFORM_MATRIX")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<16; i++)
						{
							var = token.nval();
					//		printf("ARCBALL_TRANSFORM_MATRIX: %f\n", var);
							volumeviews[current_index].arcball.Transform.M[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="ARCBALL_LASTROT_MATRIX")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<9; i++)
						{
							var = token.nval();
					//		printf("ARCBALL_LASTROT_MATRIX: %f\n", var);
							volumeviews[current_index].arcball.LastRot.M[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="ARCBALL_THISROT_MATRIX")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<9; i++)
						{
							var = token.nval();
					//		printf("ARCBALL_THISROT_MATRIX: %f\n", var);
							volumeviews[current_index].arcball.ThisRot.M[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="ARCBALL_STVEC")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_STVEC: %f\n", var);
						volumeviews[current_index].arcball.StVec.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_STVEC: %f\n", var);
						volumeviews[current_index].arcball.StVec.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_STVEC: %f\n", var);
						volumeviews[current_index].arcball.StVec.z = var;
					}
				}
				else if(label=="ARCBALL_ENVEC")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_ENVEC: %f\n", var);
						volumeviews[current_index].arcball.EnVec.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_ENVEC: %f\n", var);
						volumeviews[current_index].arcball.EnVec.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_ENVEC: %f\n", var);
						volumeviews[current_index].arcball.EnVec.z = var;
					}
				}
				else if(label=="ARCBALL_ADJWIDTH")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("ARCBALL_ADJWIDTH: %f\n", var);

						volumeviews[current_index].arcball.AdjustWidth = var;
					}
				}
				else if(label=="ARCBALL_ADJHEIGHT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_ADJHEIGHT: %f\n", var);

						volumeviews[current_index].arcball.AdjustHeight = var;
					}
				}
				else if(label=="MODELVIEW")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<16; i++)
						{
							var = token.nval();
					//		printf("MODELVIEW: %f\n", var);
							volumeviews[current_index].mvmatrix[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="PROJECTION")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<16; i++)
						{
							var = token.nval();
					//		printf("PROJECTION: %f\n", var);
							volumeviews[current_index].projmatrix[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="VIEWPORT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<4; i++)
						{
							var = token.nval();
					//		printf("VIEWPORT: %f\n", var);
							volumeviews[current_index].viewport[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="CUTTING_PLANES_SIZE")
				{
					current_plane_index = -1;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						var = token.nval();
					//	printf("CUTTING_PLANES_SIZE: %f\n", var);
						volumeviews[current_index].cutting_planes.planes.resize((int) var);
					}
				}
				else if(label=="P")
				{
					current_plane_index++;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;

						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].centre.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].centre.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].centre.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].normal.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].normal.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].normal.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v1.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v1.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v1.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v2.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v2.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v2.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v3.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v3.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v3.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v4.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v4.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v4.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v1.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v1.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v1.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v2.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v2.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v2.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v3.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v3.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v3.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v4.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v4.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v4.z = var;
						token.nextToken();
						
						for(int i=0; i<4; i++)
						{
							var = token.nval();
							volumeviews[current_index].cutting_planes.planes[current_plane_index].plane_equation[i] = var;
							token.nextToken();
						}

						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].arcball.orien_type = var;
						token.nextToken();

						for(int i=0; i<16; i++)
						{
							var = token.nval();
							volumeviews[current_index].cutting_planes.planes[current_plane_index].arcball.Transform.M[i] = var;
							token.nextToken();
						}

						for(int i=0; i<9; i++)
						{
							var = token.nval();
							volumeviews[current_index].cutting_planes.planes[current_plane_index].arcball.LastRot.M[i] = var;
							token.nextToken();
						}

						for(int i=0; i<9; i++)
						{
							var = token.nval();
							volumeviews[current_index].cutting_planes.planes[current_plane_index].arcball.ThisRot.M[i] = var;
							token.nextToken();
						}

						volumeviews[current_index].cutting_planes.planes[current_plane_index].translate = 0.0;
					}
				}
				else if(label=="PLANE_RENDERMODE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						volumeviews[current_index].cutting_planes.eval_planeintersections();

						float var;
						for(int i=0; i<6; i++)
						{
							var = token.nval();
					//		printf("PLANE_RENDERMODE: %f\n", var);
							volumeviews[current_index].plane_rendermode[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="TF_LUMINANCE")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].luminance.brightness = (int) var;
					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].luminance.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].luminance.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].luminance.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].luminance.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].luminance.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].luminance.functioneditor.bs.control_points[i] = v;
					}
					volumeviews[current_index].luminance.functioneditor.bs.compute();
				}
				else if(label=="TF_ALPHA")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].alpha.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].alpha.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].alpha.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].alpha.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].alpha.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].alpha.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].alpha.functioneditor.bs.control_points[i] = v;
					}
					volumeviews[current_index].alpha.functioneditor.bs.compute();
				}
				else if(label=="TF_REDLUM")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redluminance.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redluminance.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redluminance.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redluminance.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redluminance.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].redluminance.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].redluminance.functioneditor.bs.control_points[i] = v;
					}
					volumeviews[current_index].redluminance.functioneditor.bs.compute();
				}
				else if(label=="TF_REDALPHA")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redalpha.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redalpha.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redalpha.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redalpha.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redalpha.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].redalpha.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].redalpha.functioneditor.bs.control_points[i] = v;
					}
					volumeviews[current_index].redalpha.functioneditor.bs.compute();
				}
				else if(label=="TF_GREENLUM")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenluminance.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenluminance.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenluminance.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenluminance.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenluminance.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].greenluminance.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].greenluminance.functioneditor.bs.control_points[i] = v;
					}
					volumeviews[current_index].greenluminance.functioneditor.bs.compute();
				}
				else if(label=="TF_GREENALPHA")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenalpha.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenalpha.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenalpha.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenalpha.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenalpha.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].greenalpha.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].greenalpha.functioneditor.bs.control_points[i] = v;
					}
					volumeviews[current_index].greenalpha.functioneditor.bs.compute();
				}				
				else if(label=="TF_BLUELUM")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].blueluminance.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].blueluminance.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].blueluminance.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].blueluminance.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].blueluminance.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].blueluminance.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].blueluminance.functioneditor.bs.control_points[i] = v;
					}
					volumeviews[current_index].blueluminance.functioneditor.bs.compute();
				}
				else if(label=="TF_BLUEALPHA")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].bluealpha.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].bluealpha.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].bluealpha.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].bluealpha.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].bluealpha.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].bluealpha.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].bluealpha.functioneditor.bs.control_points[i] = v;
					}

					volumeviews[current_index].bluealpha.functioneditor.bs.compute();
				}
				else
				{
					token.nextToken();
				}
			}
			else token.nextToken();
		}

		input_file.close();

	}

	for(int i=0; i<volumeviews.size(); i++)
	{
		emit renderview_renderview_signal(volumeviews[i]);
	}
}
void QtRenderViewWidget::load_version1(QString fn)
{
	if(!fn.isEmpty())
	{
		ifstream input_file; 
		input_file.open(fn.latin1());

		if(!input_file.is_open()) return;

		Tokenizer token(input_file);
		token.eolIsSignificant(true);
		string label;

		int current_index = -1;
		int current_plane_index = -1;

		//until oef is reached step through each token
		while(!input_file.eof())
		{
			if(token.ttype()==TT_WORD)
			{
				//get the first string lable
				label = token.sval();
				
				if(label=="NUMBER_OF_VOLUMERENDERVIEW")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float size = token.nval();
						//printf("size: %f\n", size);

						volumeviews.resize(size);
					}
				}
				else if(label=="VOLUMERENDERVIEW")
				{
					current_index++;
					volumeviews[current_index].projection_toggle = ORTHO_PROJECTION;

					//printf("current_index: %d\n", current_index);
					token.nextToken();
				}
				else if(label=="WINWIDTH")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("WINWIDTH: %f\n", var);

						volumeviews[current_index].winWidth = var;
					}
				}
				else if(label=="WINHEIGT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("WINHEIGT: %f\n", var);

						volumeviews[current_index].winHeight = var;
					}
				}
				else if(label=="ZOOM")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("ZOOM: %f\n", var);

						volumeviews[current_index].zoom = var;
					}
				}
				else if(label=="BB_VONE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						printf("BB: %f\n", var);
						volumeviews[current_index].bb_v1.x = var;
						token.nextToken();
						var = token.nval();
						printf("BB: %f\n", var);
						volumeviews[current_index].bb_v1.y = var;
						token.nextToken();
						var = token.nval();
						printf("BB: %f\n", var);
						volumeviews[current_index].bb_v1.z = var;
					}
				}
				else if(label=="BB_VTWO")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v2.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v2.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v2.z = var;
					}
				}
				else if(label=="BB_VTHREE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v3.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v3.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v3.z = var;
					}
				}
				else if(label=="BB_VFOUR")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v4.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v4.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v4.z = var;
					}
				}
				else if(label=="BB_VFIVE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v5.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v5.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v5.z = var;
					}
				}
				else if(label=="BB_VSIX")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v6.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v6.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v6.z = var;
					}
				}
				else if(label=="BB_VSEVEN")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v7.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v7.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v7.z = var;
					}
				}
				else if(label=="BB_VEIGHT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v8.x = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v8.y = var;
						token.nextToken();
						var = token.nval();
				//		printf("BB: %f\n", var);
						volumeviews[current_index].bb_v8.z = var;
					}
				}
				else if(label=="CAMERA_PROJECTION_MODE")
				{
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						int var = token.nval();
						volumeviews[current_index].projection_toggle = var;
					}

				}
				else if(label=="CAMERA_EYE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						volumeviews[current_index].eye.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].eye.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].eye.z = var;
					}
				}
				else if(label=="CAMERA_UP")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						volumeviews[current_index].up.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].up.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].up.z = var;
					}
				}
				else if(label=="CAMERA_AT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
						volumeviews[current_index].at.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].at.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].at.z = var;
					}
				}
				else if(label=="CAMERA_FOV")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_FOV: %f\n", var);

						volumeviews[current_index].camera.fov = var;
					}
				}
				else if(label=="CAMERA_ZNEAR")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_ZNEAR: %f\n", var);

						volumeviews[current_index].camera.znear = var;
					}
				}
				else if(label=="CAMERA_ZFAR")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_ZFAR: %f\n", var);

						volumeviews[current_index].camera.zfar = var;
					}
				}
				else if(label=="CAMERA_ZSCREEN")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_ZSCREEN: %f\n", var);

						volumeviews[current_index].camera.zscreen = var;
					}
				}
				else if(label=="CAMERA_RATIO")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("CAMERA_RATIO: %f\n", var);

						volumeviews[current_index].camera.ratio = var;
					}
				}
				else if(label=="CAMERA_WIDTH")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_WIDTH: %f\n", var);

						volumeviews[current_index].camera.width = var;
					}
				}
				else if(label=="CAMERA_HEIGHT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("CAMERA_HEIGHT: %f\n", var);

						volumeviews[current_index].camera.height = var;
					}
				}
				else if(label=="ARCBALL_ROTTYPE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_ROTTYPE: %f\n", var);

						volumeviews[current_index].arcball.rot_type = var;
					}
				}
				else if(label=="ARCBALL_ORIENTTYPE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_ORIENTTYPE: %f\n", var);

						volumeviews[current_index].arcball.orien_type = var;
					}
				}
				else if(label=="ARCBALL_DRAGGING")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_DRAGGING: %f\n", var);

						volumeviews[current_index].arcball.isDragging = var;
					}
				}
				else if(label=="ARCBALL_ZOOMING")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_ZOOMING: %f\n", var);

						volumeviews[current_index].arcball.isZooming = var;
					}
				}
				else if(label=="ARCBALL_TRANSLATING")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_TRANSLATING: %f\n", var);

						volumeviews[current_index].arcball.isTranslate = var;
					}
				}
				else if(label=="ARCBALL_OLDPOS")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_OLDPOS: %f\n", var);
						volumeviews[current_index].arcball.oldpos.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_OLDPOS: %f\n", var);
						volumeviews[current_index].arcball.oldpos.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_OLDPOS: %f\n", var);
						volumeviews[current_index].arcball.oldpos.z = var;
					}
				}
				else if(label=="ARCBALL_TRANSFORM_MATRIX")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<16; i++)
						{
							var = token.nval();
					//		printf("ARCBALL_TRANSFORM_MATRIX: %f\n", var);
							volumeviews[current_index].arcball.Transform.M[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="ARCBALL_LASTROT_MATRIX")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<9; i++)
						{
							var = token.nval();
					//		printf("ARCBALL_LASTROT_MATRIX: %f\n", var);
							volumeviews[current_index].arcball.LastRot.M[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="ARCBALL_THISROT_MATRIX")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<9; i++)
						{
							var = token.nval();
					//		printf("ARCBALL_THISROT_MATRIX: %f\n", var);
							volumeviews[current_index].arcball.ThisRot.M[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="ARCBALL_STVEC")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_STVEC: %f\n", var);
						volumeviews[current_index].arcball.StVec.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_STVEC: %f\n", var);
						volumeviews[current_index].arcball.StVec.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_STVEC: %f\n", var);
						volumeviews[current_index].arcball.StVec.z = var;
					}
				}
				else if(label=="ARCBALL_ENVEC")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_ENVEC: %f\n", var);
						volumeviews[current_index].arcball.EnVec.x = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_ENVEC: %f\n", var);
						volumeviews[current_index].arcball.EnVec.y = var;
						token.nextToken();
						var = token.nval();
					//	printf("ARCBALL_ENVEC: %f\n", var);
						volumeviews[current_index].arcball.EnVec.z = var;
					}
				}
				else if(label=="ARCBALL_ADJWIDTH")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
				//		printf("ARCBALL_ADJWIDTH: %f\n", var);

						volumeviews[current_index].arcball.AdjustWidth = var;
					}
				}
				else if(label=="ARCBALL_ADJHEIGHT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var = token.nval();
					//	printf("ARCBALL_ADJHEIGHT: %f\n", var);

						volumeviews[current_index].arcball.AdjustHeight = var;
					}
				}
				else if(label=="MODELVIEW")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<16; i++)
						{
							var = token.nval();
					//		printf("MODELVIEW: %f\n", var);
							volumeviews[current_index].mvmatrix[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="PROJECTION")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<16; i++)
						{
							var = token.nval();
					//		printf("PROJECTION: %f\n", var);
							volumeviews[current_index].projmatrix[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="VIEWPORT")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						for(int i=0; i<4; i++)
						{
							var = token.nval();
					//		printf("VIEWPORT: %f\n", var);
							volumeviews[current_index].viewport[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="CUTTING_PLANES_SIZE")
				{
					current_plane_index = -1;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;
						var = token.nval();
					//	printf("CUTTING_PLANES_SIZE: %f\n", var);
						volumeviews[current_index].cutting_planes.planes.resize((int) var);
					}
				}
				else if(label=="P")
				{
					current_plane_index++;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						float var;

						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].centre.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].centre.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].centre.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].normal.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].normal.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].normal.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v1.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v1.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v1.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v2.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v2.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v2.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v3.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v3.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v3.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v4.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v4.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index]._v4.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v1.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v1.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v1.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v2.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v2.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v2.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v3.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v3.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v3.z = var;
						token.nextToken();
						//=====================================================================================
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v4.x = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v4.y = var;
						token.nextToken();
						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].v4.z = var;
						token.nextToken();
						
						for(int i=0; i<4; i++)
						{
							var = token.nval();
							volumeviews[current_index].cutting_planes.planes[current_plane_index].plane_equation[i] = var;
							token.nextToken();
						}

						var = token.nval();
						volumeviews[current_index].cutting_planes.planes[current_plane_index].arcball.orien_type = var;
						token.nextToken();

						for(int i=0; i<16; i++)
						{
							var = token.nval();
							volumeviews[current_index].cutting_planes.planes[current_plane_index].arcball.Transform.M[i] = var;
							token.nextToken();
						}

						for(int i=0; i<9; i++)
						{
							var = token.nval();
							volumeviews[current_index].cutting_planes.planes[current_plane_index].arcball.LastRot.M[i] = var;
							token.nextToken();
						}

						for(int i=0; i<9; i++)
						{
							var = token.nval();
							volumeviews[current_index].cutting_planes.planes[current_plane_index].arcball.ThisRot.M[i] = var;
							token.nextToken();
						}

						volumeviews[current_index].cutting_planes.planes[current_plane_index].translate = 0.0;
					}
				}
				else if(label=="PLANE_RENDERMODE")
				{
					//skip the equal sign
					token.nextToken();
					token.nextToken();
					if(token.ttype()==TT_NUMBER)
					{
						volumeviews[current_index].cutting_planes.eval_planeintersections();

						float var;
						for(int i=0; i<6; i++)
						{
							var = token.nval();
					//		printf("PLANE_RENDERMODE: %f\n", var);
							volumeviews[current_index].plane_rendermode[i] = var;
							token.nextToken();
						}
					}
				}
				else if(label=="TF_LUMINANCE")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].luminance.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].luminance.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].luminance.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].luminance.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].luminance.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].luminance.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].luminance.functioneditor.bs.control_points[i] = v;
					}
					volumeviews[current_index].luminance.functioneditor.bs.compute();
				}
				else if(label=="TF_ALPHA")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].alpha.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].alpha.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].alpha.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].alpha.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].alpha.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].alpha.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].alpha.functioneditor.bs.control_points[i] = v;
					}
					volumeviews[current_index].alpha.functioneditor.bs.compute();
				}
				else if(label=="TF_REDLUM")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redluminance.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redluminance.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redluminance.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redluminance.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redluminance.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].redluminance.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].redluminance.functioneditor.bs.control_points[i] = v;
					}
					volumeviews[current_index].redluminance.functioneditor.bs.compute();
				}
				else if(label=="TF_REDALPHA")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redalpha.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redalpha.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redalpha.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redalpha.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].redalpha.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].redalpha.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].redalpha.functioneditor.bs.control_points[i] = v;
					}
					volumeviews[current_index].redalpha.functioneditor.bs.compute();
				}
				else if(label=="TF_GREENLUM")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenluminance.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenluminance.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenluminance.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenluminance.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenluminance.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].greenluminance.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].greenluminance.functioneditor.bs.control_points[i] = v;
					}
					volumeviews[current_index].greenluminance.functioneditor.bs.compute();
				}
				else if(label=="TF_GREENALPHA")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenalpha.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenalpha.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenalpha.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenalpha.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].greenalpha.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].greenalpha.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].greenalpha.functioneditor.bs.control_points[i] = v;
					}
					volumeviews[current_index].greenalpha.functioneditor.bs.compute();
				}				
				else if(label=="TF_BLUELUM")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].blueluminance.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].blueluminance.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].blueluminance.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].blueluminance.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].blueluminance.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].blueluminance.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].blueluminance.functioneditor.bs.control_points[i] = v;
					}
					volumeviews[current_index].blueluminance.functioneditor.bs.compute();
				}
				else if(label=="TF_BLUEALPHA")
				{
					float var;

					//skip the equal sign
					token.nextToken();
					token.nextToken();
					
					//brightness
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].bluealpha.brightness = (int) var;

					//contrast
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].bluealpha.contrast = (int) var;

					//threshmin
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].bluealpha.threshold_min = (int) var;

					//threshmax
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].bluealpha.threshold_max = (int) var;

					//number of control points
					var = token.nval();
					token.nextToken();
					volumeviews[current_index].bluealpha.functioneditor.bs.control_points.resize((int)var);

					for(int i=0; i<volumeviews[current_index].bluealpha.functioneditor.bs.control_points.size(); i++)
					{
						Vector v;
						v.x = token.nval();
						token.nextToken();
						v.y = token.nval();
						token.nextToken();
						v.z = token.nval();
						token.nextToken();
						volumeviews[current_index].bluealpha.functioneditor.bs.control_points[i] = v;
					}

					volumeviews[current_index].bluealpha.functioneditor.bs.compute();
				}
				else
				{
					token.nextToken();
				}
			}
			else token.nextToken();
		}

		input_file.close();
	}

	for(int i=0; i<volumeviews.size(); i++)
	{
		emit renderview_renderview_signal(volumeviews[i]);
	}
}
void QtRenderViewWidget::renderview_addsavedview_slot(QPixmap* pixmap)
{
	QIcon viewIcon(*pixmap);

	int size = mylistwidget->count();

	QString numb;
	numb.setNum(size);

	QListWidgetItem *item = new QListWidgetItem(viewIcon,"",mylistwidget,0);
	mylistwidget->insertItem(0, item); 
}
void QtRenderViewWidget::renderview_setcurrentworkingdir_slot(QString dn)
{
	currentworkingdir = dn;
}
void QtRenderViewWidget::exit(void)
{
}
void QtRenderViewWidget::dragndrop_slot(QString sFileName)
{
	float versumb = check_file_version(sFileName);
	versumb += 0;

	if(versumb==-1.0f)
	{
		printf("VRV_VERSION=0\n");
		load_version0(sFileName);
	}
	else if(versumb==1.0f)
	{
		printf("VRV_VERSION=1.0\n");
		load_version1(sFileName);
	}

	//load_file(sFileName);

	/*for(int i=0; i<volumeviews.size(); i++)
	{
		emit renderview_renderview_signal(volumeviews[i]);
	}*/
}
void QtRenderViewWidget::dragEnterEvent(QDragEnterEvent* event)
{
}
void QtRenderViewWidget::dropEvent(QDropEvent* event)
{
	Q3StrList URIs;

	if ( Q3UriDrag::decode(event, URIs) ) 
	{
		QString text = Q3UriDrag::uriToLocalFile(URIs.getFirst());
		dragndrop_slot(text);
	}
}
void QtRenderViewWidget::saveview_event(void)
{
	//printf("boh\n");
	emit renderview_saveview_signal();
}
void QtRenderViewWidget::updateview_event(void)
{
	int selected;

	QListWidgetItem* curr = mylistwidget->currentItem();
	selected = mylistwidget->row(curr);

	if(selected==-1) return;

	emit renderview_updateview_signal(selected);
	//printf("UPDATE: %d\n", selected);
}
void QtRenderViewWidget::renderview_updateview_slot(int sel, QPixmap* pixmap, VolumeRenderView view)
{
	QIcon viewIcon(*pixmap);
	QListWidgetItem* curr = mylistwidget->item(sel);
	curr->setIcon(viewIcon);

	/*QmylistwidgetItem *item = new QmylistwidgetItem(viewIcon,"Image",mylistwidget,0);
	mylistwidget->insertItem(sel, item); */

	volumeviews[sel] = view;
}
void QtRenderViewWidget::renderview_addview_slot(QPixmap* pixmap, VolumeRenderView view)
{
	for(int i=0; i<volumeviews.size(); i++)
	{
		if(volumeviews[i].projection_toggle!=view.projection_toggle)
		{
			QMessageBox* mb = new QMessageBox( "WARNING",
			"You cannot mix visual bookmarks that use different camera modes!\nPlease change the current camera mode.",QMessageBox::Warning,
			QMessageBox::Ok | QMessageBox::Default,0,0 );
			mb->exec();

			return;
		}
	}
	//printf("SAVE RENDER VIEW SLOT\n");

	QIcon viewIcon(*pixmap);

	int size = mylistwidget->count();

	QString numb;
	numb.setNum(size);

	QListWidgetItem *item = new QListWidgetItem(viewIcon,"",mylistwidget,0);
	mylistwidget->insertItem(0, item); 

	volumeviews.push_back(view);
}
void QtRenderViewWidget::deleteview_event(void)
{
	QList<QListWidgetItem*> items = mylistwidget->selectedItems();

	vector< int > todelete;
	for(int i=0; i<items.size(); i++)
	{
		todelete.push_back( mylistwidget->row(items[i]) );
		QListWidgetItem* item = mylistwidget->takeItem(mylistwidget->row(items[i]));
		mylistwidget->removeItemWidget(item);
		delete item;
	}

	for(int i=todelete.size()-1; i>=0; i--)
	{
		//printf("delete: %d\n", todelete[i]);
		volumeviews.erase(volumeviews.begin()+todelete[i]);
	}
}
void QtRenderViewWidget::renderview_selectItem(int index)
{
	if(index<volumeviews.size())
	{
		emit renderview_setview_signal(volumeviews[index]);
	}
	else
	{
		printf("ERROR: requested Visual Bookmark is out of range\n");
	}
}
void QtRenderViewWidget::itemDoublClicked_event(QListWidgetItem* item)
{
	int row = mylistwidget->row(item);
	emit renderview_setview_signal(volumeviews[row]);
}
void QtRenderViewWidget::interpolate_event(void)
{
	emit renderview_interpolate_signal(volumeviews);
}
void QtRenderViewWidget::renderview_getallviews_slot(void)
{
	emit renderview_interpolate_signal(volumeviews);
}
