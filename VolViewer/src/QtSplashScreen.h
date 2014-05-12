/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QTSPLASHSCREEN_H
#define QTSPLASHSCREEN_H

#include <qpixmap.h>
#include <qwidget.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QDesktopWidget>

#include "MyDefines.h"
#include "SVNObject.h"

class QtSplashScreen : public QWidget
{
		Q_OBJECT
public:
	SVNObject svn;
	QtSplashScreen( const QPixmap &pixmap, int window_main_screen);

	void setWinPosition(int window_main_screen);
	void setStatus( const QString &message, int alignment = Qt::AlignLeft, const QColor &color = Qt::black );
	void repaint();
public slots:
    void finish(void);
protected:
    void mousePressEvent( QMouseEvent * );

private:
    QPixmap pix;
};

#endif //QTSPLASHSCREEN_H
