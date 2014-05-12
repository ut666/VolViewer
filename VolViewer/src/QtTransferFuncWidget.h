/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QtTransferFuncWidget_H
#define QtTransferFuncWidget_H

#include <qlabel.h>
#include <qlayout.h>
#include <qslider.h>
//#include <qmotifplusstyle.h>	//style
#include <q3buttongroup.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qdial.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <q3grid.h>
#include <q3boxlayout.h>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <q3dragobject.h>
#include <QDialog>

#include "QtHistoTfuncPixmap.h"

 class QtTransferFuncWidget: public QWidget
{
	Q_OBJECT
		
public:
		QString currentworkingdir;
		
		int channel_mode;
		
        QtHistoTfuncPixmap* histogrampixmap_luminance;
        QtHistoTfuncPixmap* histogrampixmap_opacity;

		Q3ButtonGroup* view_grp;
	
		Q3ButtonGroup* threshold_grp;

        double histo16bit[65536];
        double histo8bit[256*3];

        int histo16_logscale;
        int remap_threshmax;
        int remap_threshmin;

		//histogram, cumulatative frequency, transferfunction, logscale gui toggles
		QCheckBox *histcb;
		QCheckBox *cumfcb;
		QCheckBox *tfuncb;
		QCheckBox *logscalehistcb;
		QCheckBox *remaponflycb;

		Q3ButtonGroup *apply_grp;

		//threshold sliders
		int threshmin;
		QLabel*  threshmin_lb;
		QSlider* threshmin_slider; 
		int threshmax;
		QLabel *threshmax_lb;
		QSlider* threshmax_slider;

		//brigth, contrast sliders
		int			brightness;
		QLabel*		brightness_lb;
		QSlider*	brightness_slider; 
		int			contrast;
		QLabel *	contrast_lb;
		QSlider*	contrast_slider;

		QPushButton* invert_button;
		
        QtTransferFuncWidget(QWidget* parent = 0, const char* name = 0);
        ~QtTransferFuncWidget();

	void create(void);
	void reset(void);
	void rebuild(void);

public slots:
	void tfunc_loadfile(QString fn);
	void dragndrop_slot(QString sFileName);
    void tfunc_16bit_histogram_slot(double*, double*);
	void tfunc_remapslot(void);
	void tfunc16_setcurrentworkingdir_slot(QString dn);
	
	void threshmin_action(int i);
	void threshmax_action(int i);
private slots:

	void open_event(void);
	void save_event(void);

	void channelmode_button_event(int mode);

	void histoview_action(void);	
	void cumfreview_action(void);	
	void transferfun_action(void);	
	void logscalehistcb_action(void);
	void remaponflycb_action(void);
	void contrast_action(int i);
	void brightness_action(int i);
	
	void invert_button_action(void);
	
	void resetcurrent_button_action(void);
	void resetall_button_action(void);
	void apply_button_action(void);
	
	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);

signals:
	void tfunc_remap16bit_signal(int, int, const vector<Vector>&, int, int, const vector<Vector>&);
 	void tfunc_brightcontrast_signal(int mode, int bright, int cont);
	void tfunc_invert_signal(int, int);
};

#endif // QtTransferFuncWidget_H
