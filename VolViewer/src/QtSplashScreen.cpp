/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#include <qapplication.h>
#include <qpainter.h>
#include <qpixmap.h>
#include "QtSplashScreen.h"
//Added by qt3to4:
#include <QMouseEvent>

QtSplashScreen::QtSplashScreen( const QPixmap &pixmap, int window_main_screen)
: QWidget( 0, 0, 0 ),
pix( pixmap )
{
	setBackgroundPixmap ( pix );
	setCaption("VolViewer v3.0 rev"+ svn.revision);
	
#ifdef BIOPTONICS_SUPPORT
	setCaption( "BioptonicsViewer v3.0 rev"+ svn.revision);
#endif
	
	resize( pix.size() );
	
	setWinPosition(window_main_screen);
	raise();
	
	//QRect scr = QApplication::desktop()->geometry ();
	//move( scr.center() - rect().center() );
}

void QtSplashScreen::repaint()
{
	QWidget::repaint();
}
void QtSplashScreen::finish(void)
{
	close();
	delete this;
}
void QtSplashScreen::setWinPosition(int window_main_screen)
{
	int main_width = width();
	int main_height = height();
	int screen0_w, screen0_h, screen1_w, screen1_h;

	//printf("window_main_screen: %d\n", window_main_screen);
	
	if((int)QApplication::desktop()->numScreens()>1)
	{
		//printf("mutli-screen \n");
		
		screen0_w = QApplication::desktop()->screenGeometry(0).width();
		screen0_h = QApplication::desktop()->screenGeometry(0).height();
		screen1_w = QApplication::desktop()->screenGeometry(1).width();
		screen1_h = QApplication::desktop()->screenGeometry(1).height();

//		printf("screen0: %d x %d\n", screen0_w, screen0_h);
//		printf("screen1: %d x %d\n", screen1_w, screen1_h);
		
		if(window_main_screen==0)
		{		
//			printf("pos screen0: %d x %d\n", (int)((screen0_w/2.0) - (main_width/2.0)),  (int)((screen0_h/2.0) - (main_height/2.0)));
			move(screen0_w/2.0 - main_width/2.0,  screen0_h/2.0 - main_height/2.0);
		}
		else
		{
//			printf("pos screen1: %d x %d\n", (int) ((screen0_w + (screen1_w/2.0)) - (main_width/2.0)),  (int)((screen1_h/2.0) - (main_height/2.0)));
			move((screen0_w + (screen1_w/2.0)) - (main_width/2.0),  (screen1_h/2.0) - (main_height/2.0));		
		}
	}
	else
	{
//		printf("single-screen \n");

		screen0_w = QApplication::desktop()->width();
		screen0_h = QApplication::desktop()->height(); 
		move(screen0_w/2.0 - main_width/2.0,  screen0_h/2.0 - main_height/2.0);		
	}		

	show();
	repaint();
}
void QtSplashScreen::setStatus( const QString &message, int alignment, const QColor &color )
{
	QPixmap textPix = pix;
	/*QPainter painter( &textPix, this );
	painter.setPen( color );
	QRect r = rect();
	r.setRect( r.x() + 10, r.y() + 10, r.width() - 20, r.height() - 20 );
	painter.drawText( r, alignment, message );*/
//	setErasePixmap( textPix );
	repaint();
}
void QtSplashScreen::mousePressEvent( QMouseEvent * )
{
	hide();
}
