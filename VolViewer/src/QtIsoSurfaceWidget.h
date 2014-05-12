/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QTISOVALUE_H
#define QTISOVALUE_H

#include <qpainter.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qslider.h>
#include <qimage.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <QDialog>
#include <QMessageBox>
#include <qlineedit.h>
#include <qcolordialog.h>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QPixmap>
#include <QApplication>

#include <q3listbox.h>
#include <q3buttongroup.h>
#include <q3boxlayout.h>
#include <q3dragobject.h>

#include "QtHistogram.h"

class QtIsoSurfaceWidget: public QWidget
{
	Q_OBJECT
		
public:

	QString currentworkingdir;

	bool channels[3];

	int selected;
	QString mname;
	QColor colour;
	QPushButton* col_btn;
	QLineEdit* name_le;
	int nv, nf;

	QPushButton* red;
	QPushButton* green;
	QPushButton* blue;

	int msgbox_state;
	QDialog* msgbox;

	int lb_index;
	Q3ListBox*	 listbox;

	QPixmap* preview_pm;
	QPixmap* hsv_preview_pm;
	QLabel*	hsv_preview_lb;
	QLabel*	preview_lb;

//	QButtonGroup *threshold_grp;

	//iso surface quality (mcubes resolution)
	int		 isoqual;
	QLabel*  isoqual_lb;
	QSlider* isoqual_slider;

	//iso surface value (threshold)
	int		 isoval;
	QLabel*  isoval_lb;
	QSlider* isoval_slider;

	int		 isosmooth;
	QLabel*  isosmooth_lb;
	QSlider* isosmooth_slider;

	int clip_state;
	QPushButton* clipstate;

	QPushButton* cropstate;

	QtIsoSurfaceWidget(QWidget* parent = 0, const char* name = 0);
	~QtIsoSurfaceWidget();

	void create(void);
	void reset(void);
	void rebuild(void);
	void del_all(void);

	bool check_unique_name(QString text);

private slots:
	//isosurface tab, slider events
	void open_event(void);
	void save_event(void);
	void isoqual_event(int j);
	void isosmooth_event(int j);
	void isoval_event(int j);
	void add_event(void);
	void addnew_event(QString name, Vector col);
	void del_event(void);
	void msgbox_ok(void);
	void msgbox_cancel(void);
	void msgbox_colourdlg_event(void);
	void msg_event(void);
	void listbox_event(Q3ListBoxItem * item);
	void redchannel_button_event(void);
	void greenchannel_button_event(void);
	void bluechannel_button_event(void);
	void dragndrop_slot(QString sFileName);
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
	void crop_button_event(void);
public slots:
	void isosurface_setcurrentworkingdir_slot(QString dn);

signals:
	void isosurface_load_signal(QString fn);
	void isosurface_save_signal(void);
	void isosurface_signal(int min, int max);

	void isosurface_new_signal(QString name, int quality, int value, int smooth, Vector col, bool* channels, int clpst);
	void isosurface_get_signal(QString name, int* quality, int* value, int* smooth, Vector* col, int* nv, int* nf, int* clpst);
	void isosurface_del_signal(QString name);
	void isosurface_update_signal(QString oldname, QString name, int quality, int value, int smooth, Vector col, bool* channels, int clpst);
	
	void isosurface_crop_signal(int selected);
};

#endif // QTISOVALUE_H
