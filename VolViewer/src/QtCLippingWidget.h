/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QTCLIPPING_H
#define QTCLIPPING_H

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

#include <q3boxlayout.h>
#include <q3buttongroup.h>
#include <q3dragobject.h>

#include "QtGLObjWin.h"

class QtCLippingWidget: public QWidget
{
	Q_OBJECT
		
public:

	QtCLippingWidget(QWidget* parent = 0, const char* name = 0);
	~QtCLippingWidget();

	QString currentworkingdir;
	
	int selectedplane;
	int rotmode[6];

	QPushButton* red;
	QPushButton* green;
	QPushButton* blue;

	QPushButton* load_clip;
	QPushButton* save_clip;

	Q3ButtonGroup *selectedclip_grp;

	QPushButton* unpickplane;
	QPushButton* invert_clip;
	QPushButton* reset1;
	QPushButton* resetcur;
	QPushButton* pickplane;
	QPushButton* recentre;
	QPushButton* realign;
	QPushButton* invcrop;
	QPushButton* crop;

	QPushButton* plane_trans;
	QPushButton* plane_rotX;
	QPushButton* plane_rotY;

	QComboBox* planeselectcombo;
	QComboBox* planecombo;

	QLabel* curveplane_xang_lb;
	QSlider* curveplane_xang_slider;
	int curveplane_xang;
	QLabel* curveplane_yang_lb;
	QSlider* curveplane_yang_slider;
	int curveplane_yang;

	QPushButton*  curveplanetexcoords_btn;
	QPushButton*  curveplanetranslate_btn;

	void create(void);
	void exit(void);
	void reset(void);
	void rebuild(void);

private slots:
	void invert_button_event(void);

	void pickplane_button_event(void);
	void resetall_button_event(void);
	void resetcurr_button_event(void);
	void recentre_button_event(void);
	void crop_button_event(void);
	void invcrop_button_event(void);
	void realign_button_event(void);
	
	void cutplane_load_event(void);
	void cutplane_save_event(void);

	void red_button_event(void);
	void green_button_event(void);
	void blue_button_event(void);
	void iso_button_event(void);

	void interact_clicked_event(int);
	void unpickplane_button_event(void);

	void planecombo_event(int drawmode);
	void planespinbox_event(int);
	
	void applyall_event(void);

	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

	void curveplane_xang_event(int);
	void curveplane_yang_event(int);
	
	void curveplanetexcoords_button_event(void);
	void curveplanetranslate_button_event(void);

public slots:

	void cutplane_selectedupdate_slot(int index, int drawmode);
	void cutplane_selected_slot(int index, int drawmode);
	void cutplane_enableplanepanel_slot(void);
	void cutplane_unselect_slot(void);
	void cutplane_setcurrentworkingdir_slot(QString dn);
	void measure_setcutplane_slot(int selected, float translate, float rotang1, float rotang2);
	void measure_disableunselect_slot(void);
	void measure_enableunselect_slot(void);
	void dragndrop_slot(QString sFileName);
	
signals:
	void cutplane_load_signal(QString fn);
	void cutplane_save_signal(QString fn);
	void cutplane_showall_signal(int selected);
	void cutplane_resetall_signal(void);
	void cutplane_resetcurrent_signal(void);
	void cutplane_redclip_signal(void);
	void cutplane_greenclip_signal(void);
	void cutplane_blueclip_signal(void);
	void cutplane_isoclip_signal(void);
	void cutplane_invert_signal(int);
	void cutplane_rendermode_signal(int);

	void cutplane_crop_signal(void);
	void cutplane_invcrop_signal(void);	
	void cutplane_recentre_signal(void);
	void cutplane_realign_signal(void);
	
	void cutplane_interact_clicked_signal(int);
	void cutplane_unselect_signal(void);
	void cutplane_selected_signal(int);

	void cutplane_applyall_drawmode_signal(void);

	void cutplane_curveangle_signal(int, int);
	void cutplane_curvecoords_signal(bool);
	void cutplane_curvetrans_signal(bool);
};

#endif // QTCLIPPING_H