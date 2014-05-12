/*
 *   Copyright 2009 Jerome Avondo. All rights reserved.
 *   Use is subject to license terms supplied in LICENSE.txt
 */
#ifndef QTFILTER_H
#define QTFILTER_H

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

#include "MyDefines.h"
#include "FilterDefinitions.h"
#include "QtGLObjWin.h"

class QtFilterWidget: public QWidget
{
	Q_OBJECT
		
public:

	QtFilterWidget(QWidget* parent = 0, const char* name = 0);
	~QtFilterWidget();

	QString currentworkingdir;
	
	int filter_selected;
	
	vector<bool> channels;
	
	QPushButton* red;
	QPushButton* green;
	QPushButton* blue;
	
	QPushButton* load_pb;
	QPushButton* save_pb;
	QPushButton* reset_pb;
	
	Q3ButtonGroup* channelsop_grp;
	Q3ButtonGroup* mortho_grp;

	Q3ButtonGroup* easyfilters_grp;
	Q3ButtonGroup* filters_grp;
	Q3ButtonGroup* gpufilters_grp;
	Q3ButtonGroup* operators_grp;
	Q3ButtonGroup* regions_grp;
	Q3ButtonGroup* histogram_grp;

	QDialog* filter_dialogue;
	QPushButton* twod_pb;
	QPushButton* threed_pb;
	QLineEdit* kernel_size_le;
	QLineEdit* kernel_strength_le;
	
	QComboBox* repop_combobox;
	QComboBox* in_combobox;
	QComboBox* with_combobox;

	QComboBox* src_combobox;
	QComboBox* dst_combobox;

	QPushButton* anotbbutton;
	QPushButton* bnotabutton;
	QPushButton* adiffbbutton;
	QPushButton* aandbbutton;

	QPushButton* addbutton;
	QPushButton* subbutton;

	vector<bool> axis;
	QPushButton* xaxisbutton;
	QPushButton* yaxisbutton;
	QPushButton* zaxisbutton;

	QLineEdit* seedptwindow_le;
	QLineEdit* gaussthresh_le;

	QLineEdit* minsize_le;

	QPushButton* previewSections_pb;
	QPushButton* previewVolume_pb;

	QPushButton* maxflow_bg;
	QPushButton* maxflow_fg;
	QPushButton* maxflow_brush2d;
	QPushButton* maxflow_brush3d;

	QLineEdit* maxflow_kparam_le;
	QLineEdit* maxflow_vparam_le;
	QLineEdit* maxflow_wparam_le;
	QLineEdit* maxflow_brushparam_le;

	void create(void);
	void exit(void);
	void reset(void);
	void rebuild(void);

	void easymode(bool state);

private slots:

	void dragEnterEvent(QDragEnterEvent *event);
	void dropEvent(QDropEvent *event);
	void reset_button_event(void);
	void channel_button_event(int selected);
	void axis_button_event(int selected);
	void load_event(void);
	void save_event(void);

	void filterok_event(void);
	void filtercancel_event(void);

	void ffill_event(void);
	void addto_event(void);
	void copyto_event(void);
	void setoperator_event(void);
	void artihoperator_event(void);
	void erode_event(void);
	void dilate_event(void);
	void sieve_event(void);
	void collapse_event(void);
	void invert_event(void);
	void gaussian_event(void);
	void bilateral_event(void);
	void median_event(void);
	void unsharp_event(void);
	void thresh_event(void);
	void binary_event(void);
	void sobel_event(void);
	void autolevels_event(void);
	void contraststretch_event(void);
	void smoothdil_event(void);
	void replacewith_event(void);
	void clear_event(void);
	void morpopen_event(void);
	void morpclose_event(void);
	void maxflow_event(void);
	void peronamalik_event(void);
	void count_event(void);
	void swap_event(void);

#ifdef OPENCL_SUPPORT
	void gpu_peronamalik_event(void);
	void gpu_median_event(void);
	void gpu_gaussian_event(void);
	void gpu_dilate_event(void);
	void gpu_erode_event(void);
	void gpu_smoothdil_event(void);
	void gpu_bilateral_event(void);
#endif

	void despekleOK_event(void);
	void removenoiseOK_event(void);
	void sharpenOK_event(void);
	void smoothOK_event(void);

	void despekle_event(void);
	void removenoise_event(void);
	void sharpen_event(void);
	void smooth_event(void);

	void maxflow_fg_button_event(void);
	void maxflow_bg_button_event(void);
	void maxflow_clear_button_event(void);
	void maxflow_brushparam_event(const QString& text);

	void maxflow_2d_button_event(void);
	void maxflow_3d_button_event(void);

	void mean_event(void);

	void filtersmdiladd_event(void);
	void filtersmdilgo_event(void);
	
#ifdef OPENCL_SUPPORT
	void gpu_filtersmdilgo_event(void);
#endif

	void filtersmdilabort_event(void);
	void filtersmdilclose_event(void);

public slots:

	void dragndrop_slot(QString sFileName);
	
signals:
	void filter_signal(int filtertype, vector<bool> channels, vector<int> intparams, vector<double> doubparams);
	
	void filter_smthdilate_signal(int srcchan, int dstchan, int seed_window, int minsize, int gaussian_width, float gaussian_strength, int gaussian_window, int show_sections, int show_volume);
	
#ifdef OPENCL_SUPPORT
	void filter_gpu_smthdilate_signal(int srcchan, int dstchan, int seed_window, int minsize, int gaussian_width, float gaussian_strength, int gaussian_window, int show_sections, int show_volume);
#endif	

	void filter_smthdilateabort_signal(void);
	void filter_maxflow_interact_signal(int mode);
	void filter_maxflow_buildlabels_signal(void);
	void filter_maxflow_clear_signal(void);
	void filter_maxflow_open_signal(void);
	void filter_maxflow_save_signal(void);
	void filter_maxflow_brush_signal(float);
	void filter_maxflow_dimmode_signal(int);
};

#endif // QTFILTER_H
