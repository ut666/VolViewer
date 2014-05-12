/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
/***********************************************************************
* Project:      VolViewer
* Title:        Main.cpp
* Author:       Jerome Avondo
************************************************************************/

#include <QPixmap>
#include <qapplication.h>
#include <qgl.h>
#include <qstring.h>
#include <QtPlugin>
#include <QMessageBox>

#include "QtMainWindow.h"
#include "QtSplashScreen.h"

#include "Timer.h"

static QString abbsapppath;
static int window_consolle=0;
static int window_main_screen=0;
static int window_log_file=0;

void load_inifile_viewparams(void)
{
	string label;

	//input file for reading in parameter ranges
	ifstream input_file; 
	input_file.open(abbsapppath+"/settings.ini");

	if(!input_file.is_open())
	{
		//qWarning( "Error loading INI file!\n" );
		QMessageBox msgBox(QMessageBox::Critical, "ERROR", "The settings.ini file could not be found in the application folder!", QMessageBox::Ok, 0, 0);
		msgBox.exec();	
		exit(1);
	}

	//tokenize our input file
	Tokenizer token(input_file);

	//until oef is reached step through each token
	while(!input_file.eof()){
		if(token.ttype()==TT_WORD)
		{
			//get the first string lable
			label = token.sval();
									
			if(label=="window_main_screen")
			{		
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) window_main_screen = token.nval();
			}	
			else if(label=="window_consolle")
			{		
				//skip the equal sign
				token.nextToken(); 
				token.nextToken();
				if(token.ttype()==TT_NUMBER) window_consolle = token.nval();
			}					
      else if(label=="window_log_file")
      {		
        //skip the equal sign
        token.nextToken(); 
        token.nextToken();
        if(token.ttype()==TT_NUMBER) window_log_file = token.nval();
        //return;
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
}
void check_for_files()
{
	QDir dir(abbsapppath+"/Shaders");
	if (!dir.exists())
	{
		QMessageBox msgBox(QMessageBox::Critical, "ERROR", "The 'Shaders\' folder could not be found in the application folder!", QMessageBox::Ok, 0, 0);
		msgBox.exec();	
		exit(1);		
	}	
	QDir dir2(abbsapppath+"/Icons");
	if (!dir2.exists())
	{
		QMessageBox msgBox(QMessageBox::Critical, "ERROR", "The 'Icons\' folder could not be found in the application folder!", QMessageBox::Ok, 0, 0);
		msgBox.exec();	
		exit(1);		
	}	
}
void create_sample_data(void)
{
	VolumeObject volobject;
	volobject.create_volume();

	VolIO volio;
	volio.volobj = &volobject;

	#ifdef WIN32
	if(AllocConsole())
	{
		freopen("CON", "w", stdout);
		freopen("CON", "w", stderr);
	}
	#endif

	volio.showGUI=false;
	volio.save_PNG("E:/-=VOLUME DATA=-/-=TEST GALLERY=-/Synthetic/Cube/");
	system("pause");
}
int main( int argc, char **argv )
{
	//create a Qt application
    QApplication::setColorSpec( QApplication::CustomColor );
	//QApplication::addLibraryPath("plugins/");

    QApplication a(argc,argv,true);			
	
	//create_sample_data();
	//return 0;

	abbsapppath = "";
	abbsapppath = a.applicationDirPath();

	//check if the system supports OpenGL
    if ( !QGLFormat::hasOpenGL() ) 
	{
		qWarning( "This system has no OpenGL support. Exiting." );
		return -1;
    }

	load_inifile_viewparams();
	check_for_files();

  //create a consolle (debug)
  if(window_consolle==1 || window_log_file==1)
  {	
#ifdef WIN32
    if(AllocConsole())
    {
      freopen("CON", "w", stdout);
      freopen("CON", "w", stderr);
    }
#endif
  }

  if(window_log_file==1)
  {
    //create our file if it doesn't exist
    QFile file( abbsapppath+"/log.txt" );
    //stream all stdout and stderr verbose to it
    freopen(abbsapppath+"/log.txt", "w", stdout);
    freopen(abbsapppath+"/log.txt", "w", stderr);
  }


#ifdef BIOPTONICS_SUPPORT
	//create a splash screen
    QPixmap pixmap( "icons/splash.png" );
    QtSplashScreen *splash = new QtSplashScreen( pixmap, window_main_screen);
#endif

	//create a QtMainWindow for our application
    QtMainWindow *mw = new QtMainWindow(abbsapppath);
	a.setMainWidget( mw );

	mw->load_inifile();
	mw->load_commandlineparams(argc, argv);

#ifdef BIOPTONICS_SUPPORT
	//set the mainwindow to visible and launch the app
	QTimer::singleShot( 1000, splash, SLOT(finish()) );
	QTimer::singleShot( 1000, mw, SLOT(show()) );
#endif

	return 	a.exec();
}
