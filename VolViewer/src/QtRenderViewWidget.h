/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QTRENVIEW_H
#define QTRENVIEW_H

#include <QPainter>
#include <QLabel>
#include <QLayout>
#include <QSlider>
#include <QString>
#include <QPushButton> 
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QDir>
#include <QFileDialog>
#include <QDialog>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>

#include <Q3BoxLayout>
#include <Q3ButtonGroup>
#include <Q3DragObject>

#include <fstream>
#include <vector>
using namespace std;

#include "VolumeRenderView.h"
class QtRenderViewWidget: public QWidget
{
	Q_OBJECT
		
public:

	QString currentworkingdir;

	QListWidget* mylistwidget;
	vector< VolumeRenderView > volumeviews;

	QtRenderViewWidget(QWidget* parent = 0, const char* name = 0);
	~QtRenderViewWidget();

	void create(void);
	void exit(void);
	void reset(void);
	void rebuild(void);
	void load_file(QString fn);
	void save_file(QString fn);
	float check_file_version(QString fn);
	void load_version0(QString fn);
	void load_version1(QString fn);

private slots:

	void open_event(void);
	void save_event(void);

	void saveview_event(void);
	void deleteview_event(void);
	void updateview_event(void);

	void interpolate_event(void);

	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

	void itemDoublClicked_event(QListWidgetItem* item);

public slots:

	void renderview_selectItem(int index);
	void renderview_setcurrentworkingdir_slot(QString dn);

	void renderview_addsavedview_slot(QPixmap* pixmap);
	void renderview_addview_slot(QPixmap* pixmap, VolumeRenderView view);
	void renderview_updateview_slot(int, QPixmap*, VolumeRenderView);

	void renderview_getallviews_slot(void);

	void dragndrop_slot(QString sFileName);

signals:
	void renderview_saveview_signal(void);
	void renderview_updateview_signal(int);

	void renderview_renderview_signal(VolumeRenderView view);
	void renderview_setview_signal(VolumeRenderView view);
	void renderview_interpolate_signal(vector< VolumeRenderView > volumeviews);
};

#endif // QTCrop_H
