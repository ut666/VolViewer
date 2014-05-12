/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QTTRANFERFUNC_H
#define QTTRANFERFUNC_H

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

#include "QtHistogram.h"
#include "QtTransferFuncDialogue.h"
#include "TfuncState.h"

 class QtTransferFunctionWidget: public QWidget
{
	Q_OBJECT
		
public:
	int updating_labels;
	
	//channel mode the widget is in (I,R,G,B, IA, RA, GA, BA)
	int transfer_mode;
	int channel_selected;
	int channelmode_selected;

	Q3BoxLayout *histlayout_centre;
	Q3ButtonGroup* view_grp;
	Q3ButtonGroup* update_grp;
	
	Q3ButtonGroup* opensave_grp;
	Q3ButtonGroup *toggle_grp;
	Q3ButtonGroup *channelmode_grp;
	Q3ButtonGroup *reset_grp;
	Q3ButtonGroup* action_grp;
	Q3ButtonGroup* briconslider_grp;
	Q3BoxLayout *vboxlayout_centre;
	Q3BoxLayout *vboxlayoutRIGHT;
	Q3BoxLayout *vboxlayoutLEFT;
	Q3BoxLayout *topmost;
	
	//invert, equalize and stretch histo buttons
	QPushButton* remap_button;
	QPushButton* convR_button;
	QPushButton* convG_button;
	QPushButton* convB_button;
	QPushButton* collapse_button;
	QPushButton* invert_button;
	QPushButton* histequ_button;
	QPushButton* contstrat_button;
	QPushButton* sliceupdate_button;

	//pointer to our histogram graph
	QtHistogram* hist;

	//histogram, cumulatative frequency and transferfunction gui toggles
	QCheckBox *histcb;
	QCheckBox *cumfcb;
	QCheckBox *tfuncb;

	QCheckBox *updatehistcb;
	QCheckBox *logscalehistcb;

	//channel push buttons
	QPushButton* intensity_pb;
	QPushButton* red_pb;
	QPushButton* green_pb;
	QPushButton* blue_pb;
	QPushButton* luminance_pb;
	QPushButton* opacity_pb;

	//threshold sliders
	Q3ButtonGroup *threshold_grp;
	int		 threshmin;
	QLabel*  threshmin_lb;
	QSlider* threshmin_slider; 
	int threshmax;
	QLabel *threshmax_lb;
	QSlider* threshmax_slider;

	//brightness/contrast sliders
	int			brightness;
	QLabel*		brightness_lb;
	QSlider*	brightness_slider; 
	QDial*		brightness_dial;
	int			contrast;
	QLabel *	contrast_lb;
	QSlider*	contrast_slider;
	QDial*		contrast_dial;

	Q3ButtonGroup *transfer_grp;
	
	Q3ButtonGroup* mapping_grp;
	QCheckBox *linearmapcb;
	QCheckBox *bsplinecb;
	
	QtTransferFunctionWidget(QWidget* parent = 0, const char* name = 0);
	~QtTransferFunctionWidget();

	void create(void);
	void reset(void);
	void rebuild(void);
	void emit_transfer_function();
	void update_slider_labels(int tmin, int tmax, int b, int c);

public slots:
	
	void dragndrop_slot(QString sFileName);
	void tfunc_histogram_slot(double* histo8);
	void closebutton_event(void);
	void applybutton_event(void);
	void resetbutton_event(void);
	void resetall_button_action(void);

	void tfunc_updateAll_slot(TfuncState lum, TfuncState alp, TfuncState redlum, TfuncState redalp, TfuncState greenlum, TfuncState greenalp, TfuncState bluelum, TfuncState bluealpha);

private slots:

	void open_event(void);
	void save_event(void);

	void resetcurr_button_action(void);
	void channel_button_event(int id);
	void channelmode_button_event(int id);

	void histogram_action(int* histo);

	void histoview_action(void);	
	void cumfreview_action(void);	
	void transferfun_action(void);	

	void threshmin_action(int i);
	void threshmax_action(int i);

	void brightness_action(int i);
	void contrast_action(int i);

	void updatehisto_action(void);
	void logscalehistcb_action(void);

	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
	
	void linearmapcb_action(void);
	void bsplinecb_action(void);

	void update_tfuncs(void);
signals:

	void tfunc_rebuild_histogram_signal(void);
	void tfunc_histoupdate_signal(int toggle);
};

#endif // QTTRANFERFUNC_H
