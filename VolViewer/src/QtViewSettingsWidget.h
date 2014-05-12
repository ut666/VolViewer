/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QTVIEWSETTINGSWIDGET_H
#define QTVIEWSETTINGSWIDGET_H

#include <qpainter.h>
#include <qpixmap.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qslider.h>
#include <qstring.h>
#include <q3buttongroup.h>
#include <qcheckbox.h>
#include <qimage.h>
#include <qpushbutton.h> 
#include <qcolordialog.h>
#include <qcombobox.h>
#include <q3grid.h>
#include <q3boxlayout.h>

#include "Vector.h"
#include "ColourFun.h"

class QtViewSettingsWidget: public QWidget
{
	Q_OBJECT
		
public:

	QtViewSettingsWidget(QWidget* parent, const char* name);
	~QtViewSettingsWidget();

	//bounding box label & colour
	QLabel* bbcol_lb;
	QColor bbcolour;

	//background label & colour
	QLabel* bgcol_lb;
	QColor bgcolour;

	void create(void);
	void reset(void);
	void rebuild(void);
	void apply(void);

private slots:
	
	void bb_colourdlg_event(void);
	void bg_colourdlg_event(void);

signals:

};

#endif // QTVIEWSETTINGSWIDGET_H
