/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QTCROP_H
#define QTCROP_H

#include <qpainter.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qslider.h>
#include <qstring.h>
#include <qpushbutton.h> 
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qspinbox.h>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <qdir.h>
#include <QFileDialog>
#include <QDialog>
#include <QLineEdit>

#include <q3boxlayout.h>
#include <q3buttongroup.h>
#include <q3dragobject.h>

#include "QtGLObjWin.h"

class QtCropWidget: public QWidget
{
	Q_OBJECT
		
public:

	QtCropWidget(QWidget* parent = 0, const char* name = 0, QString app_path="");
	~QtCropWidget();

	QString appPath;
	QString currentworkingdir;
	
	vector<bool> channels;
	
	QPushButton* red;
	QPushButton* green;
	QPushButton* blue;
	
	QPushButton* load_pb;
	QPushButton* save_pb;
	QPushButton* reset_pb;
	
	QPushButton* lloopmode_button;

	QPushButton* invcrop;
	QPushButton* recentre;

	void create(void);
	void exit(void);
	void reset(void);
	void rebuild(void);

private slots:

	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
	void reset_button_event(void);
	void channel_button_event(int selected);
	void load_event(void);
	void save_event(void);

	void cropmode_event(void);
	void invcrop_button_event(void);
	void recentre_button_event(void);

public slots:

	void dragndrop_slot(QString sFileName);

signals:
	void crop_start_signal(void);
	void crop_end_signal(vector<bool> channels);
	void crop_invsel_signal(int mode);
	void crop_recentre_signal(void);
	void crop_reset_signal(void);
};

#endif // QTCrop_H
