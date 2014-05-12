/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QTDATASETVIEW_H
#define QTDATASETVIEW_H

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
#include <QTableWidget>
#include <QProgressDialog>
#include <QHeaderView>

#include <q3boxlayout.h>
#include <q3buttongroup.h>
#include <q3dragobject.h>

#include <vector>
using namespace std;

#include "DatasetView.h"
#include "QtMoviePlayer.h"

class QtDatasetViewWidget: public QWidget
{
	Q_OBJECT
		
public:

	QtMoviePlayer* movieplayer;

	QString currentworkingdir;

	DatasetView dsview;
	QTableWidget* tablewidget;

	QtDatasetViewWidget(QWidget* parent = 0, const char* name = 0);
	~QtDatasetViewWidget();
	
	void clear_all(void);

	void create(void);
	void exit(void);
	void reset(void);
	void rebuild(void);

private slots:

	void saveview_event(void);

	void generatethumbs_event(void);
	void openmovieplayer_event(void);
	
	void align_event(void);
	void computestacks_event(void);

	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

	void itemDoublClicked_event(QTableWidgetItem* item);
public slots:

	void open(void);
	void dragndrop_slot(QString sFileName);
	void datasetview_setThumbnail_slot(int i, QPixmap thumbnail);

signals:

	void datasetview_edit_MSR_signal(QString src_fn, QString dst_fn, int itemindex);

	void datasetview_align_signal(DatasetView dsv);
	void datasetview_computestacks_signal(QString dn, DatasetView dsv);

	void datasetview_saveCurrentView_signal(void);
	void datasetview_updatethumbnail_signal(int i);
	void datasetview_loadFolder_signal(QString dn);
	void datasetview_saveSettings_signal(QString dn, QString subdn);
	void datasetview_loadSettings_signal(QString dn, QString subdn);
};

#endif // QTCrop_H
